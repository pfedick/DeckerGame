#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"
#include "audiopool.h"
namespace Decker::Objects {




Representation TreasureChest::representation()
{
	return Representation(Spriteset::TreasureChest, 32);
}


TreasureChest::TreasureChest()
	:Object(Type::ObjectType::TreasureChest)
{
	sprite_set=Spriteset::TreasureChest;
	sprite_no=0;
	sprite_no_representation=0;
	next_animation=0;
	state=0;
	collisionDetection=true;
	alwaysUpdate=false;
}



void TreasureChest::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	if (state == 0) {
		double distance=ppl7::grafix::Distance(p, player.position());
		if (distance < 200) {
			state=1;
			animation.startSequence(0, 32, false, 32);
			getAudioPool().playOnce(AudioClip::treasure_chest, 0.3f);
		}
	}
	if (state > 0 && time > next_animation) {
		next_animation=time + 0.03f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
		if (state == 1 && animation.isFinished()) {
			state=2;
			animation.startSequence(33, 62, true, 33);
		}
	}
}

void TreasureChest::handleCollision(Player* player, const Collision& collision)
{
	player->addPoints(1000);
	player->countObject(type());
	collisionDetection=false;
	enabled=false;
	getAudioPool().playOnce(AudioClip::treasure_collect, 0.8f);
}


}	// EOF namespace Decker::Objects
