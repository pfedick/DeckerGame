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
	layers=4;
	sprite_set=Spriteset::GenericObjects;
	sprite_no=192;
	sprite_no_representation=192;
	collisionDetection=true;
	fragment_y=0;
	velocity=0.0f;
}

void BreakingGround::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (state>0) {
		if (time>next_animation) {
			next_animation=time+0.05f;
			state++;
			if (state>10) state=1;
			//sprite_no=ppl7::rand(182,191);
			//updateBoundary();
		}
		if (velocity<10) {
			velocity+=0.5;
		}
		fragment_y+=velocity;
		if (fragment_y>200) {
			state=0;
		}
	}
}

void BreakingGround::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	if (state>0) {
		texture->draw(renderer,
				p.x+coords.x,
				p.y+coords.y+fragment_y+10,
				195+state);

	}
	if (layers>0) Object::draw(renderer, coords);
}

void BreakingGround::handleCollision(Player *player, const Collision &collision)
{
	if (collision.onFoot()) {
		if (layers>0) {
			player->setStandingOnObject(this);
			if (state==0) {
				velocity=0.0f;
				fragment_y=0;
				state=1;
				sprite_no++;
				if (layers>0) layers--;
				next_animation=0.0f;
			}
		}

	}
}


}	// EOF namespace Decker::Objects
