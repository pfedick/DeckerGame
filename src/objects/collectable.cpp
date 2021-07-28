#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "audiopool.h"

namespace Decker::Objects {

Collectable::Collectable(Type::ObjectType type)
: Object(type)
{
	points=0;
	sample_id=0;
}

void Collectable::handleCollision(Player *player, const Collision &)
{
	enabled=false;
	player->addPoints(points);
	if (sample_id>0) {
		AudioPool &audio=getAudioPool();
		audio.playOnce((AudioClip::Id)sample_id);
	}
}

}	// EOF namespace Decker::Objects
