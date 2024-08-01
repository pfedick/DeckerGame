#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"

namespace Decker::Objects {

Representation SpawnPoint::representation()
{
	return Representation(Spriteset::GenericObjects, 297);
}


SpawnPoint::SpawnPoint()
	:Object(Type::ObjectType::SpawnPoint)
{
	sprite_set=0;
	sprite_no=297;
	sprite_no_representation=297;
	toggle_count=0;
	max_toggles=1;
	emitted_object=Type::Crystal;
	pixelExactCollision=false;
	collisionDetection=false;
	next_touch_time=0.0f;
	sample_id=0;
	max_distance=1600;
	volume=1.0f;
	visibleAtPlaytime=false;

}

SpawnPoint::~SpawnPoint()
{

}

void SpawnPoint::reset()
{
	Object::reset();
	toggle_count=0;
	next_touch_time=0.0f;
}

void SpawnPoint::emmitObject()
{
	AudioPool& audio=getAudioPool();
	if (sample_id != AudioClip::none) audio.playOnce(static_cast<AudioClip::Id>(sample_id), p, max_distance, volume);
	//next_touch_time=time + ppl7::randf(0.1f, 1.0f);

	Object* object=GetObjectSystem()->getInstance(emitted_object);
	if (object) {
		toggle_count++;
		object->initial_p=p;
		object->p=p;
		object->spawned=true;
		GetObjectSystem()->addObject(object);
	}
}

void SpawnPoint::trigger(Object*)
{
	if (toggle_count < max_toggles) emmitObject();
}


size_t SpawnPoint::saveSize() const
{
	return Object::saveSize() + 11;
}

size_t SpawnPoint::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	ppl7::Poke8(buffer + bytes + 1, max_toggles);
	ppl7::Poke8(buffer + bytes + 2, static_cast<int>(emitted_object));
	ppl7::Poke16(buffer + bytes + 3, sample_id);
	ppl7::Poke16(buffer + bytes + 5, max_distance);
	ppl7::PokeFloat(buffer + bytes + 7, volume);
	return bytes + 11;
}

size_t SpawnPoint::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	max_toggles=ppl7::Peek8(buffer + bytes + 1);
	emitted_object=(Type::ObjectType)ppl7::Peek8(buffer + bytes + 2);
	sample_id=ppl7::Peek16(buffer + bytes + 3);
	max_distance=ppl7::Peek16(buffer + bytes + 5);
	volume=ppl7::PeekFloat(buffer + bytes + 7);
	return size;
}

class SpawnPointDialog : public Decker::ui::Dialog
{
private:
	ppltk::ComboBox* object_type;
	ppltk::SpinBox* max_toggles;
	ppltk::ComboBox* audio_sample;
	ppltk::DoubleHorizontalSlider* volume;
	ppltk::HorizontalSlider* max_distance;
	ppltk::Button* test_audio_button;

	SpawnPoint* object;

public:
	SpawnPointDialog(SpawnPoint* object);
	~SpawnPointDialog();

	void valueChangedEvent(ppltk::Event* event, int value) override;
	void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	void valueChangedEvent(ppltk::Event* event, double value) override;
	//void toggledEvent(ppltk::Event* event, bool checked) override;
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
	void mouseDownEvent(ppltk::MouseEvent* event) override;
};


void SpawnPoint::openUi()
{
	SpawnPointDialog* dialog=new SpawnPointDialog(this);
	GetGameWindow()->addChild(dialog);
}



