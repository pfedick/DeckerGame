#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Representation KeyReward::representation()
{
	return Representation(Spriteset::GenericObjects, 50);
}

KeyReward::KeyReward()
: Collectable(Type::ObjectType::Savepoint)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=50;
	collisionDetection=true;
	sprite_no_representation=50;
}

}	// EOF namespace Decker::Objects
