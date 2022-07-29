#include "decker.h"
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"

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
static int death_by_falling[]={ 89,89,106,106,89,89,106,106,89,106,89,106,89,89,
		106,106,89 };




Player::Player(Game* game)
{
	x=y=0;
	sprite_resource=NULL;
	tiletype_resource=NULL;
	next_keycheck=0.0f;
	next_animation=0.0f;
	idle_timeout=0.0f;
	animation.setStaticFrame(27);
	points=0;
	health=100;
	lifes=3;
	godmode=false;
	this->game=game;
	dead=false;
	visible=true;
}

Player::~Player()
{

}

void Player::resetState()
{
	points=0;
	health=100;
	lifes=3;
	godmode=false;
	dead=false;
	visible=true;
}

void Player::setVisible(bool flag)
{
	visible=flag;
}



ppl7::grafix::PointF Player::position() const
{
	return ppl7::grafix::PointF(x, y);
}


void Player::setZeroVelocity()
{
	velocity_move.x=0;
	velocity_move.y=0;
}

void Player::setSpriteResource(const SpriteTexture& resource)
{
	sprite_resource=&resource;
}

void Player::setTileTypeResource(const SpriteTexture& resource)
{
	tiletype_resource=&resource;
}

void Player::setGodMode(bool enabled)
{
	godmode=enabled;
}

void Player::move(int x, int y)
{
	this->x=x;
	this->y=y;
}


void Player::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	if (!visible) return;
	ppl7::grafix::Point p(x + viewport.x1 - worldcoords.x, y + viewport.y1 - worldcoords.y);
	if (movement == Slide) p.y+=35;
	sprite_resource->draw(renderer, p.x, p.y + 1, animation.getFrame());
	/*
	SDL_SetRenderDrawColor(renderer,255,0,0,255);
	sprite_resource->drawBoundingBox(renderer,p.x,p.y+1,animation.getFrame());
	*/
}

void Player::drawCollision(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	ppl7::grafix::Point p(x + viewport.x1 - worldcoords.x, y + viewport.y1 - worldcoords.y);
	if (tiletype_resource) {
		for (int cy=0;cy < 6;cy++) {
			for (int cx=0;cx < 4;cx++) {
				tiletype_resource->draw(renderer, p.x - (TILE_WIDTH * 2) + (cx * TILE_WIDTH), p.y - (5 * TILE_HEIGHT) + (cy * TILE_HEIGHT), collision_matrix[cx][cy]);
			}
		}
	}
	tiletype_resource->draw(renderer, p.x + 80, p.y - 32, collision_at_pivoty[0]);
	tiletype_resource->draw(renderer, p.x + 80, p.y, collision_at_pivoty[1]);
	tiletype_resource->draw(renderer, p.x + 80, p.y + 32, collision_at_pivoty[2]);
}


void Player::turn(PlayerOrientation target)
{
	movement=Turn;
	turnTarget=target;
	if (orientation == Front) {
		if (target == Left) {
			animation.start(turn_from_mid_to_left, sizeof(turn_from_mid_to_left) / sizeof(int), false, 0);
		} else {
			animation.start(turn_from_mid_to_right, sizeof(turn_from_mid_to_right) / sizeof(int), false, 9);
		}
	} else if (orientation == Left) {
		if (target == Right) {
			animation.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 9);
		} else if (target == Front) {
			animation.start(turn_from_left_to_mid, sizeof(turn_from_left_to_mid) / sizeof(int), false, 27);
		}
	} else if (orientation == Right) {
		if (target == Left) {
			animation.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 0);
		} else if (target == Front) {
			animation.start(turn_from_right_to_mid, sizeof(turn_from_right_to_mid) / sizeof(int), false, 27);
		}
	}
}

int Player::getKeyboardMatrix(const Uint8* state)
{
	if (state == NULL) state=SDL_GetKeyboardState(NULL);
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
	if (state[SDL_SCANCODE_E] || state[SDL_SCANCODE_O]) matrix|=KeyboardKeys::Action;
	return matrix;
}

