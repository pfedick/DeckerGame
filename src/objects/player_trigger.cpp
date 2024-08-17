#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation PlayerTrigger::representation()
{
    return Representation(Spriteset::GenericObjects, 529);
}

PlayerTrigger::PlayerTrigger()
    : Object(Type::ObjectType::PlayerTrigger)
{
    sprite_set=Spriteset::GenericObjects;
    collisionDetection=true;
    pixelExactCollision=false;
    sprite_no_representation=529;
    sprite_no=sprite_no_representation;
    visibleAtPlaytime=false;
    range.setPoint(TILE_WIDTH * 6, TILE_HEIGHT * 6);
    damage_per_second_or_trigger=0;
    damage_type=Player::HealthDropReason::Unknown;
    initial_state=true;
    triggered_by_collision=false;
    disable_player_control=false;
    enable_player_control=false;
    instant_death=false;
    takeFlashlight=false;
    takeHammer=false;
    takeCheese=false;
    takeEnergyCells=false;
    takeExtralife=false;
    takeEnergy=false;
    unlimitedTrigger=false;
    for (int i=0;i < 10;i++) {
        triggerObjects[i].object_id=0;
    }
    next_node=0;
    node_after_max_trigger=0;
    maxTriggerCount=0;
    trigger_delay=0.0f;
    action=PlayerAction::Nothing;
    state=State::waiting_for_activation;
    triggerDelayTime=0.0f;
    trigger_count=0;
    last_collision_time=0.0f;
    last_collision_frame=0;

}

PlayerTrigger::~PlayerTrigger()
{

}

void PlayerTrigger::reset()
{
    state=State::waiting_for_activation;
    triggerDelayTime=0.0f;
    trigger_count=0;
    last_collision_time=0.0f;
    last_collision_frame=0;
    if (initial_state) enabled=true;
    else enabled=false;
}

size_t PlayerTrigger::saveSize() const
{
    return Object::saveSize() + 54;

}

size_t PlayerTrigger::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 3);		// Object Version
    int flags=0;
    if (initial_state) flags|=1;
    if (triggered_by_collision) flags|=2;
    if (disable_player_control) flags|=4;
    if (enable_player_control) flags|=8;
    if (instant_death) flags|=16;
    if (unlimitedTrigger) flags|=32;

    ppl7::Poke8(buffer + bytes + 1, flags);
    ppl7::Poke16(buffer + bytes + 2, range.x);
    ppl7::Poke16(buffer + bytes + 4, range.y);
    ppl7::Poke8(buffer + bytes + 6, damage_per_second_or_trigger);
    ppl7::Poke8(buffer + bytes + 7, damage_type);
    int take_items=0;
    if (takeFlashlight) take_items|=1;
    if (takeHammer) take_items|=2;
    if (takeCheese) take_items|=4;
    if (takeEnergyCells) take_items|=8;
    if (takeExtralife) take_items|=16;
    if (takeEnergy) take_items|=32;
    ppl7::Poke16(buffer + bytes + 8, take_items);
    ppl7::Poke32(buffer + bytes + 10, next_node);
    ppl7::Poke32(buffer + bytes + 14, node_after_max_trigger);
    ppl7::PokeFloat(buffer + bytes + 18, trigger_delay);
    ppl7::Poke8(buffer + bytes + 22, static_cast<int>(action));
    ppl7::Poke8(buffer + bytes + 23, maxTriggerCount);
    int p=24;
    for (int i=0;i < 10;i++) {
        ppl7::Poke16(buffer + bytes + p, triggerObjects[i].object_id);
        ppl7::Poke8(buffer + bytes + p + 2, static_cast<int>(triggerObjects[i].state));
        p+=3;
    }

    return bytes + p;
}

