#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

Representation MagicGround::representation()
{
	return Representation(Spriteset::MagicGround, 27);
}

MagicGround::MagicGround()
	: Object(Type::ObjectType::MagicGround)
{
	sprite_set=Spriteset::MagicGround;
	sprite_no=0;
	sprite_no_representation=27;
	collisionDetection=true;
	pixelExactCollision=false;
	initial_state=State::active;
	current_state=initial_state;
	color=2;
	hasDebris=true;
	hasStuds=true;
	verticalMovement=true;
	roundEdges=true;
	canDissolve=false;
	movement_speed=2.0f;
	float_offset=0.0f;
	movement_range=16;
	floatstate=FloatState::GoingUp;
	current_speed=0;
	type=0;
	width=1;
	transparency=0.0f;
	max_debris_length=8;
	min_time_visible=2.0f;
	max_time_visible=4.0f;
	min_time_invisible=2.0f;
	max_time_invisible=4.0f;
	next_transparency_change=0.0f;
	randomizeFloatState();
}

void MagicGround::updateBoundary()
{
	boundary.x1=p.x - 2 * TILE_WIDTH;
	boundary.y1=p.y;
	boundary.x2=boundary.x1 + width * 4 * TILE_WIDTH;
	boundary.y2=boundary.y1 + TILE_HEIGHT;
}

void MagicGround::randomizeFloatState()
{
	// TODO
	float_offset=ppl7::randf(0, (float)movement_range);
	if (ppl7::rand(0, 1) == 1) floatstate=FloatState::GoingUp;
	else floatstate=FloatState::GoingDown;
	current_speed=ppl7::randf(0, movement_speed);

	if (canDissolve) {
		if (ppl7::rand(0, 1) == 1) {
			transparency=0.0f;
			current_state = State::active;
		} else {
			transparency=1.0f;
			current_state = State::inactive;
		}
	}
}

void MagicGround::updateVerticalMovement(double time, float frame_rate_compensation)
{
	if (floatstate == FloatState::GoingUp) {
		if (current_speed < movement_speed) current_speed+=0.01f * frame_rate_compensation;
		if (current_speed > movement_speed) current_speed=movement_speed;
		float_offset+=current_speed * frame_rate_compensation;
		if (float_offset > (float)movement_range) floatstate=FloatState::BreakGoingUp;

	} else if (floatstate == FloatState::BreakGoingUp) {
		if (current_speed > 0.0f) current_speed-=0.02f * frame_rate_compensation;
		if (current_speed <= 0.0f) {
			current_speed=0.0f;
			floatstate=FloatState::GoingDown;
		}
		float_offset+=current_speed * frame_rate_compensation;
	} else if (floatstate == FloatState::GoingDown) {
		if (current_speed < movement_speed) current_speed+=0.01f * frame_rate_compensation;
		if (current_speed > movement_speed) current_speed=movement_speed;
		float_offset-=current_speed * frame_rate_compensation;
		if (float_offset < 0.0f) floatstate=FloatState::BreakGoingDown;
	} else if (floatstate == FloatState::BreakGoingDown) {
		if (current_speed > 0.0f) current_speed-=0.02f * frame_rate_compensation;
		if (current_speed <= 0.0f) {
			current_speed=0.0f;
			floatstate=FloatState::GoingUp;
		}
		float_offset-=current_speed * frame_rate_compensation;
	}
	p.y=initial_p.y - float_offset;
}

void MagicGround::updateTransparency(double time, float frame_rate_compensation)
{
	if (canDissolve) {
		if (next_transparency_change == 0.0f) {
			if (current_state == State::active || current_state == State::appears) next_transparency_change=time + ppl7::randf(min_time_visible, max_time_visible);
			else if (current_state == State::inactive || current_state == State::disappears) next_transparency_change=time + ppl7::randf(min_time_invisible, max_time_invisible);
		}
		if (next_transparency_change < time && current_state == State::active) {
			current_state=State::disappears;
		} else if (current_state == State::disappears) {
			transparency+=0.01 * frame_rate_compensation;
			if (transparency >= 1.0f) {
				transparency=1.0f;
				current_state=State::inactive;
				next_transparency_change=time + ppl7::randf(min_time_invisible, max_time_invisible);
			}
		} else if (next_transparency_change < time && current_state == State::inactive) {
			current_state=State::appears;
		} else if (current_state == State::appears) {
			transparency-=0.01 * frame_rate_compensation;
			if (transparency <= 0.0f) {
				transparency=0.0f;
				current_state=State::active;
				next_transparency_change=time + ppl7::randf(min_time_visible, max_time_visible);
			}
		}
	}
}