void Player::stand()
{
	movement=Stand;
	if (orientation == Left) animation.setStaticFrame(0);
	else if (orientation == Right) animation.setStaticFrame(9);
	else if (orientation == Front) animation.setStaticFrame(27);
	else if (orientation == Back) animation.setStaticFrame(28);
	idle_timeout=time + 4.0;
}

void Player::addPoints(int points)
{
	if (movement == Dead) return;
	this->points+=points;
}

void Player::addHealth(int points)
{
	if (movement == Dead) return;
	this->health+=points;
	if (health > 100) health=100;
}

void Player::addLife(int lifes)
{
	if (movement == Dead) return;
	this->lifes+=lifes;
}

void Player::dropHealth(int points, HealthDropReason reason)
{
	if (godmode) return;
	if (movement == Dead) return;
	health-=points;
	if (health <= 0 && movement != Dead) {
		health=0;
		movement=Dead;
		fallstart=0.0f;
		// we can play different animations for different reasons
		if (reason == FallingDeep)
			animation.start(death_by_falling, sizeof(death_by_falling) / sizeof(int), false, 106);
		else
			animation.start(death_animation, sizeof(death_animation) / sizeof(int), false, 106);
	}
}

void Player::addInventory(int object_id, const Decker::Objects::Representation& repr)
{
	Inventory.insert(std::pair<int, Decker::Objects::Representation>(object_id, repr));

}

bool Player::isInInventory(int object_id) const
{
	if (object_id > 0) {
		std::map<int, Decker::Objects::Representation>::const_iterator it;
		it=Inventory.find(object_id);
		if (it != Inventory.end()) return true;
	}
	return false;
}

bool Player::isDead() const
{
	return dead;
}

void Player::setSavePoint(const ppl7::grafix::Point& p)
{
	lastSavePoint=p;
}

void Player::setStandingOnObject(Decker::Objects::Object* object)
{
	player_stands_on_object=object;
}

void Player::dropLifeAndResetToLastSavePoint()
{
	dead=false;
	lifes--;
	health=100;
	x=lastSavePoint.x;
	y=lastSavePoint.y;
	stand();
}

