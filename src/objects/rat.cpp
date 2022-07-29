#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Rat::representation()
{
	return Representation(Spriteset::GenericObjects, 38);
}

Rat::Rat()
	:Enemy(Type::ObjectType::Rat)
{
	sprite_no=38;
	sprite_no_representation=38;
	state=0;
	collisionDetection=true;

}

void Rat::handleCollision(Player* player, const Collision& collision)
{
	Player::PlayerMovement movement=player->getMovement();
	if (collision.onFoot() == true && movement == Player::Falling) {
		state=0;
		collisionDetection=false;
		enabled=false;
		player->addPoints(50);
	} else {
		player->dropHealth(2);
	}
}

void Rat::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (state == 0) {
		p.x-=2 * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=1;
			sprite_no=39;
		}
		updateBoundary();
	} else if (state == 1) {
		p.x+=2 * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=0;
			sprite_no=38;
		}
		updateBoundary();
	}
}




}	// EOF namespace Decker::Objects
