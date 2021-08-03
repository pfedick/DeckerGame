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
	sprite_no_representation=83;
}


void Medikit::handleCollision(Player *player, const Collision &)
{
	player->health=100;
	enabled=false;
	collisionDetection=false;
}

}	// EOF namespace Decker::Objects
