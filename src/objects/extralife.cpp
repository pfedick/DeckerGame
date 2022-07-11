#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

static int extralife_rotate[]={ 241,242,243,244,245,246,247,248,249,250,251,252,253,
	254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270 };

Representation ExtraLife::representation()
{
	return Representation(Spriteset::GenericObjects, 241);
}

ExtraLife::ExtraLife()
	: Object(Type::ObjectType::ExtraLife)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(extralife_rotate, sizeof(extralife_rotate) / sizeof(int), true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=241;
}

void ExtraLife::update(double time, TileTypePlane&, Player&)
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

void ExtraLife::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->addPoints(100);
	player->addLife(1);
	//TODO
	//AudioPool& audio=getAudioPool();
	//audio.playOnce(AudioClip::coin1, 0.3f);
}


}	// EOF namespace Decker::Objects
