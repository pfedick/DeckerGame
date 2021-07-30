#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {


class TouchParticle : public Object
{
	friend class TouchEmitter;
private:
	ppl7::grafix::Point velocity;
	Type::ObjectType emitted_object;
public:
	TouchParticle(Type::ObjectType type);
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
};

Representation TouchEmitter::representation()
{
	return Representation(Spriteset::GenericObjects, 238);
}


TouchEmitter::TouchEmitter()
:Object(Type::ObjectType::TouchEmitter)
{
	sprite_set=0;
	sprite_no=238;
	sprite_no_representation=238;
	toogle_count=0;
	max_toggles=1;
	direction=0;
	touchtype=0;
	emitted_object=Type::Crystal;
	save_size+=4;
	pixelExactCollision=false;
	collisionDetection=true;
	next_touch_time=0.0f;
}


void TouchEmitter::handleCollision(Player *player, const Collision &collision)
{
	// emit particle, which moves in direction 2 free tiles and then emits the final
	// object
	//printf ("TouchEmitter::handleCollision %d:%d\n", toogle_count,max_toggles);
	double now=ppl7::GetMicrotime();
	//printf ("next_touch_time=%0.3f, now=%0.3f\n",next_touch_time,now);
	if (toogle_count<max_toggles && next_touch_time<now) {
		next_touch_time=now+1.0f;
		toogle_count++;
		TouchParticle *particle=new TouchParticle(emitted_object);
		particle->p=p;
		particle->initial_p=p;
		particle->spawned=true;
		switch (direction) {
		case 0: particle->velocity.setPoint(0, -4); break;
		case 1: particle->velocity.setPoint(4, 0); break;
		case 2: particle->velocity.setPoint(0, 4); break;
		case 3: particle->velocity.setPoint(-4, 0); break;
		default: particle->velocity.setPoint(0, -4); break;
		}
		GetObjectSystem()->addObject(particle);
	}
	if (toogle_count>=max_toggles) {
		collisionDetection=false;
		enabled=false;
	}
}

size_t TouchEmitter::save(unsigned char *buffer, size_t size)
{
	if (size<save_size) return 0;
	size_t bytes=Object::save(buffer,size);
	ppl7::Poke8(buffer+bytes,max_toggles);
	ppl7::Poke8(buffer+bytes+1,direction);
	ppl7::Poke8(buffer+bytes+2,emitted_object);
	ppl7::Poke8(buffer+bytes+3,touchtype);
	return bytes+4;
}

bool TouchEmitter::load(const unsigned char *buffer, size_t size)
{
	if (!Object::load(buffer,size)) return false;
	if (size<save_size) return false;
	max_toggles=ppl7::Peek8(buffer+9);
	direction=ppl7::Peek8(buffer+10);
	emitted_object=(Type::ObjectType)ppl7::Peek8(buffer+11);
	touchtype=ppl7::Peek8(buffer+12);
	if (touchtype==1) {
		sprite_no=237;
		sprite_no_representation=237;
	}
	return true;
}

void TouchEmitter::openUi()
{
	printf ("UI requested\n");
}

TouchParticle::TouchParticle(Type::ObjectType type)
: Object(Type::Particle)
{
	emitted_object=type;
	//ObjectSystem *objs=GetObjectSystem();
	//objs-
	Representation repr=getRepresentation(type);
	sprite_no=repr.sprite_no;
	sprite_set=repr.sprite_set;
	sprite_no_representation=sprite_no;
}

Representation TouchParticle::representation()
{
	return Representation(Spriteset::GenericObjects, 238);
}

void TouchParticle::update(double time, TileTypePlane &ttplane, Player &player)
{
	p+=velocity;
	if (ppl7::grafix::Distance(p, initial_p)>100) {
		deleteDefered=true;
		ObjectSystem *objs=GetObjectSystem();
		Object *object=objs->getInstance(emitted_object);
		object->p=p;
		object->initial_p=p;
		object->spawned=true;
		objs->addObject(object);
	}
}



}	// EOF namespace Decker::Objects
