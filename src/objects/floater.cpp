#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Representation FloaterHorizontal::representation()
{
	return Representation(Spriteset::GenericObjects, 115);
}


Representation FloaterVertical::representation()
{
	return Representation(Spriteset::GenericObjects, 116);
}

FloaterHorizontal::FloaterHorizontal()
: Object(Type::FloaterHorizontal)
{
	sprite_no_representation=115;
}


FloaterVertical::FloaterVertical()
: Object(Type::FloaterVertical)
{
	sprite_no_representation=116;
}

}	// EOF namespace Decker::Objects
