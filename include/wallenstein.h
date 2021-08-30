#ifndef INCLUDE_WALLENSTEIN_H_
#define INCLUDE_WALLENSTEIN_H_
#include <ppl7-grafix.h>
#include "ai_figure.h"


class SDL_Renderer;
class SpriteTexture;
class TileTypePlane;

namespace Decker::Objects {
class Wallenstein : public AiEnemy
{
private:
	enum State {
		StateWaitForEnable,
		StateStand,
		StatePatrol,
		StateGoToOrigin,
		StateFollowPlayer
	};
	double next_state;
	int state;
	int substate;
	void updateStatePatrol(double time, TileTypePlane &ttplane);

public:
	Wallenstein();
	static Representation representation();
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void toggle(bool enable, Object *source=NULL);
};

}	// EOF Decker::Objects

#endif
