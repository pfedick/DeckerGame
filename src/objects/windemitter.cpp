#include <ppl7.h>
#include <ppl7-grafix.h>
#include <SDL.h>
#include "objects.h"
#include "decker.h"

namespace Decker::Objects {

static int particle_cycle[]={216,217,218,219,220,221,222,223,224,225,226,
	227,228,229,230,231,232,233,234,235};


class WindParticle : public Object
{
	friend class WindEmitter;
public:
	class Velocity
	{
	public:
		Velocity() {
			x=0.0f;
			y=0.0f;
		}
		float x;
		float y;
	};
private:
	double next_animation;
	AnimationCycle animation;
	Velocity velocity;
	Velocity pf;
	ppl7::grafix::Point end;
public:
	WindParticle();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
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

void WindEmitter::update(double time, TileTypePlane &ttplane, Player &)
{
	if (next_birth<time) {
		next_birth=time+(float)ppl7::rand(200,1200)/1000;
		WindParticle *particle=new WindParticle();
		particle->p.x=p.x;
		particle->p.y=p.y;
		particle->initial_p.x=p.x;
		particle->initial_p.y=p.y;
		particle->pf.x=(float)p.x;
		particle->pf.y=(float)p.y;
		particle->velocity.x=(float)(ppl7::rand(0,200)-100.0f)/1000;
		particle->velocity.y=-(float)ppl7::rand(2000,2300)/1000;
		particle->end=particle->p;
		while (particle->end.y>0 && ttplane.getType(particle->end)!=TileType::AirStream)
			particle->end.y-=TILE_HEIGHT;
		while (particle->end.y>0 && ttplane.getType(particle->end)==TileType::AirStream)
			particle->end.y-=TILE_HEIGHT;
		GetObjectSystem()->addObject(particle);
	}
}


WindParticle::WindParticle()
: Object(Type::ObjectType::Particle)
{
	sprite_set=Spriteset::GenericObjects;
	collisionDetection=false;
	visibleAtPlaytime=true;
	sprite_no_representation=216;
	sprite_no=216;
	next_animation=0.0f;
	animation.startRandom(particle_cycle,sizeof(particle_cycle)/sizeof(int),true,0);
}

void WindParticle::update(double time, TileTypePlane &, Player &)
{
	if (time>next_animation) {
		next_animation=time+0.056f;
		animation.update();
		sprite_no=animation.getFrame();
		sprite_no_representation=sprite_no;
	}
	pf.x+=velocity.x;
	pf.y+=velocity.y;
	p.x=(int)pf.x;
	p.y=(int)pf.y;
	updateBoundary();
	if (p.y<end.y) deleteDefered=true;
}



}	// EOF namespace Decker::Objects
