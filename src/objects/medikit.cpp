#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Representation Medikit::representation()
{
	return Representation(Spriteset::GenericObjects, 83);
}


Medikit::Medikit()
: Collectable(Type::ObjectType::Medikit)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=83;
	collsionDetection=true;
	sprite_no_representation=83;
}

}	// EOF namespace Decker::Objects
