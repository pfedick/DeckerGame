#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation Hammer::representation()
{
	return Representation(Spriteset::GenericObjects, 407);
}

Hammer::Hammer()
	: Object(Type::ObjectType::Hammer)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandomSequence(407, 436, true, 407);
	next_animation=0.0f;
	collisionDetection=true;
	alwaysUpdate=false;
	sprite_no_representation=407;
}

void Hammer::update(double time, TileTypePlane&, Player&, float)
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

void Hammer::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->countObject(type());
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::hammer, 0.4f);
	player->addSpecialObject(static_cast<int>(Type::ObjectType::Hammer));
	if (!player->hasSpoken(3)) player->speak(3);
}


}	// EOF namespace Decker::Objects
