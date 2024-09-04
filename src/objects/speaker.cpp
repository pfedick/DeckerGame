#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "audiopool.h"

namespace Decker::Objects {



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
	enabled=true;
	initial_state=true;
	sample_type=SampleType::AudioLoop;
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
	if (enabled) {
		if (sample_type == SampleType::AudioLoop) {
			if (audio == NULL && sample_id != AudioClip::none) {
				setSample(sample_id, volume, max_distance);
			} else if (audio) {
				double f=planeFactor[static_cast<int>(myPlane)];
				if (f != 1.0f) {
					const ppl7::grafix::Point& worldcoords=GetGame().getWorldCoords();
					ppl7::grafix::Point coords=worldcoords * f;
					ppl7::grafix::Point pp=ppl7::grafix::Point(p) - coords;
					audio->setPositional(pp + worldcoords, max_distance);
				} else {
					audio->setPositional(p, max_distance);
				}
				audio->setVolume(volume);
			}
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
	ppl7::Poke8(buffer + bytes, 2);		// Object Version

	ppl7::Poke16(buffer + bytes + 1, sample_id);
	ppl7::Poke16(buffer + bytes + 3, max_distance);
	ppl7::PokeFloat(buffer + bytes + 5, volume);
	int flags=0;
	if (initial_state) flags|=1;
	if (sample_type == SampleType::Effect) flags|=2;
	ppl7::Poke16(buffer + bytes + 9, flags);
	return bytes + 11;
}

size_t Speaker::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version < 1 || version>2) return 0;

	sample_id=0;
	volume=1.0f;

	sample_id=ppl7::Peek16(buffer + bytes + 1);
	max_distance=ppl7::Peek16(buffer + bytes + 3);
	volume=ppl7::PeekFloat(buffer + bytes + 5);
	int flags=ppl7::Peek16(buffer + bytes + 9);
	if (version == 1 && flags > 3) flags=flags & 1;
	//ppl7::PrintDebug("Speaker::load, flags=%d\n", flags);

	initial_state=(bool)flags & 1;
	sample_type=SampleType::AudioLoop;
	if (flags & 2) sample_type=SampleType::Effect;

	enabled=initial_state;

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
	if (sample_type == SampleType::AudioLoop) {
		if (id > 0) {
			audio=pool.getInstance((AudioClip::Id)id, AudioClass::Ambience);
			if (audio) {
				audio->setVolume(volume);
				audio->setAutoDelete(false);
				audio->setLoop(true);
				audio->setPositional(p, max_distance);
				audio->startRandom();
				pool.playInstance(audio);
				sample_id=id;
			}
		}
	} else {
		sample_id=id;
	}
}


void Speaker::toggle(bool enable, Object* source)
{
	if (sample_type == SampleType::AudioLoop) {
		this->enabled=enable;
	} else {
		getAudioPool().playOnce(static_cast<AudioClip::Id>(sample_id), p, max_distance, volume);
	}
}

void Speaker::trigger(Object* source)
{
	toggle(!enabled, source);
}

void Speaker::test()
{
	if (sample_type == SampleType::AudioLoop) {
		setSample(sample_id, volume, max_distance);
	} else {
		getAudioPool().playOnce(static_cast<AudioClip::Id>(sample_id), p, max_distance, volume);
	}
}

class SpeakerDialog : public Decker::ui::Dialog
{
private:
	ppltk::ComboBox* sample_name;
	ppltk::HorizontalSlider* max_distance;
	ppltk::DoubleHorizontalSlider* volume;
	ppltk::CheckBox* initial_state_checkbox, * current_state_checkbox;
	ppltk::RadioButton* type_audioloop, * type_effect;
	Speaker* object;

	void setupAudioLoop();
	void setupEffect();

public:
	SpeakerDialog(Speaker* object);
	~SpeakerDialog();

	virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
	virtual void valueChangedEvent(ppltk::Event* event, double value);
	virtual void toggledEvent(ppltk::Event* event, bool checked) override;
	void dialogButtonEvent(Dialog::Buttons button) override;
};

