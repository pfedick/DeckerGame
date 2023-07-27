#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Fish::representation()
{
	return Representation(Spriteset::GenericObjects, 399);
}

Fish::Fish()
	:Enemy(Type::ObjectType::Fish)
{
	sprite_no=399;
	sprite_no_representation=399;
	state=0;
	collisionDetection=true;
	scale=ppl7::randf(0.2f, 1.0f);
	speed=ppl7::randf(0.8f, 6.0f);
	color_mod.setColor(ppl7::rand(10, 80),
		ppl7::rand(10, 80),
		ppl7::rand(10, 80),
		255);
	next_state=ppl7::rand(0, 1);
	if (next_state == 0) animation.startSequence(397, 406, true, 397);
	else animation.startSequence(387, 396, true, 387);
	sprite_no=animation.getFrame();
	updateBoundary();
}

size_t Fish::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	ppl7::PokeFloat(buffer + bytes + 1, scale);
	ppl7::PokeFloat(buffer + bytes + 5, speed);
	ppl7::Poke32(buffer + bytes + 9, color_mod.color());
	return bytes + 13;
}

size_t Fish::saveSize() const
{
	return Object::saveSize() + 13;
}

size_t Fish::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	scale=ppl7::PeekFloat(buffer + bytes + 1);
	speed=ppl7::PeekFloat(buffer + bytes + 5);
	color_mod.setColor(ppl7::Peek32(buffer + bytes + 9));
	return size;
}



void Fish::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (time > next_animation) {
		next_animation=time + 0.047f;
		animation.update();
		sprite_no=animation.getFrame();
		updateBoundary();
	}
	if (state == 0) {
		p.x-=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y + 6));
		if (t1 != TileType::Water || t2 != TileType::Water || next_state < time) {
			state=1;
			sprite_no=294;
			next_state=time + ppl7::randf(1.0f, 10.0f);
			speed=ppl7::randf(0.8f, 6.0f);
			animation.startSequence(387, 396, true, 387);
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
			speed=ppl7::randf(0.8f, 6.0f);
			animation.startSequence(397, 406, true, 397);
			sprite_no=animation.getFrame();
		}
		updateBoundary();
	}
}




}	// EOF namespace Decker::Objects
