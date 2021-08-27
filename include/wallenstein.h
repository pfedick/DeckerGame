#ifndef INCLUDE_WALLENSTEIN_H_
#define INCLUDE_WALLENSTEIN_H_
#include <ppl7-grafix.h>
#include "animation.h"
#include "objects.h"
#include "physic.h"
#include "waynet.h"

class SDL_Renderer;
class SpriteTexture;
class TileTypePlane;

namespace Decker::Objects {
class Wallenstein : public Enemy, public Physic
{
private:
	enum State {
		StateWaitForEnable,
		StateStand,
		StatePatrol,
		StateGoToOrigin,
		StateFollowPlayer
	};
	AnimationCycle animation;
	double next_state, next_animation, next_wayfind;
	double idle_timeout;
	int state;
	int substate;
	int keys;
	std::list<Connection> waypoints;
	WayPoint last_pwp;

	void turn(PlayerOrientation target);
	void stand();
	void updateStatePatrol(double time, TileTypePlane &ttplane);
	void updateStateFollowPlayer(double time, TileTypePlane &ttplane, Player &player);
	void executeKeys();
	void updateWay(Player &player);

public:
	Wallenstein();
	static Representation representation();
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void toggle(bool enable, Object *source=NULL);
};

}	// EOF Decker::Objects

#endif
