#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

Representation TouchPlateSwitch::representation()
{
    return Representation(Spriteset::Switches, 44);
}

TouchPlateSwitch::TargetObject::TargetObject()
{
    object_id=0;
    state=TargetState::enable;
}

TouchPlateSwitch::TouchPlateSwitch()
    :Object(Type::ObjectType::TouchPlateSwitch)
{
    sprite_set=Spriteset::Switches;
    sprite_no=44;
    collisionDetection=true;
    pixelExactCollision=false;
    visibleAtPlaytime=true;
    sprite_no_representation=0;
    initial_state=false;
    plate_style=PlateStyle::Wide;
    color_base=7;
    touched=false;
    cooldown=0.0f;
    last_collision=0.0f;
    current_state=initial_state;
    init();
}

void TouchPlateSwitch::init()
{
    switch (plate_style) {
        case PlateStyle::Narrow:
            sprite_no=42;
            break;
        case PlateStyle::Wide:
            sprite_no=44;
            break;
    }
    touched=false;
    last_collision=0.0f;
    cooldown=0.0f;
    sprite_no_representation=sprite_no;
    const ColorPalette& palette=GetColorPalette();
    color_mod.set(palette.getColor(color_base));
    this->updateBoundary();
}

void TouchPlateSwitch::reset()
{
    touched=false;
    last_collision=0.0f;
    cooldown=0.0f;
    current_state=initial_state;
    init();
}

void TouchPlateSwitch::notify_targets()
{
    ObjectSystem* objs=GetObjectSystem();
    for (int i=0;i < 10;i++) {
        if (targets[i].object_id > 0) {
            Object* target=objs->getObject(targets[i].object_id);
            if (target) {
                TargetState target_state=targets[i].state;
                if (target_state == TargetState::trigger) target->trigger(this);
                else {
                    bool s=false;
                    if (target_state == TargetState::enable) s=true;
                    if (!touched) {
                        s=!s;
                    }
                    target->toggle(s, this);
                }
            }
        }
    }
}

void TouchPlateSwitch::toggle(bool enable, Object* source)
{
    //if (source == this) return;
    current_state=enable;
    init();
    //notify_targets();
}

void TouchPlateSwitch::trigger(Object* source)
{
    toggle(!current_state, source);
}

void TouchPlateSwitch::update(double time, TileTypePlane&, Player& player, float)
{
    if (!current_state) {
        if (plate_style == PlateStyle::Narrow) sprite_no=43;
        else sprite_no=45;
        return;
    }
    std::list<Object*> object_list;

    GetObjectSystem()->detectObjectCollision(this, object_list);
    if (object_list.size() > 0) {
        last_collision=time;
    }

    if (touched == false && last_collision > time - 0.1) {
        touched=true;
        //current_state=true;
        notify_targets();
        if (plate_style == PlateStyle::Narrow) sprite_no=43;
        else sprite_no=45;

    } else if (touched == true && last_collision < time - 0.1) {
        touched=false;
        //current_state=false;
        notify_targets();
        cooldown=time + 0.1;
        if (plate_style == PlateStyle::Narrow) sprite_no=42;
        else sprite_no=44;

    }
}

void TouchPlateSwitch::handleCollision(Player* player, const Collision& collision)
{
    //ppl7::PrintDebugTime("TouchPlateSwitch::handleCollision, cooldown: %0.3f, time: %0.3f\n", cooldown, player->time);
    if (cooldown > player->time) return;
    //ppl7::PrintDebugTime("   ===> ok\n");
    last_collision=player->time;

}

size_t TouchPlateSwitch::saveSize() const
{
    return Object::saveSize() + 4 + 10 * 3;
}

size_t TouchPlateSwitch::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 1);		// Object Version

    int flags=0;
    if (initial_state) flags|=1;
    ppl7::Poke8(buffer + bytes + 1, flags);
    ppl7::Poke8(buffer + bytes + 2, static_cast<int>(plate_style));
    ppl7::Poke8(buffer + bytes + 3, color_base);
    bytes+=4;
    for (int i=0;i < 10;i++) {
        ppl7::Poke16(buffer + bytes, targets[i].object_id);
        ppl7::Poke8(buffer + bytes + 2, static_cast<int>(targets[i].state));
        bytes+=3;
    }
    return bytes;
}

size_t TouchPlateSwitch::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0 || size < bytes + 1) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version < 1 || version>2) return 0;

    int flags=ppl7::Peek8(buffer + bytes + 1);
    initial_state=(bool)(flags & 1);
    current_state=initial_state;
    plate_style=static_cast<PlateStyle>(ppl7::Peek8(buffer + bytes + 2));
    color_base=ppl7::Peek8(buffer + bytes + 3);
    bytes+=4;
    for (int i=0;i < 10;i++) {
        targets[i].object_id=ppl7::Peek16(buffer + bytes);
        targets[i].state=static_cast<TargetState>(ppl7::Peek8(buffer + bytes + 2));
        bytes+=3;
    }
    init();
    return size;
}

class TouchPlateSwitchDialog : public Decker::ui::Dialog
{
private:
    TouchPlateSwitch* object;
    ppltk::ComboBox* plate_style;
    ppltk::CheckBox* initial_state, * current_state;
    ppltk::SpinBox* target_id[10];
    ppltk::RadioButton* target_state_on[10];
    ppltk::RadioButton* target_state_off[10];
    ppltk::RadioButton* target_state_trigger[10];
    //ppltk::Button* reset;