void Player::update(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	this->time=time;
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
	}
	if (movement == Dead) {
		if (animation.isFinished()) {
			dead=true;
		}
		return;
	}
	if (dead) return;
	dropHealth(detectFallingDamage(time), HealthDropReason::FallingDeep);
	updateMovement(frame_rate_compensation);
	player_stands_on_object=NULL;
	checkCollisionWithObjects(objects);
	if (movement == Dead) return;
	checkCollisionWithWorld(world);
	if (updatePhysics(world, frame_rate_compensation)) {
		if (movement == Slide && orientation == Left) {
			animation.start(slide_left, sizeof(slide_left) / sizeof(int), false, 86);
		} else if (movement == Slide && orientation == Right) {
			animation.start(slide_right, sizeof(slide_right) / sizeof(int), false, 82);
		}
	}
	x+=velocity_move.x;
	y+=velocity_move.y + gravity;


	if (movement == Turn) {
		if (!animation.isFinished()) return;
		//printf ("debug 2\n");
		movement=Stand;
		orientation=turnTarget;
		velocity_move.stop();
		//printf("Turn done, movement=%d, orientation=%d\n", (int)movement, (int)orientation);
	}
	if (movement == Slide || movement == Dead) {
		return;
	}
	if (movement == Jump || movement == Falling) {
		handleKeyboardWhileJumpOrFalling(time, world, objects);
		return;
	}
	//if (time>next_keycheck) {
	//next_keycheck=time+0.1f;
	const Uint8* state = SDL_GetKeyboardState(NULL);
	int keys=getKeyboardMatrix(state);
	if (keys == KeyboardKeys::Left) {
		if (orientation != Left) { turn(Left); return; }
		if (movement != Walk) {
			movement=Walk;
			animation.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
		}
	} else if (keys == (KeyboardKeys::Left | KeyboardKeys::Shift)) {
		if (movement != Run || orientation != Left) {
			movement=Run;
			orientation=Left;
			animation.start(run_cycle_left, sizeof(run_cycle_left) / sizeof(int), true, 0);
		}
	} else if (keys == KeyboardKeys::Right) {
		if (orientation != Right) { turn(Right); return; }
		if (movement != Walk) {
			movement=Walk;
			animation.start(walk_cycle_right, sizeof(walk_cycle_right) / sizeof(int), true, 0);
		}
	} else if (keys == (KeyboardKeys::Right | KeyboardKeys::Shift)) {
		if (movement != Run || orientation != Right) {
			movement=Run;
			orientation=Right;
			animation.start(run_cycle_right, sizeof(run_cycle_right) / sizeof(int), true, 0);
		}
	} else if ((keys == KeyboardKeys::Up || keys == (KeyboardKeys::Up | KeyboardKeys::Shift)) && movement != Falling && movement != Jump) {
		if (collision_matrix[1][4] == TileType::Ladder || collision_matrix[2][4] == TileType::Ladder) {
			if (movement != ClimbUp) {
				movement=ClimbUp;
				orientation=Back;
				animation.start(climb_up_cycle, sizeof(climb_up_cycle) / sizeof(int), true, 0);
			}
		} else {
			if (movement != Jump) {
				movement=Jump;
				jump_climax=time + 0.4f;
				acceleration_jump=1.0f;
				if (orientation == Front) animation.setStaticFrame(42);
				else if (orientation == Left) animation.setStaticFrame(40);
				else if (orientation == Right) animation.setStaticFrame(41);
				else animation.setStaticFrame(28);
			}
		}
	} else if ((keys & KeyboardKeys::JumpLeft) == KeyboardKeys::JumpLeft) {
		movement=Jump;
		orientation=Left;
		jump_climax=time + 0.4f;
		acceleration_jump=1.0f;
		velocity_move.x=-2;
		if (keys & KeyboardKeys::Shift) velocity_move.x=-6;
		animation.setStaticFrame(38);
	} else if ((keys & KeyboardKeys::JumpRight) == KeyboardKeys::JumpRight) {
		movement=Jump;
		orientation=Right;
		jump_climax=time + 0.4f;
		acceleration_jump=1.0f;
		velocity_move.x=2;
		if (keys & KeyboardKeys::Shift) velocity_move.x=6;
		animation.setStaticFrame(39);
	} else if (keys == KeyboardKeys::Down || keys == (KeyboardKeys::Down | KeyboardKeys::Shift)) {
		if (collision_matrix[1][4] == TileType::Ladder || collision_matrix[2][4] == TileType::Ladder
			|| collision_matrix[1][5] == TileType::Ladder || collision_matrix[2][5] == TileType::Ladder) {
			if (collision_matrix[1][5] != TileType::Blocking && collision_matrix[2][5] != TileType::Blocking) {
				if (movement != ClimbDown) {
					//printf ("climb down\n");
					movement=ClimbDown;
					orientation=Back;
					animation.start(climb_down_cycle, sizeof(climb_down_cycle) / sizeof(int), true, 0);
				}
			}
		}

	} else if (keys == (KeyboardKeys::Left) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=-2;
	} else if (keys == (KeyboardKeys::Right) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=2;
	} else if (keys == (KeyboardKeys::Left | KeyboardKeys::Shift) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=-6;
	} else if (keys == (KeyboardKeys::Right | KeyboardKeys::Shift) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=6;

	} else {
		if (movement != Stand && movement != Jump && movement != Falling) {
			//printf ("debug 1\n");
			stand();
		} else if (movement == Stand && time > idle_timeout && orientation != Front) {
			turn(Front);

		}
	}
//}
}

