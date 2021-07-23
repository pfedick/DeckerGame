#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation Rat::representation()
{
	return Representation(Spriteset::GenericObjects, 38);
}

Rat::Rat()
:Enemy(Type::ObjectType::Rat)
{
	sprite_no=38;
	sprite_no_representation=38;
}


}	// EOF namespace Decker::Objects
