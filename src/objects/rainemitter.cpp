#include <ppl7.h>
#include <ppl7-grafix.h>
#include <SDL.h>
#include "objects.h"
#include "decker.h"
#include "player.h"

namespace Decker::Objects {

static int particle_cycle[]={ 216,217,218,219,220,221,222,223,224,225,226,
	227,228,229,230,231,232,233,234,235 };


class RainParticle : public Object
{
	friend class RainEmitter;
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
	RainParticle();
	static Representation representation();
	virtual void update(double time, TileTypePlane& ttplane, Player& player);
};


Representation RainEmitter::representation()
{
	return Representation(Spriteset::GenericObjects, 214);
}


RainEmitter::RainEmitter()
	:Object(Type::ObjectType::RainEmitter)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=214;
	collisionDetection=false;
	visibleAtPlaytime=false;
	sprite_no_representation=214;
	next_birth=0.0f;
}

void RainEmitter::update(double time, TileTypePlane& ttplane, Player& player)
{
	if (next_birth < time) {
		next_birth=time + (float)ppl7::rand(20, 300) / 1000;
		double d=ppl7::grafix::Distance(ppl7::grafix::PointF(player.WorldCoords.x + player.Viewport.width() / 2,
			player.WorldCoords.y + player.Viewport.height() / 2), p);
		if (d > 2000.0f) return;
		for (int i=0;i < 4;i++) {

			RainParticle* particle=new RainParticle();
			particle->p.x=p.x + ppl7::rand(0, TILE_WIDTH * 16);
			particle->p.y=p.y;
			particle->initial_p.x=particle->p.x;
			particle->initial_p.y=particle->p.y;
			particle->pf.x=(float)particle->p.x;
			particle->pf.y=(float)particle->p.y;
			particle->velocity.x=(float)(ppl7::rand(0, 200) - 100.0f) / 1000;
			particle->velocity.y=(float)ppl7::rand(4000, 7300) / 1000;
			particle->end=particle->p;
			particle->scale=(float)(ppl7::rand(300, 1000)) / 1000.0f;
			//particle->color_mod.set(ppl7::rand(192, 255), ppl7::rand(192, 255), ppl7::rand(192, 255), 255);
			color_mod.set(255, 255, 255, 255);
			ppl7::grafix::Size levelsize=ttplane.size();
			levelsize.height*=TILE_HEIGHT;
			while (particle->end.y < levelsize.height && ttplane.getType(particle->end) != TileType::Blocking)
				particle->end.y+=TILE_HEIGHT;
			particle->end.y-=TILE_HEIGHT;
			GetObjectSystem()->addObject(particle);
		}
	}
}

RainParticle::RainParticle()
	: Object(Type::ObjectType::Particle)
{
	sprite_set=Spriteset::GenericObjects;
	collisionDetection=false;
	visibleAtPlaytime=true;
	sprite_no_representation=216;
	sprite_no=216;
	next_animation=0.0f;
	spawned=true;
	myLayer=Layer::BeforePlayer;
	animation.startRandom(particle_cycle, sizeof(particle_cycle) / sizeof(int), true, 0);
}

void RainParticle::update(double time, TileTypePlane&, Player&)
{
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		sprite_no=animation.getFrame();
		sprite_no_representation=sprite_no;
	}
	pf.x+=velocity.x;
	pf.y+=velocity.y;
	p.x=(int)pf.x;
	p.y=(int)pf.y;
	updateBoundary();
	if (p.y > end.y) deleteDefered=true;
}



}	// EOF namespace Decker::Objects
