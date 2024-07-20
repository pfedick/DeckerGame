#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"

namespace Decker::Objects {



Representation Trigger::representation()
{
    return Representation(Spriteset::GenericObjects, 296);
}

Trigger::Trigger()
    :Object(Type::ObjectType::Trigger)
{
    sprite_set=Spriteset::GenericObjects;
    sprite_no=296;
    collisionDetection=true;
    pixelExactCollision=false;
    visibleAtPlaytime=false;
    initialStateEnabled=true;
    sprite_no_representation=296;
    range.setPoint(TILE_WIDTH * 6, TILE_HEIGHT * 6);
    state=State::waiting_for_activation;
    triggeredByCollision=true;
    multiTrigger=false;
    cooldownUntilNextTrigger=0.0f;
    maxTriggerCount=0;
    cooldown=0.0f;
    trigger_count=0;
    triggerDeleay=0.0f;
    for (int i=0;i < 10;i++) {
        triggerObjects[i].object_id=0;
    }
}

Trigger::~Trigger()
{

}

void Trigger::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
    if (triggeredByCollision) {
        SDL_Rect r;
        r.x=p.x + coords.x - range.x / 2;
        r.y=p.y + coords.y - range.y / 2;
        r.w=range.x;
        r.h=range.y;

        SDL_BlendMode currentBlendMode;
        SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 192, 0, 96);
        SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawColor(renderer, 0, 192, 0, 255);
        SDL_RenderDrawRect(renderer, &r);
        SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
    }
    Object::drawEditMode(renderer, coords);
}

void Trigger::handleCollision(Player* player, const Collision& collision)
{
    if (!triggeredByCollision) return;
    last_collision_time=player->time;
    //ppl7::PrintDebugTime("set last_collision_time %d\n", id);

    if (multiTrigger == false && trigger_count > 0) return;
    if (multiTrigger == true && cooldown > player->time) return;
    if (state == State::waiting_for_activation) {
        state=State::activated;
        enabled=false;
    }
}

void Trigger::test()
{
    state=State::activated;
    enabled=false;
}

void Trigger::notifyTargets() const
{
    //ppl7::PrintDebugTime("Trigger::notifyTargets\n");
    ObjectSystem* objs=GetObjectSystem();
    for (int i=0;i < 10;i++) {
        if (triggerObjects[i].object_id > 0) {
            Object* target=objs->getObject(triggerObjects[i].object_id);
            if (target) {
                if (triggerObjects[i].state == Trigger::TargetState::trigger) target->trigger();
                else if (triggerObjects[i].state == Trigger::TargetState::enable) target->toggle(true);
                else if (triggerObjects[i].state == Trigger::TargetState::disable) target->trigger(false);
            }
        }
    }

}

void Trigger::update(double time, TileTypePlane& ttplane, Player& player, float)
{
    boundary.setRect(p.x - range.x / 2, p.y - range.y / 2, range.x, range.y);
    if (state == State::activated) {
        //ppl7::PrintDebugTime("Trigger::update activated\n");
        triggerDeleayTime=time + triggerDeleay;
        state=State::waiting_for_trigger_delay;
    }
    if (state == State::waiting_for_trigger_delay && time >= triggerDeleayTime) {
        state=State::finished;
        notifyTargets();
    }
    if (state == State::finished && multiTrigger == true && last_collision_time + cooldownUntilNextTrigger < time) {
        //ppl7::PrintDebugTime("von vorne %d\n", id);
        state=State::waiting_for_activation;
        enabled=true;
        cooldown=0.0f;
    }

}

size_t Trigger::saveSize() const
{
    size_t s=16 + 10 * 3;
    return Object::saveSize() + s;

}

