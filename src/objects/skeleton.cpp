#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation Skeleton::representation()
{
	return Representation(Spriteset::Skeleton, 27);
}

}	// EOF namespace Decker::Objects
