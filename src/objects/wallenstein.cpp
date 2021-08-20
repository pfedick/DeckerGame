#include <ppl7.h>
#include <ppl7-grafix.h>
#include "wallenstein.h"
#include "audiopool.h"

namespace Decker::Objects {

static int walk_cycle_left[]={1,2,3,4,5,6,7,8};
static int walk_cycle_right[]={10,11,12,13,14,15,16,17};
static int turn_from_left_to_mid[]={22,23,24,27};
static int turn_from_left_to_right[]={22,23,24,25,26};
static int turn_from_right_to_mid[]={18,19,20};
static int turn_from_right_to_left[]={18,19,20,21,22};
static int turn_from_mid_to_left[]={27,20,21,22};
static int turn_from_mid_to_right[]={27,24,25,26};
//static int turn_from_back_to_front[]={30,31,32,23,24};
static int run_cycle_left[]={61,62,63,64,65,66,67,68};
static int run_cycle_right[]={70,71,72,73,74,75,76,77};
static int climb_up_cycle[]={91,92,93,94,95,96,97,98,99,100,101};
static int climb_down_cycle[]={101,100,99,98,97,96,95,94,93,92,91};

static int slide_left[]={83,84,85,86};
static int slide_right[]={79,80,81,82};

static int death_animation[]={102,103,105,105,105,106,106,105,105,106,106,
		105,105,106,106,105,104,105,106,105,104,103,104,105,106};
static int death_by_falling[]={89,89,106,106,89,89,106,106,89,106,89,106,89,89,
		106,106,89};


Representation Wallenstein::representation()
{
	return Representation(Spriteset::Wallenstein, 27);
}

Wallenstein::Wallenstein()
:Enemy(Type::ObjectType::Wallenstein)
{
	sprite_set=Spriteset::Wallenstein;
	sprite_no=27;
}

void Wallenstein::handleCollision(Player *player, const Collision &collision)
{

}

void Wallenstein::update(double time, TileTypePlane &ttplane, Player &player)
{

}


}
