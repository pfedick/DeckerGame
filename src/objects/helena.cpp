#include <ppl7.h>
#include <ppl7-grafix.h>
#include "helena.h"
#include "audiopool.h"
#include "player.h"
#include "waynet.h"
#include "objects.h"

namespace Decker::Objects {


class BulletFired : public Object
{
	friend class Helena;
private:
	ppl7::grafix::Point velocity;
public:
	BulletFired();
	static Representation representation();
	virtual void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	virtual void handleCollision(Player* player, const Collision& collision);
};


BulletFired::BulletFired()
	:Object(Type::ObjectType::Arrow)
{
	collisionDetection=true;
	pixelExactCollision=false;
	sprite_set=Spriteset::GenericObjects;
	sprite_no=240;
	sprite_no_representation=240;
	spawned=true;
}

Representation BulletFired::representation()
{
	return Representation(Spriteset::GenericObjects, 239);
}


void BulletFired::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	float update=velocity.x * frame_rate_compensation;
	float mul=1.0f;
	if (update < 0) mul=-1.0f;


	for (float i=0.0f;i < fabs(update);i+=16.0f) {
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + (i * mul), p.y));
		if (t1 == TileType::Blocking) {
			deleteDefered=true;
			getAudioPool().playOnce(AudioClip::bullet_hits_wall, p, 1600, 0.4f);

		} else if (p.x < 0 || p.x>65535 || p.y < 0 || p.y>65535) {
			deleteDefered=true;
		}
	}
	p.x+=update;
	updateBoundary();

}

static void issueBlood(const ppl7::grafix::PointF& p, float degree, double time)
{

	std::list<Particle::ColorGradientItem>color_gradient;
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(156, 0, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(0.428, ppl7::grafix::Color(156, 0, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(156, 0, 0, 0)));

	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(30, 90);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.293, 0.293) + time;
		particle->p=p;
		particle->layer=Particle::Layer::BehindPlayer;
		particle->weight=randf(0.123, 0.774);
		particle->gravity.setPoint(0.000, 1.000);
		particle->velocity=calculateVelocity(randf(4.000, 7.300), degree + randf(-18.679, 18.679));
		particle->scale=randf(0.066, 0.481);
		particle->color_mod.set(183, 0, 0, 255);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		ps->addParticle(particle);
	}
}

void BulletFired::handleCollision(Player* player, const Collision& collision)
{
	deleteDefered=true;
	player->dropHealth(10);
	if (velocity.x < 0)issueBlood(p, 54.0f, player->time);
	else issueBlood(p, 306.0f, player->time);
	getAudioPool().playOnce(AudioClip::bullet_hits_player, p, 1600, 0.7f);
}

Representation Helena::representation()
{
	return Representation(Spriteset::Helena, 27);
}

Helena::Helena()
	:AiEnemy(Type::ObjectType::Helena)
{
	sprite_set=Spriteset::Helena;
	sprite_no=27;
	next_state=ppl7::GetMicrotime() + 2.0f;
	state=StatePatrol;
	animation.setStaticFrame(27);
	keys=0;
	substate=0;
	speed_walk=2.0f;
	speed_run=4.5f;
	shoot_cooldown=0.0f;
	attack=false;
	magazine=8;
}



void Helena::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(1 * collision.frame_rate_compensation);
}


void Helena::toggle(bool enable, Object* source)
{
	if (enable && state == StateWaitForEnable) state=StatePatrol;
}

void Helena::switchAttackMode(bool enable)
{
	attack=enable;
	if (attack) {
		getAudioPool().playOnce(AudioClip::holster_pistol, p, 1600, 0.7f);
		anicycleStandLeft.setStaticFrame(144);
		anicycleStandRight.setStaticFrame(153);
		anicycleRunLeft.startSequence(107, 115, true, 0);
		anicycleRunRight.startSequence(116, 124, true, 0);
	} else {
		getAudioPool().playOnce(AudioClip::fabric, p, 1600, 0.7f);
		anicycleRunLeft.startSequence(61, 68, true, 0);
		anicycleRunRight.startSequence(70, 77, true, 0);
		anicycleStandLeft.setStaticFrame(0);
		anicycleStandRight.setStaticFrame(9);
	}
}

void Helena::shoot(double time, Player& player)
{
	if (shoot_cooldown > time) return;
	if (magazine <= 0) {
		magazine=8;
		shoot_cooldown=time + 4.0f;
		return;
	}
	if (orientation == Left && player.x > p.x) return;
	if (orientation == Right && player.x < p.x) return;
	if (movement == Jump || movement == Falling) return;


	shoot_cooldown=time + 0.5f;

	BulletFired* particle=new BulletFired();
	particle->p.x=p.x;
	particle->p.y=p.y - 98;

	particle->spawned=true;
	if (player.x < p.x) {
		if (movement == Stand) {
			animation.startSequence(144, 152, false, 144);
		}
		particle->velocity.x=-20;
		particle->p.x=p.x - 74;
		issueSmoke(ppl7::grafix::PointF(p.x - 55, p.y - 98));
	} else {
		if (movement == Stand) {
			animation.startSequence(153, 161, false, 153);

		}
		particle->velocity.x=20;
		particle->p.x=p.x + 74;
		issueSmoke(ppl7::grafix::PointF(p.x + 55, p.y - 98));
	}
	issueFireParticles(particle->p, player);
	particle->initial_p=particle->p;
	GetObjectSystem()->addObject(particle);
	getAudioPool().playOnce(AudioClip::shoot, p, 1600, 0.6f);
	magazine--;
}