void Speaker::fillComboBoxWithEffects(ppltk::ComboBox* combobox, int selected_sample)
{
	combobox->clear();
	combobox->add("no sound", ppl7::ToString("%d", AudioClip::none));
	combobox->add("impact", ppl7::ToString("%d", AudioClip::impact));
	combobox->add("Big Crash", ppl7::ToString("%d", AudioClip::crash));
	combobox->add("Trap 1", ppl7::ToString("%d", AudioClip::trap1));
	combobox->add("Trap 1", ppl7::ToString("%d", AudioClip::trap2));
	combobox->add("LightSwitch", ppl7::ToString("%d", AudioClip::light_switch1));
	combobox->add("Crystal 1", ppl7::ToString("%d", AudioClip::crystal));
	combobox->add("Crystal 2", ppl7::ToString("%d", AudioClip::crystal2));
	combobox->add("Crystal 3", ppl7::ToString("%d", AudioClip::crystal3));
	combobox->add("Coin", ppl7::ToString("%d", AudioClip::coin1));
	combobox->add("Touchplate 1", ppl7::ToString("%d", AudioClip::touchplateswitch1));
	combobox->add("Touchplate 2", ppl7::ToString("%d", AudioClip::touchplateswitch2));
	combobox->add("Explosion", ppl7::ToString("%d", AudioClip::explosion1));
	combobox->add("Crate falling", ppl7::ToString("%d", AudioClip::crate_falling));
	combobox->add("Glimmer Plopp 1", ppl7::ToString("%d", AudioClip::glimmer_plopp1));
	combobox->add("Glimmer Plopp 2", ppl7::ToString("%d", AudioClip::glimmer_plopp2));
	combobox->add("Glimmer Plopp 3", ppl7::ToString("%d", AudioClip::glimmer_plopp3));
	combobox->add("Glimmer Plopp 4", ppl7::ToString("%d", AudioClip::glimmer_plopp4));
	combobox->add("Glimmer Plopp 5", ppl7::ToString("%d", AudioClip::glimmer_plopp5));
	combobox->add("Glimmer Plopp 6", ppl7::ToString("%d", AudioClip::glimmer_plopp6));
	combobox->add("Glimmer Effect 1", ppl7::ToString("%d", AudioClip::glimmer_effect1));
	combobox->add("Glimmer Effect 2", ppl7::ToString("%d", AudioClip::glimmer_effect2));
	combobox->add("Arrow swoosh", ppl7::ToString("%d", AudioClip::arrow_swoosh));
	combobox->add("Bite into peach", ppl7::ToString("%d", AudioClip::peach));
	combobox->add("Shoot", ppl7::ToString("%d", AudioClip::shoot));
	combobox->add("Break", ppl7::ToString("%d", AudioClip::break1));
	combobox->add("Squash", ppl7::ToString("%d", AudioClip::squash1));
	combobox->add("Water bubble", ppl7::ToString("%d", AudioClip::water_bubble1));
	combobox->add("Gas Spray bubble", ppl7::ToString("%d", AudioClip::gas_spray));
	combobox->sortItems();
	combobox->setCurrentIdentifier(ppl7::ToString("%d", selected_sample));
}


void Speaker::openUi()
{
	SpeakerDialog* dialog=new SpeakerDialog(this);
	GetGameWindow()->addChild(dialog);
}

