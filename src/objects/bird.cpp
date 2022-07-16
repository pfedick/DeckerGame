#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Bird::representation()
{
	return Representation(Spriteset::Bird, 0);
}

Bird::Bird()
	:Enemy(Type::ObjectType::Bird)
{
	sprite_set=Spriteset::Bird;
	sprite_no=0;
	sprite_no_representation=0;
	state=0;
	collisionDetection=true;
	animation.startSequence(1, 10, true, 0);
	next_animation=0.0f;
	velocity=-3;
}

void Bird::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(10);
}

void Bird::update(double time, TileTypePlane& ttplane, Player& player)
{
	if (time > next_animation) {
		next_animation=time + 0.03f;
		animation.update();
		sprite_no=animation.getFrame();
		updateBoundary();
	}
	p.y+=velocity;
	if (state == 0) {
		if (velocity > -3.0f) {
			velocity-=0.2f;
		}
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y - 48));
		if (t1 != TileType::NonBlocking) {
			state=1;
		}
		updateBoundary();
	} else if (state == 1) {
		if (velocity < 3.0f) {
			velocity+=0.2f;
		}

		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 8));
		if (t1 != TileType::NonBlocking) {
			state=0;
		}
		updateBoundary();
	}
}




}	// EOF namespace Decker::Objects
