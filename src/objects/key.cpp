#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

Representation KeyReward::representation()
{
	return Representation(Spriteset::GenericObjects, 50);
}

KeyReward::KeyReward()
: Object(Type::ObjectType::Savepoint)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=50;
	collisionDetection=true;
	sprite_no_representation=50;
}

void KeyReward::handleCollision(Player *player, const Collision &)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->addInventory(id, Representation(Spriteset::GenericObjects, sprite_no));
	// TODO: audio sound
	//AudioPool &audio=getAudioPool();
	//audio.playOnce(AudioClip::crystal, 1.0f);
}


}	// EOF namespace Decker::Objects