SpeakerDialog::SpeakerDialog(Speaker* object)
	: Decker::ui::Dialog(500, 280, Buttons::OK | Buttons::Test)
{
	ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle(ppl7::ToString("Speaker, Object ID: %u", object->id));

	int y=0;

	// State
	int sw=width() / 2;
	initial_state_checkbox=new ppltk::CheckBox(0, y, sw, 30, "initial State", object->initial_state);
	initial_state_checkbox->setEventHandler(this);
	addChild(initial_state_checkbox);
	current_state_checkbox=new ppltk::CheckBox(sw, y, sw, 30, "current State", object->enabled);
	current_state_checkbox->setEventHandler(this);
	addChild(current_state_checkbox);
	y+=35;

	type_audioloop=new ppltk::RadioButton(0, y, sw, 30, "Audioloop", (bool)(object->sample_type == Speaker::SampleType::AudioLoop));
	type_audioloop->setEventHandler(this);
	addChild(type_audioloop);

	type_effect=new ppltk::RadioButton(sw, y, sw, 30, "Effect", (bool)(object->sample_type == Speaker::SampleType::Effect));
	type_effect->setEventHandler(this);
	addChild(type_effect);

	y+=35;




	addChild(new ppltk::Label(0, y, 120, 30, "Sample: "));
	sample_name=new ppltk::ComboBox(120, y, client.width() - 120, 30);
	if (object->sample_type == Speaker::SampleType::AudioLoop) setupAudioLoop();
	else setupEffect();
	sample_name->setEventHandler(this);
	addChild(sample_name);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "volume: "));
	volume=new ppltk::DoubleHorizontalSlider(120, y, client.width() - 120, 30);
	volume->setLimits(0.0f, 2.0f);
	volume->setValue(object->volume);
	volume->enableSpinBox(true, 0.01f, 3, 80);
	volume->setEventHandler(this);
	addChild(volume);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "max_distance: "));
	max_distance=new ppltk::HorizontalSlider(120, y, client.width() - 120, 30);
	max_distance->setLimits(800, 3000);
	max_distance->enableSpinBox(true, 100, 80);
	max_distance->setValue(object->max_distance);
	max_distance->setEventHandler(this);
	addChild(max_distance);
	y+=35;



}

SpeakerDialog::~SpeakerDialog()
{

}

void SpeakerDialog::setupAudioLoop()
{
	sample_name->clear();
	sample_name->add("no sound", ppl7::ToString("%d", AudioClip::none));
	sample_name->add("Birds 1", ppl7::ToString("%d", AudioClip::birds1));
	sample_name->add("Birds 2", ppl7::ToString("%d", AudioClip::birds2));
	sample_name->add("Birds 3", ppl7::ToString("%d", AudioClip::birds3));
	sample_name->add("Birds in the rain", ppl7::ToString("%d", AudioClip::birds_in_rain));
	sample_name->add("Cave 1", ppl7::ToString("%d", AudioClip::cave1));
	sample_name->add("Cave 2", ppl7::ToString("%d", AudioClip::cave2));
	sample_name->add("Cave 3", ppl7::ToString("%d", AudioClip::cave3));
	sample_name->add("Cave 4", ppl7::ToString("%d", AudioClip::cave4));
	sample_name->add("Desert at Night", ppl7::ToString("%d", AudioClip::desert_at_night));
	sample_name->add("Electric", ppl7::ToString("%d", AudioClip::electric));
	sample_name->add("Fire 1", ppl7::ToString("%d", AudioClip::fire1));
	sample_name->add("Fire 2", ppl7::ToString("%d", AudioClip::fire2));
	sample_name->add("Fire 3", ppl7::ToString("%d", AudioClip::fire3));
	sample_name->add("Fire 4 - Gasburner", ppl7::ToString("%d", AudioClip::fire4));
	sample_name->add("Fireworks loop", ppl7::ToString("%d", AudioClip::fireworks_loop));
	sample_name->add("Jungle 1", ppl7::ToString("%d", AudioClip::jungle1));
	sample_name->add("Jungle 2", ppl7::ToString("%d", AudioClip::jungle2));

	sample_name->add("Lava loop 1", ppl7::ToString("%d", AudioClip::lavaloop1));
	sample_name->add("Lava loop 2", ppl7::ToString("%d", AudioClip::lavaloop2));
	sample_name->add("Lava bubbles", ppl7::ToString("%d", AudioClip::lavabubbles));

	sample_name->add("Night 1", ppl7::ToString("%d", AudioClip::night1));
	sample_name->add("Night 2", ppl7::ToString("%d", AudioClip::night2));
	sample_name->add("Night 3", ppl7::ToString("%d", AudioClip::night3));
	sample_name->add("Night 4", ppl7::ToString("%d", AudioClip::night4));
	sample_name->add("Nightowl 1", ppl7::ToString("%d", AudioClip::nightowl1));
	sample_name->add("Nightowl 2", ppl7::ToString("%d", AudioClip::nightowl2));
	sample_name->add("Nightowl 3", ppl7::ToString("%d", AudioClip::nightowl3));

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
	sample_name->add("Waterdrips 2", ppl7::ToString("%d", AudioClip::waterdrips2));
	sample_name->add("Waterdrips 3", ppl7::ToString("%d", AudioClip::waterdrips3));
	sample_name->add("Water Bubbles 1", ppl7::ToString("%d", AudioClip::water_bubble1));
	sample_name->add("Water Bubbles 2", ppl7::ToString("%d", AudioClip::water_bubble2));
	sample_name->add("Water Bubbles 3", ppl7::ToString("%d", AudioClip::water_bubble3));
	sample_name->add("Water Bubbles 4", ppl7::ToString("%d", AudioClip::water_bubble4));
	sample_name->add("Water Bubbles 5", ppl7::ToString("%d", AudioClip::water_bubble5));

	sample_name->add("Waves 1", ppl7::ToString("%d", AudioClip::waves1));
	sample_name->add("Waves 2", ppl7::ToString("%d", AudioClip::waves2));
	sample_name->add("Waves 3", ppl7::ToString("%d", AudioClip::waves3));
	sample_name->add("Waves 4", ppl7::ToString("%d", AudioClip::waves4));

	sample_name->add("Gas 1", ppl7::ToString("%d", AudioClip::gas1));
	sample_name->add("Gas 2", ppl7::ToString("%d", AudioClip::gas2));
	sample_name->add("Gas 3", ppl7::ToString("%d", AudioClip::gas3));


	sample_name->add("Wind and grills", ppl7::ToString("%d", AudioClip::wind3));
	sample_name->add("Wind strong", ppl7::ToString("%d", AudioClip::wind_strong));
	sample_name->add("Wind Howling", ppl7::ToString("%d", AudioClip::wind1));
	sample_name->add("Wind Desert", ppl7::ToString("%d", AudioClip::wind2));
	sample_name->add("Wind soft 1", ppl7::ToString("%d", AudioClip::wind4));
	sample_name->add("Wind soft 2", ppl7::ToString("%d", AudioClip::wind5));

	sample_name->add("Earthquake", ppl7::ToString("%d", AudioClip::earthquake));
	sample_name->add("Rumble", ppl7::ToString("%d", AudioClip::rumble));
	sample_name->add("Waterpuddle", ppl7::ToString("%d", AudioClip::waterpuddle));

	sample_name->sortItems();
	sample_name->setCurrentIdentifier(ppl7::ToString("%d", AudioClip::none));
	if (object->sample_type == Speaker::SampleType::AudioLoop) sample_name->setCurrentIdentifier(ppl7::ToString("%d", object->sample_id));
}

