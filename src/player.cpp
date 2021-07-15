#include "decker.h"
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"

static int walk_cycle_left[]={0,1,2,3,4,5,6,7,8};
static int walk_cycle_right[]={9,10,11,12,13,14,15,16,17};
static int turn_from_left_to_mid[]={22,23,24,27};
static int turn_from_left_to_right[]={22,23,24,25,26};
static int turn_from_right_to_mid[]={18,19,20};
static int turn_from_right_to_left[]={18,19,20,21,22};
static int turn_from_mid_to_left[]={27,20,21,22};
static int turn_from_mid_to_right[]={27,24,25,26};
//static int turn_from_back_to_front[]={30,31,32,23,24};
static int run_cycle_left[]={61,62,63,64,65,66,67,68};
static int run_cycle_right[]={70,71,72,73,74,75,76,77};


AnimationCycle::AnimationCycle()
{
	index=0;
	cycle=walk_cycle_left;
	size=1;
	loop=false;
	endframe=0;
	finished=true;
}

void AnimationCycle::start(int *cycle_array, int size, bool loop, int endframe)
{
	//printf("start\n");
	cycle=cycle_array;
	this->size=size;
	this->loop=loop;
	this->endframe=endframe;
	finished=false;
	index=0;
}

void AnimationCycle::setStaticFrame(int nr)
{
	cycle=NULL;
	index=0;
	loop=false;
	finished=true;
	endframe=nr;
}

void AnimationCycle::update()
{
	if (finished) return;
	index++;
	if (index>=size) {
		if (loop==true) {
			index=0;
		} else {
			finished=true;
		}
	}
}

int AnimationCycle::getFrame() const
{
	if(finished) return endframe;
	return cycle[index];
}

int AnimationCycle::getIndex() const
{
	return index;
}

bool AnimationCycle::isFinished() const
{
	return finished;
}

Player::Player()
{
	x=y=0;
	sprite_resource=NULL;
	tiletype_resource=NULL;
	next_keycheck=0.0f;
	next_animation=0.0f;
	idle_timeout=0.0f;
	animation.setStaticFrame(27);
	movement=Stand;
	orientation=Front;
	turnTarget=Front;
	for (int cy=0;cy<6;cy++) {
		for (int cx=0;cx<4;cx++) {
			collision_matrix[cx][cy]=TileType::NonBlocking;
		}
	}
}

Player::~Player()
{

}

void Player::setSpriteResource(const SpriteTexture &resource)
{
	sprite_resource=&resource;
}

void Player::setTileTypeResource(const SpriteTexture &resource)
{
	tiletype_resource=&resource;
}

void Player::setGameWindow(const ppl7::grafix::Rect &world)
{
	this->world=world;

}

void Player::move(int x, int y)
{
	this->x=x;
	this->y=y;
}


void Player::draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	ppl7::grafix::Point p(x+viewport.x1-worldcoords.x,y+viewport.y1-worldcoords.y);
	//if (p.inside(viewport))
	sprite_resource->draw(renderer,p.x,p.y,animation.getFrame());
	if (tiletype_resource) {
		for (int cy=0;cy<6;cy++) {
			for (int cx=0;cx<4;cx++) {
				tiletype_resource->draw(renderer,p.x-(TILE_WIDTH*2)+(cx*TILE_WIDTH),p.y-(5*TILE_HEIGHT)+(cy*TILE_HEIGHT),collision_matrix[cx][cy]);
			}
		}
	}
}

void Player::turn(PlayerOrientation target)
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

void Player::update(double time, const TileTypePlane &world)
{
	updateMovement();
	checkCollisionWithWorld(world);
	updatePhysics(world);
	x+=velocity_move.x+velocity_physic.x;
	y+=velocity_move.y+velocity_physic.y;

	if (time>next_animation) {
		next_animation=time+0.056f;
		animation.update();
	}
	if (movement==Turn) {
		if (!animation.isFinished()) return;
		movement=Stand;
		orientation=turnTarget;
		velocity_move.stop();
		//printf("Turn done, movement=%d, orientation=%d\n", (int)movement, (int)orientation);
	}
	if (time>next_keycheck) {
		next_keycheck=time+0.1f;
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_LEFT] && not state[SDL_SCANCODE_LSHIFT]) {
			if (orientation!=Left) { turn(Left); return;}
			if (movement!=Walk) {
				movement=Walk;
				animation.start(walk_cycle_left,sizeof(walk_cycle_left)/sizeof(int),true,0);
				//animation.start(run_cycle_left,sizeof(run_cycle_left)/sizeof(int),true,0);
			}
		} else if (state[SDL_SCANCODE_LEFT] && state[SDL_SCANCODE_LSHIFT]) {
			//if (orientation!=Left) { turn(Left); return;}
			if (movement!=Run || orientation!=Left ) {
				movement=Run;
				orientation=Left;
				animation.start(run_cycle_left,sizeof(run_cycle_left)/sizeof(int),true,0);
			}
		} else if (state[SDL_SCANCODE_RIGHT] && not state[SDL_SCANCODE_LSHIFT]) {
			if (orientation!=Right) { turn(Right); return;}
			if (movement!=Walk) {
				movement=Walk;
				animation.start(walk_cycle_right,sizeof(walk_cycle_right)/sizeof(int),true,0);
				//animation.start(run_cycle_left,sizeof(run_cycle_left)/sizeof(int),true,0);
			}
		} else if (state[SDL_SCANCODE_RIGHT] && state[SDL_SCANCODE_LSHIFT]) {
			//if (orientation!=Right) { turn(Right); return;}
			if (movement!=Run  || orientation!=Right) {
				movement=Run;
				orientation=Right;
				animation.start(run_cycle_right,sizeof(run_cycle_right)/sizeof(int),true,0);
			}
		} else if (state[SDL_SCANCODE_UP]) {
			movement=ClimbUp;
			orientation=Back;
		} else if (state[SDL_SCANCODE_DOWN]) {
					movement=ClimbDown;
					orientation=Back;

		} else {
			if (movement!=Stand) {
				movement=Stand;
				if (orientation==Left) animation.setStaticFrame(0);
				else if (orientation==Right) animation.setStaticFrame(9);
				else if (orientation==Front) animation.setStaticFrame(27);
				else if (orientation==Back) animation.setStaticFrame(28);
				idle_timeout=time+4.0;
			} else if (time>idle_timeout && orientation!=Front) {
				turn(Front);

			}
		}
	}
}

