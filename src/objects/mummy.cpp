#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation Mummy::representation()
{
	return Representation(Spriteset::Mummy, 27);
}

Mummy::Mummy()
:Enemy(Type::ObjectType::Mummy)
{
	sprite_set=Spriteset::Mummy;
	sprite_no=27;
	sprite_no_representation=27;
}

}	// EOF namespace Decker::Objects
