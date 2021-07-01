#include "decker.h"
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"

Player::Player()
{
	world_x=0;
	world_y=0;
	x=y=0;
	sprite_nr=27;
	sprite_resource=NULL;
	next_keycheck=0.0f;
	next_animation=0.0f;
	idle_timeout=0.0f;
	running=false;
}

Player::~Player()
{

}

void Player::setSpriteResource(const Sprite &resource)
{
	sprite_resource=&resource;
}

void Player::setGameWindow(const ppl7::grafix::Rect &world)
{
	this->world=world;
	x=world.width()/2+world.left();
	y=world.height()/2+world.top();
	world_x=x;
	world_y=y;

}

void Player::move(int x, int y)
{
	world_x=x;
	world_y=y;
}


void Player::draw(SDL_Renderer *renderer) const
{
	ppl7::grafix::Point p;
	//if (p.inside(world)) {
	sprite_resource->draw(renderer,x,y,sprite_nr);
	//
}

void Player::update(double time)
{
	if (time>next_animation) {
		next_animation=time+0.1f;
		if (running) next_animation=time+0.04f;
		if (movement==MoveLeft) {
			sprite_nr+=1;
			if (sprite_nr>8) sprite_nr=0;

		} else if (movement==MoveRight) {
			sprite_nr+=1;
			if (sprite_nr>17) sprite_nr=9;
		} else if (movement==MoveToMid) {
			if (lastmovement==StandLeft) {
				sprite_nr+=1;
				if (sprite_nr>24) {
					sprite_nr=27;
					movement=Stand;
				}
			} else if (lastmovement==StandRight) {
				sprite_nr+=1;
				if (sprite_nr>20) {
					sprite_nr=27;
					movement=Stand;
				}
			}

		} else if(time>idle_timeout && movement!=Stand) {
			movement=MoveToMid;
			if (sprite_nr==0) {
				lastmovement=StandLeft;
				sprite_nr=22;
			} else if (sprite_nr==9 || sprite_nr==18) {
				lastmovement=StandRight;
				sprite_nr=18;
			}

		}
	}
	if (time>next_keycheck) {
		next_keycheck=time+0.1f;
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_LEFT]) {
			if (movement!=MoveLeft) {
				movement=MoveLeft;
				sprite_nr=0;
			}
			if (state[SDL_SCANCODE_LSHIFT]) {
				running=true;
			} else {
				running=false;
			}
		} else if (state[SDL_SCANCODE_RIGHT]) {
			if (movement!=MoveRight) {
				movement=MoveRight;
				sprite_nr=9;
			}
			if (state[SDL_SCANCODE_LSHIFT]) {
				running=true;
			} else {
				running=false;
			}

		} else {
			if (movement==MoveLeft) {
				movement=StandLeft;
				sprite_nr=0;
				idle_timeout=time+2.0f;
			} else if (movement==MoveRight) {
				movement=StandRight;
				sprite_nr=9;
				idle_timeout=time+2.0f;
			}

			running=false;
		}

	}

}
