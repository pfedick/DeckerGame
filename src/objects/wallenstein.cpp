#include <ppl7.h>
#include <ppl7-grafix.h>
#include "wallenstein.h"
#include "audiopool.h"
#include "player.h"
#include "waynet.h"
#include "objects.h"

namespace Decker::Objects {

static int run_cycle_left[]={ 61,62,63,64,65,66,67,68 };
static int run_cycle_right[]={ 70,71,72,73,74,75,76,77 };

//static int attack_walk_left[]={107,108,109,110,111,112,113,114,115};
//static int attack_walk_right[]={116,117,118,119,120,121,122,123,124};
static int attack_run_left[]={ 126,127,128,129,130,131,132,133 };
static int attack_run_right[]={ 135,136,137,138,139,140,151,142 };
static int attack_left[]={ 144,145,147,148,149,150,151,152,153 };
static int attack_right[]={ 154,155,157,158,159,160,161,162,163 };


Representation Wallenstein::representation()
{
	return Representation(Spriteset::Wallenstein, 27);
}

Wallenstein::Wallenstein()
	:AiEnemy(Type::ObjectType::Wallenstein)
{
	sprite_set=Spriteset::Wallenstein;
	sprite_no=27;
	next_state=ppl7::GetMicrotime() + 5.0f;
	state=StateWaitForEnable;
	animation.setStaticFrame(27);
	substate=0;
	speed_walk=2.0f;
	speed_run=4.5f;
	attack=false;
}

void Wallenstein::handleCollision(Player* player, const Collision& collision)
{
	if (attack)	player->dropHealth(1);
}


void Wallenstein::toggle(bool enable, Object* source)
{
	if (enable && state == StateWaitForEnable) state=StatePatrol;
}

void Wallenstein::switchAttackMode(bool enable)
{
	attack=enable;
	if (attack) {
		anicycleStandLeft.start(attack_left, sizeof(attack_left) / sizeof(int), true, 0);
		anicycleStandRight.start(attack_right, sizeof(attack_right) / sizeof(int), true, 0);
		anicycleRunLeft.start(attack_run_left, sizeof(attack_run_left) / sizeof(int), true, 0);
		anicycleRunRight.start(attack_run_right, sizeof(attack_run_right) / sizeof(int), true, 0);
	} else {
		anicycleRunLeft.start(run_cycle_left, sizeof(run_cycle_left) / sizeof(int), true, 0);
		anicycleRunRight.start(run_cycle_right, sizeof(run_cycle_right) / sizeof(int), true, 0);
		anicycleStandLeft.setStaticFrame(0);
		anicycleStandRight.setStaticFrame(9);
	}
}

void Wallenstein::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
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
	double dist=ppl7::grafix::Distance(p, player.position());
	if (state == StateWaitForEnable && dist < 800) state=StatePatrol;
	if (state != StateFollowPlayer && dist < 600) {
		state=StateFollowPlayer;
		clearWaypoints();
	}
	if (dist < 300 && attack == false) {
		switchAttackMode(true);
	} else if (dist > 350 && attack == true) {
		switchAttackMode(false);
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
		stand();
	}

	updateMovementAndPhysics(time, ttplane, frame_rate_compensation);

	if (movement == Slide || movement == Dead || movement == Jump) {
		return;
	}
	keys=0;
	if (state == StatePatrol) updateStatePatrol(time, ttplane);
	if (state == StateFollowPlayer) {
		if (dist < 128) {
			if (movement != Stand) stand();
		} else {
			updateStateFollowPlayer(time, ttplane, ppl7::grafix::Point((int)player.x, (int)player.y));
		}

	}
	executeKeys(frame_rate_compensation);

}

void Wallenstein::updateStatePatrol(double time, TileTypePlane& ttplane)
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
		keys=KeyboardKeys::Left;
	} else {
		keys=KeyboardKeys::Right;
	}
}

}
