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

	type=ParticleType::Rain;
	ParticleColor.set(255, 255, 255, 255);
	emitter_stud_width=16;
	max_particle_birth_per_cycle = 4;
	birth_time_min=0.020f;
	birth_time_max=0.300f;
	max_velocity_x=0.5f;
	min_velocity_y=4.0f;
	max_velocity_y=7.3f;
	scale_min=0.3f;
	scale_max=1.0f;
}

static float randf(float min, float max)
{
	double range=max - min;
	double r=(((double)ppl7::rand(0, RAND_MAX)) / (double)RAND_MAX * range) + min;
	return r;
}

void RainEmitter::update(double time, TileTypePlane& ttplane, Player& player)
{
	if (next_birth < time) {
		next_birth=time + randf(birth_time_min, birth_time_max);
		double d=ppl7::grafix::Distance(ppl7::grafix::PointF(player.WorldCoords.x + player.Viewport.width() / 2,
			player.WorldCoords.y + player.Viewport.height() / 2), p);
		if (d > 2 * player.Viewport.width()) return;
		int new_particles=ppl7::rand(1, max_particle_birth_per_cycle);
		for (int i=0;i < new_particles;i++) {
			RainParticle* particle=new RainParticle();
			particle->p.x=p.x + ppl7::rand(0, TILE_WIDTH * emitter_stud_width);
			particle->p.y=p.y;
			particle->initial_p.x=particle->p.x;
			particle->initial_p.y=particle->p.y;
			particle->pf.x=(float)particle->p.x;
			particle->pf.y=(float)particle->p.y;
			particle->velocity.x=randf(0, max_velocity_x) - (max_velocity_x / 2.0f);
			particle->velocity.y=randf(min_velocity_y, max_velocity_y);
			particle->end=particle->p;
			particle->scale=randf(scale_min, scale_max);
			particle->color_mod=ParticleColor;
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
