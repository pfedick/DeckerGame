#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Crate::representation()
{
	return Representation(Spriteset::Crates, 4);
}

Crate::Crate()
	: Decker::Objects::Object(Type::Crate)
{
	sprite_set=Spriteset::Crates;
	sprite_no=4;
	sprite_no_representation=4;
	collisionDetection=true;
	color_mod.setColor(ppl7::rand(220, 250),
		ppl7::rand(220, 250),
		ppl7::rand(220, 250),
		255);
	updateBoundary();
}

size_t Crate::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	ppl7::Poke8(buffer + bytes + 1, sprite_no);
	int flags=0;
	ppl7::Poke8(buffer + bytes + 2, flags);
	ppl7::Poke32(buffer + bytes + 3, color_mod.color());
	return bytes + 7;
}

size_t Crate::saveSize() const
{
	return Object::saveSize() + 7;
}

size_t Crate::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	sprite_no=ppl7::Peek8(buffer + bytes + 1);
	int flags=ppl7::Peek8(buffer + bytes + 2);
	color_mod.setColor(ppl7::Peek32(buffer + bytes + 3));
	updateBoundary();
	return size;
}



void Crate::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	p+=velocity;
	int blockcount=0;
	for (int x=0;x < boundary.width();x+=16) {
		if (ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1)) == TileType::Blocking) blockcount++;
	}
	if (blockcount == 0) {
		velocity.y+=0.2 * frame_rate_compensation;
		if (velocity.y > 16.0f)velocity.y=16.0f;
	} else {
		velocity.y=0.0f;
		while (true) {
			blockcount=0;
			for (int x=0;x < boundary.width();x+=16) {
				if (ttplane.getType(ppl7::grafix::Point(p.x, p.y)) == TileType::Blocking) blockcount++;
			}
			if (blockcount == 0) break;
			p.y--;
		}
	}
}


void Crate::handleCollision(Player* player, const Collision& collision)
{

}


void Crate::openUi()
{

}


}	// EOF namespace Decker::Objects
