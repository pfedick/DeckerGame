#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Spider::representation()
{
	return Representation(Spriteset::Spider, 0);
}

Spider::Spider()
	:Enemy(Type::ObjectType::Spider)
{
	sprite_set=Spriteset::Spider;
	sprite_no=0;
	sprite_no_representation=0;
	state=SpiderState::Falling;
	collisionDetection=true;
	next_animation=0.0f;
	next_state_change=ppl7::randf(0.1f, 2.0f);
	audio=NULL;
	collision_cooldown=0.0f;
	velocity=2.0f;
	velocity_falling=0.0f;
}

Spider::~Spider()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

static void splatterBlood(double time, const ppl7::grafix::PointF& p)
{
	if (GetGame().config.noBlood) return;
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.005, 0.314));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.455, 1.000));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 0.010));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(255, 0, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(0.342, ppl7::grafix::Color(137, 0, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(157, 0, 0, 0)));
	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(107, 135);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.293, 0.764) + time;
		particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(49, 20), 0.000);
		particle->layer=Particle::Layer::BeforePlayer;
		particle->weight=randf(0.406, 0.642);
		particle->gravity.setPoint(0.000, 0.377);
		particle->velocity=calculateVelocity(randf(2.642, 5.849), 0.000 + randf(-54.340, 54.340));
		particle->scale=randf(0.300, 0.622);
		particle->color_mod.set(230, 0, 0, 255);
		particle->initAnimation(Particle::Type::StaticParticle);
		particle->initScaleGradient(scale_gradient, particle->scale);
		particle->initColorGradient(color_gradient);
		ps->addParticle(particle);
	}
}

void Spider::handleCollision(Player* player, const Collision& collision)
{
	if (state == SpiderState::Dead) return;
	if (player->velocity_move.y > 0.0f || player->gravity > 0.0f) {
		collisionDetection=false;
		splatterBlood(player->time, p);
		state=SpiderState::Dead;
		animation.startSequence(29, 34, false, 34);
		next_state_change=player->time + 5.0f;
		player->addPoints(50);
		getAudioPool().playOnce(AudioClip::squash1, 0.3f);
		if (audio) {
			getAudioPool().stopInstace(audio);
			delete audio;
			audio=NULL;
		}
		return;
	}


	if (collision_cooldown < player->time) {
		player->dropHealth(5);
		collision_cooldown=player->time + 0.5f;
	}
}

void Spider::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	//AudioPool& audiopool=getAudioPool();

	if (time > next_animation) {
		next_animation=time + 0.03f;
		animation.update();
		if (animation.getFrame() != sprite_no) {
			sprite_no=animation.getFrame();
		}
	}
	//ppl7::PrintDebug("Spider state: %d\n", static_cast<int>(state));
	if (state == SpiderState::StandFront && next_state_change < time) {
		if (ppl7::rand(0, 1) == 0) {
			state=SpiderState::WalkLeft;
			animation.startSequence(2, 14, true, 2);
		} else {
			state=SpiderState::WalkRight;
			animation.startSequence(16, 28, true, 16);

		}
	} else if (state == SpiderState::Falling) {
		if (velocity_falling < 12.0f) {
			velocity_falling+=0.2 * frame_rate_compensation;
			if (velocity_falling > 12.0f) velocity_falling=12.0f;
		}
		//ppl7::PrintDebug("Falling: %0.3f\n", velocity_falling);
		for (int yy=p.y;yy <= p.y + velocity_falling;yy++) {
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, yy));
			if (t1 != TileType::NonBlocking) {
				state=SpiderState::StandFront;
				p.y=yy - 1;
				next_state_change=ppl7::randf(0.1f, 2.0f);
				break;
			}
		}
		if (state == SpiderState::Falling) p.y+=velocity_falling;

	} else if (state == SpiderState::WalkLeft) {
		p.x-=velocity * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=SpiderState::StandLeft;
			animation.setStaticFrame(1);
			next_state_change=time + randf(0.5, 5.0f);
		}

	} else 	if (state == SpiderState::WalkRight) {
		p.x+=velocity * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=SpiderState::StandRight;
			animation.setStaticFrame(15);
			next_state_change=time + randf(0.5, 5.0f);
		}
	} else if (state == SpiderState::StandLeft && time > next_state_change) {
		state=SpiderState::WalkRight;
		animation.startSequence(16, 28, true, 16);
	} else if (state == SpiderState::StandRight && time > next_state_change) {
		state=SpiderState::WalkLeft;
		animation.startSequence(2, 14, true, 2);
	} else if (state == SpiderState::Dead && next_state_change < time) {
		enabled=false;
	}
	updateBoundary();
	/*
	if (!audio) {
		audio=audiopool.getInstance(AudioClip::crow_flying);
		if (audio) {
			audio->setVolume(1.0f);
			audio->setAutoDelete(false);
			audio->setLoop(true);
			audio->setPositional(p, 1600);
			audiopool.playInstance(audio);
		}
	} else if (audio) {
		audio->setPositional(p, 1600);
	}
	*/

}



}	// EOF namespace Decker::Objects
