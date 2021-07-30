#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


Representation TouchEmitter::representation()
{
	return Representation(Spriteset::GenericObjects, 37);
}


TouchEmitter::TouchEmitter()
:Object(Type::ObjectType::TouchEmitter)
{
	toogle_count=0;
	max_toggles=1;
	direction=0;
	emitted_object=Type::Diamond;
	save_size+=3;
	pixelExactCollision=false;
}


void TouchEmitter::handleCollision(Player *player, const Collision &collision)
{
	// emit particle, which moves in direction 2 free tiles and then emits the final
	// object
	if (toogle_count<max_toggles) {
		toogle_count++;

	}
}

size_t TouchEmitter::save(unsigned char *buffer, size_t size)
{
	if (size<save_size) return 0;
	size_t bytes=Object::save(buffer,size);
	ppl7::Poke8(buffer+bytes,max_toggles);
	ppl7::Poke8(buffer+bytes+1,direction);
	ppl7::Poke8(buffer+bytes+2,emitted_object);
	return bytes+3;
}

bool TouchEmitter::load(const unsigned char *buffer, size_t size)
{
	if (!Object::load(buffer,size)) return false;
	if (size<save_size) return false;
	max_toggles=ppl7::Peek8(buffer+9);
	direction=ppl7::Peek8(buffer+10);
	emitted_object=(Type::ObjectType)ppl7::Peek8(buffer+11);
	return true;
}

void TouchEmitter::openUi()
{

}

}	// EOF namespace Decker::Objects
