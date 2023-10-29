#ifndef INCLUDE_PHYSIC_H_
#define INCLUDE_PHYSIC_H_

#include "decker.h"
class TileTypePlane;

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
		Action=32,
		Flashlight=64,
		JumpLeft=5,
		JumpRight=6,
	};
};


class Physic
{
public:
	enum PlayerMovement {
		Unchanged=0,
		Stand,
		Turn,
		Walk,
		Run,
		Pickup,
		ClimbUp,
		ClimbDown,
		Jump,
		Falling,
		Slide,
		Floating,
		Dead,
		Swim,
		SwimStraight,
		SwimUp,
		SwimDown,
		Hacking,
		Crouch,
		Crawling,
		CrawlTurn
	};
	enum PlayerOrientation {
		Left,
		Right,
		Front,
		Back
	};

	enum HealthDropReason {
		Unknown,
		FallingDeep,
		Smashed,
		Drowned,
		Burned
	};

	class Keys {
	public:
		int matrix;
		int velocity_x;
		int velocity_y;
	};
	Keys keys;

	PlayerMovement movement=Stand;
	PlayerOrientation orientation=Front;
	PlayerOrientation turnTarget;

	Velocity velocity_move, acceleration;
	float gravity, acceleration_gravity;
	float acceleration_airstream;
	float acceleration_jump;
	float acceleration_jump_sideways;
	float speed_walk;
	float speed_run;

	double jump_climax;
	double time;
	double fallstart;

	int collision_matrix[4][6];
	int collision_at_pivoty[3];
	int collision_type_count[TileType::Type::MaxType];
	Decker::Objects::Object* player_stands_on_object;

	Physic();

	bool updatePhysics(const TileTypePlane& world, float frame_rate_compensation);
	PlayerMovement checkCollisionWithWorld(const TileTypePlane& world, float& x, float& y);
	int detectFallingDamage(double time, float frame_rate_compensation);
	void updateMovement(float frame_rate_compensation);

	bool isCollisionLeft() const;
	bool isCollisionRight() const;
	bool isSwimming() const;
	bool isDiving() const;
	bool isOnGround() const;
	PlayerMovement getMovement() const;
	ppl7::String getState() const;

	void updateCollisionMatrix(const TileTypePlane& world, float& x, float& y);


};

#endif // INCLUDE_PHYSIC_H_
