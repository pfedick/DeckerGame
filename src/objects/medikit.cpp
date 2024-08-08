#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"

namespace Decker::Objects {

Representation Medikit::representation()
{
	return Representation(Spriteset::GenericObjects, 83);
}


Medikit::Medikit()
	: Object(Type::ObjectType::Medikit)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=83;
	collisionDetection=true;
	alwaysUpdate=false;
	sprite_no_representation=83;
}


void Medikit::handleCollision(Player* player, const Collision&)
{
	player->health=100;
	player->countObject(type());
	enabled=false;
	collisionDetection=false;
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::fanfare2, 0.7f);

}

}	// EOF namespace Decker::Objects