void MagicGround::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (verticalMovement) updateVerticalMovement(time, frame_rate_compensation);
	updateTransparency(time, frame_rate_compensation);
	updateBoundary();
}

void MagicGround::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	const ColorPalette& palette=GetColorPalette();
	ppl7::grafix::Color myColor=palette.getColor(color);
	myColor.setAlpha(255 - (transparency * 255.0f));

	if (hasStuds) {
		for (int i=0;i < width;i++)
			texture->draw(renderer,
				p.x + coords.x + i * 4 * TILE_WIDTH,
				p.y + coords.y, 21, myColor);
	}
	if (hasDebris) {
		// TODO
	}
	int base=4 * type;
	if (width == 1) {
		int g=base + 1;
		if (roundEdges) g=base + 3;
		texture->draw(renderer,
			p.x + coords.x,
			p.y + coords.y, g, myColor);
	} else if (width > 1) {
		int start=0;
		int end=width - 1;
		if (roundEdges) {
			start++;
			end--;
			texture->draw(renderer,
				p.x + coords.x,
				p.y + coords.y, base, myColor);
			texture->draw(renderer,
				p.x + coords.x + (width - 1) * 4 * TILE_WIDTH,
				p.y + coords.y, base + 2, myColor);
		}
		for (int i=start;i <= end;i++)
			texture->draw(renderer,
				p.x + coords.x + i * 4 * TILE_WIDTH,
				p.y + coords.y, base + 1, myColor);
	}
}

void MagicGround::handleCollision(Player* player, const Collision& collision)
{
	if (current_state != State::inactive && transparency < 0.5f && collision.onFoot()) {
		player->y=p.y;
		player->setStandingOnObject(this);
	}
}


void MagicGround::toggle(bool enable, Object* source)
{
	if (enable) current_state = State::appears;
	else current_state = State::disappears;
}

void MagicGround::trigger(Object* source)
{
	if (current_state == State::inactive || current_state == State::disappears) toggle(true, source);
	else toggle(false, source);
}


size_t MagicGround::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 2);		// Object Version

	int flags=0;
	if (initial_state == State::active) flags|=1;
	if (hasDebris) flags|=2;
	if (hasStuds) flags|=4;
	if (verticalMovement) flags|=8;
	if (roundEdges) flags|=16;
	if (canDissolve) flags|=32;
	ppl7::Poke8(buffer + bytes + 1, flags);
	ppl7::Poke8(buffer + bytes + 2, type);
	ppl7::Poke8(buffer + bytes + 3, width);
	ppl7::Poke8(buffer + bytes + 4, movement_range);
	ppl7::Poke8(buffer + bytes + 5, color);
	ppl7::Poke8(buffer + bytes + 6, max_debris_length);
	ppl7::PokeFloat(buffer + bytes + 7, movement_speed);
	ppl7::PokeFloat(buffer + bytes + 11, min_time_visible);
	ppl7::PokeFloat(buffer + bytes + 15, max_time_visible);
	ppl7::PokeFloat(buffer + bytes + 19, min_time_invisible);
	ppl7::PokeFloat(buffer + bytes + 23, max_time_invisible);
	return bytes + 27;
}

size_t MagicGround::saveSize() const
{
	return Object::saveSize() + 27;
}

size_t MagicGround::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version < 1 || version == 2) return 0;
	int flags=ppl7::Peek8(buffer + bytes + 1);
	if (flags & 1) {
		current_state=State::active;
		transparency=0.0f;
	} else {
		current_state=State::inactive;
		transparency=1.0f;
	}
	hasDebris=(bool)(flags & 2);
	hasStuds=(bool)(flags & 4);
	verticalMovement=(bool)(flags & 8);
	roundEdges=(bool)(flags & 16);
	canDissolve=(bool)(flags & 32);
	canDissolve=true;

	type=ppl7::Peek8(buffer + bytes + 2);
	width=ppl7::Peek8(buffer + bytes + 3);
	movement_range=ppl7::Peek8(buffer + bytes + 4);
	color=ppl7::Peek8(buffer + bytes + 5);
	max_debris_length=ppl7::Peek8(buffer + bytes + 6);
	movement_speed=ppl7::PeekFloat(buffer + bytes + 7);
	if (version >= 2) {
		min_time_visible=ppl7::PeekFloat(buffer + bytes + 11);
		max_time_visible=ppl7::PeekFloat(buffer + bytes + 15);
		min_time_invisible=ppl7::PeekFloat(buffer + bytes + 19);
		max_time_invisible=ppl7::PeekFloat(buffer + bytes + 23);
	}
	randomizeFloatState();
	return size;
}

void MagicGround::openUi()
{

}


}	// EOF namespace Decker::Objects
