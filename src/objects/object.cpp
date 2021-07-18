#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

ppl7::String Type::name(Type::ObjectType type)
{
	switch (type) {
	case ObjectType::Coin: return ppl7::String("Coin");
	case ObjectType::Gem: return ppl7::String("Gem");
	case ObjectType::Medikit: return ppl7::String("Medikit");
	case ObjectType::Savepoint: return ppl7::String("Savepoint");
	case ObjectType::ThreeSpeers: return ppl7::String("ThreeSpeers");
	case ObjectType::Rat: return ppl7::String("Rat");
	default: return ppl7::String("unknown object type");
	}
}

Object::Object(Type::ObjectType type)
{
	myType=type;
	scale=1.0f;
	sprite_set=0;
	sprite_no=0;
	id=0;
}

Type::ObjectType Object::type() const
{
	return myType;
}

ppl7::String Object::typeName() const
{
	return Type::name(myType);
}



}	// EOF namespace Decker::Objects
