#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"

namespace Decker::Objects {




Representation VoiceTrigger::representation()
{
	return Representation(Spriteset::GenericObjects, 38);
}


VoiceTrigger::VoiceTrigger()
	:Object(Type::ObjectType::VoiceTrigger)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=38;
	collisionDetection=true;
	pixelExactCollision=false;
	visibleAtPlaytime=false;
	sprite_no_representation=38;
	volume=1.0f;
	range.setPoint(300, 300);
	triggeredByCollision=true;
	singleTrigger=true;
	cooldownUntilNextTrigger=5.0f;
	cooldown=0.0f;
	trigger_count=0;
	speechId=0;
	initialDelay=0.0f;
	triggerDeleay=0.0f;
	for (int i=0;i < 5;i++) {
		triggerObjects[i].enable=true;
		triggerObjects[i].object_id=0;
	}
	requireKeypress=false;
	pauseWorld=false;
	initialStateEnabled=true;
	state=State::waiting_for_activation;
	activeInitialDeleay=0.0f;
}

VoiceTrigger::~VoiceTrigger()
{

}

void VoiceTrigger::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
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
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 96);
		SDL_RenderFillRect(renderer, &r);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &r);
		SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
	}
	Object::drawEditMode(renderer, coords);
}

void VoiceTrigger::handleCollision(Player* player, const Collision& collision)
{
	if (!triggeredByCollision) return;
	if (singleTrigger == true && trigger_count > 0) return;
	if (singleTrigger == false && cooldown > player->time) return;
	if (state == State::waiting_for_activation) {
		state=State::activated;
		enabled=false;
	}
}


void VoiceTrigger::notifyTargets() const
{
	ObjectSystem* objs=GetObjectSystem();
	for (int i=0;i < 5;i++) {
		if (triggerObjects[i].object_id > 0) {
			Object* target=objs->getObject(triggerObjects[i].object_id);
			if (target) {
				target->trigger();
			}
		}
	}

}

void VoiceTrigger::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	boundary.setRect(p.x - range.x / 2, p.y - range.y / 2, range.x, range.y);
	if (state == State::activated) {
		//ppl7::PrintDebugTime("VoiceTrigger::update activated\n");
		activeInitialDeleay=time + initialDelay;
		state=State::waiting_for_initial_delay;
	}
	if (state == State::waiting_for_initial_delay && time >= activeInitialDeleay) {
		if (player.speak(speechId, volume)) {
			state = State::speaking;
			trigger_count++;
			cooldown=time + cooldownUntilNextTrigger;
		}
	}

	if (state == State::speaking && !player.isSpeaking()) {
		activeInitialDeleay=time + triggerDeleay;
		state=State::waiting_for_trigger_delay;
	}
	if (state == State::waiting_for_trigger_delay && time >= activeInitialDeleay) {
		state=State::finished;
		notifyTargets();
	}
	if (state == State::finished && singleTrigger == false) {
		state=State::waiting_for_activation;
		enabled=true;
	}

}

size_t VoiceTrigger::saveSize() const
{
	size_t s=24 + 5 * 3 + 2;
	s+=context.size();
	return Object::saveSize() + s;

}

size_t VoiceTrigger::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 2);		// Object Version
	int flags=0;
	if (singleTrigger) flags|=1;
	if (triggeredByCollision) flags|=2;
	if (requireKeypress) flags|=4;
	if (pauseWorld) flags|=8;
	if (initialStateEnabled) flags|=16;

	ppl7::Poke8(buffer + bytes + 1, flags);
	ppl7::PokeFloat(buffer + bytes + 2, cooldownUntilNextTrigger);
	ppl7::PokeFloat(buffer + bytes + 6, volume);
	ppl7::Poke16(buffer + bytes + 10, range.x);
	ppl7::Poke16(buffer + bytes + 12, range.y);
	ppl7::PokeFloat(buffer + bytes + 14, initialDelay);
	ppl7::PokeFloat(buffer + bytes + 18, triggerDeleay);
	ppl7::Poke16(buffer + bytes + 22, speechId);
	int p=24;
	for (int i=0;i < 5;i++) {
		ppl7::Poke16(buffer + bytes + p, triggerObjects[i].object_id);
		ppl7::Poke8(buffer + bytes + p + 2, (int)triggerObjects[i].enable);	 // not used
		p+=3;
	}
	ppl7::Poke16(buffer + bytes + p, context.size());		// context
	memcpy(buffer + bytes + p + 2, context.c_str(), context.size());
	p+=2 + context.size();
	return bytes + p;
}

