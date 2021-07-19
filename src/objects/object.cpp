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

Representation::Representation(int sprite_set, int sprite_no)
{
	this->sprite_set=sprite_set;
	this->sprite_no=sprite_no;
}


Object::Object(Type::ObjectType type)
{
	myType=type;
	sprite_set=0;
	sprite_no=0;
	id=0;
	texture=NULL;
	collsionDetection=false;
	save_size=9;
	state_size=0;
}

Object::~Object()
{

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
		boundary=texture->spriteBoundary(sprite_no,1.0f,p.x,p.y);
}


Representation Object::representation()
{
	return Representation(-1, 0);
}

void Object::update(double time)
{

}

size_t Object::save(unsigned char *buffer, size_t size)
{
	if (size<9) return 0;
	ppl7::Poke8(buffer+0,myType);
	ppl7::Poke32(buffer+1,id);
	ppl7::Poke16(buffer+5,initial_p.x);
	ppl7::Poke16(buffer+7,initial_p.y);
	return 9;
}

bool Object::load(const unsigned char *buffer, size_t size)
{
	if (size<9) return false;
	id=ppl7::Peek32(buffer+1);
	initial_p.x=ppl7::Peek16(buffer+5);
	initial_p.y=ppl7::Peek16(buffer+7);
	p=initial_p;
	return true;
}

}	// EOF namespace Decker::Objects
