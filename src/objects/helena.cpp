#include <ppl7.h>
#include <ppl7-grafix.h>
#include "helena.h"
#include "audiopool.h"
#include "player.h"
#include "waynet.h"
#include "objects.h"

namespace Decker::Objects {


Representation Helena::representation()
{
	return Representation(Spriteset::Helena, 27);
}

Helena::Helena()
:AiEnemy(Type::ObjectType::Helena)
{
	sprite_set=Spriteset::Helena;
	sprite_no=27;
	next_state=ppl7::GetMicrotime()+5.0f;
	state=StateWaitForEnable;
	animation.setStaticFrame(27);
	keys=0;
	substate=0;
	speed_walk=2.0f;
	speed_run=5.5f;
}



void Helena::handleCollision(Player *player, const Collision &collision)
{

}


void Helena::toggle(bool enable, Object *source)
{
	if (enable && state==StateWaitForEnable) state=StatePatrol;
}

void Helena::update(double time, TileTypePlane &ttplane, Player &player)
{
	//printf ("s=%d, state=%s, keys=%d\n", state, (const char*)getState(), keys);
	this->time=time;
	if (!enabled) return;
	updateAnimation(time);
	if (movement==Dead) {
		if (animation.isFinished()) {
			enabled=false;
		}
		return;
	}
	double dist=ppl7::grafix::Distance(p, player.position());
	if (state==StateWaitForEnable && dist<800) state=StatePatrol;
	if (state!=StateFollowPlayer && dist<600) {
		state=StateFollowPlayer;
		clearWaypoints();
	}


	if (time<next_state && state==StateStand) {
		state=StatePatrol;
		if (ppl7::rand(0,1)==0) turn(Left);
		else turn(Right);
	}

	if (movement==Turn) {
		if (!animation.isFinished()) return;
		movement=Stand;
		orientation=turnTarget;
		velocity_move.stop();
	}

	updateMovementAndPhysics(time,ttplane);

	if (movement==Slide || movement==Dead || movement==Jump) {
		return;
	}
	keys=0;
	if (state==StatePatrol) updateStatePatrol(time, ttplane);
	if (state==StateFollowPlayer) updateStateFollowPlayer(time, ttplane, player);
	executeKeys();

}

void Helena::updateStatePatrol(double time, TileTypePlane &ttplane)
{
	//printf ("movement=%s, keys=%d, time=%d, next_state=%d\n", (const char*)getState(),
	//		keys, (int)time, (int)next_state);
	if (movement==Turn) return;
	if (movement==Stand && next_state<time && substate==0) {
		//printf ("next_state is turn\n");
		if (orientation==Left) turn(Right);
		else turn(Left);
		substate=1;
		return;
	} else if (movement==Stand && substate==0) {
		return;
	}
	substate=0;
	if (orientation==Left) {
		keys=KeyboardKeys::Left;
	} else {
		keys=KeyboardKeys::Right;
	}
}



}
