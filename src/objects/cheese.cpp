#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation Cheese::representation()
{
	return Representation(Spriteset::Rat, 78);
}

Cheese::Cheese()
	: Object(Type::ObjectType::Cheese)
{
	sprite_set=Spriteset::Rat;
	animation.startRandomSequence(63, 93, true, 63);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=78;
	alwaysUpdate=false;
}

void Cheese::update(double time, TileTypePlane&, Player&, float)
{
	if (time > next_animation) {
		next_animation=time + 0.046f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
}

void Cheese::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->countObject(type());
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::cheese, 0.4f);
	player->addSpecialObject(static_cast<int>(Type::ObjectType::Cheese));
	if (!player->hasSpoken(2)) player->speak(2);
}


}	// EOF namespace Decker::Objects
