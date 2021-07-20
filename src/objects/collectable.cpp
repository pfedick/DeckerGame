#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"

namespace Decker::Objects {

Collectable::Collectable(Type::ObjectType type)
: Object(type)
{
	points=0;
}

void Collectable::handleCollision(Player *player)
{
	enabled=false;
	player->addPoints(points);
}

}	// EOF namespace Decker::Objects
