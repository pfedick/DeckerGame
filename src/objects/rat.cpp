#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Rat::representation()
{
	return Representation(Spriteset::GenericObjects, 38);
}

Rat::Rat()
	:Enemy(Type::ObjectType::Rat)
{
	sprite_no=38;
	sprite_no_representation=38;
	state=0;
	collisionDetection=true;
	audio=NULL;

}

Rat::~Rat()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void Rat::handleCollision(Player* player, const Collision& collision)
{
	Player::PlayerMovement movement=player->getMovement();
	if (collision.onFoot() == true && movement == Player::Falling) {
		state=0;
		collisionDetection=false;
		enabled=false;
		player->addPoints(50);
		if (audio) {
			getAudioPool().stopInstace(audio);
			delete audio;
			audio=NULL;
			state=3;

		}
	} else {
		player->dropHealth(2);
	}
}

void Rat::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (state == 3) return;
	if (state == 0) {
		p.x-=2 * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=1;
			sprite_no=39;
		}
		updateBoundary();
	} else if (state == 1) {
		p.x+=2 * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=0;
			sprite_no=38;
		}
		updateBoundary();
	}
	if (!audio) {
		AudioPool& pool=getAudioPool();
		audio=pool.getInstance(AudioClip::rat_squeek);
		if (audio) {
			audio->setVolume(0.4f);
			audio->setAutoDelete(false);
			audio->setLoop(true);
			audio->setPositional(p, 960);
			pool.playInstance(audio);
		}
	} else {
		audio->setPositional(p, 960);
	}
}




}	// EOF namespace Decker::Objects
