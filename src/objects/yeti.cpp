#include <ppl7.h>
#include <ppl7-grafix.h>
#include "audiopool.h"
#include "player.h"
#include "waynet.h"
#include "objects.h"
#include "ai_figure.h"

namespace Decker::Objects {


Representation Yeti::representation()
{
	return Representation(Spriteset::Yeti, 27);
}

Yeti::Yeti()
	:AiEnemy(Type::ObjectType::Yeti)
{
	sprite_set=Spriteset::Yeti;
	sprite_no=27;
	next_state=ppl7::GetMicrotime() + 5.0f;
	state=StatePatrol;
	animation.setStaticFrame(27);
	keys=0;
	substate=0;
	speed_walk=3.0f;
	speed_run=5.0f;
	attack=false;
	myLayer=Layer::BeforePlayer;
	last_sprite_no=0;
}


static void play_step(AudioPool& ap, const ppl7::grafix::PointF& position)
{
	int r=ppl7::rand(1, 5);
	switch (r) {
	case 1: ap.playOnce(AudioClip::helena_step1, position, 1600, 0.5f); break;
	case 2: ap.playOnce(AudioClip::helena_step2, position, 1600, 0.5f); break;
	case 3: ap.playOnce(AudioClip::helena_step3, position, 1600, 0.5f); break;
	case 4: ap.playOnce(AudioClip::helena_step4, position, 1600, 0.5f); break;
	case 5: ap.playOnce(AudioClip::helena_step5, position, 1600, 0.5f); break;
	default: ap.playOnce(AudioClip::helena_step1, position, 1600, 0.5f); break;
	}
}

static void issueBlood(const ppl7::grafix::PointF& p, float degree, double time)
{

	std::list<Particle::ColorGradientItem>color_gradient;
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(156, 0, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(0.428, ppl7::grafix::Color(156, 0, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(156, 0, 0, 0)));

	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(4, 20);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.123, 0.233) + time;
		particle->p=p;
		particle->layer=Particle::Layer::BeforePlayer;
		particle->weight=randf(0.123, 0.774);
		particle->gravity.setPoint(0.000, 1.000);
		particle->velocity=calculateVelocity(randf(4.000, 7.300), degree + randf(-180, 180));
		particle->scale=randf(0.066, 0.481);
		particle->color_mod.set(183, 0, 0, 255);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		ps->addParticle(particle);
	}
}


void Yeti::playSoundOnAnimationSprite()
{
	AudioPool& ap=getAudioPool();
	//if (sprite_no == 245 || sprite_no == 224)  ap.playOnce(AudioClip::hackstone, 1.0f);
	if (sprite_no == 3 || sprite_no == 7 || sprite_no == 12 || sprite_no == 16 || sprite_no == 64 || sprite_no == 68
		|| sprite_no == 73 || sprite_no == 77) play_step(ap, p);
	// attack mode
	if (sprite_no == 110 || sprite_no == 114 || sprite_no == 119 || sprite_no == 123 || sprite_no == 129 || sprite_no == 133
		|| sprite_no == 138 || sprite_no == 142) play_step(ap, p);

}



void Yeti::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(1 * collision.frame_rate_compensation);
}


void Yeti::toggle(bool enable, Object* source)
{
	if (enable && state == StateWaitForEnable) state=StatePatrol;
}

void Yeti::switchAttackMode(bool enable)
{
	attack=enable;
	if (attack) {
		//getAudioPool().playOnce(AudioClip::holster_pistol, p, 1600, 0.7f);
		anicycleStandLeft.setStaticFrame(106);
		anicycleStandRight.setStaticFrame(115);
		anicycleRunLeft.startSequence(106, 114, true, 0);
		anicycleRunRight.startSequence(115, 123, true, 0);
	} else {
		//getAudioPool().playOnce(AudioClip::fabric, p, 1600, 0.7f);
		anicycleRunLeft.startSequence(61, 68, true, 0);
		anicycleRunRight.startSequence(70, 77, true, 0);
		anicycleStandLeft.setStaticFrame(0);
		anicycleStandRight.setStaticFrame(9);
	}
}


