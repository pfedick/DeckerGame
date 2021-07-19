#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Representation SavePoint::representation()
{
	return Representation(Spriteset::GenericObjects, 52);
}


}	// EOF namespace Decker::Objects