size_t PlayerTrigger::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0 || size < bytes + 1) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version < 1 || version>3) return 0;

    int flags=ppl7::Peek8(buffer + bytes + 1);
    initial_state=(bool)(flags & 1);
    enabled=initial_state;
    triggered_by_collision=(bool)(flags & 2);
    disable_player_control=(bool)(flags & 4);
    enable_player_control=(bool)(flags & 8);
    instant_death=(bool)(flags & 16);

    range.x=ppl7::Peek16(buffer + bytes + 2);
    range.y=ppl7::Peek16(buffer + bytes + 4);
    damage_per_second_or_trigger=ppl7::Peek8(buffer + bytes + 6);
    damage_type=ppl7::Peek8(buffer + bytes + 7);
    int take_items=0;
    if (version >= 2) take_items=ppl7::Peek16(buffer + bytes + 8);

    takeFlashlight=take_items & 1;
    takeHammer=take_items & 2;
    takeCheese=take_items & 4;
    takeEnergyCells=take_items & 8;
    takeExtralife=take_items & 16;
    takeEnergy=take_items & 32;

    if (version >= 3) {
        unlimitedTrigger=(bool)(flags & 32);

        next_node=ppl7::Peek32(buffer + bytes + 10);
        node_after_max_trigger=ppl7::Peek32(buffer + bytes + 14);
        trigger_delay=ppl7::PeekFloat(buffer + bytes + 18);
        action=static_cast<PlayerAction>(ppl7::Peek8(buffer + bytes + 22));
        maxTriggerCount=ppl7::Peek8(buffer + bytes + 23);
        int p=24;
        for (int i=0;i < 10;i++) {
            triggerObjects[i].object_id=ppl7::Peek16(buffer + bytes + p);
            triggerObjects[i].state=static_cast<TriggerTarget::State>(ppl7::Peek8(buffer + bytes + p + 2));
            p+=3;
        }
    }

    return size;
}

void PlayerTrigger::notifyTargets() const
{
    ObjectSystem* objs=GetObjectSystem();
    for (int i=0;i < 10;i++) {
        if (triggerObjects[i].object_id > 0) {
            Object* target=objs->getObject(triggerObjects[i].object_id);
            if (target) {
                if (triggerObjects[i].state == TriggerTarget::State::trigger) target->trigger();
                else if (triggerObjects[i].state == TriggerTarget::State::enable) target->toggle(true);
                else if (triggerObjects[i].state == TriggerTarget::State::disable) target->toggle(false);
            }
        }
    }
}


void PlayerTrigger::update(double time, TileTypePlane&, Player& player, float)
{
    boundary.setRect(p.x - range.x / 2, p.y - range.y / 2, range.x, range.y);
    if (state == State::activated && (trigger_count < maxTriggerCount || unlimitedTrigger == true)) {
        trigger_count++;
        //ppl7::PrintDebugTime("GlimmerNode %d update, activated, count is: %d\n", id, trigger_count);

        if (trigger_delay > 0.0f) {
            triggerDelayTime=time + trigger_delay;
            state=State::waiting_for_trigger_delay;
        } else {
            state=State::finished;
            notifyTargets();
        }
        switch (action) {
            case PlayerAction::WalkToNode:
            case PlayerAction::WaynetToNode:
                if (next_node > 0) {
                    ObjectSystem* objs=GetObjectSystem();
                    Object* target=objs->getObject(next_node);
                    if (target) {
                        player.disableControl();
                        player.walkToNode(target->p, (bool)(action == PlayerAction::WaynetToNode));
                    }
                }
                break;
        }

    } else if (state == State::finished) {
        state=State::waiting_for_activation;
    } else if (state == State::activated && trigger_count >= maxTriggerCount) {
        state=State::disabled;
        if (node_after_max_trigger > 0) {
            ObjectSystem* objs=GetObjectSystem();
            Object* target=objs->getObject(node_after_max_trigger);
            if (target->type() == Decker::Objects::Type::PlayerTrigger) {
                Decker::Objects::PlayerTrigger* glimmernode=static_cast<Decker::Objects::PlayerTrigger*>(target);
                //ppl7::PrintDebugTime("trigger it %d => %d\n", id, glimmernode->id);
                glimmernode->trigger();
            }
        }
    } else if (state == State::waiting_for_trigger_delay && time >= triggerDelayTime) {
        state=State::finished;
        notifyTargets();
    }
}

void PlayerTrigger::handleCollision(Player* player, const Collision& col)
{
    if (!enabled) return;
    if (!triggered_by_collision) return;

    if (damage_per_second_or_trigger != 0) {
        player->dropHealth(((float)damage_per_second_or_trigger) / 60.0f * col.frame_rate_compensation,
            static_cast<Physic::HealthDropReason>(damage_type));
    }
    triggerFlags(player);
    uint64_t frame_no=GetFrameNo();
    if (frame_no < last_collision_frame + 5) {
        last_collision_frame=frame_no;
        return;
    }
    last_collision_frame=frame_no;
    last_collision_time=player->time;
    if (state == State::waiting_for_activation) {
        state=State::activated;
    }
}


