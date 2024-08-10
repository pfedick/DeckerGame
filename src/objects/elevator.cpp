#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "light.h"

namespace Decker::Objects {


Representation GreatElevator::representation()
{
	return Representation(Spriteset::GreatElevator, 0);
}



GreatElevator::GreatElevator()
	: Object(Objects::Type::GreatElevator)
{
	sprite_no_representation=0;
	sprite_set=Spriteset::GreatElevator;
	sprite_no=0;
	state=State::Wait;
	collisionDetection=true;
	pixelExactCollision=false;
	audio=NULL;
	initial_state=false;
	enabled=true;

	light.color.set(255, 255, 255, 255);
	light.sprite_no=0;
	light.scale_x=0.8f;
	light.scale_y=0.8f;
	light.intensity=255;
	light.plane=static_cast<int>(LightPlaneId::Player);
	light.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);
	light.has_lensflare=false;
}

GreatElevator::~GreatElevator()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void GreatElevator::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (!enabled) return;
	/*
	AudioPool& pool=getAudioPool();
	if (!audio) {
		audio=pool.getInstance(AudioClip::thruster);
		audio->setVolume(0.3f);
		audio->setPositional(p, 1800);
		audio->setLoop(true);
	}
	if (audio) audio->setPositional(p, 1800);
	*/

	//LightSystem& lights=GetGame().getLightSystem();
}


void GreatElevator::handleCollision(Player* player, const Collision& collision)
{
	if (!enabled) return;
	if (collision.onFoot()) {
		//printf ("collision with player\n");
		player->setStandingOnObject(this);
		player->y+=(velocity * collision.frame_rate_compensation);
		if (player->y > p.y - 58) player->y=p.y - 58;
	}
}

void GreatElevator::toggle(bool enable, Object* source)
{
	this->enabled=enable;
	//printf ("Floater::toggle %d: %d\n", id, (int)enable);
}

void GreatElevator::trigger(Object* source)
{
	enabled=!enabled;
}


size_t GreatElevator::saveSize() const
{
	return Object::saveSize() + 2;
}


size_t GreatElevator::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	int flags=0;
	if (initial_state) flags|=1;

	ppl7::Poke8(buffer + bytes + 1, flags);
	return bytes + 2;
}

size_t GreatElevator::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	int flags=ppl7::Peek8(buffer + bytes + 1);
	initial_state=(bool)flags & 1;
	enabled=true;
	return size;
}

}	// EOF namespace Decker::Objects
