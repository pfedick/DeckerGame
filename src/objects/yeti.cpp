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
	speed_walk=2.0f;
	speed_run=5.5f;
}



void Yeti::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(100);
}


void Yeti::toggle(bool enable, Object* source)
{
	if (enable && state == StateWaitForEnable) state=StatePatrol;
}

void Yeti::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
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
	/*
	if (state != StateFollowPlayer && dist < 600) {
		state=StateFollowPlayer;
		clearWaypoints();
	}
	*/


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
	if (state == StateFollowPlayer) updateStateFollowPlayer(time, ttplane, ppl7::grafix::Point((int)player.x, (int)player.y));
	executeKeys();

}

void Yeti::updateStatePatrol(double time, TileTypePlane& ttplane)
{
	//printf("Yeti::updateStatePatrol: movement=%s, keys=%d, time=%d, next_state=%d\n", (const char*)getState(),
	//	keys, (int)time, (int)next_state);
	if (movement == Turn) return;
	/*
	if (movement == Stand && next_state < time && substate == 0) {
		printf("next_state is turn\n");
		if (orientation == Left) turn(Right);
		else turn(Left);
		substate=1;
		return;
	} else if (movement == Stand && substate == 0) {
		return;
	}
	*/
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
