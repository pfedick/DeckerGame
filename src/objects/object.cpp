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
	case ObjectType::Skeleton: return ppl7::String("Skeleton");
	case ObjectType::Mummy: return ppl7::String("Mummy");
	case ObjectType::LaserBeamHorizontal: return ppl7::String("LaserBeamHorizontal");
	case ObjectType::LaserBeamVertical: return ppl7::String("LaserBeamVertical");

	case ObjectType::FloaterHorizontal: return ppl7::String("FloaterHorizontal");
	case ObjectType::FloaterVertical: return ppl7::String("FloaterVertical");
	case ObjectType::Door: return ppl7::String("Door");

	case ObjectType::Fire: return ppl7::String("Fire");
	case ObjectType::WindEmitter: return ppl7::String("WindEmitter");
	case ObjectType::Vent: return ppl7::String("Vent");
	case ObjectType::Speaker: return ppl7::String("Speaker");
	case ObjectType::TouchEmitter: return ppl7::String("TouchEmitter");
	case ObjectType::Particle: return ppl7::String("Particle");
	case ObjectType::BreakingGround: return ppl7::String("BreakingGround");

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
	sprite_no_representation=0;
	id=0;
	texture=NULL;
	collisionDetection=false;
	visibleAtPlaytime=true;
	enabled=true;
	save_size=9;
	state_size=0;
	pixelExactCollision=true;
	spawned=false;
	deleteDefered=false;
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
	if (texture) {
		boundary=texture->spriteBoundary(sprite_no,1.0f,p.x,p.y);
		initial_boundary=texture->spriteBoundary(sprite_no_representation,1.0f,initial_p.x,initial_p.y);
	}
}


Representation Object::representation()
{
	return Representation(-1, 0);
}

void Object::update(double, TileTypePlane &, Player &)
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

void Object::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	texture->draw(renderer,
			p.x+coords.x,
			p.y+coords.y,
			sprite_no);
}

void Object::handleCollision(Player *player, const Collision &collision)
{

}

void Object::openUi()
{

}

void Object::reset()
{
	if (!spawned) enabled=true;
}

void Object::toggle(bool , Object *)
{

}

}	// EOF namespace Decker::Objects
