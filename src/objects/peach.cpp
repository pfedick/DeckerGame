#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

Representation PeachReward::representation()
{
	return Representation(Spriteset::GenericObjects, 498);
}

PeachReward::PeachReward()
	: Object(Type::ObjectType::Peach)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandomSequence(498, 527, true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=498;
}

void PeachReward::update(double time, TileTypePlane&, Player&, float)
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

void PeachReward::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->addHealth(10);
	player->addPoints(10);
	player->countObject(type());
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::peach, 1.0f);
}


}	// EOF namespace Decker::Objects
