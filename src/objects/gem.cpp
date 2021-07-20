#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"

namespace Decker::Objects {

static int diamond_rotate[]={4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25,26,27,28,29,30,31,32,33};
static int crystal_rotate[]={120,121,122,123,124,125,126,127,128,129,130,
	131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,
	147,148,149};

Representation GemReward::representation()
{
	return Representation(Spriteset::GenericObjects, 4);
}

GemReward::GemReward()
: Collectable(Type::ObjectType::Diamond)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(diamond_rotate,sizeof(diamond_rotate)/sizeof(int),true,0);
	next_animation=0.0f;
	collsionDetection=true;
	sprite_no_representation=4;
	points=50;
}

void GemReward::update(double time)
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

Representation CrystalReward::representation()
{
	return Representation(Spriteset::GenericObjects, 0);
}

CrystalReward::CrystalReward()
: Collectable(Type::ObjectType::Crystal)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(crystal_rotate,sizeof(crystal_rotate)/sizeof(int),true,0);
	next_animation=0.0f;
	collsionDetection=true;
	points=100;
}

void CrystalReward::update(double time)
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
