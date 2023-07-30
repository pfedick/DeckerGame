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

}

Rat::~Rat()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void Rat::handleCollision(Player* player, const Collision& collision)
{
	Player::PlayerMovement movement=player->getMovement();
	if (collision.onFoot() == true && movement == Player::Falling) {
		collisionDetection=false;
		enabled=false;
		state=RatState::dead;
		player->addPoints(50);
		getAudioPool().playOnce(AudioClip::squash1, 0.3f);
		if (audio) {
			getAudioPool().stopInstace(audio);
			delete audio;
			audio=NULL;
			ppl7::PrintDebugTime("audio aus\n");
		}
	} else {
		player->dropHealth(2);
	}
}

void Rat::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	//ppl7::PrintDebugTime("state=%d, next_state=%0.3f, time=%0.3f\n", (int)state, next_state, time);
	if (time > next_animation) {
		next_animation=time + 0.03f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	if (state == RatState::idle) {
		// Fix initial position from older savegames
		while (ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1)) != TileType::NonBlocking) p.y--;
		while (ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1)) != TileType::NonBlocking) p.y++;


		int r=ppl7::rand(0, 1);
		if (r == 0) {
			state=RatState::walk_left;
			animation.startSequence(0, 14, true, 0);
		} else {
			state=RatState::walk_right;
			animation.startSequence(15, 29, true, 15);
		}
	} else if (state == RatState::walk_left) {
		p.x-=2 * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=static_cast<RatState>(ppl7::rand(static_cast<int>(RatState::wait_left),
				static_cast<int>(RatState::turn_left_to_right)));
			if (state == RatState::wait_left) {
				animation.startSequence(30, 39, true, 30);
				next_state=time + ppl7::randf(0.5f, 5.0f);
			} else animation.startSequence(55, 59, false, 60);
		}
	} else if (state == RatState::wait_left && time > next_state) {
		animation.startSequence(55, 59, false, 60);
		state=RatState::turn_left_to_right;
	} else if (state == RatState::turn_left_to_right && animation.isFinished()) {
		state=RatState::walk_right;
		animation.startSequence(15, 29, true, 15);
	} else if (state == RatState::walk_right) {
		p.x+=2 * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=static_cast<RatState>(ppl7::rand(static_cast<int>(RatState::wait_right),
				static_cast<int>(RatState::turn_right_to_left)));
			if (state == RatState::wait_right) {
				animation.startSequence(40, 49, true, 40);
				next_state=time + ppl7::randf(0.5f, 5.0f);
			} else animation.startSequence(50, 54, false, 0);
		}
	} else if (state == RatState::wait_right && time > next_state) {
		animation.startSequence(50, 54, false, 0);
		state=RatState::turn_right_to_left;
	} else if (state == RatState::turn_right_to_left && animation.isFinished()) {
		state=RatState::walk_left;
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