size_t Trigger::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 2);		// Object Version
    int flags=0;
    if (multiTrigger) flags|=1;
    if (triggeredByCollision) flags|=2;
    if (initialStateEnabled) flags|=4;

    ppl7::Poke8(buffer + bytes + 1, flags);
    ppl7::PokeFloat(buffer + bytes + 2, cooldownUntilNextTrigger);
    ppl7::PokeFloat(buffer + bytes + 6, triggerDeleay);
    ppl7::Poke16(buffer + bytes + 10, range.x);
    ppl7::Poke16(buffer + bytes + 12, range.y);
    ppl7::Poke16(buffer + bytes + 14, maxTriggerCount);
    int p=16;
    for (int i=0;i < 10;i++) {
        ppl7::Poke16(buffer + bytes + p, triggerObjects[i].object_id);
        ppl7::Poke8(buffer + bytes + p + 2, static_cast<int>(triggerObjects[i].state));
        p+=3;
    }
    return bytes + p;
}

size_t Trigger::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0 || size < bytes + 1) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version < 1 || version>2) return 0;

    int flags=ppl7::Peek8(buffer + bytes + 1);
    multiTrigger=false;
    triggeredByCollision=false;
    initialStateEnabled=false;
    if (flags & 1) multiTrigger=true;
    if (flags & 2) triggeredByCollision=true;
    if (flags & 4) initialStateEnabled=true;
    if (!initialStateEnabled) enabled=false;
    if (!triggeredByCollision) collisionDetection=false;
    state=State::waiting_for_activation;
    cooldownUntilNextTrigger=ppl7::PeekFloat(buffer + bytes + 2);
    triggerDeleay=ppl7::PeekFloat(buffer + bytes + 6);
    range.x=ppl7::Peek16(buffer + bytes + 10);
    range.y=ppl7::Peek16(buffer + bytes + 12);
    maxTriggerCount=ppl7::Peek16(buffer + bytes + 14);
    int p=16;
    if (version == 1) {
        for (int i=0;i < 10;i++) {
            triggerObjects[i].object_id=ppl7::Peek16(buffer + bytes + p);
            triggerObjects[i].state=TargetState::trigger;
            p+=2;
        }
    } else {
        for (int i=0;i < 10;i++) {
            triggerObjects[i].object_id=ppl7::Peek16(buffer + bytes + p);
            triggerObjects[i].state=static_cast<TargetState>(ppl7::Peek8(buffer + bytes + p + 2));
            p+=3;
        }
    }
    return size;
}

void Trigger::reset()
{
    cooldown=0.0f;
    trigger_count=0;
    if (initialStateEnabled) enabled=true;
    else enabled=false;
    state=State::waiting_for_activation;
}

void Trigger::toggle(bool enable, Object* source)
{
    enabled=enable;
    if (!enable) state=State::disabled;
    else state=State::waiting_for_activation;
}

void Trigger::trigger(Object* source)
{
    if (!enabled) {
        enabled=true;
        state=State::waiting_for_activation;
        trigger_count=0;
        cooldown=0.0f;
        triggerDeleayTime=0.0f;
        last_collision_time=0.0f;
        return;
    }
    if (state == State::disabled) return;
    if (state == State::waiting_for_activation) {
        state=State::activated;
    }

}



class TriggerDialog : public Decker::ui::Dialog
{
private:
    ppltk::HorizontalSlider* range_x;
    ppltk::HorizontalSlider* range_y;
    ppltk::HorizontalSlider* maxTriggerCount;
    ppltk::DoubleHorizontalSlider* cooldownUntilNextTrigger;
    ppltk::DoubleHorizontalSlider* triggerDeleay;
    ppltk::CheckBox* initialStateEnabled, * currentState;
    ppltk::CheckBox* multiTrigger, * triggeredByCollision;
    ppltk::SpinBox* target_id[10];
    ppltk::RadioButton* target_state_on[10];
    ppltk::RadioButton* target_state_off[10];
    ppltk::RadioButton* target_state_trigger[10];

    Trigger* object;

public:
    TriggerDialog(Trigger* object);
    ~TriggerDialog();

    //virtual void valueChangedEvent(ppltk::Event* event, int value);
    virtual void valueChangedEvent(ppltk::Event* event, int value);
    virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
    virtual void valueChangedEvent(ppltk::Event* event, double value);
    virtual void toggledEvent(ppltk::Event* event, bool checked) override;
    virtual void dialogButtonEvent(Dialog::Buttons button) override;
};


