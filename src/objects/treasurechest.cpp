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
}



void TreasureChest::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (state==0) {
		double distance=ppl7::grafix::Distance(p,player.position());
		if (distance<200) {
			state=1;
			animation.startSequence(0,32,false,32);
		}
	}
	if (state==1 && time>next_animation) {
		next_animation=time+0.07f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite!=sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
		if (animation.isFinished()) state=2;
	}

}

void TreasureChest::handleCollision(Player *player, const Collision &collision)
{
	player->addPoints(1000);
	collisionDetection=false;
	enabled=false;
}


}	// EOF namespace Decker::Objects
