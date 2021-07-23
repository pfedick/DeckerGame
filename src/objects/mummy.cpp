#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation Mummy::representation()
{
	return Representation(Spriteset::Mummy, 27);
}

}	// EOF namespace Decker::Objects
