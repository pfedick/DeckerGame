#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include <SDL.h>

namespace Decker::Objects {

Representation HangingSpider::representation()
{
	return Representation(Spriteset::GenericObjects, 49);
}

HangingSpider::HangingSpider()
:Enemy(Type::ObjectType::HangingSpider)
{
	sprite_no=49;
	next_state=ppl7::GetMicrotime()+(double)ppl7::rand(5,20);
	sprite_no_representation=49;
	state=0;
	velocity=0.0f;
	collisionDetection=true;
}

void HangingSpider::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (state==0) {
		double dist=ppl7::grafix::Distance(p, ppl7::grafix::Point(player.x, player.y));
		if (dist<400 || next_state<time) {
			state=1;
			velocity=0.5f;
		}
	} else if (state==1) {
		if (velocity<8.0) velocity+=0.5f;
		p.y+=velocity;
		updateBoundary();
		TileType::Type t=ttplane.getType(ppl7::grafix::Point(p.x, p.y+4));
		if (t!=TileType::NonBlocking) {
			state=2;
			next_state=time+(double)ppl7::rand(2,10);
		}
	} else if (state==2 && next_state<time) {
		state=3;

	} else if (state==3) {
		if (p.y>initial_p.y) {
			p.y-=2;
			updateBoundary();
		} else {
			state=0;
			next_state=time+(double)ppl7::rand(5,20);
		}
	}

}

void HangingSpider::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	SDL_SetRenderDrawColor(renderer,0,0,0,255);
	SDL_RenderDrawLine(renderer,initial_p.x+coords.x, initial_p.y+coords.y-20,
			p.x+coords.x, p.y+coords.y-20);
	SDL_RenderDrawLine(renderer,initial_p.x+coords.x+1, initial_p.y+coords.y-20,
			p.x+coords.x+1, p.y+coords.y-20);

	Object::draw(renderer, coords);
}

void HangingSpider::handleCollision(Player *player)
{
	player->dropHealth(2);
}



}	// EOF namespace Decker::Objects
