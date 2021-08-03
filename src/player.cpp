#include "decker.h"
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"

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
	collision_at_pivoty[0]=0;
	collision_at_pivoty[1]=0;
	collision_at_pivoty[2]=0;
	acceleration_airstream=0.0f;
	acceleration_gravity=0.0f;
	gravity=0.0f;
	jump_climax=0.0f;
	acceleration_jump=0.0f;
	time=0.0f;
	points=0;
	health=100;
	lifes=3;
	player_stands_on_object=NULL;
	fallstart=0.0f;
}

Player::~Player()
{

}
static const char* movement_string[11]={"Stand",
		"Turn", "Walk", "Run", "Pickup", "ClimbUp", "ClimbDown",
		"Jump","Falling", "Slide", "Dead"};

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

void Player::move(int x, int y)
{
	this->x=x;
	this->y=y;
}


void Player::draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	ppl7::grafix::Point p(x+viewport.x1-worldcoords.x,y+viewport.y1-worldcoords.y);
	if (movement==Slide) p.y+=35;
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
	tiletype_resource->draw(renderer,p.x+80,p.y-32,collision_at_pivoty[0]);
	tiletype_resource->draw(renderer,p.x+80,p.y,collision_at_pivoty[1]);
	tiletype_resource->draw(renderer,p.x+80,p.y+32,collision_at_pivoty[2]);
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
	if (state[SDL_SCANCODE_J] || state[SDL_SCANCODE_A]) matrix|=KeyboardKeys::Left;
	if (state[SDL_SCANCODE_RIGHT]) matrix|=KeyboardKeys::Right;
	if (state[SDL_SCANCODE_L] || state[SDL_SCANCODE_D]) matrix|=KeyboardKeys::Right;
	if (state[SDL_SCANCODE_UP]) matrix|=KeyboardKeys::Up;
	if (state[SDL_SCANCODE_I] || state[SDL_SCANCODE_W]) matrix|=KeyboardKeys::Up;
	if (state[SDL_SCANCODE_DOWN]) matrix|=KeyboardKeys::Down;
	if (state[SDL_SCANCODE_K] || state[SDL_SCANCODE_S]) matrix|=KeyboardKeys::Down;
	if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) matrix|=KeyboardKeys::Shift;
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

void Player::addPoints(int points)
{
	if (movement==Dead) return;
	this->points+=points;
}

void Player::dropHealth(int points, HealthDropReason reason)
{
	if (movement==Dead) return;
	health-=points;
	if (health<=0 && movement!=Dead) {
		health=0;
		movement=Dead;
		// we can play different animations for different reasons
		if (reason==FallingDeep)
			animation.start(death_by_falling,sizeof(death_by_falling)/sizeof(int),false,106);
		else
			animation.start(death_animation,sizeof(death_animation)/sizeof(int),false,106);
	}
}

void Player::addInventory(int object_id, const Decker::Objects::Representation &repr)
{
	Inventory.insert(std::pair<int,Decker::Objects::Representation>(object_id,repr));

}

void Player::setSavePoint(const ppl7::grafix::Point &p)
{
	lastSavePoint=p;
}

void Player::setStandingOnObject(Decker::Objects::Object *object)
{
	player_stands_on_object=object;
}

