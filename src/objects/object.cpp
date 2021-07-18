#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"

namespace Decker::Objects {

ppl7::String Type::name(Type::ObjectType type)
{
	switch (type) {
	case ObjectType::PlayerStartpoint: return ppl7::String("PlayerStartpoint");
	case ObjectType::Savepoint: return ppl7::String("Savepoint");
	case ObjectType::Medikit: return ppl7::String("Medikit");

	case ObjectType::Crystal: return ppl7::String("Crystal");
	case ObjectType::Diamond: return ppl7::String("Diamond");
	case ObjectType::Coin: return ppl7::String("Coin");
	case ObjectType::Key: return ppl7::String("Key");

	case ObjectType::Arrow: return ppl7::String("Arrow");
	case ObjectType::ThreeSpeers: return ppl7::String("ThreeSpeers");

	case ObjectType::Rat: return ppl7::String("Rat");
	case ObjectType::HangingSpider: return ppl7::String("HangingSpider");

	case ObjectType::FloaterHorizontal: return ppl7::String("FloaterHorizontal");
	case ObjectType::FloaterVertical: return ppl7::String("FloaterVertical");
	case ObjectType::Door: return ppl7::String("Door");

	default: return ppl7::String("unknown object type");
	}
}

Object::Object(Type::ObjectType type)
{
	myType=type;
	sprite_set=0;
	sprite_no=0;
	id=0;
	texture=NULL;
}

Type::ObjectType Object::type() const
{
	return myType;
}

ppl7::String Object::typeName() const
{
	return Type::name(myType);
}


void Object::updateBoundary()
{
	if (texture)
		boundary=texture->spriteBoundary(sprite_no,scale,p.x,p.y);
}


}	// EOF namespace Decker::Objects
