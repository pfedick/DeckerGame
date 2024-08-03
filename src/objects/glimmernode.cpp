#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "glimmer.h"

namespace Decker::Objects {

Representation GlimmerNode::representation()
{
	return Representation(Spriteset::GenericObjects, 3);
}

GlimmerNode::GlimmerNode()
	:Object(Type::ObjectType::GlimmerNode)
{
	glimmer=GetGame().getGlimmer();
	sprite_set=Spriteset::GenericObjects;
	sprite_no=3;
	sprite_no_representation=3;
	collisionDetection=true;
	pixelExactCollision=false;
	initialStateEnabled=true;
	visibleAtPlaytime=false;
	enabled=initialStateEnabled;
	triggeredByPlayerCollision=false;
	triggeredByGlimmerCollision=false;
	state=State::waiting_for_activation;
	cooldown=0.0f;
	triggerDeleayTime=0.0f;
	trigger_count=0;
	last_collision_time=0.0f;
	last_collision_frame=0;
	action=GlimmerAction::Wait;
	next_node=0;
	maxSpeed=15.0f;
	range.setPoint(TILE_WIDTH * 2, TILE_WIDTH * 2);
	for (int i=0;i < 10;i++) {
		triggerObjects[i].object_id=0;
	}
}

GlimmerNode::~GlimmerNode()
{

}

size_t GlimmerNode::saveSize() const
{
	return Object::saveSize() + 15 + 10 * 3;;
}


size_t GlimmerNode::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	int flags=0;
	if (initialStateEnabled) flags|=1;
	if (triggeredByPlayerCollision) flags|=2;
	if (triggeredByGlimmerCollision) flags|=4;
	ppl7::Poke8(buffer + bytes + 1, flags);
	ppl7::Poke16(buffer + bytes + 2, range.x);
	ppl7::Poke16(buffer + bytes + 4, range.y);
	ppl7::Poke8(buffer + bytes + 6, static_cast<int>(action));
	ppl7::Poke32(buffer + bytes + 7, next_node);
	ppl7::PokeFloat(buffer + bytes + 11, maxSpeed);

	int p=15;
	for (int i=0;i < 10;i++) {
		ppl7::Poke16(buffer + bytes + p, triggerObjects[i].object_id);
		ppl7::Poke8(buffer + bytes + p + 2, static_cast<int>(triggerObjects[i].state));
		p+=3;
	}
	return bytes + p;
}

size_t GlimmerNode::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1 || size < bytes + 16) return 0;
	int flags=ppl7::Peek8(buffer + bytes + 1);
	initialStateEnabled=(bool)(flags & 1);
	triggeredByPlayerCollision=(bool)(flags & 2);
	triggeredByGlimmerCollision=(bool)(flags & 4);
	enabled=initialStateEnabled;
	range.x=ppl7::Peek16(buffer + bytes + 2);
	range.y=ppl7::Peek16(buffer + bytes + 4);
	state=State::waiting_for_activation;
	action=static_cast<GlimmerAction>(ppl7::Peek8(buffer + bytes + 6));
	next_node=ppl7::Peek32(buffer + bytes + 7);
	maxSpeed=ppl7::PeekFloat(buffer + bytes + 11);
	int p=15;
	for (int i=0;i < 10;i++) {
		triggerObjects[i].object_id=ppl7::Peek16(buffer + bytes + p);
		triggerObjects[i].state=static_cast<TargetState>(ppl7::Peek8(buffer + bytes + p + 2));
		p+=3;
	}
	return size;

}

void GlimmerNode::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	if (triggeredByPlayerCollision || triggeredByGlimmerCollision) {
		SDL_Rect r;
		r.x=p.x + coords.x - range.x / 2;
		r.y=p.y + coords.y - range.y / 2;
		r.w=range.x;
		r.h=range.y;

		SDL_BlendMode currentBlendMode;
		SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		if (triggeredByGlimmerCollision)
			SDL_SetRenderDrawColor(renderer, 192, 192, 255, 96);
		else
			SDL_SetRenderDrawColor(renderer, 0, 192, 0, 96);
		SDL_RenderFillRect(renderer, &r);
		if (triggeredByGlimmerCollision)
			SDL_SetRenderDrawColor(renderer, 192, 192, 255, 255);
		else
			SDL_SetRenderDrawColor(renderer, 0, 192, 0, 255);
		SDL_RenderDrawRect(renderer, &r);
		SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
	}
	Object::drawEditMode(renderer, coords);
}

void GlimmerNode::notifyTargets() const
{
	ObjectSystem* objs=GetObjectSystem();
	for (int i=0;i < 10;i++) {
		if (triggerObjects[i].object_id > 0) {
			Object* target=objs->getObject(triggerObjects[i].object_id);
			if (target) {
				if (triggerObjects[i].state == GlimmerNode::TargetState::trigger) target->trigger();
				else if (triggerObjects[i].state == GlimmerNode::TargetState::enable) target->toggle(true);
				else if (triggerObjects[i].state == GlimmerNode::TargetState::disable) target->toggle(false);
			}
		}
	}
}

