#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation DamageTrigger::representation()
{
	return Representation(Spriteset::GenericObjects, 335);
}

DamageTrigger::DamageTrigger()
	: Object(Type::ObjectType::DamageTrigger)
{
	sprite_set=Spriteset::GenericObjects;
	collisionDetection=true;
	pixelExactCollision=false;
	sprite_no_representation=335;
	sprite_no=sprite_no_representation;
	visibleAtPlaytime=false;
	range.setPoint(TILE_WIDTH * 6, TILE_HEIGHT * 6);
	damage_per_second_or_trigger=50;
	damage_type=Player::HealthDropReason::Unknown;
	initial_state=true;
    triggered_by_collision=true;
}

DamageTrigger::~DamageTrigger()
{

}

size_t DamageTrigger::saveSize() const
{
    return Object::saveSize() + 8;

}

size_t DamageTrigger::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 1);		// Object Version
    int flags=0;
    if (initial_state) flags|=1;
    if (triggered_by_collision) flags|=2;
    ppl7::Poke8(buffer + bytes + 1, flags);
    ppl7::Poke16(buffer + bytes + 2, range.x);
    ppl7::Poke16(buffer + bytes + 4, range.y);
    ppl7::Poke8(buffer + bytes + 6, damage_per_second_or_trigger);
    ppl7::Poke8(buffer + bytes + 7, damage_type);
    return bytes + 8;
}

size_t DamageTrigger::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0 || size < bytes + 1) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version < 1 || version>1) return 0;

    int flags=ppl7::Peek8(buffer + bytes + 1);
    initial_state=(bool) (flags & 1);
    triggered_by_collision=(bool) (flags & 1);
    range.x=ppl7::Peek16(buffer + bytes + 2);
    range.y=ppl7::Peek16(buffer + bytes + 4);
    damage_per_second_or_trigger=ppl7::Peek8(buffer + bytes + 6);
    damage_type=ppl7::Peek8(buffer + bytes + 7);
    return size;
}


void DamageTrigger::update(double time, TileTypePlane&, Player&, float)
{
    boundary.setRect(p.x - range.x / 2, p.y - range.y / 2, range.x, range.y);
}

void DamageTrigger::handleCollision(Player* player, const Collision& col)
{
    if (!enabled) return;
    if (triggered_by_collision) {
        player->dropHealth(((float)damage_per_second_or_trigger)/60.0f*col.frame_rate_compensation,
            static_cast<Physic::HealthDropReason>(damage_type));
    }
}


void DamageTrigger::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
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


void DamageTrigger::trigger(Object* source)
{
    if (!enabled) return;
    if (triggered_by_collision) return;
    GetGame().getPlayer()->dropHealth((float)damage_per_second_or_trigger,
            static_cast<Physic::HealthDropReason>(damage_type));
}

void DamageTrigger::toggle(bool enable, Object* source)
{
    this->enabled=enable;
}

class DamageTriggerDialog : public Decker::ui::Dialog
{
private:
    ppltk::HorizontalSlider* range_x;
    ppltk::HorizontalSlider* range_y;
    ppltk::HorizontalSlider* damage_per_second_or_trigger;
    ppltk::CheckBox* initialStateEnabled, * currentState;
    ppltk::RadioButton* triggered_by_collision;
    ppltk::RadioButton* triggered_by_trigger;
    ppltk::ComboBox *damage_type;
    DamageTrigger* object;

public:
    DamageTriggerDialog(DamageTrigger* object);

    void valueChangedEvent(ppltk::Event* event, int value) override;
    void valueChangedEvent(ppltk::Event* event, int64_t value) override;
    void valueChangedEvent(ppltk::Event* event, double value) override;
    void toggledEvent(ppltk::Event* event, bool checked) override;
    void dialogButtonEvent(Dialog::Buttons button) override;
};


void DamageTrigger::openUi()
{
    DamageTriggerDialog* dialog=new DamageTriggerDialog(this);
    GetGameWindow()->addChild(dialog);
}

