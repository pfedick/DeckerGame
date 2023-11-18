#include <ppl7.h>
#include <ppl7-grafix.h>
#include "wallenstein.h"
#include "audiopool.h"
#include "player.h"
#include "waynet.h"
#include "objects.h"
#include "animation.h"


namespace Decker::Objects {

class KnifeThrown : public Object
{
	friend class Zombie;
private:
	ppl7::grafix::Point velocity;
	double next_animation;
	float gravity;
	AnimationCycle animation;
public:
	KnifeThrown();
	static Representation representation();
	virtual void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	virtual void handleCollision(Player* player, const Collision& collision);
};


KnifeThrown::KnifeThrown()
	:Object(Type::ObjectType::Arrow)
{
	collisionDetection=true;
	pixelExactCollision=false;
	sprite_set=Spriteset::Zombie;
	sprite_no=167;
	sprite_no_representation=167;
	spawned=true;
	gravity=0.0f;
	next_animation=0.0f;
}

Representation KnifeThrown::representation()
{
	return Representation(Spriteset::Zombie, 167);
}

void KnifeThrown::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (time > next_animation) {
		next_animation=time + 0.03f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	if (gravity < 5) gravity+=0.2 * frame_rate_compensation;

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
	p.y+=gravity;
	updateBoundary();

}

static void issueBlood(const ppl7::grafix::PointF& p, float degree, double time)
{
	if (GetGame().config.noBlood) return;
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

void KnifeThrown::handleCollision(Player* player, const Collision& collision)
{
	deleteDefered=true;
	player->dropHealth(10);
	ppl7::grafix::PointF pos=p;
	pos.x=player->x;
	if (pos.y < player->y - 4 * TILE_HEIGHT) pos.y=player->y - 4 * TILE_HEIGHT;
	if (pos.y > player->y) pos.y=player->y;
	if (velocity.x < 0)issueBlood(pos, 54.0f, player->time);
	else issueBlood(pos, 306.0f, player->time);
	getAudioPool().playOnce(AudioClip::bullet_hits_player, p, 1600, 0.7f);
}


/*********************************************************************************
 * Zombie
 *********************************************************************************/


Representation Zombie::representation()
{
	return Representation(Spriteset::Zombie, 27);
}

Zombie::Zombie()
	:AiEnemy(Type::ObjectType::Zombie)
{
	sprite_set=Spriteset::Zombie;
	sprite_no=27;
	next_state=ppl7::GetMicrotime() + 5.0f;
	state=StatePatrol;
	substate=0;
	animation.setStaticFrame(27);
	keys=0;
	substate=0;
	speed_walk=1.6f;
	speed_run=4.0f;
	attack=false;
	myLayer=Layer::BeforePlayer;
	//last_sprite_no=0;
	shoot_cooldown=0.0f;
	knifeThrown=false;
}


static void play_step(AudioPool& ap, const ppl7::grafix::PointF& position)
{
	int r=ppl7::rand(1, 5);
	switch (r) {
	case 1: ap.playOnce(AudioClip::yeti_step1, position, 1600, 0.5f); break;
	case 2: ap.playOnce(AudioClip::yeti_step2, position, 1600, 0.5f); break;
	case 3: ap.playOnce(AudioClip::yeti_step3, position, 1600, 0.5f); break;
	case 4: ap.playOnce(AudioClip::yeti_step4, position, 1600, 0.5f); break;
	case 5: ap.playOnce(AudioClip::yeti_step5, position, 1600, 0.5f); break;
	default: ap.playOnce(AudioClip::yeti_step1, position, 1600, 0.5f); break;
	}
}



void Zombie::playSoundOnAnimationSprite()
{
	AudioPool& ap=getAudioPool();
	//if (sprite_no == 245 || sprite_no == 224)  ap.playOnce(AudioClip::hackstone, 1.0f);
	if (sprite_no == 3 || sprite_no == 7 || sprite_no == 12 || sprite_no == 16 || sprite_no == 64 || sprite_no == 68
		|| sprite_no == 73 || sprite_no == 77) play_step(ap, p);

}


void Zombie::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(1 * collision.frame_rate_compensation);
}


void Zombie::toggle(bool enable, Object* source)
{
	if (enable && state == StateWaitForEnable) state=StatePatrol;
}

void Zombie::switchAttackMode(bool enable)
{
	attack=enable;
	if (attack) {
		//getAudioPool().playOnce(AudioClip::holster_pistol, p, 1600, 0.7f);
		anicycleStandLeft.setStaticFrame(0);
		anicycleStandRight.setStaticFrame(9);
		anicycleRunLeft.startSequence(61, 68, true, 0);
		anicycleRunRight.startSequence(70, 77, true, 0);
	} else {
		//getAudioPool().playOnce(AudioClip::fabric, p, 1600, 0.7f);
		anicycleRunLeft.startSequence(61, 68, true, 0);
		anicycleRunRight.startSequence(70, 77, true, 0);
		anicycleStandLeft.setStaticFrame(0);
		anicycleStandRight.setStaticFrame(9);
	}
}

void Zombie::shoot(double time, Player& player)
{
	if (shoot_cooldown > time) return;

	if (orientation == Left && player.x > p.x) return;
	if (orientation == Right && player.x < p.x) return;
	if (movement == Jump || movement == Falling) return;

	shoot_cooldown=time + 2.0f;
	animation_speed=0.03f;

	movement=Stand;
	state=StateThrowKnife;
	knifeThrown=false;

	if (player.x < p.x) {
		animation.startSequence(106, 135, false, 0);
	} else {
		animation.startSequence(136, 166, false, 9);
	}


	// TODO
	//getAudioPool().playOnce(AudioClip::shoot, p, 1600, 0.6f);

}

void Zombie::throwKnife(Player& player)
{
	knifeThrown=true;;
	KnifeThrown* particle=new KnifeThrown();
	particle->p.x=p.x;
	particle->p.y=p.y - 180;
	particle->next_animation=time + 0.03f;
	particle->spawned=true;
	particle->p.x=p.x;
	if (player.x < p.x) {
		particle->velocity.x=-15;
		particle->animation.startSequence(167, 174, true, 167);
	} else {
		particle->velocity.x=15;
		particle->animation.startSequence(175, 182, true, 175);
	}
	particle->initial_p=particle->p;
	GetObjectSystem()->addObject(particle);
}


void Zombie::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	this->time=time;
	if (!enabled) return;
	updateAnimation(time);
	if (sprite_no == 131 || sprite_no == 162) {
		if (!knifeThrown) throwKnife(player);
	}


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
		ap.playOnce(AudioClip::yeti_jump, p, 1600, volume);
	}


	double dist=ppl7::grafix::Distance(p, player.position());
	//ppl7::PrintDebugTime("dist=%0.3f\n", dist);
	if (state == StateWaitForEnable && dist < 800) state=StatePatrol;
	if (state == StateThrowKnife) {
		if (animation.isFinished()) {
			state=StateFollowPlayer;
			animation_speed=0.07f;
		}
	} else {
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
				if (attack && abs(player.y - p.y) < 2 * TILE_HEIGHT) shoot(time, player);
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
		if (abs(player.x - p.x) < 400 && abs(player.y - p.y) < 20) {
			if (movement != Stand) {
				stand();
			}
			return;
		} else {
			updateStateFollowPlayer(time, ttplane, ppl7::grafix::Point((int)player.x, (int)player.y));
		}
	}
	if (state != StateThrowKnife)	executeKeys(frame_rate_compensation);

}

void Zombie::updateStatePatrol(double time, TileTypePlane& ttplane)
{
	//printf("Yeti::updateStatePatrol: movement=%s, keys=%d, time=%d, next_state=%d\n", (const char*)getState(),
	//	keys, (int)time, (int)next_state);
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