void Trigger::openUi()
{
    TriggerDialog* dialog=new TriggerDialog(this);
    GetGameWindow()->addChild(dialog);
}

TriggerDialog::TriggerDialog(Trigger* object)
    : Decker::ui::Dialog(700, 650, Buttons::OK | Buttons::Test | Buttons::Reset)
{
    ppl7::grafix::Rect client=clientRect();
    this->object=object;
    setWindowTitle(ppl7::ToString("Trigger, Object ID: %u", object->id));
    int y=0;

    int sw=client.width() / 2;
    initialStateEnabled=new ppltk::CheckBox(0, y, sw, 30, "initial state");
    initialStateEnabled->setEventHandler(this);
    initialStateEnabled->setChecked(object->initialStateEnabled);
    addChild(initialStateEnabled);
    currentState=new ppltk::CheckBox(sw, y, sw, 30, "current state");
    currentState->setEventHandler(this);
    currentState->setChecked(object->enabled);
    addChild(currentState);
    y+=35;

    multiTrigger=new ppltk::CheckBox(0, y, sw, 30, "multiTrigger");
    multiTrigger->setEventHandler(this);
    multiTrigger->setChecked(object->multiTrigger);
    addChild(multiTrigger);
    triggeredByCollision=new ppltk::CheckBox(sw, y, sw, 30, "triggeredByCollision");
    triggeredByCollision->setChecked(object->triggeredByCollision);
    triggeredByCollision->setEventHandler(this);
    addChild(triggeredByCollision);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Collision Range x:"));
    range_x=new ppltk::HorizontalSlider(120, y, sw - 120, 30);
    range_x->setLimits(0, 1600);
    range_x->setValue(object->range.x);
    range_x->enableSpinBox(true, 1, 80);
    range_x->setEventHandler(this);
    addChild(range_x);
    addChild(new ppltk::Label(sw, y, 60, 30, "Range y:"));
    range_y=new ppltk::HorizontalSlider(sw + 60, y, sw - 60, 30);
    range_y->setLimits(0, 1600);
    range_y->setValue(object->range.y);
    range_y->enableSpinBox(true, 1, 80);
    range_y->setEventHandler(this);
    addChild(range_y);
    y+=35;


    addChild(new ppltk::Label(0, y, 120, 30, "max trigger count: "));
    maxTriggerCount=new ppltk::HorizontalSlider(120, y, sw - 120, 30);
    maxTriggerCount->setLimits(0, 1024);
    maxTriggerCount->setValue(object->maxTriggerCount);
    maxTriggerCount->enableSpinBox(true, 1, 80);
    maxTriggerCount->setEventHandler(this);
    addChild(maxTriggerCount);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Cooldown: "));
    cooldownUntilNextTrigger=new ppltk::DoubleHorizontalSlider(120, y, 300, 30);
    cooldownUntilNextTrigger->setLimits(0.0f, 120.0f);
    cooldownUntilNextTrigger->setValue(object->cooldownUntilNextTrigger);
    cooldownUntilNextTrigger->enableSpinBox(true, 0.2f, 3, 80);
    cooldownUntilNextTrigger->setEventHandler(this);
    addChild(cooldownUntilNextTrigger);
    addChild(new ppltk::Label(420, y, 120, 30, "seconds"));
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Trigger delay: "));
    triggerDeleay=new ppltk::DoubleHorizontalSlider(120, y, 300, 30);
    triggerDeleay->setLimits(0.0f, 60.0f);
    triggerDeleay->setValue(object->triggerDeleay);
    triggerDeleay->enableSpinBox(true, 0.2f, 3, 80);
    triggerDeleay->setEventHandler(this);
    addChild(triggerDeleay);
    addChild(new ppltk::Label(420, y, 120, 30, "seconds"));
    y+=35;


    addChild(new ppltk::Label(0, y, 400, 30, "Trigger objects:"));
    y+=35;
    for (int i=0;i < 10;i++) {
        int x=0;
        addChild(new ppltk::Label(x + 10, y, 80, 30, ppl7::ToString("Object %d: ", i + 1)));
        target_id[i]=new ppltk::SpinBox(x + 90, y, 100, 30, object->triggerObjects[i].object_id);
        target_id[i]->setLimits(0, 65535);
        target_id[i]->setEventHandler(this);
        addChild(target_id[i]);

        ppltk::Frame* frame=new ppltk::Frame(x + 190, y, 210, 30, ppltk::Frame::BorderStyle::NoBorder);
        frame->setTransparent(true);

        target_state_on[i]=new ppltk::RadioButton(0, 0, 50, 30, "on", object->triggerObjects[i].state == Trigger::TargetState::enable);
        target_state_on[i]->setEventHandler(this);
        frame->addChild(target_state_on[i]);
        target_state_off[i]=new ppltk::RadioButton(50, 0, 60, 30, "off", object->triggerObjects[i].state == Trigger::TargetState::disable);
        target_state_off[i]->setEventHandler(this);
        frame->addChild(target_state_off[i]);
        target_state_trigger[i]=new ppltk::RadioButton(100, 0, 90, 30, "trigger", object->triggerObjects[i].state == Trigger::TargetState::trigger);
        target_state_trigger[i]->setEventHandler(this);
        frame->addChild(target_state_trigger[i]);
        addChild(frame);

        y+=30;
    }
    //current_state_checkbox->setChecked(object->current_state);
    //initial_state_checkbox->setChecked(object->flags & static_cast<int>(Speaker::Flags::initialStateEnabled));


}

