#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"

namespace Decker::Objects {

static int vent_cycle[]={ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14 };

Representation Vent::representation()
{
	return Representation(Spriteset::Vent, 0);
}


Vent::Vent()
	:Object(Type::ObjectType::Vent)
{
	sprite_set=Spriteset::Vent;
	animation.start(vent_cycle, sizeof(vent_cycle) / sizeof(int), true, 0);
	sprite_no=0;
	collisionDetection=false;
	sprite_no_representation=0;
	next_animation=0;
	audio=NULL;
}

Vent::~Vent()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void Vent::update(double time, TileTypePlane&, Player&, float)
{
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		sprite_no=animation.getFrame();
		sprite_no_representation=sprite_no;
		updateBoundary();
	}
	if (!audio) {
		AudioPool& pool=getAudioPool();
		audio=pool.getInstance(AudioClip::vent1);
		if (audio) {
			audio->setLoop(true);
			audio->setPositional(p, 800);
			audio->setVolume(0.2f);
			pool.playInstance(audio);
		}
	} else {
		audio->setPositional(p, 1600);
	}

}

}	// EOF namespace Decker::Objects
