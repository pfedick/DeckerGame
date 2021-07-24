#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

Floater::Floater(Type::ObjectType type)
: Object(type)
{
	sprite_no_representation=115;
	direction=0;
}

void Floater::update(double time, TileTypePlane &ttplane, Player &player)
{

}

void Floater::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	Object::draw(renderer, coords);
}

void Floater::handleCollision(Player *player, const Collision &collision)
{

}

Representation FloaterHorizontal::representation()
{
	return Representation(Spriteset::GenericObjects, 115);
}


Representation FloaterVertical::representation()
{
	return Representation(Spriteset::GenericObjects, 116);
}

FloaterHorizontal::FloaterHorizontal()
: Floater(Type::FloaterHorizontal)
{
	sprite_no_representation=115;
	sprite_no=115;
	direction=0;
}


FloaterVertical::FloaterVertical()
: Floater(Type::FloaterVertical)
{
	sprite_no_representation=116;
	sprite_no=116;
	direction=1;
}




}	// EOF namespace Decker::Objects
