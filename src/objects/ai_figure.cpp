#include <ppl7.h>
#include <ppl7-grafix.h>
#include "ai_figure.h"
#include "audiopool.h"
#include "player.h"
#include "waynet.h"
#include "objects.h"

#define DEBUGWAYNET

namespace Decker::Objects {

static int walk_cycle_left[]={ 1,2,3,4,5,6,7,8 };
static int walk_cycle_right[]={ 10,11,12,13,14,15,16,17 };
static int turn_from_left_to_mid[]={ 22,23,24,27 };
static int turn_from_left_to_right[]={ 22,23,24,25,26 };
static int turn_from_right_to_mid[]={ 18,19,20 };
static int turn_from_right_to_left[]={ 18,19,20,21,22 };
static int turn_from_mid_to_left[]={ 27,20,21,22 };
static int turn_from_mid_to_right[]={ 27,24,25,26 };
//static int turn_from_back_to_front[]={30,31,32,23,24};
static int run_cycle_left[]={ 61,62,63,64,65,66,67,68 };
static int run_cycle_right[]={ 70,71,72,73,74,75,76,77 };
static int climb_up_cycle[]={ 91,92,93,94,95,96,97,98,99,100,101 };
static int climb_down_cycle[]={ 101,100,99,98,97,96,95,94,93,92,91 };

static int slide_left[]={ 83,84,85,86 };
static int slide_right[]={ 79,80,81,82 };

static int death_animation[]={ 102,103,105,105,105,106,106,105,105,106,106,
		105,105,106,106,105,104,105,106,105,104,103,104,105,106 };


AiEnemy::AiEnemy(Type::ObjectType type)
	:Enemy(type)
{
	next_animation=0.0f;
	keys=0;
	next_wayfind=0.0f;
	collisionDetection=true;
	animation.setStaticFrame(27);
	anicycleWalkLeft.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
	anicycleWalkRight.start(walk_cycle_right, sizeof(walk_cycle_right) / sizeof(int), true, 0);
	anicycleRunLeft.start(run_cycle_left, sizeof(run_cycle_left) / sizeof(int), true, 0);
	anicycleRunRight.start(run_cycle_right, sizeof(run_cycle_right) / sizeof(int), true, 0);
	anicycleClimbUp.start(climb_up_cycle, sizeof(climb_up_cycle) / sizeof(int), true, 0);
	anicycleClimbDown.start(climb_down_cycle, sizeof(climb_down_cycle) / sizeof(int), true, 0);
	anicycleJumpUpFront.setStaticFrame(42);
	anicycleJumpUpBack.setStaticFrame(28);
	anicycleJumpUpLeft.setStaticFrame(40);
	anicycleJumpUpRight.setStaticFrame(41);
	anicycleJumpLeft.setStaticFrame(38);
	anicycleJumpRight.setStaticFrame(39);
	anicycleStandFront.setStaticFrame(27);
	anicycleStandBack.setStaticFrame(28);
	anicycleStandLeft.setStaticFrame(0);
	anicycleStandRight.setStaticFrame(9);
	anicycleTurnFromMidToLeft.start(turn_from_mid_to_left, sizeof(turn_from_mid_to_left) / sizeof(int), false, 0);
	anicycleTurnFromMidToRight.start(turn_from_mid_to_right, sizeof(turn_from_mid_to_right) / sizeof(int), false, 9);
	anicycleTurnFromLeftToRight.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 9);
	anicycleTurnFromLeftToMid.start(turn_from_left_to_mid, sizeof(turn_from_left_to_mid) / sizeof(int), false, 27);
	anicycleTurnFromRightToLeft.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 0);
	anicycleTurnFromRightToMid.start(turn_from_right_to_mid, sizeof(turn_from_right_to_mid) / sizeof(int), false, 27);
	anicycleSlideLeft.start(slide_left, sizeof(slide_left) / sizeof(int), false, 86);
	anicycleSlideRight.start(slide_right, sizeof(slide_right) / sizeof(int), false, 82);
	anicycleDeath.start(death_animation, sizeof(death_animation) / sizeof(int), false, 106);


}

void AiEnemy::clearWaypoints()
{
	current_way.clear();
}

void AiEnemy::turn(PlayerOrientation target)
{
	movement=Turn;
	turnTarget=target;
	if (orientation == Front) {
		if (target == Left) {
			animation.start(anicycleTurnFromMidToLeft);
		} else {
			animation.start(anicycleTurnFromMidToRight);
		}
	} else if (orientation == Left) {
		if (target == Right) {
			animation.start(anicycleTurnFromLeftToRight);
		} else if (target == Front) {
			animation.start(anicycleTurnFromLeftToMid);
		}
	} else if (orientation == Right) {
		if (target == Left) {
			animation.start(anicycleTurnFromRightToLeft);
		} else if (target == Front) {
			animation.start(anicycleTurnFromRightToMid);
		}
	}
}