DamageTriggerDialog::DamageTriggerDialog(DamageTrigger* object)
    : Decker::ui::Dialog(700, 250, Buttons::OK | Buttons::Test)
{
    ppl7::grafix::Rect client=clientRect();
    this->object=object;
    setWindowTitle(ppl7::ToString("DamageTrigger, Object ID: %u", object->id));
    int y=0;

    int sw=client.width() / 2;
    initialStateEnabled=new ppltk::CheckBox(0, y, sw, 30, "initial state", object->initial_state);
    initialStateEnabled->setEventHandler(this);
    addChild(initialStateEnabled);
    currentState=new ppltk::CheckBox(sw, y, sw, 30, "current state", object->enabled);
    currentState->setEventHandler(this);
    addChild(currentState);
    y+=35;
    
    triggered_by_collision=new ppltk::RadioButton(0, y, sw, 30, "triggered by collision", (bool)(object->triggered_by_collision==true));
    triggered_by_collision->setEventHandler(this);
    addChild(triggered_by_collision);
    triggered_by_trigger=new ppltk::RadioButton(sw, y, sw, 30, "triggered by trigger", (bool)(object->triggered_by_collision==false));
    triggered_by_trigger->setEventHandler(this);
    addChild(triggered_by_trigger);
    y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Damage Type: "));
	damage_type=new ppltk::ComboBox(120, y, sw-120, 30);
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


    addChild(new ppltk::Label(sw, y, 120, 30, "Damage:"));
    damage_per_second_or_trigger=new ppltk::HorizontalSlider(sw+120, y, sw - 120, 30);
    damage_per_second_or_trigger->setLimits(1, 255);
    damage_per_second_or_trigger->setValue(object->damage_per_second_or_trigger);
    damage_per_second_or_trigger->enableSpinBox(true, 1, 80);
    damage_per_second_or_trigger->setEventHandler(this);
    addChild(damage_per_second_or_trigger);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Collision Range x:"));
    range_x=new ppltk::HorizontalSlider(120, y, sw - 120, 30);
    range_x->setLimits(0, 2000);
    range_x->setValue(object->range.x);
    range_x->enableSpinBox(true, 1, 80);
    range_x->setEventHandler(this);
    addChild(range_x);
    addChild(new ppltk::Label(sw, y, 60, 30, "Range y:"));
    range_y=new ppltk::HorizontalSlider(sw + 60, y, sw - 60, 30);
    range_y->setLimits(0, 2000);
    range_y->setValue(object->range.y);
    range_y->enableSpinBox(true, 1, 80);
    range_y->setEventHandler(this);
    addChild(range_y);
    y+=35;



}


void DamageTriggerDialog::valueChangedEvent(ppltk::Event* event, int value)
{
if (event->widget() == damage_type) {
		object->damage_type=damage_type->currentIdentifier().toInt();
	}
}

void DamageTriggerDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    if (event->widget() == range_x) {
        object->range.x=(int)value;
    } else if (event->widget() == range_y) {
        object->range.y=(int)value;
    } else if (event->widget() == damage_per_second_or_trigger) {
        object->damage_per_second_or_trigger=(int)value;
    }
}

void DamageTriggerDialog::valueChangedEvent(ppltk::Event* event, double value)
{

}

void DamageTriggerDialog::toggledEvent(ppltk::Event* event, bool checked)
{
    if (event->widget() == triggered_by_collision && checked==true) {
        object->triggered_by_collision=true;
        object->collisionDetection=true;
    } else if (event->widget() == initialStateEnabled) {
        object->initial_state=checked;
    } else if (event->widget() == currentState) {
        object->enabled=checked;
    }  else if (event->widget() == triggered_by_trigger && checked==true) {
        object->triggered_by_collision=false;
        object->collisionDetection=false;
    }
}

void DamageTriggerDialog::dialogButtonEvent(Dialog::Buttons button)
{
    if (button & Buttons::Test) {
        object->trigger();
    }
}


}	// EOF namespace Decker::Objects
