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

};

class Player
{
private:
	int x,y;
	ppl7::grafix::Point velocity;
	const SpriteTexture *sprite_resource;
	ppl7::grafix::Rect world;
	double next_keycheck;
	double next_animation;
	double idle_timeout;
	AnimationCycle animation;

	enum PlayerMovement {
		Stand,
		Turn,
		Walk,
		Run,
		Pickup,
		Climb,
		Jump
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

public:
	Player();
	~Player();
	void setSpriteResource(const SpriteTexture &resource);
	void setGameWindow(const ppl7::grafix::Rect &world);
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void move(int x, int y);

	void update(double time);


};
#endif /* INCLUDE_PLAYER_H_ */
