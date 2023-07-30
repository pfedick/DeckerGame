#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "audiopool.h"

namespace Decker::Objects {


class SpeakerDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::ComboBox* sample_name;
	ppl7::tk::HorizontalSlider* max_distance;
	ppl7::tk::DoubleHorizontalSlider* volume;
	ppl7::tk::CheckBox* initial_state_checkbox, * current_state_checkbox;
	Speaker* object;

public:
	SpeakerDialog(Speaker* object);
	~SpeakerDialog();

	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int64_t value);
	virtual void valueChangedEvent(ppl7::tk::Event* event, double value);
	virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked) override;
};


Representation Speaker::representation()
{
	return Representation(Spriteset::GenericObjects, 215);
}


Speaker::Speaker()
	:Object(Type::ObjectType::Speaker)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=215;
	collisionDetection=false;
	visibleAtPlaytime=false;
	sprite_no_representation=215;
	audio=NULL;
	sample_id=AudioClip::none;
	volume=1.0f;
	max_distance=1600;
	flags=static_cast<int>(Flags::initialStateEnabled);
	current_state=true;
}

Speaker::~Speaker()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}


void Speaker::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	if (current_state) {
		if (audio == NULL && sample_id != AudioClip::none) {
			setSample(sample_id, volume, max_distance);
		} else if (audio) {
			audio->setPositional(p, max_distance);
			audio->setVolume(volume);
		}
	} else if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

size_t Speaker::saveSize() const
{
	return Object::saveSize() + 11;
}

size_t Speaker::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	ppl7::Poke16(buffer + bytes + 1, sample_id);
	ppl7::Poke16(buffer + bytes + 3, max_distance);
	ppl7::PokeFloat(buffer + bytes + 5, volume);
	ppl7::Poke16(buffer + bytes + 9, flags);
	return bytes + 11;
}

size_t Speaker::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	sample_id=0;
	volume=1.0f;

	sample_id=ppl7::Peek16(buffer + bytes + 1);
	max_distance=ppl7::Peek16(buffer + bytes + 3);
	volume=ppl7::PeekFloat(buffer + bytes + 5);
	flags=ppl7::Peek16(buffer + bytes + 9);
	current_state=false;
	if (flags & static_cast<int>(Flags::initialStateEnabled)) current_state=true;

	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
	return size;
}

void Speaker::setSample(int id, float volume, int max_distance)
{
	AudioPool& pool=getAudioPool();
	if (audio) {
		pool.stopInstace(audio);
		delete audio;
		audio=NULL;
		sample_id=0;
	}
	if (id > 0) {
		audio=pool.getInstance((AudioClip::Id)id);
		if (audio) {
			audio->setVolume(volume);
			audio->setAutoDelete(false);
			audio->setLoop(true);
			audio->setPositional(p, max_distance);
			pool.playInstance(audio);
			sample_id=id;
		}
	}
}


void Speaker::toggle(bool enable, Object* source)
{
	current_state=enable;
}


void Speaker::openUi()
{
	SpeakerDialog* dialog=new SpeakerDialog(this);
	GetGameWindow()->addChild(dialog);
}

