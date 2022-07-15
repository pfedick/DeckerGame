#ifndef INCLUDE_AI_FIGURE_H_
#define INCLUDE_AI_FIGURE_H_
#include <ppl7-grafix.h>
#include "animation.h"
#include "objects.h"
#include "physic.h"
#include "waynet.h"


class TileTypePlane;

namespace Decker::Objects {



class AiEnemy : public Enemy, public Physic
{
private:
	double next_animation, next_wayfind;
	//int state;
	//int substate;
	std::list<Connection> waypoints;
	WayPoint last_pwp;
	Connection current_way;


	void updateWay(double time, Player &player);


public:
	AnimationCycle animation;
	int keys;

	AnimationCycle anicycleWalkLeft;
	AnimationCycle anicycleWalkRight;
	AnimationCycle anicycleTurnFromLeftToMid;
	AnimationCycle anicycleTurnFromLeftToRight;
	AnimationCycle anicycleTurnFromRightToMid;
	AnimationCycle anicycleTurnFromRightToLeft;
	AnimationCycle anicycleTurnFromMidToLeft;
	AnimationCycle anicycleTurnFromMidToRight;
	AnimationCycle anicycleRunLeft;
	AnimationCycle anicycleRunRight;
	AnimationCycle anicycleClimbUp;
	AnimationCycle anicycleClimbDown;
	AnimationCycle anicycleSlideLeft;
	AnimationCycle anicycleSlideRight;
	AnimationCycle anicycleJumpUpFront;
	AnimationCycle anicycleJumpUpBack;
	AnimationCycle anicycleJumpUpLeft;
	AnimationCycle anicycleJumpUpRight;
	AnimationCycle anicycleJumpLeft;
	AnimationCycle anicycleJumpRight;
	AnimationCycle anicycleDeath;
	AnimationCycle anicycleStandFront;
	AnimationCycle anicycleStandBack;
	AnimationCycle anicycleStandLeft;
	AnimationCycle anicycleStandRight;

	AiEnemy(Type::ObjectType type);

	void turn(PlayerOrientation target);
	void stand();
	void executeKeys();
	void updateStateFollowPlayer(double time, TileTypePlane &ttplane, Player &player);

	void updateAnimation(double time);
	void updateMovementAndPhysics(double time, TileTypePlane &ttplane);
	void clearWaypoints();
};
}	// EOF Decker::Objects

#endif
