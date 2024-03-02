#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Skull::representation()
{
	return Representation(Spriteset::GenericObjects, 468);
}

Skull::Skull()
	:Enemy(Type::ObjectType::Skull)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=468;
	sprite_no_representation=468;
	state=0;
	speed=0.0f;
	collisionDetection=true;
	updateBoundary();
	light.color.set(255, 255, 255, 255);
	light.sprite_no=469;
	light.scale_x=1.0f;
	light.scale_y=1.0f;
	light.custom_texture=NULL;
	light.intensity=255;
	light.plane=static_cast<int>(LightPlaneId::Player);
	light.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

	shine.color.set(0, 255, 0, 255);
	shine.sprite_no=0;
	shine.scale_x=0.15f;
	shine.scale_y=0.15f;
	shine.intensity=128;
	shine.plane=static_cast<int>(LightPlaneId::Player);
	shine.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

}




void Skull::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (state == 0) {
		speed+=0.05f * frame_rate_compensation;
		p.y-=speed;
		if (p.y < initial_p.y - 5.0f) state=1;
	} else if (state == 1) {
		speed-=0.1f * frame_rate_compensation;
		if (speed < 0) {
			state=3;
			speed=0.0f;
		} else p.y-=speed;
	} else if (state == 3) {
		speed+=0.05f * frame_rate_compensation;
		p.y+=speed;
		if (p.y >= initial_p.y) state=4;
	} else if (state == 4) {
		speed-=0.05f * frame_rate_compensation;
		if (speed < 0) {
			state=0;
			speed=0.0f;
		} else p.y+=speed;
	} else if (state == 5) {
		speed+=0.1f * frame_rate_compensation;
		p.y+=speed;
		if (p.y > initial_p.y + 1080) {
			state=6;
			enabled=false;
		}

	}
	if (player.isFlashlightOn() && state < 5) {
		float y_dist=abs(player.y - p.y - 50);
		float x_dist=abs(player.x - p.x);
		if (y_dist < TILE_HEIGHT && x_dist < 4 * TILE_WIDTH) {
			if ((player.orientation == Player::PlayerOrientation::Left && p.x < player.x) ||
				(player.orientation == Player::PlayerOrientation::Right && p.x > player.x)) {
					// In light cone
				state=5;
				sprite_no=470;
				light.sprite_no=471;
				collisionDetection=false;
				speed=0.1f;
				player.addPoints(20);
			}
		}
	}
	light.custom_texture=this->texture;
	light.x=p.x;
	light.y=p.y;
	shine.x=p.x;
	shine.y=p.y - 22;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&light);
	lights.addObjectLight(&shine);
}

void Skull::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(0.1f);
}



}	// EOF namespace Decker::Objects