void GlimmerNode::toggle(bool enable, Object* source)
{
	if (triggeredByGlimmerCollision || triggeredByPlayerCollision) {
		enabled=enable;
	} else if (enable && state == State::waiting_for_activation) {
		state=State::activated;
		enabled=true;
	}
}

void GlimmerNode::trigger(Object* source)
{
	toggle(!enabled, source);
}

void GlimmerNode::reset()
{
	cooldown=0.0f;
	trigger_count=0;
	if (initialStateEnabled) enabled=true;
	else enabled=false;
	state=State::waiting_for_activation;
}


void GlimmerNode::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	boundary.setRect(p.x - range.x / 2, p.y - range.y / 2, range.x, range.y);
	if (state == State::activated) {
		state=State::finished;
		notifyTargets();
		switch (action) {
			case GlimmerAction::Appear:
				glimmer->setPosition(p);
				glimmer->setNextNode(next_node);
				glimmer->appear();
				break;
			case GlimmerAction::Awaken:
				glimmer->setPosition(p);
				glimmer->setNextNode(next_node);
				glimmer->awaken();
				break;
			case GlimmerAction::Disappear:
				glimmer->setNextNode(next_node);
				glimmer->disappear();
				break;
			case GlimmerAction::FollowPlayer:
				glimmer->followPlayer();
				break;
			case GlimmerAction::FlyToAndStop:
			case GlimmerAction::FlyTo:
				if (next_node > 0) {
					ObjectSystem* objs=GetObjectSystem();
					Object* target=objs->getObject(next_node);
					if (target) {
						glimmer->flyTo(target->p, maxSpeed, (bool)(action == GlimmerAction::FlyToAndStop));
					}
				}
				break;
			case GlimmerAction::Wait:
				glimmer->wait(p);
				break;
		}
	}
}

void GlimmerNode::handleCollision(Player* player, const Collision& collision)
{
	if (!triggeredByPlayerCollision) return;
	uint64_t frame_no=GetFrameNo();
	if (frame_no == last_collision_frame + 1) {
		last_collision_frame=frame_no;
		return;
	}
	last_collision_frame=frame_no;
	last_collision_time=player->time;
	if (state == State::waiting_for_activation) {
		state=State::activated;
	}
}

void GlimmerNode::test()
{
	state=State::activated;
	//enabled=false;
}


void GlimmerNode::handleCollisionByGlimmer()
{
	if (!triggeredByGlimmerCollision) return;
	//ppl7::PrintDebug("GlimmerNode::handleCollisionByGlimmer\n");
	uint64_t frame_no=GetFrameNo();
	if (frame_no == last_collision_frame + 1) {
		last_collision_frame=frame_no;
		return;
	}
	last_collision_frame=frame_no;
	last_collision_time=ppl7::GetMicrotime();
	if (state == State::waiting_for_activation) {
		state=State::activated;
	}

}

class GlimmerNodeDialog : public Decker::ui::Dialog
{
private:
	ppltk::HorizontalSlider* range_x;
	ppltk::HorizontalSlider* range_y;
	//ppltk::HorizontalSlider* maxTriggerCount;
	ppltk::DoubleHorizontalSlider* maxSpeed;
	ppltk::SpinBox* next_node;
	ppltk::ComboBox* action;
	ppltk::CheckBox* initialStateEnabled, * currentState;
	ppltk::CheckBox* triggeredByPlayer, * triggeredByGlimmer;
	ppltk::SpinBox* target_id[10];
	ppltk::RadioButton* target_state_on[10];
	ppltk::RadioButton* target_state_off[10];
	ppltk::RadioButton* target_state_trigger[10];

	GlimmerNode* object;

public:
	GlimmerNodeDialog(GlimmerNode* object);
	~GlimmerNodeDialog();

	//virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
	virtual void valueChangedEvent(ppltk::Event* event, double value);
	virtual void toggledEvent(ppltk::Event* event, bool checked) override;
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
};


void GlimmerNode::openUi()
{
	GlimmerNodeDialog* dialog=new GlimmerNodeDialog(this);
	GetGameWindow()->addChild(dialog);
}

