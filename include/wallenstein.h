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
	bool attack;
	void updateStatePatrol(double time, TileTypePlane& ttplane);
	void switchAttackMode(bool enable);
public:
	Wallenstein();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void toggle(bool enable, Object* source=NULL) override;
};

}	// EOF Decker::Objects

#endif