size_t VoiceTrigger::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 2) return 0;

	int flags=ppl7::Peek8(buffer + bytes + 1);
	singleTrigger=false;
	triggeredByCollision=false;
	requireKeypress=false;
	pauseWorld=false;
	initialStateEnabled=false;
	if (flags & 1) singleTrigger=true;
	if (flags & 2) triggeredByCollision=true;
	if (flags & 4) requireKeypress=true;
	if (flags & 8) pauseWorld=true;
	if (flags & 16) initialStateEnabled=true;
	if (!initialStateEnabled) enabled=false;
	if (!triggeredByCollision) collisionDetection=false;
	cooldownUntilNextTrigger=ppl7::PeekFloat(buffer + bytes + 2);
	volume=ppl7::PeekFloat(buffer + bytes + 6);
	range.x=ppl7::Peek16(buffer + bytes + 10);
	range.y=ppl7::Peek16(buffer + bytes + 12);
	initialDelay=ppl7::PeekFloat(buffer + bytes + 14);
	triggerDeleay=ppl7::PeekFloat(buffer + bytes + 18);
	speechId=ppl7::Peek16(buffer + bytes + 22);
	int p=24;
	for (int i=0;i < 5;i++) {
		triggerObjects[i].object_id=ppl7::Peek16(buffer + bytes + p);
		triggerObjects[i].enable=ppl7::Peek8(buffer + bytes + p + 2);	// not used
		p+=3;
	}
	size_t s=ppl7::Peek16(buffer + bytes + p);
	context.set((const char*)(buffer + bytes + p + 2), s);
	return size;
}

void VoiceTrigger::reset()
{
	cooldown=0.0f;
	trigger_count=0;
	state=State::waiting_for_activation;
}

void VoiceTrigger::toggle(bool enable, Object* source)
{
	enabled=enable;
	if (!enable) state=State::disabled;
	else state=State::waiting_for_activation;
}

void VoiceTrigger::trigger(Object* source)
{
	if (!enabled) {
		enabled=true;
		return;
	}
	if (state == State::disabled) return;
	if (state == State::waiting_for_activation) {
		state=State::activated;
	}

}



class VoiceTriggerDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::LineInput* context;
	ppl7::tk::SpinBox* speechId;
	ppl7::tk::HorizontalSlider* range_x;
	ppl7::tk::HorizontalSlider* range_y;
	ppl7::tk::HorizontalSlider* max_trigger;
	ppl7::tk::DoubleHorizontalSlider* volume;
	ppl7::tk::DoubleHorizontalSlider* cooldownUntilNextTrigger;
	ppl7::tk::DoubleHorizontalSlider* initialDelay;
	ppl7::tk::DoubleHorizontalSlider* triggerDeleay;
	ppl7::tk::CheckBox* initialStateEnabled, * currentState;
	ppl7::tk::CheckBox* singleTrigger, * triggeredByCollision;
	ppl7::tk::CheckBox* requireKeypress, * pauseWorld;
	ppl7::tk::SpinBox* target_id[5];
	ppl7::tk::CheckBox* target_state[5];

	VoiceTrigger* object;

public:
	VoiceTriggerDialog(VoiceTrigger* object);
	~VoiceTriggerDialog();

	//virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int64_t value);
	virtual void valueChangedEvent(ppl7::tk::Event* event, double value);
	virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked) override;
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
};


void VoiceTrigger::openUi()
{
	VoiceTriggerDialog* dialog=new VoiceTriggerDialog(this);
	GetGameWindow()->addChild(dialog);
}

