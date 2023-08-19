#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

static int coin_rotate[]={ 84,85,86,87,88,89,90,91,92,
	93,94,95,96,97,98,99,100,101,102,103,104,105,106,
	107,108,109,110,111,112,113 };

Representation CoinReward::representation()
{
	return Representation(Spriteset::GenericObjects, 84);
}

CoinReward::CoinReward()
	: Object(Type::ObjectType::Coin)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(coin_rotate, sizeof(coin_rotate) / sizeof(int), true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=84;
}

void CoinReward::update(double time, TileTypePlane&, Player&, float)
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

void CoinReward::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->addPoints(10);
	player->countObject(type());
	AudioPool& audio=getAudioPool();

	int sample=ppl7::rand(0, 1);
	switch (sample) {
	case 1:	audio.playOnce(AudioClip::coin3, 0.3f);break;
	default: audio.playOnce(AudioClip::coin1, 0.3f);break;
	}
}


}	// EOF namespace Decker::Objects
