#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Piranha::representation()
{
	return Representation(Spriteset::Piranha, 0);
}

Piranha::Piranha()
	:Enemy(Type::ObjectType::Piranha)
{
	sprite_set=Spriteset::Piranha;
	sprite_no=0;
	sprite_no_representation=0;
	state=0;
	collisionDetection=true;
	//scale=ppl7::randf(0.2f, 1.0f);
	speed=ppl7::randf(2.0f, 4.0f);
	next_state=ppl7::rand(0, 1);
	if (next_state == 0) animation.startSequence(0, 19, true, 0);
	else animation.startSequence(20, 39, true, 20);
	sprite_no=animation.getFrame();
	updateBoundary();
}




void Piranha::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (time > next_animation) {
		next_animation=time + 0.047f;
		animation.update();
		sprite_no=animation.getFrame();
		updateBoundary();
		if (sprite_no == 13 || sprite_no == 33) {
			AudioPool& audio=getAudioPool();
			audio.playOnce(AudioClip::crunch, p, 1600, 0.5f);
		}
		//ppl7::PrintDebugTime("Piranha::update %d\n", sprite_no);
	}
	if (state == 0) {
		p.x-=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y + 6));
		if (t1 != TileType::Water || t2 != TileType::Water || next_state < time) {
			state=1;
			next_state=time + ppl7::randf(1.0f, 10.0f);
			speed=ppl7::randf(2.0f, 4.0f);
			animation.startSequence(20, 39, true, 20);
			sprite_no=animation.getFrame();

		}
		updateBoundary();
	} else {
		p.x+=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y + 6));
		if (t1 != TileType::Water || t2 != TileType::Water || next_state < time) {
			state=0;
			next_state=time + ppl7::randf(1.0f, 10.0f);
			speed=ppl7::randf(2.0f, 4.0f);
			animation.startSequence(0, 19, true, 0);
			sprite_no=animation.getFrame();
		}
		updateBoundary();
	}
}

void Piranha::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(10);
}



}	// EOF namespace Decker::Objects
