#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Skull::representation()
{
	return Representation(Spriteset::GenericObjects, 297);
}

Skull::Skull()
	:Enemy(Type::ObjectType::Skull)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=297;
	sprite_no_representation=297;
	state=0;
	collisionDetection=true;
	updateBoundary();
	light.color.set(255, 255, 255, 255);
	light.sprite_no=3;
	light.scale_x=0.1f;
	light.scale_y=0.1f;
	light.intensity=255;
	light.plane=static_cast<int>(LightPlaneId::Player);
	light.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

}




void Skull::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	light.x=p.x;
	light.y=p.y - 28;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&light);
}

void Skull::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(1);
}



}	// EOF namespace Decker::Objects
