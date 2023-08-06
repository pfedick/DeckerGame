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
	double shoot_cooldown;
	int state;
	int substate;
	bool attack;
	int magazine;

	void switchAttackMode(bool enable);
	void updateStatePatrol(double time, TileTypePlane& ttplane);
	void shoot(double time, Player& player);
	void issueFireParticles(const ppl7::grafix::PointF& p, Player& player) const;
	void issueSmoke(const ppl7::grafix::PointF& p) const;

public:
	Helena();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void toggle(bool enable, Object* source=NULL) override;
	void playSoundOnAnimationSprite() override;
};

}	// EOF Decker::Objects

#endif
