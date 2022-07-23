#include <ppl7.h>
#include <ppl7-grafix.h>
#include <SDL.h>
#include "objects.h"
#include "decker.h"

namespace Decker::Objects {

class WindParticle : public Particle
{
	friend class WindEmitter;
public:
private:
	ppl7::grafix::Point end;
public:
	virtual void update(double time, TileTypePlane& ttplane);
};


Representation WindEmitter::representation()
{
	return Representation(Spriteset::GenericObjects, 214);
}


WindEmitter::WindEmitter()
	:Object(Type::ObjectType::WindEmitter)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=214;
	collisionDetection=false;
	visibleAtPlaytime=false;
	sprite_no_representation=214;
	next_birth=0.0f;
}

void WindEmitter::update(double time, TileTypePlane& ttplane, Player&)
{
	if (next_birth < time) {
		next_birth=time + (float)ppl7::rand(200, 1200) / 1000;
		WindParticle* particle=new WindParticle();
		particle->layer=Particle::Layer::BeforePlayer;
		particle->birth_time=time;
		particle->death_time=time + 20.0f;
		particle->p.x=p.x;
		particle->p.y=p.y;
		particle->velocity.x=(float)(ppl7::rand(0, 200) - 100.0f) / 1000;
		particle->velocity.y=-(float)ppl7::rand(2000, 2300) / 1000;
		particle->end=particle->p;
		while (particle->end.y > 0 && ttplane.getType(particle->end) != TileType::AirStream)
			particle->end.y-=TILE_HEIGHT;
		while (particle->end.y > 0 && ttplane.getType(particle->end) == TileType::AirStream)
			particle->end.y-=TILE_HEIGHT;
		particle->initAnimation(Particle::Type::RotatingParticleTransparent);
		GetParticleSystem()->addParticle(particle);
	}
}

void WindParticle::update(double time, TileTypePlane& ttplane)
{
	Particle::update(time, ttplane);
	if (p.y < end.y) death_time=0;
}



}	// EOF namespace Decker::Objects
