#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"

namespace Decker::Objects {


static int savepoint_animation[]={ 52,53,54,55,56,57,58,59,60,61,62,63,64,
	65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81 };


Representation SavePoint::representation()
{
	return Representation(Spriteset::GenericObjects, 52);
}


SavePoint::SavePoint()
	: Object(Type::ObjectType::Savepoint)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(savepoint_animation, sizeof(savepoint_animation) / sizeof(int), true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=52;
}

void SavePoint::update(double time, TileTypePlane&, Player&, float)
{
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
}

void SavePoint::handleCollision(Player* player, const Collision&)
{
	player->setSavePoint(p);
	enabled=false;
	collisionDetection=false;
}


}	// EOF namespace Decker::Objects
