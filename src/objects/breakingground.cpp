#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

Representation BreakingGround::representation()
{
	return Representation(Spriteset::GenericObjects, 192);
}

BreakingGround::BreakingGround()
: Object(Type::ObjectType::BreakingGround)
{
	next_animation=0.0f;
	state=0;
	sprite_set=Spriteset::GenericObjects;
	sprite_no=192;
	sprite_no_representation=192;
	collisionDetection=true;
}

void BreakingGround::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (time>next_animation) {
		next_animation=time+0.05f;
		//sprite_no=ppl7::rand(182,191);
		//updateBoundary();
	}
}

void BreakingGround::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	Object::draw(renderer, coords);
}

void BreakingGround::handleCollision(Player *player, const Collision &collision)
{
	if (collision.onFoot()) {
		//printf ("collision with floater\n");
		player->setStandingOnObject(this);
	}
}


}	// EOF namespace Decker::Objects
