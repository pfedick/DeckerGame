#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation WindEmitter::representation()
{
	return Representation(Spriteset::GenericObjects, 214);
}


WindEmitter::WindEmitter()
:Object(Type::ObjectType::WindEmitter)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=214;
	collisionDetection=false;
	visibleAtPlaytime=false;
	sprite_no_representation=214;
}

}	// EOF namespace Decker::Objects
