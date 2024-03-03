#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include <SDL.h>

namespace Decker::Objects {

Representation HangingSpider::representation()
{
	return Representation(Spriteset::GenericObjects, 48);
}

HangingSpider::HangingSpider()
	:Enemy(Type::ObjectType::HangingSpider)
{
	sprite_no=48;
	next_state=ppl7::GetMicrotime() + (double)ppl7::rand(5, 20);
	sprite_no_representation=48;
	state=0;
	velocity=0.0f;
	collisionDetection=true;
	eyes_light.color.set(255, 255, 255, 255);
	eyes_light.sprite_no=49;
	eyes_light.scale_x=1.0f;
	eyes_light.scale_y=1.0f;
	eyes_light.custom_texture=NULL;
	eyes_light.intensity=255;
	eyes_light.plane=static_cast<int>(LightPlaneId::Player);
	eyes_light.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);
}

void HangingSpider::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (state == 0) {
		double dist=ppl7::grafix::Distance(p, player.position());
		if (dist < 400 || next_state < time) {
			state=1;
			velocity=0.5f * frame_rate_compensation;
		}
	} else if (state == 1) {
		if (velocity < 8.0) velocity+=0.2f * frame_rate_compensation;
		p.y+=velocity * frame_rate_compensation;
		updateBoundary();
		TileType::Type t=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 70));
		if (t != TileType::NonBlocking) {
			state=2;
			next_state=time + (double)ppl7::rand(2, 10);
		}
	} else if (state == 2) {
		if (velocity > 0.5) {
			velocity-=0.4f * frame_rate_compensation;
			p.y+=velocity * frame_rate_compensation;
			updateBoundary();
		} else {
			state=3;
			next_state=time + (double)ppl7::rand(2, 10);
		}
	} else if (state == 3 && next_state < time) {
		state=4;

	} else if (state == 4) {
		if (p.y > initial_p.y) {
			p.y-=2 * frame_rate_compensation;
			updateBoundary();
		} else {
			state=0;
			next_state=time + (double)ppl7::rand(5, 20);
		}
	}
	eyes_light.custom_texture=this->texture;
	eyes_light.x=p.x;
	eyes_light.y=p.y;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&eyes_light);

}

void HangingSpider::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	SDL_SetRenderDrawColor(renderer, 204, 204, 204, 255);
	SDL_RenderDrawLine(renderer, initial_p.x + coords.x, initial_p.y + coords.y - 20,
		p.x + coords.x, p.y + coords.y - 20);
	SDL_RenderDrawLine(renderer, initial_p.x + coords.x + 1, initial_p.y + coords.y - 20,
		p.x + coords.x + 1, p.y + coords.y - 20);

	Object::draw(renderer, coords);
}

void HangingSpider::handleCollision(Player* player, const Collision&)
{
	player->dropHealth(2);
}



}	// EOF namespace Decker::Objects
