#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

Floater::Floater(Type::ObjectType type)
: Object(type)
{
	sprite_no_representation=115;
	direction=0;
	collisionDetection=true;
	next_animation=0.0f;
	next_state=ppl7::GetMicrotime()+5.0f;
	state=0;
}

void Floater::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (direction==0) {
		if (next_state<time && (state==0 || state==2)) {
			state++;
		} else if (state==1) {
			velocity.x=4;
			velocity.y=0;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x+64, p.y));
			if (t1==TileType::Blocking) {
				velocity.setPoint(0,0);
				state=2;
				next_state=time+5.0f;
			}
		} else if (state==3) {
			velocity.x=-4;
			velocity.y=0;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x-64, p.y));
			if (t1==TileType::Blocking) {
				velocity.setPoint(0,0);
				state=0;
				next_state=time+5.0f;
			}
		}
	}
}

void Floater::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	Object::draw(renderer, coords);
}

void Floater::handleCollision(Player *player, const Collision &collision)
{
	if (collision.onFoot()) {
		//printf ("collision with floater\n");
		player->setStandingOnObject(this);
		player->x+=velocity.x;
		player->y+=velocity.y;
	}
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
