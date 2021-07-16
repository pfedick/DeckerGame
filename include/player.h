#ifndef INCLUDE_PLAYER_H_
#define INCLUDE_PLAYER_H_
#include <ppl7-grafix.h>

class SDL_Renderer;
class SpriteTexture;

class AnimationCycle
{
private:
	int *cycle;
	int index;
	int size;
	int endframe;
	bool loop;
	bool finished;

public:
	AnimationCycle();
	void setStaticFrame(int nr);
	void start(int *cycle_array, int size, bool loop, int endframe);
	void update();
	int getFrame() const;
	bool isFinished() const;
	int getIndex() const;

};

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
private:
	Velocity velocity_move, acceleration;
	float gravity, acceleration_gravity;
	float acceleration_airstream;
	float acceleration_jump;
	const SpriteTexture *sprite_resource;
	const SpriteTexture *tiletype_resource;

	ppl7::grafix::Rect world;
	double next_keycheck;
	double next_animation;
	double idle_timeout;
	double jump_climax;
	double time;
	AnimationCycle animation;

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
	PlayerMovement movement=Stand;
	PlayerOrientation orientation=Front;
	PlayerOrientation turnTarget;

	void turn(PlayerOrientation target);
	void stand();
	void updateMovement();
	void checkCollisionWithWorld(const TileTypePlane &world);
	void updatePhysics(const TileTypePlane &world);
	int getKeyboardMatrix(const Uint8 *state);
	int collision_matrix[4][6];


public:
	int x,y;
	Player();
	~Player();
	void setSpriteResource(const SpriteTexture &resource);
	void setTileTypeResource(const SpriteTexture &resource);
	void setGameWindow(const ppl7::grafix::Rect &world);
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void drawCollision(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void move(int x, int y);
	ppl7::String getState() const;

	void update(double time, const TileTypePlane &world);



};
#endif /* INCLUDE_PLAYER_H_ */