void AiEnemy::stand()
{
	movement=Stand;
	keys=0;
	if (orientation == Left) animation.start(anicycleStandLeft);
	else if (orientation == Right) animation.start(anicycleStandRight);
	else if (orientation == Front) animation.start(anicycleStandFront);
	else if (orientation == Back) animation.start(anicycleStandBack);
}

void AiEnemy::updateAnimation(double time)
{
	if (time > next_animation) {
		next_animation=time + 0.07f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
}

void AiEnemy::updateMovementAndPhysics(double time, TileTypePlane& ttplane, float frame_rate_compensation)
{
	updateMovement(frame_rate_compensation);
	if (movement == Dead) return;
	Physic::PlayerMovement new_movement=Physic::checkCollisionWithWorld(ttplane, p.x, p.y);
	if (new_movement == Stand && movement != Stand) {
		//printf ("checkCollisionWithWorld sagt: stand\n");
		stand();
	}
	if (updatePhysics(ttplane, frame_rate_compensation)) {
		if (movement == Slide && orientation == Left) {
			animation.start(anicycleSlideLeft);
		} else if (movement == Slide && orientation == Right) {
			animation.start(anicycleSlideRight);
		}
	}
	p.x+=velocity_move.x;
	p.y+=velocity_move.y + gravity;

	//p.x+=velocity_move.x * frame_rate_compensation;
	//p.y+=(velocity_move.y + gravity) * frame_rate_compensation;
	updateBoundary();
}

void AiEnemy::updateWay(double time, const ppl7::grafix::Point& player)
{
	Waynet& waynet=GetObjectSystem()->getWaynet();
	WayPoint pwp=waynet.findNearestWaypoint(Position((uint16_t)player.x, (uint16_t)player.y));
	Position source_p(p.x, p.y);
	Position target_p(player.x, player.y);

#ifdef DEBUGWAYNET


	ppl7::PrintDebugTime("AiEnemy::updateWay, Object: %d:%d, Target: %d:%d, nearest wp=%d:%d, last_pwp=%d:%d\n",
		(int)p.x, (int)p.y,
		player.x, player.y,
		pwp.x, pwp.y, last_pwp.x, last_pwp.y);
#endif
	next_wayfind=time + 3.0f;
	if (pwp.id == last_pwp.id && waypoints.size() > 0) return;
	last_pwp=pwp;
	waypoints.clear();
	if (waynet.findWay(waypoints, Position(p.x, p.y), Position(player.x, player.y))) {
		const Connection& first=waypoints.front();
		current_way.type=Connection::Walk;
		current_way.source=0;
		current_way.target=first.source;
		/*
		if (current_way.target.y < p.y / TILE_HEIGHT) {
			if (current_way.target.x < p.x / TILE_WIDTH) current_way.type=Connection::JumpLeft;
			if (current_way.target.x > p.x / TILE_WIDTH) current_way.type=Connection::JumpRight;
		}
		*/
	/*
		if (source_p.isNear(current_way.target)) {
			ppl7::PrintDebugTime("already near, using next waypoint\n");
			current_way=waypoints.front();
			waypoints.pop_front();
		}
		*/
#ifdef DEBUGWAYNET
		ppl7::PrintDebugTime("found a way, starting at: %d:%d, to get there: %s ==========================\n",
			first.source.x, first.source.y, current_way.name());

#endif

#ifdef DEBUGWAYNET
		std::list<Connection>::const_iterator it;
		for (it=waypoints.begin();it != waypoints.end();++it) {
			ppl7::PrintDebugTime("source: (%d:%d), target: (%d:%d), type: %9s, cost: %0.3f\n",
				(*it).source.x, (*it).source.y,
				(*it).target.x, (*it).target.y,
				(*it).name(), (*it).cost);
		}
#endif
	} else {
#ifdef DEBUGWAYNET
		ppl7::PrintDebugTime("found no way\n");
#endif
		waypoints.clear();
		current_way.clear();
	}

}

void AiEnemy::updateStateFollowPlayer(double time, TileTypePlane& ttplane, const ppl7::grafix::Point& player)
{
	double dist=ppl7::grafix::Distance(p, ppl7::grafix::PointF(player));
	if (dist > 2048 && (movement == Falling || movement == Dead)) {
		//printf("something's wrong, back to patrol\n");
		p=initial_p;
		current_way.clear();
		movement=Stand;
	}
	int y_dist=abs((int)p.y - player.y);
	if (current_way.type == Connection::Invalid || next_wayfind < time) {
#ifdef DEBUGWAYNET
		ppl7::PrintDebugTime("current_way.type=%d, next_wayfinf=%0.3f, time=%0.3f\n", current_way.type, next_wayfind, time);
#endif
		if (movement == Walk || movement == Run || movement == Stand || movement == Falling) {
#ifdef DEBUGWAYNET
			ppl7::PrintDebug("updateWay Reason 1\n");
#endif
			updateWay(time, player);
		}
	}
	/*
	if (next_wayfind<time) {

		next_wayfind=time+1.0f;
	}
	*/
	if (current_way.type == Connection::Invalid) {
		/*
		printf ("Back to patrol\n");
		p=initial_p;
		state=StatePatrol;
		movement=Stand;
		*/
		return;
	}

	//const Connection &first=waypoints.front();
	bool arrived=false;
	double wp_dist=ppl7::grafix::Distance(p, ppl7::grafix::PointF(current_way.target.x, current_way.target.y));
#ifdef DEBUGWAYNET
	Waynet& waynet=GetObjectSystem()->getWaynet();
	const WayPoint& wp=waynet.getPoint(current_way.target);
	ppl7::PrintDebug("Target: %d, distance=%0.0f, ", wp.as, wp_dist);

#endif

	if (current_way.type == Connection::Walk) {
#ifdef DEBUGWAYNET
		ppl7::PrintDebug("walk\n");
#endif
		if ((uint16_t)(p.x) > current_way.target.x) keys=KeyboardKeys::Left | KeyboardKeys::Shift;
		else if ((uint16_t)(p.x) < current_way.target.x) keys=KeyboardKeys::Right | KeyboardKeys::Shift;
		else if (wp_dist < 20) {
#ifdef DEBUGWAYNET
			ppl7::PrintDebug("arrived\n");
#endif
			arrived=true;
		}
	} else if (current_way.type == Connection::Go) {
		if ((uint16_t)(p.x) > current_way.target.x) keys=KeyboardKeys::Left;
		else if ((uint16_t)(p.x) < current_way.target.x) keys=KeyboardKeys::Right;
		else if (wp_dist < 20) {
			arrived=true;
		}
	} else if (current_way.type == Connection::JumpLeft) {
		if (movement == Falling) arrived=true;
		else keys=KeyboardKeys::Left | KeyboardKeys::Shift | KeyboardKeys::Up;
	} else if (current_way.type == Connection::JumpRight) {
		if (movement == Falling) arrived=true;
		else keys=KeyboardKeys::Right | KeyboardKeys::Shift | KeyboardKeys::Up;
	} else if (current_way.type == Connection::JumpUp) {
		if (movement == Falling) arrived=true;
		else keys=KeyboardKeys::Shift | KeyboardKeys::Up;
	} else if (current_way.type == Connection::Climb) {
		if ((uint16_t)(p.y) > current_way.target.y) keys=KeyboardKeys::Up;
		else if ((uint16_t)(p.y) < current_way.target.y) keys=KeyboardKeys::Down;
		else if (wp_dist < 20) arrived=true;
	}
	if (arrived) {
		if (movement == Falling) {
			last_pwp.id=0;
		}
		keys=0;
#ifdef DEBUGWAYNET
		ppl7::PrintDebugTime("arrived at point %d: %d:%d, real: %d:%d, movement was: %d\n", wp.as, current_way.target.x, current_way.target.y,
			(int)p.x, (int)p.y, movement);
#endif
		current_way.clear();
		if (movement == Walk || movement == Run) {
			if (player.x < p.x && orientation != Left && y_dist < TILE_HEIGHT) turn(Left);
			if (player.x > p.x && orientation != Right && y_dist < TILE_HEIGHT) turn(Right);
		}
#ifdef DEBUGWAYNET
		ppl7::PrintDebug("updateWay Reason 2\n");
#endif
		updateWay(time, player);
		if (waypoints.size() > 0) {
			current_way=waypoints.front();
			if (movement == ClimbUp || movement == ClimbDown) {
				//printf("movement was climb\n");
			}
			//stand();
			//printf("move to next point: %d:%d, type: %d\n", current_way.target.x, current_way.target.y, current_way.type);
			waypoints.pop_front();
		}
	}

}

void AiEnemy::executeKeys(float frame_rate_compensation)
{
	//printf ("Wallenstein::executeKeys: %d\n",keys);
	if (keys == KeyboardKeys::Left) {
		if (orientation != Left) { turn(Left); return; }
		if (movement != Walk) {
			movement=Walk;
			animation.start(anicycleWalkLeft);
		}
	} else if (keys == (KeyboardKeys::Left | KeyboardKeys::Shift)) {
		if (movement != Run || orientation != Left) {
			movement=Run;
			orientation=Left;
			animation.start(anicycleRunLeft);
		}
	} else if (keys == KeyboardKeys::Right) {
		if (orientation != Right) { turn(Right); return; }
		if (movement != Walk) {
			movement=Walk;
			animation.start(anicycleWalkRight);
		}
	} else if (keys == (KeyboardKeys::Right | KeyboardKeys::Shift)) {
		if (movement != Run || orientation != Right) {
			movement=Run;
			orientation=Right;
			animation.start(anicycleRunRight);
		}
	} else if ((keys == KeyboardKeys::Up || keys == (KeyboardKeys::Up | KeyboardKeys::Shift)) && movement != Falling && movement != Jump) {
		if (collision_matrix[1][4] == TileType::Ladder || collision_matrix[2][4] == TileType::Ladder) {
			if (movement != ClimbUp) {
				movement=ClimbUp;
				orientation=Back;
				animation.start(anicycleClimbUp);
			}
		} else {
			if (movement != Jump) {
				movement=Jump;
				if (keys & KeyboardKeys::Shift) {
					jump_climax=time + 0.45f;
					acceleration_jump=2.0f * frame_rate_compensation;
					acceleration_jump_sideways=0;
				} else {
					jump_climax=time + 0.3f;
					acceleration_jump=0.3f * frame_rate_compensation;
					acceleration_jump_sideways=0;
				}
				if (orientation == Front) animation.start(anicycleJumpUpFront);
				else if (orientation == Left) animation.start(anicycleJumpUpLeft);
				else if (orientation == Right) animation.start(anicycleJumpUpRight);
				else animation.start(anicycleJumpUpBack);
			}
		}
	} else if ((keys & KeyboardKeys::JumpLeft) == KeyboardKeys::JumpLeft) {
		movement=Jump;
		orientation=Left;
		if (keys & KeyboardKeys::Shift) {
			jump_climax=time + 0.45f;
			acceleration_jump=2.0f * frame_rate_compensation;
			acceleration_jump_sideways=-6;

		} else {
			jump_climax=time + 0.3f;
			acceleration_jump=0.3f * frame_rate_compensation;
			acceleration_jump_sideways=-2;
		}
		velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
		animation.start(anicycleJumpLeft);
	} else if ((keys & KeyboardKeys::JumpRight) == KeyboardKeys::JumpRight) {
		movement=Jump;
		orientation=Right;
		if (keys & KeyboardKeys::Shift) {
			jump_climax=time + 0.45f;
			acceleration_jump=2.0f * frame_rate_compensation;
			acceleration_jump_sideways=6.0f;
			velocity_move.x=8 * frame_rate_compensation;
		} else {
			jump_climax=time + 0.3f;
			acceleration_jump=0.3f * frame_rate_compensation;
			acceleration_jump_sideways=2.0f;
		}
		velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
		animation.start(anicycleJumpRight);
	} else if (keys == KeyboardKeys::Down || keys == (KeyboardKeys::Down | KeyboardKeys::Shift)) {
		if (collision_matrix[1][4] == TileType::Ladder || collision_matrix[2][4] == TileType::Ladder
			|| collision_matrix[1][5] == TileType::Ladder || collision_matrix[2][5] == TileType::Ladder) {
			if (collision_matrix[1][5] != TileType::Blocking && collision_matrix[2][5] != TileType::Blocking) {
				if (movement != ClimbDown) {
					//printf ("climb down\n");
					movement=ClimbDown;
					orientation=Back;
					animation.start(anicycleClimbDown);
				}
			}
		}

	} else if (keys == (KeyboardKeys::Left) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=-2 * frame_rate_compensation;
	} else if (keys == (KeyboardKeys::Right) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=2 * frame_rate_compensation;
	} else if (keys == (KeyboardKeys::Left | KeyboardKeys::Shift) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=-8 * frame_rate_compensation;
	} else if (keys == (KeyboardKeys::Right | KeyboardKeys::Shift) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=8 * frame_rate_compensation;

	} else {
		if (movement != Stand && movement != Jump && movement != Falling && movement != Turn) {
			//printf ("debug 1\n");
			stand();
		}
	}
}


}