SpeakerDialog::SpeakerDialog(Speaker* object)
	: Decker::ui::Dialog(500, 230)
{
	ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle("Speaker");
	int y=0;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Sample: "));
	sample_name=new ppl7::tk::ComboBox(120, y, client.width() - 120, 30);
	sample_name->add("no sound", ppl7::ToString("%d", AudioClip::none));
	sample_name->add("Birds 1", ppl7::ToString("%d", AudioClip::birds1));
	sample_name->add("Birds 2", ppl7::ToString("%d", AudioClip::birds2));
	sample_name->add("Birds 3", ppl7::ToString("%d", AudioClip::birds3));
	sample_name->add("Birds in the rain", ppl7::ToString("%d", AudioClip::birds_in_rain));
	sample_name->add("Cave 1", ppl7::ToString("%d", AudioClip::cave1));
	sample_name->add("Cave 2", ppl7::ToString("%d", AudioClip::cave2));
	sample_name->add("Cave 3", ppl7::ToString("%d", AudioClip::cave3));
	sample_name->add("Desert at Night", ppl7::ToString("%d", AudioClip::desert_at_night));
	sample_name->add("Electric", ppl7::ToString("%d", AudioClip::electric));
	sample_name->add("Fire 1", ppl7::ToString("%d", AudioClip::fire1));
	sample_name->add("Fire 2", ppl7::ToString("%d", AudioClip::fire2));
	sample_name->add("Fire 3", ppl7::ToString("%d", AudioClip::fire3));
	sample_name->add("Jungle 1", ppl7::ToString("%d", AudioClip::jungle1));
	sample_name->add("Jungle 2", ppl7::ToString("%d", AudioClip::jungle2));

	sample_name->add("Rain 1", ppl7::ToString("%d", AudioClip::rain1));
	sample_name->add("Rain 2", ppl7::ToString("%d", AudioClip::rain2));
	sample_name->add("Rain 3", ppl7::ToString("%d", AudioClip::rain3));

	sample_name->add("Underwater", ppl7::ToString("%d", AudioClip::underwaterloop1));
	sample_name->add("Vent 1", ppl7::ToString("%d", AudioClip::vent1));
	sample_name->add("Vent 2", ppl7::ToString("%d", AudioClip::vent2));

	sample_name->add("Waterflow 1", ppl7::ToString("%d", AudioClip::waterflow1));
	sample_name->add("Waterflow 2", ppl7::ToString("%d", AudioClip::waterflow2));
	sample_name->add("Waterflow 3", ppl7::ToString("%d", AudioClip::waterflow3));
	sample_name->add("Waterdrips in a cave", ppl7::ToString("%d", AudioClip::waterdrips));
	sample_name->add("Waves 1", ppl7::ToString("%d", AudioClip::waves1));

	sample_name->add("Wind and grills", ppl7::ToString("%d", AudioClip::wind3));
	sample_name->add("Wind strong", ppl7::ToString("%d", AudioClip::wind_strong));
	sample_name->add("Wind Howling", ppl7::ToString("%d", AudioClip::wind1));
	sample_name->add("Wind Desert", ppl7::ToString("%d", AudioClip::wind2));




	sample_name->setCurrentIdentifier(ppl7::ToString("%d", object->sample_id));
	sample_name->setEventHandler(this);
	addChild(sample_name);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "volume: "));
	volume=new ppl7::tk::DoubleHorizontalSlider(120, y, client.width() - 120, 30);
	volume->setLimits(0.0f, 2.0f);
	volume->setValue(object->volume);
	volume->enableSpinBox(true, 0.01f, 3, 80);
	volume->setEventHandler(this);
	addChild(volume);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "max_distance: "));
	max_distance=new ppl7::tk::HorizontalSlider(120, y, client.width() - 120, 30);
	max_distance->setLimits(800, 3000);
	max_distance->enableSpinBox(true, 100, 80);
	max_distance->setValue(object->max_distance);
	max_distance->setEventHandler(this);
	addChild(max_distance);
	y+=35;

	// State
	int sw=width() / 2;
	initial_state_checkbox=new ppl7::tk::CheckBox(0, y, sw, 30, "initial State");
	initial_state_checkbox->setEventHandler(this);
	addChild(initial_state_checkbox);
	current_state_checkbox=new ppl7::tk::CheckBox(sw, y, sw, 30, "current State");
	current_state_checkbox->setEventHandler(this);
	addChild(current_state_checkbox);

	current_state_checkbox->setChecked(object->current_state);
	initial_state_checkbox->setChecked(object->flags & static_cast<int>(Speaker::Flags::initialStateEnabled));


}

SpeakerDialog::~SpeakerDialog()
{

}

void SpeakerDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	if (event->widget() == initial_state_checkbox) {
		int flags=object->flags & (0xffff - static_cast<int>(Speaker::Flags::initialStateEnabled));
		if (initial_state_checkbox->checked()) flags|=static_cast<int>(Speaker::Flags::initialStateEnabled);
		object->flags=flags;
		needsRedraw();
	} else if (event->widget() == current_state_checkbox) {
		object->current_state=current_state_checkbox->checked();
		needsRedraw();
	}
}


void SpeakerDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (int): >>%d<<", value);
	if (event->widget() == sample_name) {
		int id=sample_name->currentIdentifier().toInt();
		//printf ("id=%d",id);
		object->setSample(id, object->volume, object->max_distance);
	}
}

void SpeakerDialog::valueChangedEvent(ppl7::tk::Event* event, int64_t value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (int64_t): >>%d<<", (int)value);
	if (event->widget() == max_distance) {
		object->max_distance=(int)value;
	}
}

void SpeakerDialog::valueChangedEvent(ppl7::tk::Event* event, double value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (volume): >>%0.3f<<", value);
	if (event->widget() == volume) {
		object->volume=value;
	}
}

void SpeakerDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	//ppl7::PrintDebugTime("SpeakerDialog::textChangedEvent: >>%s<<", (const char*)text);
	if (event->widget() == volume) {
		float volume=text.toFloat();
		//printf ("new volume: %0.3f\n",volume);
		if (volume >= 0.0f && volume <= 1.0f)
			object->volume=volume;

	} else 	if (event->widget() == max_distance) {
		int max_distance=text.toInt();
		if (max_distance > 0 && max_distance < 65535)
			object->max_distance=max_distance;

	}
}


}	// EOF namespace Decker::Objects
