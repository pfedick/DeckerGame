#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation Flashlight::representation()
{
	return Representation(Spriteset::GenericObjects, 437);
}

Flashlight::Flashlight()
	: Object(Type::ObjectType::Flashlight)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandomSequence(437, 466, true, 437);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=437;
	alwaysUpdate=false;
}

void Flashlight::update(double time, TileTypePlane&, Player&, float)
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

void Flashlight::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->countObject(type());
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::flashlight_pickup, 0.7f);
	player->addSpecialObject(static_cast<int>(Type::ObjectType::Flashlight));
	//if (!player->hasSpoken(3)) player->speak(3);
}


}	// EOF namespace Decker::Objects
