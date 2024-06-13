#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"
#include "widgets.h"

namespace Decker::Objects {


Representation LightSignal::representation()
{
    return Representation(Spriteset::GenericObjects, 2);
}

LightSignal::LightSignal()
    :Object(Type::ObjectType::LightSignal)
{
    color_on.set(0, 192, 0, 255);
    color_off.set(192, 0, 0, 255);
    dualColor=false;
    initialState=true;
    currentState=true;
    blink=false;
    sprite_set=Spriteset::GenericObjects;
    sprite_no=2;
    sprite_no_representation=2;
    collisionDetection=false;

    led.sprite_no=0;
    led.scale_x=0.2f;
    led.scale_y=0.2f;
    led.custom_texture=NULL;
    led.intensity=255;
    led.plane=static_cast<int>(LightPlaneId::Player);
    led.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);
    led.has_lensflare=true;
    led.flare_intensity=128;
    led.flare_useLightColor=true;
    led.flarePlane=static_cast<int>(LightPlayerPlaneMatrix::Back);
    blink_state=false;
    next_state=0.0f;
}

LightSignal::~LightSignal()
{

}

void LightSignal::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
    if (blink == true) {
        if (next_state < time) {
            next_state=time + 0.500f;
            blink_state=!blink_state;
        }
        if (!blink_state) {
            sprite_no=1;
            color_mod=color_on;
            if (dualColor) color_mod=color_off;
            return;
        }
    }
    if (currentState) {
        led.color=color_on;
        led.x=p.x;
        led.y=p.y;
        LightSystem& lights=GetGame().getLightSystem();
        lights.addObjectLight(&led);
        sprite_no=2;
        color_mod=color_on;
    } else if (dualColor) {
        led.color=color_off;
        led.x=p.x;
        led.y=p.y;
        LightSystem& lights=GetGame().getLightSystem();
        lights.addObjectLight(&led);
        sprite_no=2;
        color_mod=color_off;
    } else {
        sprite_no=1;
        color_mod=color_on;
    }

}

void LightSignal::toggle(bool enable, Object* source)
{
    currentState=enable;
    next_state=0.0f;
    blink_state=true;
}

void LightSignal::trigger(Object* source)
{
    toggle(!currentState, source);
}

size_t LightSignal::saveSize() const
{
    return Object::saveSize() + 10;
}

size_t LightSignal::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 1);		// Object Version
    uint8_t flags=0;
    if (initialState) flags|=1;
    if (dualColor) flags|=2;
    if (blink) flags|=4;
    ppl7::Poke8(buffer + bytes + 1, flags);
    ppl7::Poke32(buffer + bytes + 2, color_on.color());
    ppl7::Poke32(buffer + bytes + 6, color_off.color());
    return bytes + 10;

}

size_t LightSignal::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0) return 0;
    if (size < bytes + 10) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version != 1) return 0;
    uint8_t flags=ppl7::Peek8(buffer + bytes + 1);
    initialState=flags & 1;
    currentState=initialState;
    dualColor=flags & 2;
    blink=flags & 4;
    color_on.setColor(ppl7::Peek32(buffer + bytes + 2));
    color_off.setColor(ppl7::Peek32(buffer + bytes + 6));
    return size;

}

class LightSignalDialog : public Decker::ui::Dialog
{
private:
    ppltk::CheckBox* initial_state_checkbox, * current_state_checkbox;
    ppltk::CheckBox* dual_color_checkbox, * blink_checkbox;
    Decker::ui::ColorSliderWidget* color_on;
    Decker::ui::ColorSliderWidget* color_off;

    LightSignal* object;

public:
    LightSignalDialog(LightSignal* object);
    void toggledEvent(ppltk::Event* event, bool checked) override;
    void valueChangedEvent(ppltk::Event* event, int value) override;
    void valueChangedEvent(ppltk::Event* event, int64_t value) override;

};


void LightSignal::openUi()
{
    LightSignalDialog* dialog=new LightSignalDialog(this);
    GetGameWindow()->addChild(dialog);
}

LightSignalDialog::LightSignalDialog(LightSignal* object)
    : Decker::ui::Dialog(570, 480, Dialog::Buttons::OK)
{
    this->object=object;
    this->setWindowTitle(ppl7::ToString("Light Signal, ID: %d", object->id));
    ppl7::grafix::Rect client=clientRect();
    int y=0;
    int sw=(client.width()) / 2;
    // State
    initial_state_checkbox=new ppltk::CheckBox(0, y, sw, 30, "initial State", object->initialState);
    initial_state_checkbox->setEventHandler(this);
    addChild(initial_state_checkbox);
    current_state_checkbox=new ppltk::CheckBox(sw, y, sw, 30, "current State", object->currentState);
    current_state_checkbox->setEventHandler(this);
    addChild(current_state_checkbox);
    y+=35;
    dual_color_checkbox=new ppltk::CheckBox(0, y, sw, 30, "dual color", object->dualColor);
    dual_color_checkbox->setEventHandler(this);
    addChild(dual_color_checkbox);
    blink_checkbox=new ppltk::CheckBox(sw, y, sw, 30, "blink", object->blink);
    blink_checkbox->setEventHandler(this);
    addChild(blink_checkbox);
    y+=40;

    addChild(new ppltk::Label(0, y, 80, 30, "Color On:"));
    y+=30;
    color_on=new Decker::ui::ColorSliderWidget(0, y, client.width(), 100);
    color_on->setEventHandler(this);
    color_on->setColor(object->color_on);
    color_on->setColorPreviewSize(64, 90);
    this->addChild(color_on);
    y+=110;

    addChild(new ppltk::Label(sw, y, 80, 30, "Color Off:"));
    y+=30;
    color_off=new Decker::ui::ColorSliderWidget(0, y, client.width(), 100);
    color_off->setEventHandler(this);
    color_off->setColor(object->color_off);
    color_off->setColorPreviewSize(64, 90);
    this->addChild(color_off);
    y+=110;

}

void LightSignalDialog::toggledEvent(ppltk::Event* event, bool checked)
{
    if (event->widget() == initial_state_checkbox) {
        object->initialState=checked;
    } else if (event->widget() == current_state_checkbox) {
        object->currentState=checked;
    } else if (event->widget() == dual_color_checkbox) {
        object->dualColor=checked;
    } else if (event->widget() == blink_checkbox) {
        object->blink=checked;
    }
}

void LightSignalDialog::valueChangedEvent(ppltk::Event* event, int value)
{
    if (event->widget() == color_on) {
        object->color_on=color_on->color();
    } else if (event->widget() == color_off) {
        object->color_off=color_off->color();
    }
}

void LightSignalDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{

}


}	// EOF namespace Decker::Objects