void Helena::issueFireParticles(const ppl7::grafix::PointF& p, Player& player) const
{
	std::list<Particle::ColorGradientItem>color_gradient;
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(255, 218, 41, 53)));
	color_gradient.push_back(Particle::ColorGradientItem(0.117, ppl7::grafix::Color(255, 135, 0, 128)));
	color_gradient.push_back(Particle::ColorGradientItem(0.351, ppl7::grafix::Color(140, 0, 0, 112)));
	color_gradient.push_back(Particle::ColorGradientItem(0.986, ppl7::grafix::Color(0, 0, 0, 0)));

	float direction=90;
	if (player.x < p.x) direction=270;
	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(81, 171);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.010, 0.198) + time;
		particle->p=p;
		particle->initColorGradient(color_gradient);
		particle->layer=Particle::Layer::BehindPlayer;
		particle->weight=randf(0.000, 0.000);
		particle->gravity.setPoint(0.000, 0.000);
		particle->velocity=calculateVelocity(randf(5.660, 11.698), direction + randf(-8.491, 8.491));
		particle->scale=randf(0.066, 0.434);
		particle->color_mod.set(255, 255, 255, 255);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		ps->addParticle(particle);
	}
}

void Helena::issueSmoke(const ppl7::grafix::PointF& p) const
{
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;

	scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.010));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(213, 206, 205, 113)));
	color_gradient.push_back(Particle::ColorGradientItem(0.126, ppl7::grafix::Color(46, 51, 52, 126)));
	color_gradient.push_back(Particle::ColorGradientItem(0.941, ppl7::grafix::Color(201, 199, 198, 0)));


	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(143, 194);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.010, 0.387) + time;
		particle->p=p;
		particle->layer=Particle::Layer::BehindPlayer;
		particle->weight=randf(0.000, 0.000);
		particle->gravity.setPoint(0.000, 0.000);
		particle->velocity=calculateVelocity(randf(6.038, 9.623), 0.000 + randf(-25.472, 25.472));
		particle->scale=randf(0.911, 1.987);
		particle->color_mod.set(255, 255, 255, 255);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		particle->initScaleGradient(scale_gradient, particle->scale);
		particle->initColorGradient(color_gradient);
		ps->addParticle(particle);

	}
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


void Helena::playSoundOnAnimationSprite()
{
	AudioPool& ap=getAudioPool();
	//if (sprite_no == 245 || sprite_no == 224)  ap.playOnce(AudioClip::hackstone, 1.0f);
	if (sprite_no == 3 || sprite_no == 7 || sprite_no == 12 || sprite_no == 16 || sprite_no == 64 || sprite_no == 68
		|| sprite_no == 73 || sprite_no == 77) play_step(ap, p);
	// with weapon
	if (sprite_no == 110 || sprite_no == 114 || sprite_no == 119 || sprite_no == 123 || sprite_no == 129 || sprite_no == 133
		|| sprite_no == 138 || sprite_no == 142) play_step(ap, p);
}

void Helena::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	//printf ("s=%d, state=%s, keys=%d\n", state, (const char*)getState(), keys);
	this->time=time;
	if (!enabled) return;
	updateAnimation(time);
	if (movement == Dead) {
		if (animation.isFinished()) {
			enabled=false;
		}
		return;
	}
	AudioPool& ap=getAudioPool();
	if (movement == Jump || movement == Falling || movement == Slide || gravity >= 1.0f) {
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
	if (state == StateWaitForEnable && dist < 800) state=StatePatrol;
	if (!player.isDead()) {
		if (state != StateFollowPlayer && dist < 600) {
			state=StateFollowPlayer;
			clearWaypoints();
		}
		if (state == StateFollowPlayer) {
			if (dist < 1000 && attack == false) {
				shoot_cooldown=time + 1.0f;
				switchAttackMode(true);
			} else if (dist > 1100 && attack == true) {
				switchAttackMode(false);
			} else if (dist < 300 && abs(p.y - player.y) < 30 && attack == true && movement == Stand && movement != Turn) {
				if (orientation == Right && player.x < p.x) {
					//orientation=Left;
					//stand();
					turn(Left);
					return;
				} else if (orientation == Left && player.x > p.x) {
					//orientation=Right;
					//stand();
					turn(Right);
					return;
				}

			}
			if (attack && abs(player.y - p.y) < 20) shoot(time, player);
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
	if (state == StatePatrol) updateStatePatrol(time, ttplane);
	if (state == StateFollowPlayer) {
		if (abs(player.x - p.x) < 300 && abs(player.y - p.y) < 20) {
			if (movement != Stand) {
				stand();
				return;
			}
		} else {
			updateStateFollowPlayer(time, ttplane, ppl7::grafix::Point((int)player.x, (int)player.y));
		}
	}
	executeKeys(frame_rate_compensation);

}

void Helena::updateStatePatrol(double time, TileTypePlane& ttplane)
{
	//printf ("movement=%s, keys=%d, time=%d, next_state=%d\n", (const char*)getState(),
	//		keys, (int)time, (int)next_state);
	if (movement == Turn) return;
	if (movement == Stand && next_state < time && substate == 0) {
		//printf ("next_state is turn\n");
		if (orientation == Left) turn(Right);
		else turn(Left);
		substate=1;
		return;
	} else if (movement == Stand && substate == 0) {
		return;
	}
	substate=0;
	if (orientation == Left) {
		//printf("move left\n");
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y - 64));
		if (t1 != TileType::NonBlocking) {
			stand();
			next_state=time + ppl7::randf(1.0f, 5.0f);
		} else keys=KeyboardKeys::Left;
	} else {
		//printf("move right\n");
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y - 64));
		if (t1 != TileType::NonBlocking) {
			stand();
			next_state=time + ppl7::randf(1.0f, 5.0f);
		} else keys=KeyboardKeys::Right;
	}
}



}