void Player::handleKeyboardWhileJumpOrFalling(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	int keys=getKeyboardMatrix(state);
	if (movement == Jump) {
		if (!(keys & KeyboardKeys::Up)) {
			movement=Falling;
			return;
		}
		if ((keys & KeyboardKeys::Left) && velocity_move.x == 0) {
			velocity_move.x=-2;
			if (keys & KeyboardKeys::Shift) velocity_move.x=-6;
			orientation=Left;
			animation.setStaticFrame(38);
		} else if ((keys & KeyboardKeys::Right) && velocity_move.x == 0) {
			velocity_move.x=2;
			if (keys & KeyboardKeys::Shift) velocity_move.x=6;
			orientation=Right;
			animation.setStaticFrame(39);
		}
	} else {
		if (keys & KeyboardKeys::Left) {
			velocity_move.x=-2;
			if (keys & KeyboardKeys::Shift) velocity_move.x=-6;
			orientation=Left;
			animation.setStaticFrame(38);
		} else if (keys & KeyboardKeys::Right) {
			velocity_move.x=2;
			if (keys & KeyboardKeys::Shift) velocity_move.x=6;
			orientation=Right;
			animation.setStaticFrame(39);
		}
	}
}

void Player::checkCollisionWithWorld(const TileTypePlane& world)
{
	Physic::PlayerMovement new_movement=Physic::checkCollisionWithWorld(world, x, y);
	if (new_movement == Stand) stand();
	if (collision_type_count[TileType::Type::Speer] > 0) {
		this->dropHealth(10);
	}
	if (collision_type_count[TileType::Type::Fire] > 0) {
		this->dropHealth(10);
	}

}


void Player::checkCollisionWithObjects(Decker::Objects::ObjectSystem* objects)
{
	// we try to find existing pixels inside the player boundary
	// to build a list with points we want to check against the
	// objects
	if (movement == Dead) return;
	std::list<ppl7::grafix::Point> checkpoints;
	checkpoints.push_back(ppl7::grafix::Point(x, y));

	const ppl7::grafix::Drawable& draw=sprite_resource->getDrawable(animation.getFrame());
	ppl7::grafix::Rect boundary=sprite_resource->spriteBoundary(animation.getFrame(), 1.0f, x, y);

	if (draw.width()) {
		//printf ("boundary= %d:%d - %d:%d\n", boundary.x1, boundary.y1, boundary.x2, boundary.y2);
		int stepx=boundary.width() / 6;
		int stepy=boundary.height() / 12;
		for (int py=boundary.y1;py < boundary.y2;py+=stepx) {
			for (int px=boundary.x1;px < boundary.x2;px+=stepy) {
				ppl7::grafix::Color c=draw.getPixel(px - boundary.x1, py - boundary.y1);
				if (c.alpha() > 92) {
					checkpoints.push_back(ppl7::grafix::Point(px, py));
				}
			}
		}
	}
	//printf ("check collision against %zd points:\n", checkpoints.size());

	Decker::Objects::Object* object=objects->detectCollision(checkpoints);
	if (!object) return;
	Decker::Objects::Collision col;
	col.detect(object, checkpoints, *this);
	col.bounding_box_player=getBoundingBox();
	col.bounding_box_object=object->boundary;
	col.bounding_box_intersection=col.bounding_box_player.intersected(col.bounding_box_object);

	object->handleCollision(this, col);

	//printf ("Detected Collision with Object: %s, ID: %d\n",
	//		(const char*)object->typeName(), object->id);
	//const ppl7::grafix::Rect &bbi=col.bounding_box_intersection;
	//printf ("BoundingBox Player: %d:%d - %d:%d\n", bbp.x1, bbp.y1, bbp.x2, bbp.y2);
	//printf ("BoundingBox Object: %d:%d - %d:%d\n", bbo.x1, bbo.y1, bbo.x2, bbo.y2);
	//printf ("Intersection:       %d:%d - %d:%d\n", bbi.x1, bbi.y1, bbi.x2, bbi.y2);


}

ppl7::grafix::Rect Player::getBoundingBox() const
{
	return sprite_resource->spriteBoundary(animation.getFrame(), 1.0f, x, y);

}
