#ifndef INCLUDE_HELENA_H_
#define INCLUDE_HELENA_H_
#include <ppl7-grafix.h>
#include "ai_figure.h"

class TileTypePlane;

namespace Decker::Objects {
class Helena : public AiEnemy
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

	void updateStatePatrol(double time, TileTypePlane& ttplane);

public:
	Helena();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void toggle(bool enable, Object* source=NULL) override;
};

}	// EOF Decker::Objects

#endif
