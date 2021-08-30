#include <ppl7.h>
#include <ppl7-grafix.h>
#include "helena.h"
#include "audiopool.h"
#include "player.h"
#include "waynet.h"
#include "objects.h"

namespace Decker::Objects {

static int walk_cycle_left[]={1,2,3,4,5,6,7,8};
static int walk_cycle_right[]={10,11,12,13,14,15,16,17};
static int turn_from_left_to_mid[]={22,23,24,27};
static int turn_from_left_to_right[]={22,23,24,25,26};
static int turn_from_right_to_mid[]={18,19,20};
static int turn_from_right_to_left[]={18,19,20,21,22};
static int turn_from_mid_to_left[]={27,20,21,22};
static int turn_from_mid_to_right[]={27,24,25,26};
//static int turn_from_back_to_front[]={30,31,32,23,24};
static int run_cycle_left[]={61,62,63,64,65,66,67,68};
static int run_cycle_right[]={70,71,72,73,74,75,76,77};
static int climb_up_cycle[]={91,92,93,94,95,96,97,98,99,100,101};
static int climb_down_cycle[]={101,100,99,98,97,96,95,94,93,92,91};

static int slide_left[]={83,84,85,86};
static int slide_right[]={79,80,81,82};

static int death_animation[]={102,103,105,105,105,106,106,105,105,106,106,
		105,105,106,106,105,104,105,106,105,104,103,104,105,106};
static int death_by_falling[]={89,89,106,106,89,89,106,106,89,106,89,106,89,89,
		106,106,89};


Representation Helena::representation()
{
	return Representation(Spriteset::Helena, 27);
}

Helena::Helena()
:Enemy(Type::ObjectType::Helena)
{
	sprite_set=Spriteset::Helena;
	sprite_no=27;
	next_state=ppl7::GetMicrotime()+5.0f;
	next_animation=0.0f;
	idle_timeout=0.0f;
	state=StateWaitForEnable;
	animation.setStaticFrame(27);
	keys=0;
	substate=0;
	next_wayfind=0.0f;
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

void Helena::turn(PlayerOrientation target)
{
	movement=Turn;
	turnTarget=target;
	if (orientation==Front) {
		if (target==Left) {
			animation.start(turn_from_mid_to_left,sizeof(turn_from_mid_to_left)/sizeof(int),false,0);
		} else {
			animation.start(turn_from_mid_to_right,sizeof(turn_from_mid_to_right)/sizeof(int),false,9);
		}
	} else if (orientation==Left) {
		if (target==Right) {
			animation.start(turn_from_left_to_right,sizeof(turn_from_left_to_right)/sizeof(int),false,9);
		} else if (target==Front) {
			animation.start(turn_from_left_to_mid,sizeof(turn_from_left_to_mid)/sizeof(int),false,27);
		}
	} else if (orientation==Right) {
		if (target==Left) {
			animation.start(turn_from_right_to_left,sizeof(turn_from_right_to_left)/sizeof(int),false,0);
		} else if (target==Front) {
			animation.start(turn_from_right_to_mid,sizeof(turn_from_right_to_mid)/sizeof(int),false,27);
		}
	}
}

void Helena::stand()
{
	movement=Stand;
	keys=0;
	if (orientation==Left) animation.setStaticFrame(0);
	else if (orientation==Right) animation.setStaticFrame(9);
	else if (orientation==Front) animation.setStaticFrame(27);
	else if (orientation==Back) animation.setStaticFrame(28);
	idle_timeout=time+8.0;
	next_state=time+(double)ppl7::rand(1,5);
}

void Helena::update(double time, TileTypePlane &ttplane, Player &player)
{
	//printf ("s=%d, state=%s, keys=%d\n", state, (const char*)getState(), keys);
	this->time=time;
	if (!enabled) return;
	if (time>next_animation) {
		next_animation=time+0.07f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite!=sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
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
		current_way.clear();
	}


	if (time<next_state && state==StateStand) {
		state=StatePatrol;
		if (ppl7::rand(0,1)==0) turn(Left);
		else turn(Right);
	}

	if (movement==Turn) {
		if (!animation.isFinished()) return;
		//printf ("debug 2\n");
		movement=Stand;
		//if (state==StatePatrol) movement=Walk;
		orientation=turnTarget;
		velocity_move.stop();
		//printf("Turn done, movement=%d, orientation=%d\n", (int)movement, (int)orientation);
	}

	updateMovement();
	if (movement==Dead) return;
	Physic::PlayerMovement new_movement=Physic::checkCollisionWithWorld(ttplane, p.x,p.y);
	if (new_movement==Stand && movement!=Stand) {
		//printf ("checkCollisionWithWorld sagt: stand\n");
		stand();
	}

	if (updatePhysics(ttplane)) {
		if (movement==Slide && orientation==Left) {
			animation.start(slide_left,sizeof(slide_left)/sizeof(int),false,86);
		} else if (movement==Slide && orientation==Right) {
			animation.start(slide_right,sizeof(slide_right)/sizeof(int),false,82);
		}
	}
	p.x+=velocity_move.x;
	p.y+=velocity_move.y+gravity;
	updateBoundary();


	if (movement==Slide || movement==Dead) {
		return;
	}
	if (movement==Jump) {
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

void Helena::updateWay(Player &player)
{
	Waynet &waynet=GetObjectSystem()->getWaynet();
	WayPoint pwp=waynet.findNearestWaypoint(Position((uint16_t)player.x/TILE_WIDTH, (uint16_t)player.y/TILE_HEIGHT));
	//printf ("pwp=%d:%d, last_pwp=%d:%d\n", pwp.x,pwp.y,last_pwp.x,last_pwp.y);
	if (pwp.id==last_pwp.id) return;
	last_pwp=pwp;


	waypoints.clear();
	if (waynet.findWay(waypoints, Position(p.x, p.y), Position(player.x, player.y))) {
		const Connection &first=waypoints.front();
		printf ("found a way, starting at: %d:%d ==========================\n", first.source.x, first.source.y);
		current_way.type=Connection::Walk;
		current_way.source=0;
		current_way.target=first.source;
		/*
		std::list<Connection>::const_iterator it;
		for (it=waypoints.begin();it!=waypoints.end();++it) {
			printf ("source: (%d:%d), target: (%d:%d), type: %d, cost: %d\n",
					(*it).source.x, (*it).source.y,
					(*it).target.x, (*it).target.y,
					(*it).type, (*it).cost);
		}
		*/
	} else {
		printf ("found no way\n");
		waypoints.clear();
		current_way.clear();
	}

}

void Helena::updateStateFollowPlayer(double time, TileTypePlane &ttplane, Player &player)
{
	double dist=ppl7::grafix::Distance(p, player.position());
	if (dist>2048 && (movement==Falling||movement==Dead)) {
		printf ("something's wrong, back to patrol\n");
		p=initial_p;
		current_way.clear();
		state=StatePatrol;
		movement=Stand;
	}
	if (current_way.type==Connection::Invalid) {
		updateWay(player);
	}
	/*
	if (next_wayfind<time) {

		next_wayfind=time+1.0f;
	}
	*/
	if (current_way.type==Connection::Invalid) {
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
	if (current_way.type==Connection::Walk) {
		if ((uint16_t)(p.x/TILE_WIDTH)>current_way.target.x) keys=KeyboardKeys::Left|KeyboardKeys::Shift;
		else if ((uint16_t)(p.x/TILE_WIDTH)<current_way.target.x) keys=KeyboardKeys::Right|KeyboardKeys::Shift;
		else {
			arrived=true;
		}
	} else if (current_way.type==Connection::JumpLeft) {
		if (movement==Falling) arrived=true;
		else keys=KeyboardKeys::Left|KeyboardKeys::Shift|KeyboardKeys::Up;
	} else if (current_way.type==Connection::JumpRight) {
		if (movement==Falling) arrived=true;
		else keys=KeyboardKeys::Right|KeyboardKeys::Shift|KeyboardKeys::Up;
	} else if (current_way.type==Connection::JumpUp) {
		if (movement==Falling) arrived=true;
		else keys=KeyboardKeys::Shift|KeyboardKeys::Up;
	} else if (current_way.type==Connection::Climb) {
		if ((uint16_t)(p.y/TILE_HEIGHT)>current_way.target.y) keys=KeyboardKeys::Up;
		else if ((uint16_t)(p.y/TILE_HEIGHT)<current_way.target.y) keys=KeyboardKeys::Down;
		else arrived=true;
	}
	if (arrived) {
		if (movement==Falling) {
			last_pwp.id=0;
		}
		keys=0;
		printf ("arrived at point: %d:%d, real: %d:%d\n",current_way.target.x, current_way.target.y,
				(int)p.x/TILE_WIDTH,(int)p.y/TILE_HEIGHT);
		current_way.clear();
		updateWay(player);
		if (waypoints.size()>0) {
			current_way=waypoints.front();
			printf ("move to next point: %d:%d\n",current_way.target.x, current_way.target.y);
			waypoints.pop_front();
		}
	}

}

void Helena::executeKeys()
{
	//printf ("Helena::executeKeys: %d\n",keys);
	if (keys==KeyboardKeys::Left) {
		if (orientation!=Left) { turn(Left); return;}
		if (movement!=Walk) {
			movement=Walk;
			animation.start(walk_cycle_left,sizeof(walk_cycle_left)/sizeof(int),true,0);
		}
	} else if (keys==(KeyboardKeys::Left|KeyboardKeys::Shift)) {
		if (movement!=Run || orientation!=Left ) {
			movement=Run;
			orientation=Left;
			animation.start(run_cycle_left,sizeof(run_cycle_left)/sizeof(int),true,0);
		}
	} else if (keys==KeyboardKeys::Right) {
		if (orientation!=Right) { turn(Right); return;}
		if (movement!=Walk) {
			movement=Walk;
			animation.start(walk_cycle_right,sizeof(walk_cycle_right)/sizeof(int),true,0);
		}
	} else if (keys==(KeyboardKeys::Right|KeyboardKeys::Shift)) {
		if (movement!=Run  || orientation!=Right) {
			movement=Run;
			orientation=Right;
			animation.start(run_cycle_right,sizeof(run_cycle_right)/sizeof(int),true,0);
		}
	} else if ((keys==KeyboardKeys::Up || keys==(KeyboardKeys::Up|KeyboardKeys::Shift)) && movement!=Falling && movement!=Jump) {
		if (collision_matrix[1][4]==TileType::Ladder || collision_matrix[2][4]==TileType::Ladder) {
			if (movement!=ClimbUp) {
				movement=ClimbUp;
				orientation=Back;
				animation.start(climb_up_cycle,sizeof(climb_up_cycle)/sizeof(int),true,0);
			}
		} else {
			if (movement!=Jump) {
				movement=Jump;
				jump_climax=time+0.4f;
				acceleration_jump=1.0f;
				if (orientation==Front) animation.setStaticFrame(42);
				else if (orientation==Left) animation.setStaticFrame(40);
				else if (orientation==Right) animation.setStaticFrame(41);
				else animation.setStaticFrame(28);
			}
		}
	} else if ((keys&KeyboardKeys::JumpLeft)==KeyboardKeys::JumpLeft) {
		movement=Jump;
		orientation=Left;
		jump_climax=time+0.4f;
		acceleration_jump=1.0f;
		velocity_move.x=-2;
		if (keys&KeyboardKeys::Shift) velocity_move.x=-6;
		animation.setStaticFrame(38);
	} else if ((keys&KeyboardKeys::JumpRight)==KeyboardKeys::JumpRight) {
		movement=Jump;
		orientation=Right;
		jump_climax=time+0.4f;
		acceleration_jump=1.0f;
		velocity_move.x=2;
		if (keys&KeyboardKeys::Shift) velocity_move.x=6;
		animation.setStaticFrame(39);
	} else if (keys==KeyboardKeys::Down|| keys==(KeyboardKeys::Down|KeyboardKeys::Shift)) {
		if (collision_matrix[1][4]==TileType::Ladder || collision_matrix[2][4]==TileType::Ladder
				||collision_matrix[1][5]==TileType::Ladder || collision_matrix[2][5]==TileType::Ladder) {
			if (collision_matrix[1][5]!=TileType::Blocking && collision_matrix[2][5]!=TileType::Blocking) {
				if (movement!=ClimbDown) {
					//printf ("climb down\n");
					movement=ClimbDown;
					orientation=Back;
					animation.start(climb_down_cycle,sizeof(climb_down_cycle)/sizeof(int),true,0);
				}
			}
		}

	} else if (keys==(KeyboardKeys::Left) && movement==Jump) {
		if (!isCollisionLeft()) velocity_move.x=-2;
	} else if (keys==(KeyboardKeys::Right) && movement==Jump) {
		if (!isCollisionLeft()) velocity_move.x=2;
	} else if (keys==(KeyboardKeys::Left|KeyboardKeys::Shift) && movement==Jump) {
		if (!isCollisionLeft()) velocity_move.x=-6;
	} else if (keys==(KeyboardKeys::Right|KeyboardKeys::Shift) && movement==Jump) {
		if (!isCollisionLeft()) velocity_move.x=6;

	} else {
		if (movement!=Stand && movement!=Jump && movement!=Falling && movement!=Turn) {
			//printf ("debug 1\n");
			stand();
		}
	}
}


}