VoiceTriggerDialog::VoiceTriggerDialog(VoiceTrigger* object)
	: Decker::ui::Dialog(700, 650, Buttons::OK | Buttons::Test | Buttons::Reset)
{
	ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle(ppl7::ToString("Voice Trigger, Object ID: %u", object->id));
	int y=0;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Context:"));
	context=new ppl7::tk::LineInput(120, y, client.width() - 120, 30, object->context);
	context->setEventHandler(this);
	addChild(context);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Text ID:"));
	speechId=new ppl7::tk::SpinBox(120, y, 100, 30, object->speechId);
	speechId->setLimits(0, 65535);
	speechId->setEventHandler(this);
	addChild(speechId);
	y+=35;

	// State
	int sw=width() / 2;
	initialStateEnabled=new ppl7::tk::CheckBox(0, y, sw, 30, "initial state");
	initialStateEnabled->setEventHandler(this);
	initialStateEnabled->setChecked(object->initialStateEnabled);
	addChild(initialStateEnabled);
	currentState=new ppl7::tk::CheckBox(sw, y, sw, 30, "current state");
	currentState->setEventHandler(this);
	currentState->setChecked(object->enabled);
	addChild(currentState);
	y+=35;

	singleTrigger=new ppl7::tk::CheckBox(0, y, sw, 30, "singleTrigger");
	singleTrigger->setEventHandler(this);
	singleTrigger->setChecked(object->singleTrigger);
	addChild(singleTrigger);
	triggeredByCollision=new ppl7::tk::CheckBox(sw, y, sw, 30, "triggeredByCollision");
	triggeredByCollision->setChecked(object->triggeredByCollision);
	triggeredByCollision->setEventHandler(this);
	addChild(triggeredByCollision);
	y+=35;

	requireKeypress=new ppl7::tk::CheckBox(0, y, sw, 30, "require keypress:");
	requireKeypress->setEventHandler(this);
	requireKeypress->setChecked(object->requireKeypress);
	addChild(requireKeypress);
	pauseWorld=new ppl7::tk::CheckBox(sw, y, sw, 30, "pause world while text is shown");
	pauseWorld->setChecked(object->pauseWorld);
	pauseWorld->setEventHandler(this);
	addChild(pauseWorld);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Collision Range x:"));
	range_x=new ppl7::tk::HorizontalSlider(120, y, sw - 120, 30);
	range_x->setLimits(0, 1600);
	range_x->setValue(object->range.x);
	range_x->enableSpinBox(true, 1, 80);
	range_x->setEventHandler(this);
	addChild(range_x);
	addChild(new ppl7::tk::Label(sw, y, 60, 30, "Range y:"));
	range_y=new ppl7::tk::HorizontalSlider(sw + 60, y, sw - 60, 30);
	range_y->setLimits(0, 1600);
	range_y->setValue(object->range.y);
	range_y->enableSpinBox(true, 1, 80);
	range_y->setEventHandler(this);
	addChild(range_y);
	y+=35;


	addChild(new ppl7::tk::Label(0, y, 120, 30, "volume: "));
	volume=new ppl7::tk::DoubleHorizontalSlider(120, y, client.width() - 120, 30);
	volume->setLimits(0.0f, 2.0f);
	volume->setValue(object->volume);
	volume->enableSpinBox(true, 0.01f, 3, 80);
	volume->setEventHandler(this);
	addChild(volume);
	y+=35;
	addChild(new ppl7::tk::Label(0, y, 120, 30, "Initial delay: "));
	initialDelay=new ppl7::tk::DoubleHorizontalSlider(120, y, 300, 30);
	initialDelay->setLimits(0.0f, 60.0f);
	initialDelay->setValue(object->initialDelay);
	initialDelay->enableSpinBox(true, 0.2f, 3, 80);
	initialDelay->setEventHandler(this);
	addChild(initialDelay);
	addChild(new ppl7::tk::Label(420, y, 120, 30, "seconds"));
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Voice cooldown: "));
	cooldownUntilNextTrigger=new ppl7::tk::DoubleHorizontalSlider(120, y, 300, 30);
	cooldownUntilNextTrigger->setLimits(0.0f, 120.0f);
	cooldownUntilNextTrigger->setValue(object->cooldownUntilNextTrigger);
	cooldownUntilNextTrigger->enableSpinBox(true, 0.2f, 3, 80);
	cooldownUntilNextTrigger->setEventHandler(this);
	addChild(cooldownUntilNextTrigger);
	addChild(new ppl7::tk::Label(420, y, 120, 30, "seconds"));
	y+=45;

	addChild(new ppl7::tk::Label(0, y, 400, 30, "Trigger other objects, when text finished:"));
	y+=35;
	addChild(new ppl7::tk::Label(0, y, 120, 30, "Trigger delay: "));
	triggerDeleay=new ppl7::tk::DoubleHorizontalSlider(120, y, 300, 30);
	triggerDeleay->setLimits(0.0f, 60.0f);
	triggerDeleay->setValue(object->triggerDeleay);
	triggerDeleay->enableSpinBox(true, 0.2f, 3, 80);
	triggerDeleay->setEventHandler(this);
	addChild(triggerDeleay);
	addChild(new ppl7::tk::Label(420, y, 120, 30, "seconds"));
	y+=35;

	for (int i=0;i < 5;i++) {
		addChild(new ppl7::tk::Label(120, y, 80, 30, ppl7::ToString("Object %d: ", i + 1)));
		target_id[i]=new ppl7::tk::SpinBox(220, y, 100, 30, object->triggerObjects[i].object_id);
		target_id[i]->setLimits(0, 65535);
		target_id[i]->setEventHandler(this);
		addChild(target_id[i]);
		/*
		target_state[i]=new ppl7::tk::CheckBox(325, y, 100, 30, "enable", object->triggerObjects[i].enable);
		target_state[i]->setEventHandler(this);
		addChild(target_state[i]);
		*/
		y+=35;
	}
	//current_state_checkbox->setChecked(object->current_state);
	//initial_state_checkbox->setChecked(object->flags & static_cast<int>(Speaker::Flags::initialStateEnabled));


}

