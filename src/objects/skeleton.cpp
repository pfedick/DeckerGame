#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
namespace Decker::Objects {


Representation Skeleton::representation()
{
	return Representation(Spriteset::Skeleton, 27);
}

Skeleton::Skeleton()
:Enemy(Type::ObjectType::Skeleton)
{
	sprite_set=Spriteset::Skeleton;
	sprite_no=27;
	animation.setStaticFrame(27);
	sprite_no_representation=27;
	next_state=ppl7::GetMicrotime()+(double)ppl7::rand(5,20);
	state=0;
	collisionDetection=true;
}

void Skeleton::update(double time, TileTypePlane &ttplane, Player &player)
{

}

void Skeleton::handleCollision(Player *player)
{
	player->dropHealth(4);
}

}	// EOF namespace Decker::Objects