void PlayerTrigger::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
    if (triggered_by_collision) {
        SDL_Rect r;
        r.x=p.x + coords.x - range.x / 2;
        r.y=p.y + coords.y - range.y / 2;
        r.w=range.x;
        r.h=range.y;

        SDL_BlendMode currentBlendMode;
        SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 192, 192, 0, 96);
        SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawColor(renderer, 192, 192, 0, 255);
        SDL_RenderDrawRect(renderer, &r);
        SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
    }
    Object::drawEditMode(renderer, coords);
}

void PlayerTrigger::triggerFlags(Player* player)
{
    if (instant_death) player->dropHealth(1000.0f, static_cast<Physic::HealthDropReason>(damage_type));

    if (disable_player_control) player->disableControl();
    if (enable_player_control) player->enableControl();

    if (takeFlashlight) player->takeAllItems(Objects::Type::Flashlight);
    if (takeHammer) player->takeAllItems(Objects::Type::Hammer);
    if (takeCheese) player->takeAllItems(Objects::Type::Cheese);
    if (takeEnergyCells) player->takeAllItems(Objects::Type::PowerCell);
    if (takeExtralife) player->takeAllItems(Objects::Type::ExtraLife);
    if (takeEnergy) player->drainBatteryCompletely();
}

void PlayerTrigger::trigger(Object* source)
{
    if (!enabled) return;
    //if (triggered_by_collision) return;
    Player* player=GetGame().getPlayer();
    if (damage_per_second_or_trigger != 0) GetGame().getPlayer()->dropHealth((float)damage_per_second_or_trigger,
        static_cast<Physic::HealthDropReason>(damage_type));
    state=State::activated;
    triggerFlags(player);
    //notifyTargets();


}

void PlayerTrigger::toggle(bool enable, Object* source)
{
    this->enabled=enable;
    //ppl7::PrintDebug("PlayerTrigger::toggle: %d [%d]\n", (int)enable, id);
}

class PlayerTriggerDialog : public Decker::ui::Dialog
{
private:
    ppltk::HorizontalSlider* range_x;
    ppltk::HorizontalSlider* range_y;
    ppltk::HorizontalSlider* damage_per_second_or_trigger;
    ppltk::CheckBox* initialStateEnabled, * currentState, * unlimitedTrigger;
    ppltk::CheckBox* disable_player_control, * enable_player_control, * instant_death;
    ppltk::RadioButton* triggered_by_collision;
    ppltk::RadioButton* triggered_by_trigger;
    ppltk::ComboBox* damage_type;
    ppltk::CheckBox* takeFlashlight, * takeHammer;
    ppltk::CheckBox* takeCheese, * takeEnergyCells, * takeExtralife;
    ppltk::CheckBox* takeEnergy;
    ppltk::SpinBox* target_id[10];
    ppltk::RadioButton* target_state_on[10];
    ppltk::RadioButton* target_state_off[10];
    ppltk::RadioButton* target_state_trigger[10];
    ppltk::SpinBox* next_node, * node_after_max_trigger;
    ppltk::SpinBox* maxTriggerCount;
    ppltk::ComboBox* action;
    ppltk::DoubleHorizontalSlider* trigger_delay;



    PlayerTrigger* object;

public:
    PlayerTriggerDialog(PlayerTrigger* object);

    void valueChangedEvent(ppltk::Event* event, int value) override;
    void valueChangedEvent(ppltk::Event* event, int64_t value) override;
    void valueChangedEvent(ppltk::Event* event, double value) override;
    void toggledEvent(ppltk::Event* event, bool checked) override;
    void dialogButtonEvent(Dialog::Buttons button) override;
};


void PlayerTrigger::openUi()
{
    PlayerTriggerDialog* dialog=new PlayerTriggerDialog(this);
    GetGameWindow()->addChild(dialog);
}

