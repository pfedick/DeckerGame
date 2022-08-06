#include <ppl7.h>
#include <ppl7-grafix.h>
#include "helena.h"
#include "audiopool.h"
#include "player.h"
#include "waynet.h"
#include "objects.h"

namespace Decker::Objects {



AutoGeorge::AutoGeorge()
	:AiEnemy(Type::ObjectType::AutoGeorge)
{
	sprite_set=Spriteset::George;
	sprite_no=27;
	animation.setStaticFrame(27);
	keys=0;
	speed_walk=2.0f;
	speed_run=5.5f;
}

Representation AutoGeorge::representation()
{
	return Representation(Spriteset::George, 27);
}



void AutoGeorge::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	//printf ("s=%d, state=%s, keys=%d\n", state, (const char*)getState(), keys);
	player.x=p.x;
	player.y=p.y;


	this->time=time;
	if (!enabled) return;
	updateAnimation(time);
	if (movement == Dead) {
		if (animation.isFinished()) {
			enabled=false;
		}
		return;
	}

	if (movement == Turn) {
		if (!animation.isFinished()) return;
		movement=Stand;
		orientation=turnTarget;
		velocity_move.stop();
	}

	updateMovementAndPhysics(time, ttplane, frame_rate_compensation);

	if (movement == Slide || movement == Dead || movement == Jump) {
		return;
	}
	Decker::Objects::ObjectSystem* objects=GetObjectSystem();
	if (target.isNull()) {
		objects->resetPlayerStart();
		target=objects->findPlayerStart();
		//printf("Next Target: %d, %d\n", target.x, target.y);
	}
	double dist=ppl7::grafix::Distance(p, ppl7::grafix::PointF(target));
	if (dist < 500) {
		target=objects->nextPlayerStart();
		//printf("Next Target: %d, %d\n", target.x, target.y);
	}
	fflush(stdout);



	updateStateFollowPlayer(time, ttplane, target);


	executeKeys();
	player.x=p.x;
	player.y=p.y;
}




}	// EOF namespace
