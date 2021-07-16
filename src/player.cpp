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
	acceleration_airstream=0.0f;
	acceleration_gravity=0.0f;
	gravity=0.0f;
	jump_climax=0.0f;
	acceleration_jump=0.0f;
	time=0.0f;
}

Player::~Player()
{

}
static const char* movement_string[9]={"Stand",
		"Turn", "Walk", "Run", "Pickup", "ClimbUp", "ClimbDown",
		"Jump","Falling"};

static const char* orientation_string[4]={"Left",
		"Right", "Front", "Back"};


ppl7::String Player::getState() const
{
	ppl7::String s;
	s.setf("%s:%s, velocity: %0.3f:%0.3f", movement_string[movement],
			orientation_string[orientation],
			velocity_move.x, velocity_move.y);
	return s;
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
	sprite_resource->draw(renderer,p.x,p.y+1,animation.getFrame());
}

void Player::drawCollision(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	ppl7::grafix::Point p(x+viewport.x1-worldcoords.x,y+viewport.y1-worldcoords.y);
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

int Player::getKeyboardMatrix(const Uint8 *state)
{
	int matrix=0;
	if (state[SDL_SCANCODE_LEFT]) matrix|=KeyboardKeys::Left;
	if (state[SDL_SCANCODE_RIGHT]) matrix|=KeyboardKeys::Right;
	if (state[SDL_SCANCODE_UP]) matrix|=KeyboardKeys::Up;
	if (state[SDL_SCANCODE_DOWN]) matrix|=KeyboardKeys::Down;
	if (state[SDL_SCANCODE_LSHIFT]) matrix|=KeyboardKeys::Shift;
	return matrix;
}

void Player::stand()
{
	movement=Stand;
	if (orientation==Left) animation.setStaticFrame(0);
	else if (orientation==Right) animation.setStaticFrame(9);
	else if (orientation==Front) animation.setStaticFrame(27);
	else if (orientation==Back) animation.setStaticFrame(28);
	idle_timeout=time+4.0;
}

void Player::update(double time, const TileTypePlane &world)
{
	this->time=time;
	updateMovement();
	checkCollisionWithWorld(world);
	updatePhysics(world);
	x+=velocity_move.x;
	y+=velocity_move.y+gravity;

	if (time>next_animation) {
		next_animation=time+0.056f;
		animation.update();
	}
	if (movement==Turn) {
		if (!animation.isFinished()) return;
		printf ("debug 2\n");
		movement=Stand;
		orientation=turnTarget;
		velocity_move.stop();
		//printf("Turn done, movement=%d, orientation=%d\n", (int)movement, (int)orientation);
	}
	if (time>next_keycheck) {
		next_keycheck=time+0.1f;
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		int keys=getKeyboardMatrix(state);
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
		} else if (keys==KeyboardKeys::Up && movement!=Falling && movement!=Falling && movement!=Jump) {
			if (collision_matrix[1][4]==TileType::Ladder || collision_matrix[2][4]==TileType::Ladder) {
				movement=ClimbUp;
				orientation=Back;
			} else {
				if (movement!=Jump) {
					movement=Jump;
					orientation=Front;
					jump_climax=time+0.1f;
					acceleration_jump=1.5f;
					animation.setStaticFrame(42);
				}
			}
		} else if (keys==(KeyboardKeys::Up|KeyboardKeys::Left) && movement!=Falling && movement!=Jump) {
			movement=Jump;
			orientation=Left;
			jump_climax=time+0.3f;
			acceleration_jump=0.5f;
			velocity_move.x=-2;
			animation.setStaticFrame(38);
		} else if (keys==(KeyboardKeys::Up|KeyboardKeys::Right) && movement!=Falling && movement!=Jump) {
			movement=Jump;
			orientation=Right;
			jump_climax=time+0.3f;
			acceleration_jump=0.5f;
			velocity_move.x=2;
			animation.setStaticFrame(39);
		} else if (keys==(KeyboardKeys::Up|KeyboardKeys::Left|KeyboardKeys::Shift) && movement!=Falling && movement!=Jump) {
			movement=Jump;
			orientation=Left;
			jump_climax=time+0.5f;
			acceleration_jump=0.5f;
			velocity_move.x=-6;
			animation.setStaticFrame(38);
		} else if (keys==(KeyboardKeys::Up|KeyboardKeys::Right|KeyboardKeys::Shift) && movement!=Falling && movement!=Jump) {
			movement=Jump;
			orientation=Right;
			jump_climax=time+0.5f;
			acceleration_jump=0.5f;
			velocity_move.x=6;
			animation.setStaticFrame(39);
		} else if (keys==KeyboardKeys::Down) {
			if (collision_matrix[1][4]==TileType::Ladder || collision_matrix[2][4]==TileType::Ladder) {
				movement=ClimbDown;
				orientation=Back;
			}

		} else {
			if (movement!=Stand && movement!=Jump && movement!=Falling) {
				printf ("debug 1\n");
				stand();
			} else if (movement==Stand && time>idle_timeout && orientation!=Front) {
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
	} else if (movement==Jump) {
		printf ("we are jumping... ");
		if (jump_climax>time) {
			if (acceleration_jump<2.0f) acceleration_jump+=0.02f;
			if (acceleration_jump>2.0f) acceleration_jump=8.0f;
			printf ("under climax, accelerating %0.3f ", acceleration_jump);
		} else {
			if (acceleration_jump>0) acceleration_jump-=acceleration_jump/5.0;
			if (acceleration_jump<0.01f) {
				acceleration_jump=0.0;
				movement=Falling;
				printf ("done\n");
			}
			printf ("over time, decelerating %0.3f", acceleration_jump);
		}
		velocity_move.y-=acceleration_jump;
		if (velocity_move.y<-4.0f) velocity_move.y=-4.0f;
		if (velocity_move.y>-0.1f) {
			velocity_move.y=0.0f;
		}
		printf ("\n");
	} else if (movement==Falling) {
		if (velocity_move.y<-0.1f) velocity_move.y-=velocity_move.y/3.0f;
		if (velocity_move.y>-0.1f) velocity_move.y=0;


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

	if (collision_matrix[1][4]==TileType::Plate2h || collision_matrix[2][4]==TileType::Plate2h) {
		int ty=((int)((y-1)/TILE_HEIGHT))*TILE_HEIGHT+1*(TILE_HEIGHT/3);
		printf ("Plate2h y=%d, ty=%d\n", y, ty);
		if (y>ty) {
			y-=4;
			if (y<ty) y=ty;
		} else if (y<ty) {
			y+=4;
			if (y>ty) y=ty;
		}
	} else if (collision_matrix[1][4]==TileType::Plate1h || collision_matrix[2][4]==TileType::Plate1h) {
		int ty=((int)((y-1)/TILE_HEIGHT))*TILE_HEIGHT+2*(TILE_HEIGHT/3);
		printf ("Plate1h y=%d, ty=%d\n", y, ty);
		if (y>ty) {
			y-=4;
			if (y<ty) y=ty;
		} else if (y<ty) {
			y+=4;
			if (y>ty) y=ty;
		}
	}


	if (collision_matrix[1][5]==TileType::Blocking || collision_matrix[2][5]==TileType::Blocking) {
		if (gravity>0.0f) {
			acceleration_gravity=0.0f;
			gravity=0.0f;
			if (movement==Falling || movement==Jump) stand();
		}
	}
	if (collision_matrix[1][4]==TileType::Blocking || collision_matrix[2][4]==TileType::Blocking) {
		acceleration_gravity=0.0f;
		gravity=0.0f;
		y--;
	}

	if (movement==Walk || movement==Run || movement==Jump || movement==Falling) {
		if (orientation==Left) {
			for (int cy=1;cy<5;cy++) {
				if (collision_matrix[0][cy]==TileType::Blocking) {
					velocity_move.x=0;
					printf ("debug 3a\n");
					if (movement!=Jump || movement!=Falling)	stand();
					else movement=Falling;
				}
			}
		} else if (orientation==Right) {
			for (int cy=1;cy<5;cy++) {
				if (collision_matrix[3][cy]==TileType::Blocking) {
					velocity_move.x=0;
					printf ("debug 3b\n");
					if (movement!=Jump || movement!=Falling)	stand();
					else movement=Falling;

				}
			}
		}
	}
}

void Player::updatePhysics(const TileTypePlane &world)
{
	if (movement==Jump) return;
	bool match=false;
	if (collision_matrix[1][4]==TileType::NonBlocking && collision_matrix[2][4]==TileType::NonBlocking) {
		if (collision_matrix[1][5]==TileType::NonBlocking && collision_matrix[2][5]==TileType::NonBlocking) {
			if (acceleration_gravity<10.0f) acceleration_gravity+=0.2f;
			if (acceleration_gravity>10.0f) acceleration_gravity=10.0f;
			match=true;
		}
	}
	if (acceleration_gravity>0.0f) {
		if (!match) {
			acceleration_gravity-=acceleration_gravity/2.0f;
			if (acceleration_gravity<0.0f) acceleration_gravity=0.0f;
		}
		gravity+=acceleration_gravity;
		if (gravity>10.0f) gravity=10.0f;
	}
	match=false;
	if (collision_matrix[1][4]==TileType::AirStream || collision_matrix[2][4]==TileType::AirStream) {
		//if (acceleration_airstream<8.0f) acceleration_airstream+=(0.1f+(acceleration_airstream/200.0f));
		if (acceleration_airstream<8.0f) acceleration_airstream+=0.02f;
		if (acceleration_airstream>8.0f) acceleration_airstream=8.0f;
		match=true;
	}
	if (acceleration_airstream>0.0f) {
		if (!match) {
			acceleration_airstream-=acceleration_airstream/5.0f;
			if (acceleration_airstream<0.0f) acceleration_airstream=0.0f;
		}
		gravity-=acceleration_airstream;
		if (gravity<-8.0f) gravity=-8.0f;

	}
	/*
	printf ("gravity: %2.3f, acceleration_gravity: %2.3f, acceleration_airstream: %2.3f\n",
			gravity, acceleration_gravity, acceleration_airstream);
	*/

}
