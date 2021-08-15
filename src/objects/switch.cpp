#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "audiopool.h"

namespace Decker::Objects {


Representation Switch::representation()
{
	return Representation(Spriteset::GenericObjects, 1);
}


Switch::Switch()
:Object(Type::ObjectType::Switch)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=1;
	collisionDetection=false;
	visibleAtPlaytime=true;
	sprite_no_representation=215;
	//save_size+=8;
}



void Switch::update(double time, TileTypePlane &ttplane, Player &player)
{

}

void Switch::handleCollision(Player *player, const Collision &collision)
{

}

size_t Switch::save(unsigned char *buffer, size_t size)
{
	if (size<save_size) return 0;
	size_t bytes=Object::save(buffer,size);
	/*

	ppl7::Poke16(buffer+bytes,sample_id);
	ppl7::Poke16(buffer+bytes+2,max_distance);
	ppl7::PokeFloat(buffer+bytes+4,volume);
	*/
	return bytes;
}

bool Switch::load(const unsigned char *buffer, size_t size)
{
	if (!Object::load(buffer,size)) return false;
	/*
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
	*/
	return true;
}

void Switch::openUi()
{
	/*
	SpeakerDialog *dialog=new SpeakerDialog(this);
	GetGameWindow()->addChild(dialog);
	*/
}



}	// EOF namespace Decker::Objects
