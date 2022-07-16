#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

static int cherry_rotate[]={ 304, 305, 306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,
321,322,323,324,325,326,327,328,329,330,331,332,333 };

Representation CoinReward::representation()
{
	return Representation(Spriteset::GenericObjects, 304);
}

CherryReward::CherryReward()
	: Object(Type::ObjectType::Apple)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(cherry_rotate, sizeof(cherry_rotate) / sizeof(int), true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	sprite_no_representation=304;
}

void CherryReward::update(double time, TileTypePlane&, Player&)
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

void CherryReward::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->addHealth(1);
	//AudioPool& audio=getAudioPool();
	//audio.playOnce(AudioClip::coin1, 0.3f);
}


}	// EOF namespace Decker::Objects