PlayerTriggerDialog::PlayerTriggerDialog(PlayerTrigger* object)
    : Decker::ui::Dialog(700, 700, Buttons::OK | Buttons::Test | Buttons::Reset)
{
    ppl7::grafix::Rect client=clientRect();
    this->object=object;
    setWindowTitle(ppl7::ToString("PlayerTrigger, Object ID: %u", object->id));
    int y=0;

    int sw=client.width() / 2;
    initialStateEnabled=new ppltk::CheckBox(0, y, sw, 30, "initial state", object->initial_state);
    initialStateEnabled->setEventHandler(this);
    addChild(initialStateEnabled);
    currentState=new ppltk::CheckBox(sw, y, sw, 30, "current state", object->enabled);
    currentState->setEventHandler(this);
    addChild(currentState);
    y+=30;

    triggered_by_collision=new ppltk::RadioButton(0, y, sw, 30, "triggered by collision", (bool)(object->triggered_by_collision == true));
    triggered_by_collision->setEventHandler(this);
    addChild(triggered_by_collision);
    triggered_by_trigger=new ppltk::RadioButton(sw, y, sw, 30, "triggered by trigger", (bool)(object->triggered_by_collision == false));
    triggered_by_trigger->setEventHandler(this);
    addChild(triggered_by_trigger);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Collision Range x:"));
    range_x=new ppltk::HorizontalSlider(120, y, sw - 120, 30);
    range_x->setLimits(0, 2000);
    range_x->setValue(object->range.x);
    range_x->enableSpinBox(true, 1, 80);
    range_x->setEventHandler(this);
    addChild(range_x);
    addChild(new ppltk::Label(sw, y, 70, 30, "Range y:"));
    range_y=new ppltk::HorizontalSlider(sw + 80, y, sw - 80, 30);
    range_y->setLimits(0, 2000);
    range_y->setValue(object->range.y);
    range_y->enableSpinBox(true, 1, 80);
    range_y->setEventHandler(this);
    addChild(range_y);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "cause damage:"));
    damage_type=new ppltk::ComboBox(120, y, sw - 120, 30);
    damage_type->add("Misc", ppl7::ToString("%d", Physic::HealthDropReason::Unknown));
    damage_type->add("Falling Deep", ppl7::ToString("%d", Physic::HealthDropReason::FallingDeep));
    damage_type->add("Smashed", ppl7::ToString("%d", Physic::HealthDropReason::Smashed));
    damage_type->add("Drowned", ppl7::ToString("%d", Physic::HealthDropReason::Drowned));
    damage_type->add("Burned", ppl7::ToString("%d", Physic::HealthDropReason::Burned));
    damage_type->add("Etched", ppl7::ToString("%d", Physic::HealthDropReason::Etched));
    damage_type->add("Smashed Sideways", ppl7::ToString("%d", Physic::HealthDropReason::SmashedSideways));
    damage_type->sortItems();
    damage_type->setCurrentIdentifier(ppl7::ToString("%d", object->damage_type));
    damage_type->setEventHandler(this);
    addChild(damage_type);


    addChild(new ppltk::Label(sw, y, 80, 30, "Damage:"));
    damage_per_second_or_trigger=new ppltk::HorizontalSlider(sw + 80, y, sw - 80, 30);
    damage_per_second_or_trigger->setLimits(-100, +100);
    damage_per_second_or_trigger->setValue(object->damage_per_second_or_trigger);
    damage_per_second_or_trigger->enableSpinBox(true, 1, 80);
    damage_per_second_or_trigger->setEventHandler(this);
    addChild(damage_per_second_or_trigger);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Action:"));
    action=new ppltk::ComboBox(120, y, sw - 120, 30);
    action->add("Nothing", ppl7::ToString("%d", static_cast<int>(PlayerTrigger::PlayerAction::Nothing)));
    action->add("WalkToNode", ppl7::ToString("%d", static_cast<int>(PlayerTrigger::PlayerAction::WalkToNode)));
    //action->add("Wait", ppl7::ToString("%d", static_cast<int>(PlayerTrigger::PlayerAction::Wait)));
    action->add("WaynetToNode", ppl7::ToString("%d", static_cast<int>(PlayerTrigger::PlayerAction::WaynetToNode)));
    action->sortItems();
    action->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->action)));
    action->setEventHandler(this);
    addChild(action);

    addChild(new ppltk::Label(sw, y, 80, 30, "Next Node:"));
    next_node=new ppltk::SpinBox(sw + 80, y, 100, 30, object->next_node);
    next_node->setLimits(0, 65535);
    next_node->setEventHandler(this);
    addChild(next_node);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Max Trigger:"));
    maxTriggerCount=new ppltk::SpinBox(120, y, 100, 30, object->maxTriggerCount);
    maxTriggerCount->setLimits(1, 255);
    maxTriggerCount->setEventHandler(this);
    addChild(maxTriggerCount);
    addChild(new ppltk::Label(sw, y, 165, 30, "Node after max trigger:"));
    node_after_max_trigger=new ppltk::SpinBox(sw + 165, y, 100, 30, object->node_after_max_trigger);
    node_after_max_trigger->setLimits(0, 65535);
    node_after_max_trigger->setEventHandler(this);
    addChild(node_after_max_trigger);
    y+=30;



    y+=10;
    int ystart=y;
    takeFlashlight=new ppltk::CheckBox(0, y, sw, 30, "remove Flashlight", object->takeFlashlight);
    takeFlashlight->setEventHandler(this);
    addChild(takeFlashlight);
    y+=30;

    takeHammer=new ppltk::CheckBox(0, y, sw, 30, "remove Hammer", object->takeHammer);
    takeHammer->setEventHandler(this);
    addChild(takeHammer);
    y+=30;

    takeCheese=new ppltk::CheckBox(0, y, sw, 30, "remove Cheese", object->takeCheese);
    takeCheese->setEventHandler(this);
    addChild(takeCheese);
    y+=30;

    takeEnergyCells=new ppltk::CheckBox(0, y, sw, 30, "remove Power Cells", object->takeEnergyCells);
    takeEnergyCells->setEventHandler(this);
    addChild(takeEnergyCells);
    y+=30;

    takeExtralife=new ppltk::CheckBox(0, y, sw, 30, "remove Extra Life", object->takeExtralife);
    takeExtralife->setEventHandler(this);
    addChild(takeExtralife);
    y+=30;

    takeEnergy=new ppltk::CheckBox(0, y, sw, 30, "remove Energy", object->takeEnergy);
    takeEnergy->setEventHandler(this);
    addChild(takeEnergy);
    y+=30;
    int max_y=y + 5;
    y=ystart;
    instant_death=new ppltk::CheckBox(sw, y, sw, 30, "instant death", object->instant_death);
    instant_death->setEventHandler(this);
    addChild(instant_death);
    y+=30;

    disable_player_control=new ppltk::CheckBox(sw, y, sw, 30, "disable player control", object->disable_player_control);
    disable_player_control->setEventHandler(this);
    addChild(disable_player_control);
    y+=30;

    enable_player_control=new ppltk::CheckBox(sw, y, sw, 30, "enable player control", object->enable_player_control);
    enable_player_control->setEventHandler(this);
    addChild(enable_player_control);
    y+=30;

    unlimitedTrigger=new ppltk::CheckBox(sw, y, sw, 30, "unlimited trigger", (bool)(object->unlimitedTrigger == true));
    unlimitedTrigger->setEventHandler(this);
    addChild(unlimitedTrigger);
    y+=35;

    if (y > max_y) max_y=y;


    y=max_y;




    addChild(new ppltk::Label(0, y, 120, 30, "Trigger Delay:"));
    trigger_delay=new ppltk::DoubleHorizontalSlider(120, y, 300, 30);
    trigger_delay->setLimits(0.0f, 60.0f);
    trigger_delay->setValue(object->trigger_delay);
    trigger_delay->enableSpinBox(true, 0.2f, 3, 80);
    trigger_delay->setEventHandler(this);
    addChild(trigger_delay);
    addChild(new ppltk::Label(420, y, 120, 30, "seconds"));
    y+=30;
    addChild(new ppltk::Label(0, y, 400, 30, "Trigger objects:"));
    y+=30;
    int x=0;
    int y1=y;
    for (int i=0;i < 10;i++) {
        if (i == 5) {
            x=340;
            y1=y;
        }
        addChild(new ppltk::Label(x, y1, 80, 30, ppl7::ToString("Object %d: ", i + 1)));
        target_id[i]=new ppltk::SpinBox(x + 80, y1, 80, 28, object->triggerObjects[i].object_id);
        target_id[i]->setLimits(0, 65535);
        target_id[i]->setEventHandler(this);
        addChild(target_id[i]);

        ppltk::Frame* frame=new ppltk::Frame(x + 160, y1, 180, 30, ppltk::Frame::BorderStyle::NoBorder);
        frame->setTransparent(true);

        target_state_on[i]=new ppltk::RadioButton(0, 0, 50, 30, "on", object->triggerObjects[i].state == TriggerTarget::State::enable);
        target_state_on[i]->setEventHandler(this);
        frame->addChild(target_state_on[i]);
        target_state_off[i]=new ppltk::RadioButton(50, 0, 50, 30, "off", object->triggerObjects[i].state == TriggerTarget::State::disable);
        target_state_off[i]->setEventHandler(this);
        frame->addChild(target_state_off[i]);
        target_state_trigger[i]=new ppltk::RadioButton(100, 0, 70, 30, "trigger", object->triggerObjects[i].state == TriggerTarget::State::trigger);
        target_state_trigger[i]->setEventHandler(this);
        frame->addChild(target_state_trigger[i]);
        addChild(frame);

        y1+=30;
    }

}


