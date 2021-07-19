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

}


FloaterVertical::FloaterVertical()
: Object(Type::FloaterVertical)
{

}

}	// EOF namespace Decker::Objects
