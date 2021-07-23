#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation Arrow::representation()
{
	return Representation(Spriteset::GenericObjects, 37);
}


Arrow::Arrow()
:Trap(Type::ObjectType::Arrow)
{

}

}	// EOF namespace Decker::Objects
