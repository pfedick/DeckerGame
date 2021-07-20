#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Representation PlayerStartPoint::representation()
{
	return Representation(Spriteset::GenericObjects, 151);
}


PlayerStartPoint::PlayerStartPoint()
: Object(Type::ObjectType::PlayerStartpoint)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=151;
	collisionDetection=false;
	visibleAtPlaytime=false;
	sprite_no_representation=151;
}

}	// EOF namespace Decker::Objects