void Player::update(double time, const TileTypePlane &world, Decker::Objects::ObjectSystem *objects)
{
	this->time=time;
	if (time>next_animation) {
		next_animation=time+0.056f;
		animation.update();
	}
	if (movement==Dead) {
		if (animation.isFinished()) {
			lifes--;
			health=100;
			x=lastSavePoint.x;
			y=lastSavePoint.y;
			stand();
		}
		return;
	}
	detectFallingDamage(time);
	updateMovement();
	player_stands_on_object=NULL;
	checkCollisionWithObjects(objects);
	if (movement==Dead) return;
	checkCollisionWithWorld(world);
	updatePhysics(world);
	x+=velocity_move.x;
	y+=velocity_move.y+gravity;


	if (movement==Turn) {
		if (!animation.isFinished()) return;
		//printf ("debug 2\n");
		movement=Stand;
		orientation=turnTarget;
		velocity_move.stop();
		//printf("Turn done, movement=%d, orientation=%d\n", (int)movement, (int)orientation);
	}
	if (movement==Slide || movement==Dead) {
		return;
	}
	if (movement==Jump || movement==Falling) {
		handleKeyboardWhileJumpOrFalling(time, world, objects);
		return;
	}
	//if (time>next_keycheck) {
	//next_keycheck=time+0.1f;
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
					acceleration_jump=0.5f;
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
			acceleration_jump=0.5f;
			velocity_move.x=-2;
			if (keys&KeyboardKeys::Shift) velocity_move.x=-6;
			animation.setStaticFrame(38);
		} else if ((keys&KeyboardKeys::JumpRight)==KeyboardKeys::JumpRight) {
			movement=Jump;
			orientation=Right;
			jump_climax=time+0.4f;
			acceleration_jump=0.5f;
			velocity_move.x=2;
			if (keys&KeyboardKeys::Shift) velocity_move.x=6;
			animation.setStaticFrame(39);
		} else if (keys==KeyboardKeys::Down) {
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
			if (movement!=Stand && movement!=Jump && movement!=Falling) {
				//printf ("debug 1\n");
				stand();
			} else if (movement==Stand && time>idle_timeout && orientation!=Front) {
				turn(Front);

			}
		}
	//}
}

void Player::detectFallingDamage(double time)
{
	if (movement!=Falling && fallstart>0.0f) {
		double falltime=time-fallstart;
		//printf ("Falling ended at %0.3f, falltime: %0.3f\n", time, falltime);
		if (falltime>1.1f) dropHealth(100, HealthDropReason::FallingDeep);
		else if (falltime>1.0f) dropHealth(80, HealthDropReason::FallingDeep);
		else if (falltime>0.9f) dropHealth(50, HealthDropReason::FallingDeep);
		else if (falltime>0.8f) dropHealth(20, HealthDropReason::FallingDeep);
		fallstart=0.0f;
	} else if (movement==Falling && fallstart==0.0f && gravity>15.0f) {
		//printf ("we start to fall at %0.3f\n",time);
		fallstart=time;
	} else if (movement==Falling && fallstart>0.0f && time-fallstart>10.0f) {
		// the player probably falls through the level
		dropHealth(100);
	}
}

void Player::handleKeyboardWhileJumpOrFalling(double time, const TileTypePlane &world, Decker::Objects::ObjectSystem *objects)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	int keys=getKeyboardMatrix(state);
	if ((keys&KeyboardKeys::Left) && velocity_move.x==0) {
		velocity_move.x=-2;
		if (keys&KeyboardKeys::Shift) velocity_move.x=-6;
		orientation=Left;
		animation.setStaticFrame(38);
	} else if ((keys&KeyboardKeys::Right) && velocity_move.x==0) {
		velocity_move.x=2;
		if (keys&KeyboardKeys::Shift) velocity_move.x=6;
		orientation=Right;
		animation.setStaticFrame(39);
	}
}

void Player::updateMovement()
{
	if (movement==Slide || movement==Dead) return;
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
		//printf ("we are jumping... ");
		if (jump_climax>time) {
			if (acceleration_jump<2.0f) acceleration_jump+=0.1f;
			if (acceleration_jump>4.0f) acceleration_jump=4.0f;
			//printf ("under climax, accelerating %0.3f ", acceleration_jump);
		} else {
			if (acceleration_jump>0) acceleration_jump-=acceleration_jump/5.0;
			if (acceleration_jump<0.01f) {
				acceleration_jump=0.0;
				movement=Falling;
				//printf ("done\n");
			}
			//printf ("over time, decelerating %0.3f", acceleration_jump);
		}
		velocity_move.y-=acceleration_jump;
		if (velocity_move.y<-8.0f) velocity_move.y=-8.0f;
		if (velocity_move.y>-0.1f) {
			velocity_move.y=0.0f;
		}
		//printf ("\n");
	} else if (movement==Falling) {
		if (velocity_move.y<-0.1f) velocity_move.y-=velocity_move.y/3.0f;
		if (velocity_move.y>-0.1f) velocity_move.y=0;


	} else {
		velocity_move.y=0;
		velocity_move.x=0;
	}
	if(gravity>10) movement=Falling;
}

