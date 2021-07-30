#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

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
	sprite_no_representation=215;
}

}	// EOF namespace Decker::Objects