void PlayerTriggerDialog::valueChangedEvent(ppltk::Event* event, int value)
{
    if (event->widget() == damage_type) {
        object->damage_type=damage_type->currentIdentifier().toInt();
    } else if (event->widget() == action) {
        object->action=static_cast<PlayerTrigger::PlayerAction>(action->currentIdentifier().toInt());
    }

}

void PlayerTriggerDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    if (event->widget() == range_x) {
        object->range.x=(int)value;
    } else if (event->widget() == range_y) {
        object->range.y=(int)value;
    } else if (event->widget() == damage_per_second_or_trigger) {
        object->damage_per_second_or_trigger=(int)value;
    } else if (event->widget() == next_node) {
        object->next_node=(uint32_t)value;
    } else if (event->widget() == maxTriggerCount) {
        object->maxTriggerCount=(uint8_t)value;
    } else if (event->widget() == node_after_max_trigger) {
        object->node_after_max_trigger=(uint32_t)value;
    } else {
        for (int i=0;i < 10;i++) {
            if (event->widget() == target_id[i]) object->triggerObjects[i].object_id=(uint16_t)value;
        }
    }

}

void PlayerTriggerDialog::valueChangedEvent(ppltk::Event* event, double value)
{
    if (event->widget() == trigger_delay) {
        object->trigger_delay=value;
    }
}

