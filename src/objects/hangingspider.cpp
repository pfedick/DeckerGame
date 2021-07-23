#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Representation HangingSpider::representation()
{
	return Representation(Spriteset::GenericObjects, 49);
}

HangingSpider::HangingSpider()
:Enemy(Type::ObjectType::HangingSpider)
{
	sprite_no=49;
	sprite_no_representation=49;
}


}	// EOF namespace Decker::Objects
