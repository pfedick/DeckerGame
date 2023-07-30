#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Bat::representation()
{
	return Representation(Spriteset::Bat, 0);
}

Bat::Bat()
	:Enemy(Type::ObjectType::Bat)
{
	sprite_set=Spriteset::Bat;
	sprite_no=0;
	sprite_no_representation=0;
	state=0;
	collisionDetection=true;
	animation.startSequence(0, 9, true, 0);
	next_animation=0.0f;
	velocity=-3;
	audio=NULL;
}

Bat::~Bat()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void Bat::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(10);
}

void Bat::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (time > next_animation) {
		next_animation=time + 0.03f;
		animation.update();
		sprite_no=animation.getFrame();
		updateBoundary();
	}
	p.y+=(velocity * frame_rate_compensation);
	if (state == 0) {
		if (velocity > -3.0f) {
			velocity-=(0.2f * frame_rate_compensation);
		}
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y - 48));
		if (t1 != TileType::NonBlocking) {
			state=1;
		}
		updateBoundary();
	} else if (state == 1) {
		if (velocity < 3.0f) {
			velocity+=(0.2f * frame_rate_compensation);
		}

		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 8));
		if (t1 != TileType::NonBlocking) {
			state=0;
		}
		updateBoundary();
	}
	if (!audio) {
		AudioPool& audiopool=getAudioPool();
		audio=audiopool.getInstance(AudioClip::bat_wings);
		if (audio) {
			audio->setVolume(0.6f);
			audio->setAutoDelete(false);
			audio->setLoop(true);
			audio->setPositional(p, 1200);
			audiopool.playInstance(audio);
		}
	} else if (audio) {
		audio->setPositional(p, 1600);
	}
}




}	// EOF namespace Decker::Objects
