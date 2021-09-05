#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Scorpion::representation()
{
	return Representation(Spriteset::Scorpion, 0);
}

Scorpion::Scorpion()
:Enemy(Type::ObjectType::Scorpion)
{
	sprite_set=Spriteset::Scorpion;
	sprite_no=0;
	sprite_no_representation=0;
	state=0;
	collisionDetection=true;
	animation.startSequence(0, 9, true, 0);
	next_animation=0.0f;
}

void Scorpion::handleCollision(Player *player, const Collision &collision)
{
	player->dropHealth(10);
}

void Scorpion::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (time>next_animation) {
		next_animation=time+0.07f;
		animation.update();
		sprite_no=animation.getFrame();
		updateBoundary();
	}
	if (state==0) {
		p.x-=2;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x-60, p.y-12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x-60, p.y+6));
		if (t1!=TileType::NonBlocking || t2!=TileType::Blocking) {
			state=1;
			sprite_no=0;
		}
		updateBoundary();
	} else if (state==1) {
		p.x+=2;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x+60, p.y-12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x+60, p.y+6));
		if (t1!=TileType::NonBlocking || t2!=TileType::Blocking) {
			state=0;
			sprite_no=0;
		}
		updateBoundary();
	}
}




}	// EOF namespace Decker::Objects
