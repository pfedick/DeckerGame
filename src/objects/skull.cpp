#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Skull::representation()
{
	return Representation(Spriteset::Skull, 0);
}

Skull::Skull()
	:Enemy(Type::ObjectType::Skull)
{
	sprite_set=Spriteset::Skull;
	sprite_no=0;
	sprite_no_representation=0;
	state=ppl7::rand(0, 4);
	speed=ppl7::randf(0.0f, 2.0f);;
	collisionDetection=true;
	updateBoundary();
	light.color.set(255, 255, 255, 255);
	light.sprite_no=36;
	light.scale_x=1.0f;
	light.scale_y=1.0f;
	light.custom_texture=NULL;
	light.intensity=255;
	light.plane=static_cast<int>(LightPlaneId::Player);
	light.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

	shine.color.set(0, 255, 0, 255);
	shine.sprite_no=0;
	shine.scale_x=0.15f;
	shine.scale_y=0.15f;
	shine.intensity=128;
	shine.plane=static_cast<int>(LightPlaneId::Player);
	shine.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

}

static void emmitParticles(double time, const ppl7::grafix::PointF& p)
{
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.286));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(107, 145, 104, 29)));
	color_gradient.push_back(Particle::ColorGradientItem(0.279, ppl7::grafix::Color(155, 255, 160, 29)));
	color_gradient.push_back(Particle::ColorGradientItem(0.653, ppl7::grafix::Color(125, 237, 216, 21)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(84, 100, 69, 0)));

	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(200, 300);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.104, 1.235) + time;
		particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(49, 1), 0.000);
		particle->layer=Particle::Layer::BeforePlayer;
		particle->weight=randf(0.000, 0.000);
		particle->gravity.setPoint(0.000, 0.000);
		particle->velocity=calculateVelocity(randf(1.396, 3.981), 0.000 + randf(-10.189, 10.189));
		particle->scale=randf(0.723, 1.799);
		particle->color_mod.set(210, 255, 203, 29);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		particle->initScaleGradient(scale_gradient, particle->scale);
		particle->initColorGradient(color_gradient);
		ps->addParticle(particle);
	}
}


void Skull::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (!enabled) return;
	if (time > next_animation) {
		next_animation=time + 0.033f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}

	if (state == 0) {
		speed+=0.05f * frame_rate_compensation;
		p.y-=speed;
		if (p.y < initial_p.y - 5.0f) state=1;
	} else if (state == 1) {
		speed-=0.1f * frame_rate_compensation;
		if (speed < 0) {
			state=3;
			speed=0.0f;
		} else p.y-=speed;
	} else if (state == 3) {
		speed+=0.05f * frame_rate_compensation;
		p.y+=speed;
		if (p.y >= initial_p.y) state=4;
	} else if (state == 4) {
		speed-=0.1f * frame_rate_compensation;
		if (speed < 0) {
			state=0;
			speed=0.0f;
		} else p.y+=speed;
	} else if (state == 5) {
		speed+=0.1f * frame_rate_compensation;
		p.y+=speed;
		if (p.y > initial_p.y + 1080) {
			state=6;
			enabled=false;
		}

	}
	if (player.isFlashlightOn() && state < 5) {
		float y_dist=abs((player.y - 40) - p.y);
		float x_dist=abs(player.x - p.x);
		if (y_dist < 2 * TILE_HEIGHT && x_dist < 4 * TILE_WIDTH) {
			if ((player.orientation == Player::PlayerOrientation::Left && p.x < player.x) ||
				(player.orientation == Player::PlayerOrientation::Right && p.x > player.x)) {
					// In light cone
				state=5;
				animation.startSequence(24, 35, false, 35);
				collisionDetection=false;
				speed=1.0f;
				player.addPoints(20);
				emmitParticles(time, p);
				myLayer = Layer::BeforeBricks;

			}
		}
	}
	light.custom_texture=this->texture;
	light.sprite_no=36 + sprite_no;
	light.x=p.x;
	light.y=p.y;
	shine.x=p.x;
	shine.y=p.y - 1;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&shine);
	lights.addObjectLight(&light);
}

void Skull::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(0.1f);
}



}	// EOF namespace Decker::Objects