void PlayerTriggerDialog::toggledEvent(ppltk::Event* event, bool checked)
{
    if (event->widget() == triggered_by_collision && checked == true) {
        object->triggered_by_collision=true;
        object->collisionDetection=true;
    } else if (event->widget() == initialStateEnabled) {
        object->initial_state=checked;
    } else if (event->widget() == currentState) {
        object->enabled=checked;
    } else if (event->widget() == unlimitedTrigger) {
        object->unlimitedTrigger=checked;
    } else if (event->widget() == triggered_by_trigger && checked == true) {
        object->triggered_by_collision=false;
        object->collisionDetection=false;
    } else  if (event->widget() == takeFlashlight) {
        object->takeFlashlight=checked;
    } else if (event->widget() == takeHammer) {
        object->takeHammer=checked;
    } else if (event->widget() == takeCheese) {
        object->takeCheese=checked;
    } else if (event->widget() == takeEnergyCells) {
        object->takeEnergyCells=checked;
    } else if (event->widget() == takeExtralife) {
        object->takeExtralife=checked;
    } else if (event->widget() == takeEnergy) {
        object->takeEnergy=checked;
    } else if (event->widget() == disable_player_control) {
        object->disable_player_control=checked;
    } else if (event->widget() == enable_player_control) {
        object->enable_player_control=checked;
    } else if (event->widget() == instant_death) {
        object->instant_death=checked;
    }
    if (checked) {
        for (int i=0;i < 10;i++) {
            if (event->widget() == target_state_on[i]) {
                object->triggerObjects[i].state=TriggerTarget::State::enable;
            } else 	if (event->widget() == target_state_off[i]) {
                object->triggerObjects[i].state=TriggerTarget::State::disable;
            } else 	if (event->widget() == target_state_trigger[i]) {
                object->triggerObjects[i].state=TriggerTarget::State::trigger;
            }
        }
    }
}

void PlayerTriggerDialog::dialogButtonEvent(Dialog::Buttons button)
{
    if (button & Buttons::Test) {
        object->trigger();
    } else if (button & Buttons::Reset) {
        object->reset();
    }
}


}	// EOF namespace Decker::Objects
