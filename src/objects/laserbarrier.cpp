#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Representation LaserBarrier::representation(Type::ObjectType type)
{
	if (type==Type::LaserBeamHorizontal) return Representation(Spriteset::GenericObjects, 213);
	return Representation(Spriteset::GenericObjects, 212);
}

LaserBarrier::LaserBarrier(Type::ObjectType type)
: Trap(type)
{
	next_state=0.0f;
	state=1;
}

void LaserBarrier::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{

}

void LaserBarrier::update(double time, TileTypePlane &ttplane, Player &player)
{

}

void LaserBarrier::handleCollision(Player *player, const Collision &collision)
{

}

}	// EOF namespace Decker::Objects
