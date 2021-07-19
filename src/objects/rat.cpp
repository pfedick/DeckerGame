#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation Rat::representation()
{
	return Representation(Spriteset::GenericObjects, 38);
}

}	// EOF namespace Decker::Objects
