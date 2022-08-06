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


	void updateWay(double time, const ppl7::grafix::Point& player);


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
	void updateStateFollowPlayer(double time, TileTypePlane& ttplane, const ppl7::grafix::Point& player);

	void updateAnimation(double time);
	void updateMovementAndPhysics(double time, TileTypePlane& ttplane, float frame_rate_compensation);
	void clearWaypoints();
};


class Yeti : public AiEnemy
{
private:
	enum State {
		StateWaitForEnable,
		StateStand,
		StatePatrol,
		StatePatrolLeft,
		StatePatrolRight,
		StateGoToOrigin,
		StateFollowPlayer
	};
	double next_state;
	int state;
	int substate;

	void updateStatePatrol(double time, TileTypePlane& ttplane);

public:
	Yeti();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void toggle(bool enable, Object* source=NULL) override;
};

class AutoGeorge : public AiEnemy
{
private:
	ppl7::grafix::Point target;


public:
	AutoGeorge();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

};





}	// EOF Decker::Objects

#endif
