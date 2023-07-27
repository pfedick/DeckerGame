#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
//#include "decker.h"

namespace Decker::Objects {

Representation Fire::representation()
{
	return Representation(Spriteset::GenericObjects, 185);
}

Fire::Fire()
	: Trap(Type::ObjectType::Fire)
{
	next_animation=0.0f;
	sprite_set=Spriteset::GenericObjects;
	sprite_no=185;
	sprite_no_representation=185;
	collisionDetection=true;
}

void Fire::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	if (time > next_animation) {
		next_animation=time + 0.05f;
		sprite_no=ppl7::rand(182, 191);
		updateBoundary();
	}
}

void Fire::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(2, Physic::HealthDropReason::Burned);
}




}	// EOF namespace Decker::Objects