void Yeti::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	//printf ("s=%d, state=%s, keys=%d\n", state, (const char*)getState(), keys);
	myLayer=Layer::BeforePlayer;
	this->time=time;
	if (!enabled) return;
	updateAnimation(time);
	//if (sprite_no != last_sprite_no) {
	last_sprite_no=sprite_no;
	if (sprite_no == 124 || sprite_no == 126 || sprite_no == 128 || sprite_no == 130) {
		issueBlood(ppl7::grafix::PointF(player.x, player.y - 100), 0, time);
	}
	if (sprite_no == 125 || sprite_no == 131) {
		issueBlood(ppl7::grafix::PointF(player.x, player.y - 70), 0, time);
	}
	if (sprite_no == 127 || sprite_no == 129) {
		issueBlood(ppl7::grafix::PointF(player.x, player.y - 140), 0, time);
	}

//}
	if (movement == Dead) {
		if (animation.isFinished()) {
			enabled=false;
		}
		return;
	}

	AudioPool& ap=getAudioPool();
	if (!isOnGround()) {
		if (airStart == 0.0f) {
			airStart=time;
		}
	} else if (airStart > 0.0f) {
		double volume=(time - airStart) * 1.0f;
		if (volume > 1.0f) volume=1.0f;
		airStart=0.0f;
		ap.playOnce(AudioClip::helena_jump, p, 1600, volume);
	}


	double dist=ppl7::grafix::Distance(p, player.position());
	//if (state == StateWaitForEnable && dist < 800) state=StatePatrol;


	if (!player.isDead()) {
		if (state != StateFollowPlayer && dist < 800) {
			state=StateFollowPlayer;
			clearWaypoints();
		}
		if (state == StateFollowPlayer) {
			if (dist < 700 && attack == false) {
				switchAttackMode(true);
			} else if (dist > 800 && attack == true) {
				switchAttackMode(false);
				stand();
				state=StatePatrol;
				if (ppl7::rand(0, 1) == 0) turn(Left);
				else turn(Right);

			} else if (dist < 300 && abs(p.y - player.y) < 30 && attack == true && movement == Stand && movement != Turn) {
				if (orientation == Right && player.x < p.x) {
					turn(Left);
					return;
				} else if (orientation == Left && player.x > p.x) {
					turn(Right);
					return;
				}
			}
			//if (attack && abs(player.y - p.y) < 20) shoot(time, player);
		}
	} else if (attack) {
		switchAttackMode(false);
		state=StateStand;
		next_state=0.0f;
		clearWaypoints();
	}

	if (time < next_state && state == StateStand) {
		state=StatePatrol;
		if (ppl7::rand(0, 1) == 0) turn(Left);
		else turn(Right);
	}

	if (movement == Turn) {
		if (!animation.isFinished()) return;
		movement=Stand;
		orientation=turnTarget;
		velocity_move.stop();
	}

	updateMovementAndPhysics(time, ttplane, frame_rate_compensation);

	if (movement == Slide || movement == Dead || movement == Jump) {
		return;
	}
	keys=0;
	if (state == StatePatrol) {
		updateStatePatrol(time, ttplane);
	}
	if (state == StateFollowPlayer) {
		if (abs(player.x - p.x) < 80 && abs(player.y - p.y) < 20) {
			if (movement != Stand) {
				stand();
				if (orientation == Left)
					animation.startSequence(128, 131, true, 0);
				else animation.startSequence(124, 127, true, 0);
				return;
			}
		} else {
			updateStateFollowPlayer(time, ttplane, ppl7::grafix::Point((int)player.x, (int)player.y));
		}
	}

	executeKeys(frame_rate_compensation);

}

void Yeti::updateStatePatrol(double time, TileTypePlane& ttplane)
{
	//printf("Yeti::updateStatePatrol: movement=%s, keys=%d, time=%d, next_state=%d\n", (const char*)getState(),
	//	keys, (int)time, (int)next_state);
	if (movement == Turn) return;

	substate=0;
	if (orientation == Left) {
		//printf("move left\n");
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y - 64));
		if (t1 != TileType::NonBlocking) turn(Right);
		else keys=KeyboardKeys::Left;
	} else {
		//printf("move right\n");
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y - 64));
		if (t1 != TileType::NonBlocking) turn(Left);
		else keys=KeyboardKeys::Right;
	}
}



}
