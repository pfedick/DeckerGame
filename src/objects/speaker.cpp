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
	Decker::ui::ComboBox *sample_name;
	ppl7::tk::LineInput *max_distance;
	ppl7::tk::LineInput *volume;
	Speaker *speaker;

public:
	SpeakerDialog(Speaker *object);
	~SpeakerDialog();

	virtual void valueChangedEvent(ppl7::tk::Event *event, int value);
	virtual void textChangedEvent(ppl7::tk::Event *event, const ppl7::String &text);
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
	save_size+=8;
	max_distance=1600;
}

Speaker::~Speaker()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}


void Speaker::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (audio==NULL && sample_id!=AudioClip::none) {
		setSample(sample_id,volume,max_distance);
	} else if (audio) {
		audio->setPositional(p, max_distance);
		audio->setVolume(volume);
	}
}


size_t Speaker::save(unsigned char *buffer, size_t size)
{
	if (size<save_size) return 0;
	size_t bytes=Object::save(buffer,size);
	ppl7::Poke16(buffer+bytes,sample_id);
	ppl7::Poke16(buffer+bytes+2,max_distance);
	ppl7::PokeFloat(buffer+bytes+4,volume);
	return bytes+8;
}

bool Speaker::load(const unsigned char *buffer, size_t size)
{
	if (!Object::load(buffer,size)) return false;
	sample_id=0;
	volume=1.0f;
	if (size<save_size) return false;
	sample_id=ppl7::Peek16(buffer+9);
	max_distance=ppl7::Peek16(buffer+11);
	volume=ppl7::PeekFloat(buffer+13);
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
	return true;
}

void Speaker::setSample(int id, float volume, int max_distance)
{
	AudioPool &pool=getAudioPool();
	if (audio) {
		pool.stopInstace(audio);
		delete audio;
		audio=NULL;
		sample_id=0;
	}
	if (id>0) {
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

void Speaker::openUi()
{
	SpeakerDialog *dialog=new SpeakerDialog(this);
	GetGameWindow()->addChild(dialog);
}

SpeakerDialog::SpeakerDialog(Speaker *object)
: Decker::ui::Dialog(500,200)
{
	speaker=object;
	setWindowTitle("Speaker");
	addChild(new ppl7::tk::Label(0,0,120,30,"Sample: "));
	addChild(new ppl7::tk::Label(0,40,120,30,"volume: "));
	addChild(new ppl7::tk::Label(0,80,120,30,"max_distance: "));

	sample_name=new Decker::ui::ComboBox(120,0,360,30);
	sample_name->add("no sound", ppl7::ToString("%d",AudioClip::none));
	sample_name->add("Birds 1", ppl7::ToString("%d",AudioClip::birds1));
	sample_name->add("Electric", ppl7::ToString("%d",AudioClip::electric));
	sample_name->add("Vent 1", ppl7::ToString("%d",AudioClip::vent1));
	sample_name->add("Vent 2", ppl7::ToString("%d",AudioClip::vent2));
	sample_name->add("Howling Wind 1", ppl7::ToString("%d",AudioClip::wind1));
	sample_name->add("Fire 1", ppl7::ToString("%d",AudioClip::fire1));
	sample_name->add("Fire 2", ppl7::ToString("%d",AudioClip::fire2));
	sample_name->setCurrentIdentifier(ppl7::ToString("%d",object->sample_id));
	sample_name->setEventHandler(this);
	addChild(sample_name);

	volume=new ppl7::tk::LineInput(120,40,100,30);
	volume->setText(ppl7::ToString("%0.3f", object->volume));
	volume->setEventHandler(this);
	addChild(volume);

	max_distance=new ppl7::tk::LineInput(120,80,100,30);
	max_distance->setText(ppl7::ToString("%d", object->max_distance));
	max_distance->setEventHandler(this);
	addChild(max_distance);

}

SpeakerDialog::~SpeakerDialog()
{

}

void SpeakerDialog::valueChangedEvent(ppl7::tk::Event *event, int value)
{
	if (event->widget()==sample_name) {
		int id=sample_name->currentIdentifier().toInt();
		//printf ("id=%d",id);
		speaker->setSample(id,speaker->volume,speaker->max_distance);
	}
}

void SpeakerDialog::textChangedEvent(ppl7::tk::Event *event, const ppl7::String &text)
{
	//printf ("SpeakerDialog::textChangedEvent: >>%s<<",(const char*)text);
	if (event->widget()==volume) {
		float volume=text.toFloat();
		//printf ("new volume: %0.3f\n",volume);
		if (volume>=0.0f && volume<=1.0f)
			speaker->volume=volume;

	} else 	if (event->widget()==max_distance) {
		int max_distance=text.toFloat();
		if (max_distance>0 && max_distance<65535)
			speaker->max_distance=max_distance;

	}
}


}	// EOF namespace Decker::Objects