    Decker::ui::ColorSelectionFrame* colorframe;


public:
    TouchPlateSwitchDialog(TouchPlateSwitch* object);
    virtual void valueChangedEvent(ppltk::Event* event, int value);
    virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
    virtual void toggledEvent(ppltk::Event* event, bool checked);
    virtual void dialogButtonEvent(Dialog::Buttons button) override;
    //void mouseDownEvent(ppltk::MouseEvent* event) override;

};

void TouchPlateSwitch::openUi()
{
    TouchPlateSwitchDialog* dialog=new TouchPlateSwitchDialog(this);
    GetGameWindow()->addChild(dialog);
}


TouchPlateSwitchDialog::TouchPlateSwitchDialog(TouchPlateSwitch* object)
    : Decker::ui::Dialog(900, 600, Dialog::Buttons::OK | Dialog::Buttons::Reset)
{
    this->object=object;

    setWindowTitle(ppl7::ToString("Touchplate, ID: %d", object->id));
    //ppl7::grafix::Rect client=clientRect();
    int y=0;
    //int sw=(client.width()) / 2;
    int sw=480;


    addChild(new ppltk::Label(sw, y, 120, 30, "Targets: "));
    y+=35;
    for (int i=0;i < 10;i++) {
        addChild(new ppltk::Label(sw + 20, y, 80, 30, ppl7::ToString("Object %d: ", i + 1)));
        target_id[i]=new ppltk::SpinBox(sw + 100, y, 100, 30, object->targets[i].object_id);
        target_id[i]->setLimits(0, 65535);
        target_id[i]->setEventHandler(this);
        addChild(target_id[i]);

        ppltk::Frame* frame=new ppltk::Frame(sw + 210, y, 210, 30, ppltk::Frame::BorderStyle::NoBorder);
        frame->setTransparent(true);

        target_state_on[i]=new ppltk::RadioButton(0, 0, 50, 30, "on", object->targets[i].state == TouchPlateSwitch::TargetState::enable);
        target_state_on[i]->setEventHandler(this);
        frame->addChild(target_state_on[i]);
        target_state_off[i]=new ppltk::RadioButton(50, 0, 60, 30, "off", object->targets[i].state == TouchPlateSwitch::TargetState::disable);
        target_state_off[i]->setEventHandler(this);
        frame->addChild(target_state_off[i]);
        target_state_trigger[i]=new ppltk::RadioButton(100, 0, 90, 30, "trigger", object->targets[i].state == TouchPlateSwitch::TargetState::trigger);
        target_state_trigger[i]->setEventHandler(this);
        frame->addChild(target_state_trigger[i]);
        addChild(frame);

        y+=30;
    }
    y=0;


    addChild(new ppltk::Label(0, y, 100, 30, "Plate-Style: "));
    plate_style=new ppltk::ComboBox(100, y, 300, 30);
    plate_style->add("Narrow", "0");
    plate_style->add("Wide", "1");
    plate_style->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->plate_style)));
    plate_style->setEventHandler(this);
    addChild(plate_style);
    y+=40;

    addChild(new ppltk::Label(0, y, 100, 30, "Flags: "));
    initial_state=new ppltk::CheckBox(100, y, 160, 30, "Initial state: on", object->initial_state);
    initial_state->setEventHandler(this);
    addChild(initial_state);

    current_state=new ppltk::CheckBox(260, y, 160, 30, "current state: on", object->current_state);
    current_state->setEventHandler(this);
    addChild(current_state);
    y+=35;

    addChild(new ppltk::Label(0, y, 80, 30, "Color:"));
    y+=35;
    ColorPalette& palette=GetColorPalette();
    colorframe=new Decker::ui::ColorSelectionFrame(170, y, 300, 300, palette);
    colorframe->setEventHandler(this);
    colorframe->setColorIndex(object->color_base);
    this->addChild(colorframe);

}

void TouchPlateSwitchDialog::valueChangedEvent(ppltk::Event* event, int value)
{
    ppltk::Widget* widget=event->widget();
    if (widget == plate_style) {
        object->plate_style=static_cast<TouchPlateSwitch::PlateStyle>(plate_style->currentIdentifier().toInt());
        object->init();
    }
    if (widget == colorframe) {
        object->color_base=value;
        object->init();
    }
}

void TouchPlateSwitchDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    for (int i=0;i < 10;i++) {
        if (event->widget() == target_id[i]) {
            //handled=true;
            object->targets[i].object_id=(value & 0xffff);
        }
    }
}

void TouchPlateSwitchDialog::toggledEvent(ppltk::Event* event, bool checked)
{
    if (checked) {
        for (int i=0;i < 10;i++) {
            if (event->widget() == target_state_on[i]) {
                object->targets[i].state=TouchPlateSwitch::TargetState::enable;
            } else 	if (event->widget() == target_state_off[i]) {
                object->targets[i].state=TouchPlateSwitch::TargetState::disable;
            } else 	if (event->widget() == target_state_trigger[i]) {
                object->targets[i].state=TouchPlateSwitch::TargetState::trigger;
            }
        }
    }
    if (event->widget() == initial_state) object->initial_state=checked;
    else if (event->widget() == current_state) object->toggle(checked);

}

void TouchPlateSwitchDialog::dialogButtonEvent(Dialog::Buttons button)
{
    if (button == Dialog::Buttons::Reset) object->reset();
}


}	// EOF namespace Decker::Objects
