#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

static int apple_rotate[]={ 335, 336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,
351,352,353,354,355,356,357,358,359,360,361,362,363,364 };

Representation CoinReward::representation()
{
	return Representation(Spriteset::GenericObjects, 335);
}

AppleReward::AppleReward()
	: Object(Type::ObjectType::Apple)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(apple_rotate, sizeof(apple_rotate) / sizeof(int), true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=335;
}

void AppleReward::update(double time, TileTypePlane&, Player&)
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

void AppleReward::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->addHealth(10);
	//AudioPool& audio=getAudioPool();
	//audio.playOnce(AudioClip::coin1, 0.3f);
}


}	// EOF namespace Decker::Objects