void SpeakerDialog::setupEffect()
{
	Speaker::fillComboBoxWithEffects(sample_name, static_cast<int>(AudioClip::none));
	if (object->sample_type == Speaker::SampleType::Effect) sample_name->setCurrentIdentifier(ppl7::ToString("%d", object->sample_id));
}

void SpeakerDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (event->widget() == initial_state_checkbox) {
		object->initial_state=checked;
	} else if (event->widget() == current_state_checkbox) {
		object->enabled=checked;
	} else if (event->widget() == type_audioloop && checked == true) {
		if (object->sample_type != Speaker::SampleType::AudioLoop) {
			setupAudioLoop();
			object->sample_type = Speaker::SampleType::AudioLoop;
			object->sample_id=0;
		}
	} else if (event->widget() == type_effect && checked == true) {
		if (object->sample_type != Speaker::SampleType::Effect) {
			setupEffect();
			object->sample_type = Speaker::SampleType::Effect;
			object->sample_id=0;
		}
	}
}


void SpeakerDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (int): >>%d<<", value);
	if (event->widget() == sample_name) {
		object->sample_id=sample_name->currentIdentifier().toInt();
		object->test();
	}
}

void SpeakerDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (int64_t): >>%d<<", (int)value);
	if (event->widget() == max_distance) {
		object->max_distance=(int)value;
	}
}

void SpeakerDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	//ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (volume): >>%0.3f<<", value);
	if (event->widget() == volume) {
		object->volume=value;
	}
}

void SpeakerDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button == Buttons::Test) object->test();
}


}	// EOF namespace Decker::Objects