static TileType::Type filterRelevantTileTypes(TileType::Type t)
{
	if (t==TileType::EnemyBlocker) return TileType::NonBlocking;
	return t;
}


void Player::checkCollisionWithWorld(const TileTypePlane &world)
{
	if (movement==Dead) return;
	int collision_type_count[TileType::Type::MaxType];
	for (int i=0;i<TileType::Type::MaxType;i++) collision_type_count[i]=0;
	//TileType::Type t;
	for (int cy=1;cy<5;cy++) {
		for (int cx=0;cx<4;cx++) {
			TileType::Type t=world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
					y+(TILE_HEIGHT/2)-(5*TILE_HEIGHT)+(cy*TILE_HEIGHT)));
			collision_matrix[cx][cy]=filterRelevantTileTypes(t);
			collision_type_count[t]+=1;
		}
	}
	collision_at_pivoty[0]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x, y-1)));
	collision_at_pivoty[1]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x, y)));
	collision_at_pivoty[2]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x, y-(TILE_WIDTH/2))));
	collision_matrix[1][4]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x-(TILE_WIDTH/2), y-1)));
	collision_matrix[2][4]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2), y-1)));
	for (int cx=0;cx<4;cx++) {
		collision_matrix[cx][0]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
				y-(TILE_HEIGHT*4))));

		collision_matrix[cx][5]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
				y)));

	}
	if (movement==Slide) {
		if (orientation==Left) {
			if (collision_matrix[1][4]==TileType::Blocking) {
				stand();
			}
		} else {
			if (collision_matrix[2][4]==TileType::Blocking) {
				stand();
			}
		}
		while (world.getType(ppl7::grafix::Point(x,y-1))==TileType::Blocking) {
			y--;
		}
		return;
	}

	if (collision_at_pivoty[1]==TileType::Plate2h) {
		int th=TILE_HEIGHT/3;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+th;
		//printf ("Plate2h 1, y=%d, ty=%d\n", (int)y, ty);
		if (y<ty) {
			y+=3;
			if (y>ty) y=ty;
		} else if (y>ty) {
			y-=3;
			if (y<ty) y=ty;
		}
		//y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	} else if (collision_at_pivoty[0]==TileType::Plate2h) {
		int th=TILE_HEIGHT/3;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+th-(2*th);
		printf ("Plate2h 2, y=%d, ty=%d\n", (int)y, ty);
		//y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;

	} else if (collision_at_pivoty[1]==TileType::Plate1h) {
		int th=TILE_HEIGHT/3;
		int ty=((int)((y-1)/TILE_HEIGHT))*TILE_HEIGHT+2*(th);
		if (y<ty) {
			y+=3;
			if (y>ty) y=ty;
		} else if (y>ty) {
			y-=3;
			if (y<ty) y=ty;
		}
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	} else if (collision_at_pivoty[0]==TileType::Plate1h) {
		int th=TILE_HEIGHT/3;
		int ty=((int)((y-1)/TILE_HEIGHT))*TILE_HEIGHT+2*th;
		if (y<ty) {
			y+=3;
			if (y>ty) y=ty;
		} else if (y>ty) {
			y-=3;
			if (y<ty) y=ty;
		}
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	} else if (collision_at_pivoty[1]==TileType::ShallowRampLeftUpper) {
		int tx=(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+th-(th*tx/TILE_WIDTH);
		//printf ("ShallowRampLeftUpper y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	} else if (collision_at_pivoty[1]==TileType::ShallowRampLeftLower) {
		int tx=(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+TILE_HEIGHT-(th*tx/TILE_WIDTH);
		//printf ("ShallowRampLeftLower 1, y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	} else if (collision_at_pivoty[0]==TileType::ShallowRampLeftLower) {
		int tx=(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+TILE_HEIGHT-(th*tx/TILE_WIDTH)-TILE_HEIGHT;
		//printf ("ShallowRampLeftLower 2, y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	} else if (collision_at_pivoty[1]==TileType::ShallowRampRightUpper) {
		int tx=TILE_WIDTH-(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+th-(th*tx/TILE_WIDTH);
		//printf ("ShallowRampLeftUpper y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	} else if (collision_at_pivoty[1]==TileType::ShallowRampRightLower) {
		int tx=TILE_WIDTH-(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+TILE_HEIGHT-(th*tx/TILE_WIDTH);
		//printf ("ShallowRampLeftLower 1, y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	} else if (collision_at_pivoty[0]==TileType::ShallowRampRightLower) {
		int tx=TILE_WIDTH-(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+TILE_HEIGHT-(th*tx/TILE_WIDTH)-TILE_HEIGHT;
		//printf ("ShallowRampLeftLower 2, y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		return;
	}

	if (collision_matrix[1][0]==TileType::Blocking || collision_matrix[2][0]==TileType::Blocking) {
		acceleration_gravity=0.1f;
		gravity=1.0f;
		velocity_move.y=1;
		acceleration_jump=0.0f;
		movement=Falling;
		//if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
	}
	if (collision_matrix[1][5]==TileType::Blocking || collision_matrix[2][5]==TileType::Blocking
			|| collision_matrix[1][5]==TileType::Ladder || collision_matrix[2][5]==TileType::Ladder ) {
		if (gravity>0.0f) {
			//printf ("col 2\n");
			acceleration_gravity=0.0f;
			gravity=0.0f;
			y=(y/TILE_HEIGHT)*TILE_HEIGHT;
			if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
		}
	}
	if (collision_matrix[1][4]==TileType::Blocking || collision_matrix[2][4]==TileType::Blocking) {
		//printf ("col 1\n");
		while (world.getType(ppl7::grafix::Point(x-(TILE_WIDTH/2), y-1))==TileType::Blocking
				|| world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2), y-1))==TileType::Blocking) {
			y--;
		}
		velocity_move.x=0;
		velocity_move.y=0;
		acceleration_gravity=0.0f;
		gravity=0.0f;
		if (movement==ClimbDown) stand();
	}

	if (player_stands_on_object) {
		if ((movement==Falling || movement==Jump) && gravity>0.0f) {
			stand();
		}
		acceleration_gravity=0.0f;
		gravity=0.0f;

	}

	if (movement==Walk || movement==Run || movement==Jump || movement==Falling) {
		if (orientation==Left) {
			if (isCollisionLeft()) {
				velocity_move.x=0;
				//printf ("debug 3a\n");
				if (movement!=Jump && movement!=Falling)	stand();
				//else movement=Falling;
			}
		} else if (orientation==Right) {
			if (isCollisionRight()) {
				velocity_move.x=0;
				//printf ("debug 3b\n");
				if (movement!=Jump && movement!=Falling)	stand();
				//else movement=Falling;

			}
		}
	}

	if (collision_type_count[TileType::Type::Speer]>0) {
		this->dropHealth(10);
	}
	if (collision_type_count[TileType::Type::Fire]>0) {
		this->dropHealth(10);
	}

}

bool Player::isCollisionLeft() const
{
	for (int cy=1;cy<5;cy++) {
		if (collision_matrix[0][cy]==TileType::Blocking) {
			return true;
		}
	}
	return false;
}

bool Player::isCollisionRight() const
{
	for (int cy=1;cy<5;cy++) {
		if (collision_matrix[3][cy]==TileType::Blocking) {
			return true;
		}
	}
	return false;
}

Player::PlayerMovement Player::getMovement() const
{
	return movement;
}

void Player::updatePhysics(const TileTypePlane &world)
{
	if (movement==Jump || movement==Slide || movement==Dead) return;
	bool match=false;
	if (collision_matrix[1][4]==TileType::NonBlocking && collision_matrix[2][4]==TileType::NonBlocking) {
		if (collision_matrix[1][5]==TileType::NonBlocking && collision_matrix[2][5]==TileType::NonBlocking) {
			if (!player_stands_on_object) {
				//printf ("gravity\n");
				if (acceleration_gravity<6.0f) acceleration_gravity+=0.05f;
				if (acceleration_gravity>6.0f) acceleration_gravity=6.0f;
				match=true;
			}
		}
	}
	if (acceleration_gravity>0.0f) {
		if (!match) {
			acceleration_gravity-=acceleration_gravity/2.0f;
			if (acceleration_gravity<0.0f) acceleration_gravity=0.0f;
		}
		gravity+=acceleration_gravity;
		if (gravity>16.0f) gravity=16.0f;
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
	if (collision_at_pivoty[1]==TileType::SteepRampLeft && movement!=Slide) {
		// Start Slide
		velocity_move.x=-((float)TILE_WIDTH/4.0f);
		velocity_move.y=((float)TILE_HEIGHT/4.0f);
		gravity=0.0f;
		acceleration_gravity=0.0f;
		movement=Slide;
		orientation=Left;
		animation.start(slide_left,sizeof(slide_left)/sizeof(int),false,86);
	} else if (collision_at_pivoty[1]==TileType::SteepRampRight && movement!=Slide) {
		// Start Slide
		velocity_move.x=((float)TILE_WIDTH/4.0f);
		velocity_move.y=((float)TILE_HEIGHT/4.0f);
		gravity=0.0f;
		acceleration_gravity=0.0f;
		movement=Slide;
		orientation=Right;
		animation.start(slide_right,sizeof(slide_right)/sizeof(int),false,82);
	}

	/*
	printf ("gravity: %2.3f, acceleration_gravity: %2.3f, acceleration_airstream: %2.3f\n",
			gravity, acceleration_gravity, acceleration_airstream);
	*/
}


void Player::checkCollisionWithObjects(Decker::Objects::ObjectSystem *objects)
{
	// we try to find existing pixels inside the player boundary
	// to build a list with points we want to check against the
	// objects
	if (movement==Dead) return;
	std::list<ppl7::grafix::Point> checkpoints;
	checkpoints.push_back(ppl7::grafix::Point(x,y));

	const ppl7::grafix::Drawable &draw=sprite_resource->getDrawable(animation.getFrame());
	ppl7::grafix::Rect boundary=sprite_resource->spriteBoundary(animation.getFrame(),1.0f,x,y);

	if (draw.width()) {
		//printf ("boundary= %d:%d - %d:%d\n", boundary.x1, boundary.y1, boundary.x2, boundary.y2);
		int stepx=boundary.width()/6;
		int stepy=boundary.height()/6;
		for (int py=boundary.y1;py<boundary.y2;py+=stepx) {
			for (int px=boundary.x1;px<boundary.x2;px+=stepy) {
				ppl7::grafix::Color c=draw.getPixel(px-boundary.x1, py-boundary.y1);
				if (c.alpha()>92) {
					checkpoints.push_back(ppl7::grafix::Point(px,py));
				}
			}
		}
	}
	//printf ("check collision against %zd points:\n", checkpoints.size());

	Decker::Objects::Object *object=objects->detectCollision(checkpoints);
	if (!object) return;
	Decker::Objects::Collision col;
	col.detect(object,checkpoints, *this);
	object->handleCollision(this, col);
	/*
	printf ("Detected Collision with Object: %s, ID: %d\n",
			(const char*)object->typeName(), object->id);
			*/

}
