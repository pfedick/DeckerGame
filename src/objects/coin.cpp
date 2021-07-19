#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

static int coin_rotate[]={84,85,86,87,88,89,90,91,92,
	93,94,95,96,97,98,99,100,101,102,103,104,105,106,
	107,108,109,110,111,112,113};

Representation CoinReward::representation()
{
	return Representation(Spriteset::GenericObjects, 84);
}

CoinReward::CoinReward()
: Collectable(Type::ObjectType::Coin)
{
	sprite_set=Spriteset::GenericObjects;
	animation.start(coin_rotate,sizeof(coin_rotate)/sizeof(int),true,0);
	next_animation=0.0f;
	collsionDetection=true;
}

void CoinReward::update(double time)
{
	if (time>next_animation) {
		next_animation=time+0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite!=sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
}

}	// EOF namespace Decker::Objects
