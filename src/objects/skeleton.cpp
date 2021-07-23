#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation Skeleton::representation()
{
	return Representation(Spriteset::Skeleton, 27);
}

Skeleton::Skeleton()
:Enemy(Type::ObjectType::Skeleton)
{
	sprite_set=Spriteset::Skeleton;
	sprite_no=27;
	sprite_no_representation=27;
}

}	// EOF namespace Decker::Objects