void Player::updateMovement()
{
	if (movement==Walk) {
		if (orientation==Left) {
			velocity_move.x=-2;
		} else if (orientation==Right) {
			velocity_move.x=2;
		}
	} else if (movement==Run) {
		if (orientation==Left) {
			velocity_move.x=-6;
		} else if (orientation==Right) {
			velocity_move.x=6;
		}
	} else if (movement==ClimbUp) {
		velocity_move.y=-4;
	} else if (movement==ClimbDown) {
		velocity_move.y=4;

	} else {
		velocity_move.y=0;
		velocity_move.x=0;
	}
}

void Player::checkCollisionWithWorld(const TileTypePlane &world)
{
	TileType::Type t;
	for (int cy=1;cy<5;cy++) {
		for (int cx=0;cx<4;cx++) {
			collision_matrix[cx][cy]=world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
					y+(TILE_HEIGHT/2)-(5*TILE_HEIGHT)+(cy*TILE_HEIGHT)));
		}
	}
	collision_matrix[1][4]=world.getType(ppl7::grafix::Point(x-(TILE_WIDTH/2), y-1));
	collision_matrix[2][4]=world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2), y-1));

	for (int cx=0;cx<4;cx++) {
		collision_matrix[cx][0]=world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
				y-(TILE_HEIGHT*5)));

		collision_matrix[cx][5]=world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
				y));

	}

	if (collision_matrix[1][4]==TileType::Plate1h && collision_matrix[2][4]==TileType::Plate1h) {

	}
	if (collision_matrix[1][4]==TileType::Plate2h && collision_matrix[2][4]==TileType::Plate2h) {

	}

	if (collision_matrix[1][5]==TileType::Blocking || collision_matrix[2][5]==TileType::Blocking) {
		if (velocity_physic.y>0) {

		}
		acceleration.stop();
		velocity_physic.stop();
	}
	if (collision_matrix[1][4]==TileType::Blocking || collision_matrix[2][4]==TileType::Blocking) {
		acceleration.stop();
		velocity_physic.stop();
		y--;
	}

	if (movement==Walk || movement==Run) {
		if (orientation==Left) {
			for (int cy=1;cy<5;cy++) {
				if (collision_matrix[0][cy]==TileType::Blocking) {
					velocity_move.stop();
				}
			}
		} else if (orientation==Right) {
			for (int cy=1;cy<5;cy++) {
				if (collision_matrix[3][cy]==TileType::Blocking) {
					velocity_move.stop();
				}
			}
		}
	}
}

void Player::updatePhysics(const TileTypePlane &world)
{
	bool match=false;
	if (collision_matrix[1][4]==TileType::NonBlocking && collision_matrix[2][4]==TileType::NonBlocking) {
		if (collision_matrix[1][5]==TileType::NonBlocking && collision_matrix[2][5]==TileType::NonBlocking) {
			if (acceleration.y<10.0f) acceleration.y+=(0.1f+(acceleration.y/5.0f));
			if (acceleration.y>10.0f) acceleration.y=10.0f;
			velocity_physic.y+=acceleration.y;
			if (velocity_physic.y>10.0f) velocity_physic.y=10.0f;
			match=true;
		} else {
			acceleration.stop();
			velocity_physic.stop();
			match=true;
		}
	} else if (collision_matrix[1][4]==TileType::AirStream || collision_matrix[2][4]==TileType::AirStream) {
		if (acceleration.y<8.0f) acceleration.y+=(0.1f+(acceleration.y/8.0f));
		if (acceleration.y>8.0f) acceleration.y=8.0f;
		velocity_physic.y-=acceleration.y;
		if (velocity_physic.y<-8.0f) velocity_physic.y=-8.0f;
		match=true;
	}
	/*
	if (!match) {
		if (velocity_physic.y>0)
		if (acceleration>0)
		acceleration.stop();
		velocity_physic.stop();
	}
	*/
}
