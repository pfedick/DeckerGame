#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Collectable::Collectable(Type::ObjectType type)
: Object(type)
{
	points=0;
}

}	// EOF namespace Decker::Objects
