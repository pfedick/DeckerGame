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
	return Representation(Spriteset::GenericObjects, 215);
}


VoiceTrigger::VoiceTrigger()
	:Object(Type::ObjectType::VoiceTrigger)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=215;
	collisionDetection=true;
	pixelExactCollision=false;
	visibleAtPlaytime=false;
	sprite_no_representation=215;
	volume=1.0f;
	range.setPoint(300, 300);
	triggeredByCollision=true;
	singleTrigger=true;
	cooldownUntilNextTrigger=5.0f;
	cooldown=0.0f;
	trigger_count=0;
}

VoiceTrigger::~VoiceTrigger()
{

}

void VoiceTrigger::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
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
	Object::drawEditMode(renderer, coords);
}

void VoiceTrigger::handleCollision(Player* player, const Collision& collision)
{
	if (!triggeredByCollision) return;
	if (singleTrigger == true && trigger_count > 0) return;
	else if (singleTrigger == false && cooldown > player->time) return;

	if (player->speak(audiofile, text, phonetics, volume)) {
		trigger_count++;
		cooldown=player->time + cooldownUntilNextTrigger;
	}
}

void VoiceTrigger::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	boundary.setRect(p.x - range.x / 2, p.y - range.y / 2, range.x, range.y);
	//initial_boundary=boundary;

}

size_t VoiceTrigger::saveSize() const
{
	size_t s=14 + 4 * 2;
	s+=context.size();
	s+=audiofile.size();
	s+=text.size();
	s+=phonetics.size();
	return Object::saveSize() + s;

}

size_t VoiceTrigger::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	int flags=0;
	if (singleTrigger) flags|=1;
	if (triggeredByCollision) flags|=2;

	ppl7::Poke8(buffer + bytes + 1, flags);
	ppl7::PokeFloat(buffer + bytes + 2, cooldownUntilNextTrigger);
	ppl7::PokeFloat(buffer + bytes + 6, volume);
	ppl7::Poke16(buffer + bytes + 10, range.x);
	ppl7::Poke16(buffer + bytes + 12, range.y);
	int p=14;
	ppl7::Poke16(buffer + bytes + p, context.size());		// context
	memcpy(buffer + bytes + p + 2, context.c_str(), context.size());
	p+=2 + context.size();
	ppl7::Poke16(buffer + bytes + p, audiofile.size());		// audiofile
	memcpy(buffer + bytes + p + 2, audiofile.c_str(), audiofile.size());
	p+=2 + audiofile.size();
	ppl7::Poke16(buffer + bytes + p, text.size());			// text
	memcpy(buffer + bytes + p + 2, text.c_str(), text.size());
	p+=2 + text.size();
	ppl7::Poke16(buffer + bytes + p, phonetics.size());		// phonetics
	memcpy(buffer + bytes + p + 2, phonetics.c_str(), phonetics.size());
	p+=2 + phonetics.size();
	return bytes + p;
}

size_t VoiceTrigger::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	int flags=ppl7::Peek8(buffer + bytes + 1);
	singleTrigger=false;
	triggeredByCollision=false;
	if (flags & 1) singleTrigger=true;
	if (flags & 2) triggeredByCollision=true;
	cooldownUntilNextTrigger=ppl7::PeekFloat(buffer + bytes + 2);
	volume=ppl7::PeekFloat(buffer + bytes + 6);
	range.x=ppl7::Peek16(buffer + bytes + 10);
	range.y=ppl7::Peek16(buffer + bytes + 12);
	int p=14;
	size_t s=ppl7::Peek16(buffer + bytes + p);
	context.set((const char*)(buffer + bytes + p + 2), s);
	p+=2 + s;
	s=ppl7::Peek16(buffer + bytes + p);
	audiofile.set((const char*)(buffer + bytes + p + 2), s);
	p+=2 + s;
	s=ppl7::Peek16(buffer + bytes + p);
	text.set((const char*)(buffer + bytes + p + 2), s);
	p+=2 + s;
	s=ppl7::Peek16(buffer + bytes + p);
	phonetics.set((const char*)(buffer + bytes + p + 2), s);
	p+=2 + s;
	return size;
}

void VoiceTrigger::reset()
{
	cooldown=0.0f;
	trigger_count=0;
}

void VoiceTrigger::toggle(bool enable, Object* source)
{
	//current_state=enable;
	ppl7::PrintDebugTime("TODO:  VoiceTrigger::toggle\n");
}


class VoiceTriggerDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::LineInput* context;
	ppl7::tk::LineInput* audiofile;
	ppl7::tk::LineInput* text;
	ppl7::tk::LineInput* phonetics;
	ppl7::tk::HorizontalSlider* range_x;
	ppl7::tk::HorizontalSlider* range_y;
	ppl7::tk::HorizontalSlider* max_trigger;
	ppl7::tk::DoubleHorizontalSlider* volume;
	ppl7::tk::DoubleHorizontalSlider* cooldownUntilNextTrigger;
	ppl7::tk::CheckBox* singleTrigger, * triggeredByCollision;
	VoiceTrigger* object;

public:
	VoiceTriggerDialog(VoiceTrigger* object);
	~VoiceTriggerDialog();

	//virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
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
	: Decker::ui::Dialog(700, 400, Buttons::OK | Buttons::Test | Buttons::Reset)
{
	ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle(ppl7::ToString("Voice Trigger, Object ID: %u", object->id));
	int y=0;

	// State
	int sw=width() / 2;
	singleTrigger=new ppl7::tk::CheckBox(0, y, sw, 30, "singleTrigger");
	singleTrigger->setEventHandler(this);
	singleTrigger->setChecked(object->singleTrigger);
	addChild(singleTrigger);
	triggeredByCollision=new ppl7::tk::CheckBox(sw, y, sw, 30, "triggeredByCollision");
	triggeredByCollision->setChecked(object->triggeredByCollision);
	triggeredByCollision->setEventHandler(this);
	addChild(triggeredByCollision);

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

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Context:"));
	context=new ppl7::tk::LineInput(120, y, client.width() - 120, 30, object->context);
	context->setEventHandler(this);
	addChild(context);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Text:"));
	text=new ppl7::tk::LineInput(120, y, client.width() - 120, 30, object->text);
	text->setEventHandler(this);
	addChild(text);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Audiofile:"));
	audiofile=new ppl7::tk::LineInput(120, y, client.width() - 120, 30, object->audiofile);
	audiofile->setEventHandler(this);
	addChild(audiofile);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Phonetics:"));
	phonetics=new ppl7::tk::LineInput(120, y, client.width() - 120, 30, object->phonetics);
	phonetics->setEventHandler(this);
	addChild(phonetics);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "volume: "));
	volume=new ppl7::tk::DoubleHorizontalSlider(120, y, client.width() - 120, 30);
	volume->setLimits(0.0f, 2.0f);
	volume->setValue(object->volume);
	volume->enableSpinBox(true, 0.01f, 3, 80);
	volume->setEventHandler(this);
	addChild(volume);
	y+=35;
	addChild(new ppl7::tk::Label(0, y, 120, 30, "Voice cooldown: "));
	cooldownUntilNextTrigger=new ppl7::tk::DoubleHorizontalSlider(120, y, 300, 30);
	cooldownUntilNextTrigger->setLimits(0.0f, 120.0f);
	cooldownUntilNextTrigger->setValue(object->cooldownUntilNextTrigger);
	cooldownUntilNextTrigger->enableSpinBox(true, 0.01f, 0, 80);
	cooldownUntilNextTrigger->setEventHandler(this);
	addChild(cooldownUntilNextTrigger);
	addChild(new ppl7::tk::Label(420, y, 120, 30, "seconds"));
	y+=35;


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
	}
}



void VoiceTriggerDialog::valueChangedEvent(ppl7::tk::Event* event, double value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (volume): >>%0.3f<<", value);
	if (event->widget() == volume) {
		object->volume=value;
	}
	if (event->widget() == cooldownUntilNextTrigger) {
		object->cooldownUntilNextTrigger=value;
	}
}

void VoiceTriggerDialog::valueChangedEvent(ppl7::tk::Event* event, int64_t value)
{
	if (event->widget() == range_x) {
		object->range.x=(int)value;
	} else if (event->widget() == range_y) {
		object->range.y=(int)value;
	}
}

void VoiceTriggerDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	//ppl7::PrintDebugTime("SpeakerDialog::textChangedEvent: >>%s<<", (const char*)text);
	if (event->widget() == context) {
		object->context=text;
	}
	if (event->widget() == audiofile) {
		object->audiofile=text;
	}
	if (event->widget() == this->text) {
		object->text=text;
	}
	if (event->widget() == phonetics) {
		object->phonetics=text;
	}

}

void VoiceTriggerDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button & Buttons::Reset) {
		object->reset();
	} else if (button & Buttons::Test) {
		GetGame().getPlayer()->speak(object->audiofile, object->text, object->phonetics, object->volume);
	}

}

}	// EOF namespace Decker::Objects
