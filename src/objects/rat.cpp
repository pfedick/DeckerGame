#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Rat::representation()
{
	return Representation(Spriteset::Rat, 0);
}

Rat::Rat()
	:Enemy(Type::ObjectType::Rat)
{
	sprite_set=Spriteset::Rat;
	sprite_no=0;
	sprite_no_representation=0;
	state=RatState::idle;
	collisionDetection=true;
	audio=NULL;
	next_state=0.0f;
	speed=0.0f;
	next_animation=0.0f;
	int r=ppl7::rand(128, 255);
	int r2=ppl7::rand(r, 255);
	color_mod.setColor(r2, r, r, 255);

}

Rat::~Rat()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

static void splatterBlood(double time, const ppl7::grafix::PointF& p)
{
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


void Rat::handleCollision(Player* player, const Collision& collision)
{
	if (state == RatState::dead) return;
	//Player::PlayerMovement movement=player->getMovement();
	//ppl7::PrintDebugTime("player v: %0.3f, gravity: %0.3f\n", player->velocity_move.y, player->gravity,
	//		(int)movement);
	if (player->x<p.x - 48 || player->x>p.x + 48) return;
	if (player->velocity_move.y > 0.0f || player->gravity > 0.0f) {
		if (ppl7::rand(0, 1) == 0) {
			// run away
			speed=12.0f;
			next_state=player->time + 10.0f;
			if (state == RatState::walk_left || state == RatState::wait_left || state == RatState::turn_right_to_left) {
				state=RatState::walk_right;
				animation.startSequence(15, 29, true, 15);

			} else {
				state=RatState::walk_left;
				animation.startSequence(0, 14, true, 0);
			}
			player->dropHealth(2);
			collisionDetection=false;
			return;
		}
		collisionDetection=false;
		splatterBlood(player->time, p);
		//enabled=false;
		if (state == RatState::walk_left || state == RatState::wait_left || state == RatState::turn_right_to_left)
			sprite_no=61;
		else sprite_no=62;
		state=RatState::dead;

		player->addPoints(50);
		getAudioPool().playOnce(AudioClip::squash1, 0.3f);
		if (audio) {
			getAudioPool().stopInstace(audio);
			delete audio;
			audio=NULL;
		}
	} else {
		if (player->hasSpecialObject(Type::ObjectType::Cheese)) return;
		player->dropHealth(2);
	}
}

void Rat::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	//ppl7::PrintDebugTime("rat %d: state=%d, next_state=%0.3f, time=%0.3f, sprite: %d, animation: %d\n", id, (int)state, next_state, time, sprite_no, animation.isFinished());
	if (time > next_animation && state != RatState::dead) {
		next_animation=time + 0.03f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	if (ppl7::rand(0, 20) == 0)speed=ppl7::randf(4.0f, 10.0f);
	if (state == RatState::idle) {
		// Fix initial position from older savegames
		while (ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1)) != TileType::NonBlocking) p.y--;
		while (ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1)) != TileType::Blocking) p.y++;


		int r=ppl7::rand(0, 1);
		if (r == 0) {
			state=RatState::walk_left;
			animation.startSequence(0, 14, true, 0);
			speed=ppl7::randf(4.0f, 10.0f);
			next_state=time + ppl7::randf(1.0f, 10.0f);
		} else {
			state=RatState::walk_right;
			animation.startSequence(15, 29, true, 15);
			speed=ppl7::randf(4.0f, 10.0f);
			next_state=time + ppl7::randf(1.0f, 10.0f);
		}
	} else if (state == RatState::walk_left) {
		p.x-=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking || next_state < time) {
			collisionDetection=true;
			state=static_cast<RatState>(ppl7::rand(static_cast<int>(RatState::wait_left),
				static_cast<int>(RatState::turn_left_to_right)));
			if (state == RatState::wait_left) {
				animation.startSequence(30, 39, true, 30);
				next_state=time + ppl7::randf(0.5f, 5.0f);
			} else animation.startSequence(55, 59, false, 60);
		}
	} else if (state == RatState::wait_left && time > next_state) {
		animation.startSequence(55, 59, false, 15);
		state=RatState::turn_left_to_right;
	} else if (state == RatState::turn_left_to_right && animation.isFinished()) {
		state=RatState::walk_right;
		speed=ppl7::randf(4.0f, 10.0f);
		next_state=time + ppl7::randf(1.0f, 10.0f);
		animation.startSequence(15, 29, true, 15);
	} else if (state == RatState::walk_right) {
		p.x+=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking || next_state < time) {
			collisionDetection=true;
			state=static_cast<RatState>(ppl7::rand(static_cast<int>(RatState::wait_right),
				static_cast<int>(RatState::turn_right_to_left)));
			if (state == RatState::wait_right) {
				animation.startSequence(40, 49, true, 40);
				next_state=time + ppl7::randf(1.5f, 5.0f);
			} else animation.startSequence(50, 54, false, 0);
		}
	} else if (state == RatState::wait_right && time > next_state) {
		animation.startSequence(50, 54, false, 0);
		state=RatState::turn_right_to_left;
	} else if (state == RatState::turn_right_to_left && animation.isFinished()) {
		state=RatState::walk_left;
		next_state=time + ppl7::randf(1.0f, 10.0f);
		speed=ppl7::randf(4.0f, 10.0f);
		animation.startSequence(0, 14, true, 0);
	}
	if (!audio && state != RatState::dead) {
		AudioPool& pool=getAudioPool();
		audio=pool.getInstance(AudioClip::rat_squeek);
		if (audio) {
			audio->setVolume(0.4f);
			audio->setAutoDelete(false);
			audio->setLoop(true);
			audio->setPositional(p, 960);
			pool.playInstance(audio);
		}
	} else if (audio) {
		audio->setPositional(p, 960);
	}
}








}	// EOF namespace Decker::Objects
