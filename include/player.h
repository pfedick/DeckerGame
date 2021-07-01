#ifndef INCLUDE_PLAYER_H_
#define INCLUDE_PLAYER_H_
#include <ppl7-grafix.h>

class SDL_Renderer;
class Sprite;
class Player
{
private:
	int world_x;
	int world_y;
	int x,y;
	int sprite_nr;
	const Sprite *sprite_resource;
	ppl7::grafix::Rect world;
	double next_keycheck;
	double next_animation;
	double idle_timeout;
	bool running;

	enum PlayerMovement {
		Stand,
		StandLeft,
		StandRight,
		TurnLeft,
		TurnRight,
		MoveLeft,
		MoveRight,
		MoveToMid,

	};
	enum PlayerOrientation {
		Left,
		Right,
		Front,
		Back
	};
	PlayerMovement movement=Stand;
	PlayerMovement lastmovement=Stand;

public:
	Player();
	~Player();
	void setSpriteResource(const Sprite &resource);
	void setGameWindow(const ppl7::grafix::Rect &world);
	void draw(SDL_Renderer *renderer) const;
	void move(int x, int y);

	void update(double time);


};
#endif /* INCLUDE_PLAYER_H_ */
