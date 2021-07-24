#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
namespace Decker::Objects {

static int walk_cycle_left[]={0,1,2,3,4,5,6,7,8};
static int walk_cycle_right[]={9,10,11,12,13,14,15,16,17};
static int turn_from_left_to_right[]={22,23,24,25,26};
static int turn_from_right_to_left[]={18,19,20,21,22};
static int turn_from_mid_to_left[]={27,20,21,22};

Representation Skeleton::representation()
{
	return Representation(Spriteset::Skeleton, 27);
}

Skeleton::Skeleton()
:Enemy(Type::ObjectType::Skeleton)
{
	sprite_set=Spriteset::Skeleton;
	sprite_no=27;
	animation.setStaticFrame(27);
	sprite_no_representation=27;
	next_state=ppl7::GetMicrotime()+(double)ppl7::rand(5,20);
	state=0;
	next_animation=0.0f;
	collisionDetection=true;
	animation.start(turn_from_mid_to_left,sizeof(turn_from_mid_to_left)/sizeof(int),false,0);
}

void Skeleton::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (time>next_animation) {
		next_animation=time+0.07f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite!=sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}

	if (state==0 && animation.isFinished()) {
		state=1;
		animation.start(walk_cycle_left,sizeof(walk_cycle_left)/sizeof(int),true,0);
	} else if (state==1) {	// walk left
		p.x-=2;
		updateBoundary();
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x-20, p.y-6));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x-20, p.y+6));
		if (t1!=TileType::NonBlocking || t2!=TileType::Blocking) {
			state=2;
			animation.setStaticFrame(0);
			next_state=time+(double)ppl7::rand(1,5);
		}
	} else if (state==2 && time>next_state) {
		animation.start(turn_from_left_to_right,sizeof(turn_from_left_to_right)/sizeof(int),false,9);
		state=3;
	} else if (state==3 && animation.isFinished()) {
		state=4;
		animation.start(walk_cycle_right,sizeof(walk_cycle_right)/sizeof(int),true,9);

	} else if (state==4) {
		p.x+=2;
		updateBoundary();
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x+20, p.y-6));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x+20, p.y+6));
		if (t1!=TileType::NonBlocking || t2!=TileType::Blocking) {
			state=5;
			animation.setStaticFrame(9);
			next_state=time+(double)ppl7::rand(1,5);
		}
	} else if (state==5 && time>next_state) {
		animation.start(turn_from_right_to_left,sizeof(turn_from_right_to_left)/sizeof(int),false,9);
		state=0;
	} else if (state==6) {
		velocity++;
		p.x-=velocity;
		p.y-=velocity;
		if (p.x<0 || p.y<0) {
			enabled=false;
			state=0;
		}
	}
}

void Skeleton::handleCollision(Player *player, const Collision &collision)
{
	if (collision.onFoot()) {
		state=6;
		velocity=1;
		collisionDetection=false;
		//enabled=false;
		player->addPoints(100);

	} else {
		player->dropHealth(4);
	}
}

}	// EOF namespace Decker::Objects