SpawnPointDialog::SpawnPointDialog(SpawnPoint* object)
	: Decker::ui::Dialog(640, 480, Buttons::OK | Buttons::Reset | Buttons::Test)
{
	ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle(ppl7::ToString("SpawnPoint, ID: %d", object->id));
	int y=0;

	addChild(new ppltk::Label(0, y, 120, 30, "Object to spawn: "));
	object_type=new ppltk::ComboBox(120, y, 400, 30);
	object_type->setEventHandler(this);
	object_type->add("Medikit", ppl7::ToString("%d", Type::Medikit));
	object_type->add("ExtraLife", ppl7::ToString("%d", Type::ExtraLife));
	object_type->add("Savepoint", ppl7::ToString("%d", Type::Savepoint));
	object_type->add("PowerCell", ppl7::ToString("%d", Type::PowerCell));
	object_type->add("Crystal", ppl7::ToString("%d", Type::Crystal));
	object_type->add("Hammer", ppl7::ToString("%d", Type::Hammer));
	object_type->add("Diamond", ppl7::ToString("%d", Type::Diamond));
	object_type->add("Coin", ppl7::ToString("%d", Type::Coin));
	object_type->add("Cheese", ppl7::ToString("%d", Type::Cheese));
	object_type->add("Cherry", ppl7::ToString("%d", Type::Cherry));
	object_type->add("Apple", ppl7::ToString("%d", Type::Apple));
	object_type->add("Mushroom", ppl7::ToString("%d", Type::Mushroom));
	object_type->add("Scorpion", ppl7::ToString("%d", Type::Scorpion));
	object_type->add("Scarabeus", ppl7::ToString("%d", Type::Scarabeus));
	object_type->add("Spider", ppl7::ToString("%d", Type::Spider));
	object_type->add("Piranha", ppl7::ToString("%d", Type::Piranha));
	object_type->add("Bat", ppl7::ToString("%d", Type::Bat));
	object_type->add("Skeleton", ppl7::ToString("%d", Type::Skeleton));
	object_type->add("Rat", ppl7::ToString("%d", Type::Rat));
	object_type->add("Bird", ppl7::ToString("%d", Type::Bird));
	object_type->add("Breaking Ground", ppl7::ToString("%d", Type::BreakingGround));
	object_type->add("Breaking Wall", ppl7::ToString("%d", Type::BreakingWall));

	object_type->sortItems();
	object_type->setCurrentIdentifier(ppl7::ToString("%d", object->emitted_object));
	addChild(object_type);

	y+=35;
	addChild(new ppltk::Label(0, y, 120, 30, "max toggles: "));
	max_toggles=new ppltk::SpinBox(120, y, 100, 30, object->max_toggles);
	max_toggles->setLimits(1, 10);
	max_toggles->setEventHandler(this);
	addChild(max_toggles);
	y+=35;
	addChild(new ppltk::Label(0, y, 120, 30, "Audio sample: "));
	audio_sample=new ppltk::ComboBox(120, y, 400, 30);
	audio_sample->add("None", ppl7::ToString("%d", AudioClip::none));
	audio_sample->add("Impact", ppl7::ToString("%d", AudioClip::impact));
	audio_sample->add("Big Crash", ppl7::ToString("%d", AudioClip::crash));
	audio_sample->add("Trap", ppl7::ToString("%d", AudioClip::trap1));
	audio_sample->add("LightSwitch", ppl7::ToString("%d", AudioClip::light_switch1));
	audio_sample->add("Crystal", ppl7::ToString("%d", AudioClip::crystal));
	audio_sample->add("Coin", ppl7::ToString("%d", AudioClip::coin1));
	audio_sample->add("Touchplate", ppl7::ToString("%d", AudioClip::touchplateswitch1));
	audio_sample->add("Explosion", ppl7::ToString("%d", AudioClip::explosion1));
	audio_sample->sortItems();
	audio_sample->setCurrentIdentifier(ppl7::ToString("%d", object->sample_id));
	audio_sample->setEventHandler(this);
	addChild(audio_sample);

	test_audio_button=new ppltk::Button(525, y, 80, 30, "Test");
	test_audio_button->setEventHandler(this);
	addChild(test_audio_button);
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

SpawnPointDialog::~SpawnPointDialog()
{

}

void SpawnPointDialog::dialogButtonEvent(Dialog::Buttons button)
{
	//ppl7::PrintDebug("SpawnPointDialog::dialogButtonEvent: %d\n", (int)button);
	if (button == Dialog::Buttons::Reset) object->reset();
	else if (button == Dialog::Buttons::Test) object->emmitObject();
}

void SpawnPointDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	if (event->widget() == object_type) {
		object->emitted_object=(Type::ObjectType)object_type->currentIdentifier().toInt();
	} else if (event->widget() == audio_sample) {
		object->sample_id=audio_sample->currentIdentifier().toInt();
	}
}

void SpawnPointDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	if (event->widget() == max_toggles) {
		object->max_toggles=(unsigned char)value;
	} else if (event->widget() == max_distance) {
		object->max_distance=(int)value;
	}
}

void SpawnPointDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	if (event->widget() == volume) {
		object->volume=value;
	}
}


void SpawnPointDialog::mouseDownEvent(ppltk::MouseEvent* event)
{
	if (event->widget() == test_audio_button && object->sample_id != AudioClip::none) {
		getAudioPool().playOnce(static_cast<AudioClip::Id>(object->sample_id), object->p, object->max_distance, object->volume);
	}
	Decker::ui::Dialog::mouseDownEvent(event);
}




}	// EOF namespace Decker::Objects