VoiceTriggerDialog::~VoiceTriggerDialog()
{

}

void VoiceTriggerDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	if (event->widget() == singleTrigger) {
		object->singleTrigger=checked;
	} else if (event->widget() == triggeredByCollision) {
		object->triggeredByCollision=checked;
		object->collisionDetection=checked;
	} else if (event->widget() == requireKeypress) {
		object->requireKeypress=checked;
	} else if (event->widget() == pauseWorld) {
		object->pauseWorld=checked;
	} else if (event->widget() == initialStateEnabled) {
		object->initialStateEnabled=checked;
	} else if (event->widget() == currentState) {
		object->enabled=checked;
	}
}



void VoiceTriggerDialog::valueChangedEvent(ppl7::tk::Event* event, double value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (volume): >>%0.3f<<", value);
	if (event->widget() == volume) {
		object->volume=value;
	} else if (event->widget() == cooldownUntilNextTrigger) {
		object->cooldownUntilNextTrigger=value;
	} else if (event->widget() == initialDelay) {
		object->initialDelay=value;
	} else if (event->widget() == triggerDeleay) {
		object->triggerDeleay=value;
	}
}

void VoiceTriggerDialog::valueChangedEvent(ppl7::tk::Event* event, int64_t value)
{
	//ppl7::PrintDebugTime("VoiceTriggerDialog::valueChangedEvent int64_t\n");
	if (event->widget() == range_x) {
		object->range.x=(int)value;
	} else if (event->widget() == range_y) {
		object->range.y=(int)value;
	} else 	if (event->widget() == speechId) {
		object->speechId=(uint16_t)value;
	} else {
		for (int i=0;i < 5;i++) {
			if (event->widget() == target_id[i]) object->triggerObjects[i].object_id=(uint16_t)value;
		}
	}
}

void VoiceTriggerDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	//ppl7::PrintDebugTime("VoiceTriggerDialog::valueChangedEvent int\n");
	if (event->widget() == speechId) {

		object->speechId=(int)value;
	}
}

void VoiceTriggerDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	//ppl7::PrintDebugTime("SpeakerDialog::textChangedEvent: >>%s<<", (const char*)text);
	if (event->widget() == context) {
		object->context=text;
	}
}

void VoiceTriggerDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button & Buttons::Reset) {
		object->reset();
	} else if (button & Buttons::Test) {
		GetGame().getPlayer()->speak(object->speechId, object->volume);
	}

}

}	// EOF namespace Decker::Objects
