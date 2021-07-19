#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Representation GemReward::representation()
{
	return Representation(Spriteset::GenericObjects, 0);
}

}	// EOF namespace Decker::Objects
