#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation OxygenTank::representation()
{
	return Representation(Spriteset::GenericObjects, 295);
}

OxygenTank::OxygenTank()
	: Object(Type::ObjectType::Oxygen)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=295;
	collisionDetection=true;
	sprite_no_representation=295;
	next_birth=0.0f;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.005, 0.044));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));

}

void OxygenTank::update(double time, TileTypePlane&, Player& player, float)
{
	if (!enabled) return;
	if (next_birth < time) {
		next_birth=time + randf(0.020, 0.300);
		ParticleSystem* ps=GetParticleSystem();
		if (!emitterInPlayerRange(p, player)) return;
		int new_particles=ppl7::rand(3, 15);
		for (int i=0;i < new_particles;i++) {
			Particle* particle=new Particle();
			particle->birth_time=time;
			particle->death_time=randf(0.481, 1.424) + time;
			particle->p=p;
			particle->layer=Particle::Layer::BehindPlayer;
			particle->weight=randf(0.000, 0.000);
			particle->gravity.setPoint(0.000, 0.000);
			particle->velocity=calculateVelocity(randf(0.755, 3.208), 0.000 + randf(-11.887, 11.887));
			particle->scale=randf(0.300, 1.000);
			particle->color_mod.set(255, 255, 255, 255);
			particle->initAnimation(Particle::Type::RotatingParticleTransparent);
			particle->initScaleGradient(scale_gradient, particle->scale);
			//particle->initColorGradient(color_gradient);
			ps->addParticle(particle);
		}
	}
}

void OxygenTank::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	player->addAir(30.0f);
	player->countObject(type());
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::water_bubble1, 0.3f);
	audio.playOnce(AudioClip::gas_spray, 0.1f);
}

}
