#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

Representation PowerCell::representation()
{
	return Representation(Spriteset::GenericObjects, 468);
}

PowerCell::PowerCell()
	: Object(Type::ObjectType::PowerCell)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandomSequence(468, 496, true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=468;
}

void PowerCell::update(double time, TileTypePlane&, Player&, float)
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

void PowerCell::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->countObject(type());
	player->addSpecialObject(type());
	//AudioPool& audio=getAudioPool();
	//audio.playOnce(AudioClip::decker, 0.5f);
}


}	// EOF namespace Decker::Objects