GlimmerNodeDialog::GlimmerNodeDialog(GlimmerNode* object)
	: Decker::ui::Dialog(700, 650, Buttons::OK | Buttons::Test | Buttons::Reset)
{
	ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle(ppl7::ToString("GlimmerNode, Object ID: %u", object->id));
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

	triggeredByPlayer=new ppltk::CheckBox(0, y, sw, 30, "Triggered by Player");
	triggeredByPlayer->setEventHandler(this);
	triggeredByPlayer->setChecked(object->triggeredByPlayerCollision);
	addChild(triggeredByPlayer);
	triggeredByGlimmer=new ppltk::CheckBox(sw, y, sw, 30, "Triggered by Glimmer");
	triggeredByGlimmer->setChecked(object->triggeredByGlimmerCollision);
	triggeredByGlimmer->setEventHandler(this);
	addChild(triggeredByGlimmer);
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

	addChild(new ppltk::Label(0, y, 120, 30, "Action:"));
	action=new ppltk::ComboBox(120, y, 200, 30);
	action->add("Awaken", ppl7::ToString("%d", static_cast<int>(GlimmerNode::GlimmerAction::Awaken)));
	action->add("Appear", ppl7::ToString("%d", static_cast<int>(GlimmerNode::GlimmerAction::Appear)));
	action->add("Disappear", ppl7::ToString("%d", static_cast<int>(GlimmerNode::GlimmerAction::Disappear)));
	action->add("FollowPlayer", ppl7::ToString("%d", static_cast<int>(GlimmerNode::GlimmerAction::FollowPlayer)));
	action->add("FlyTo", ppl7::ToString("%d", static_cast<int>(GlimmerNode::GlimmerAction::FlyTo)));
	action->add("FlyToAndStop", ppl7::ToString("%d", static_cast<int>(GlimmerNode::GlimmerAction::FlyToAndStop)));
	action->add("Wait", ppl7::ToString("%d", static_cast<int>(GlimmerNode::GlimmerAction::Wait)));
	action->sortItems();
	action->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->action)));
	action->setEventHandler(this);
	addChild(action);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Next Node:"));
	next_node=new ppltk::SpinBox(120, y, 100, 30, object->next_node);
	next_node->setLimits(0, 65535);
	next_node->setEventHandler(this);
	addChild(next_node);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Max speed:"));
	maxSpeed=new ppltk::DoubleHorizontalSlider(120, y, 300, 30);
	maxSpeed->setLimits(0.0f, 15.0f);
	maxSpeed->setValue(object->maxSpeed);
	maxSpeed->enableSpinBox(true, 0.2f, 3, 80);
	maxSpeed->setEventHandler(this);
	addChild(maxSpeed);
	addChild(new ppltk::Label(420, y, 120, 30, "Pixel/Frame"));
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

		target_state_on[i]=new ppltk::RadioButton(0, 0, 50, 30, "on", object->triggerObjects[i].state == GlimmerNode::TargetState::enable);
		target_state_on[i]->setEventHandler(this);
		frame->addChild(target_state_on[i]);
		target_state_off[i]=new ppltk::RadioButton(50, 0, 60, 30, "off", object->triggerObjects[i].state == GlimmerNode::TargetState::disable);
		target_state_off[i]->setEventHandler(this);
		frame->addChild(target_state_off[i]);
		target_state_trigger[i]=new ppltk::RadioButton(100, 0, 90, 30, "trigger", object->triggerObjects[i].state == GlimmerNode::TargetState::trigger);
		target_state_trigger[i]->setEventHandler(this);
		frame->addChild(target_state_trigger[i]);
		addChild(frame);

		y+=30;
	}

}


GlimmerNodeDialog::~GlimmerNodeDialog()
{

}

void GlimmerNodeDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (event->widget() == triggeredByPlayer) {
		object->triggeredByPlayerCollision=checked;
		if (checked) {
			object->triggeredByGlimmerCollision=false;
			triggeredByGlimmer->setChecked(false);
		}
		object->collisionDetection=checked;
	} else if (event->widget() == triggeredByGlimmer) {
		object->triggeredByGlimmerCollision=checked;
		if (checked) {
			object->triggeredByPlayerCollision=false;
			triggeredByPlayer->setChecked(false);
		}
		object->collisionDetection=checked;
	} else if (event->widget() == initialStateEnabled) {
		object->initialStateEnabled=checked;
	} else if (event->widget() == currentState) {
		object->enabled=checked;
	}
	if (checked) {
		for (int i=0;i < 10;i++) {
			if (event->widget() == target_state_on[i]) {
				object->triggerObjects[i].state=GlimmerNode::TargetState::enable;
			} else 	if (event->widget() == target_state_off[i]) {
				object->triggerObjects[i].state=GlimmerNode::TargetState::disable;
			} else 	if (event->widget() == target_state_trigger[i]) {
				object->triggerObjects[i].state=GlimmerNode::TargetState::trigger;
			}
		}
	}
}

void GlimmerNodeDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (volume): >>%0.3f<<", value);

	if (event->widget() == maxSpeed) {
		object->maxSpeed=value;
	}
}

void GlimmerNodeDialog::valueChangedEvent(ppltk::Event* event, int)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (volume): >>%0.3f<<", value);

	if (event->widget() == action) {
		object->action=static_cast<GlimmerNode::GlimmerAction>(action->currentIdentifier().toInt());
	}
}

void GlimmerNodeDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	//ppl7::PrintDebugTime("VoiceTriggerDialog::valueChangedEvent int64_t\n");
	if (event->widget() == range_x) {
		object->range.x=(int)value;
	} else if (event->widget() == range_y) {
		object->range.y=(int)value;
	} else if (event->widget() == next_node) {
		object->next_node=(uint32_t)value;
	} else {
		for (int i=0;i < 10;i++) {
			if (event->widget() == target_id[i]) object->triggerObjects[i].object_id=(uint16_t)value;
		}
	}
}

void GlimmerNodeDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button & Buttons::Reset) {
		object->reset();
	} else if (button & Buttons::Test) {
		object->test();
	}

}

}	// EOF namespace Decker::Objects