TriggerDialog::~TriggerDialog()
{

}

void TriggerDialog::toggledEvent(ppltk::Event* event, bool checked)
{
    if (event->widget() == multiTrigger) {
        object->multiTrigger=checked;
    } else if (event->widget() == triggeredByCollision) {
        object->triggeredByCollision=checked;
        object->collisionDetection=checked;
    } else if (event->widget() == initialStateEnabled) {
        object->initialStateEnabled=checked;
    } else if (event->widget() == currentState) {
        object->enabled=checked;
    }
    if (checked) {
        for (int i=0;i < 10;i++) {
            if (event->widget() == target_state_on[i]) {
                object->triggerObjects[i].state=Trigger::TargetState::enable;
            } else 	if (event->widget() == target_state_off[i]) {
                object->triggerObjects[i].state=Trigger::TargetState::disable;
            } else 	if (event->widget() == target_state_trigger[i]) {
                object->triggerObjects[i].state=Trigger::TargetState::trigger;
            }
        }
    }

}



void TriggerDialog::valueChangedEvent(ppltk::Event* event, double value)
{
    //ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (volume): >>%0.3f<<", value);
    if (event->widget() == cooldownUntilNextTrigger) {
        object->cooldownUntilNextTrigger=value;
    } else if (event->widget() == triggerDeleay) {
        object->triggerDeleay=value;
    }
}

void TriggerDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    //ppl7::PrintDebugTime("VoiceTriggerDialog::valueChangedEvent int64_t\n");
    if (event->widget() == range_x) {
        object->range.x=(int)value;
    } else if (event->widget() == range_y) {
        object->range.y=(int)value;
    } else if (event->widget() == maxTriggerCount) {
        object->maxTriggerCount=(uint16_t)value;
    } else {
        for (int i=0;i < 10;i++) {
            if (event->widget() == target_id[i]) object->triggerObjects[i].object_id=(uint16_t)value;
        }
    }
}

void TriggerDialog::valueChangedEvent(ppltk::Event* event, int value)
{
    ppl7::PrintDebugTime("VoiceTriggerDialog::valueChangedEvent int\n");
    if (event->widget() == maxTriggerCount) {

        object->maxTriggerCount=(uint16_t)value;
    }
}

void TriggerDialog::dialogButtonEvent(Dialog::Buttons button)
{
    if (button & Buttons::Reset) {
        object->reset();
    } else if (button & Buttons::Test) {
        object->test();
    }

}




} // namespace Decker::Objects
