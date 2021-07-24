#ifndef INCLUDE_PLAYER_H_
#define INCLUDE_PLAYER_H_
#include <ppl7-grafix.h>

class SDL_Renderer;
class SpriteTexture;

#include "animation.h"

class TileTypePlane;
namespace Decker::Objects {
	class ObjectSystem;
} // EOF namespace Decker::Objects

class Velocity
{
public:
	Velocity(float x=0.0f, float y=0.0f) {
		this->x=x;
		this->y=y;
	}
	float x;
	float y;
	void stop() {
		x=0.0f;
		y=0.0f;
	}

};
class KeyboardKeys
{
public:
	enum {
		Left=1,
		Right=2,
		Up=4,
		Down=8,
		Shift=16,
	};
};

class Player
{
public:
	enum PlayerMovement {
		Stand,
		Turn,
		Walk,
		Run,
		Pickup,
		ClimbUp,
		ClimbDown,
		Jump,
		Falling
	};
	enum PlayerOrientation {
		Left,
		Right,
		Front,
		Back
	};
private:
	Velocity velocity_move, acceleration;
	float gravity, acceleration_gravity;
	float acceleration_airstream;
	float acceleration_jump;
	const SpriteTexture *sprite_resource;
	const SpriteTexture *tiletype_resource;

	double next_keycheck;
	double next_animation;
	double idle_timeout;
	double jump_climax;
	double time;
	AnimationCycle animation;


	PlayerMovement movement=Stand;
	PlayerOrientation orientation=Front;
	PlayerOrientation turnTarget;

	ppl7::grafix::Point lastSavePoint;

	void turn(PlayerOrientation target);
	void stand();
	void updateMovement();
	void checkCollisionWithWorld(const TileTypePlane &world);
	void checkCollisionWithObjects(Decker::Objects::ObjectSystem *objects);
	void updatePhysics(const TileTypePlane &world);
	int getKeyboardMatrix(const unsigned char *state);
	int collision_matrix[4][6];

	bool isCollisionLeft() const;
	bool isCollisionRight() const;


public:
	int x,y;
	int points, health, lifes;


	Player();
	~Player();
	void addPoints(int points);
	void dropHealth(int points);
	void setSavePoint(const ppl7::grafix::Point &p);
	void setSpriteResource(const SpriteTexture &resource);
	void setTileTypeResource(const SpriteTexture &resource);
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void drawCollision(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void move(int x, int y);
	PlayerMovement getMovement() const;
	ppl7::String getState() const;

	void update(double time, const TileTypePlane &world, Decker::Objects::ObjectSystem *objects);



};
#endif /* INCLUDE_PLAYER_H_ */
