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
	enabled=true;
	visibleAtPlaytime=true;
	collisionDetection=true;
	pixelExactCollision=false;
	audio=NULL;
	initial_state=false;
	current_state=false;
	next_state=0.0f;
	velocity=0.0f;

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
	/*ppl7::PrintDebug("GreatElevator::update: current_state=%d, enabled=%d, state=%d, velocity=%0.3f\n", (int)current_state,
		(int)enabled,
		(int)state, velocity);
		*/
	boundary.setCoords(p.x - 3 * TILE_WIDTH, p.y, p.x + 3 * TILE_WIDTH, p.y + 2 * TILE_HEIGHT);
	if (!current_state) {
		if (state == State::GoingUp || state == State::BreakUp || state == State::AtTop) {
			state=State::GoingDown;
			//ppl7::PrintDebug("Going down\n");
		}
	}
	if (state == State::Wait && next_state == 0.0f) next_state=time + 2.0f;
	else if (state == State::Wait && next_state < time && current_state == true) {
		state=State::GoingUp;
		velocity=0.0f;
		AudioPool& pool=getAudioPool();
		pool.playOnce(AudioClip::elevator_start, 1.0f);
		if (audio) {
			pool.stopInstace(audio);
			delete audio;
			audio=NULL;
		}
		audio=pool.getInstance(AudioClip::elevator_noiseloop);
		audio->setVolume(1.0f);
		audio->setPositional(p, 1800);
		audio->setLoop(true);
		pool.playInstance(audio);
		GetGame().getSoundtrack().playSong("res/audio/Arnaud_Conde_-_Coming_Soon__Intro (CC BY-SA 3.0).mp3");

	} else if (state == State::GoingUp) {
		if (velocity > -6.0f) velocity-=0.2f * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y - 2 * TILE_HEIGHT));
		if (t1 != TileType::NonBlocking) {
			state=State::BreakUp;
			getAudioPool().playOnce(AudioClip::elevator_exit, 1.0f);
		}
	} else if (state == State::BreakUp) {
		if (velocity < -0.2f) velocity+=0.2f * frame_rate_compensation;
		if (velocity > -0.2f) velocity=-0.2f;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1 + TILE_HEIGHT));
		if (t1 != TileType::NonBlocking) {
			AudioPool& pool=getAudioPool();
			if (audio) {
				pool.stopInstace(audio);
				delete audio;
				audio=NULL;
			}
			state=State::AtTop;
			velocity=0.0f;
		}
	} else if (state == State::GoingDown) {
		if (velocity < 6.0f) velocity+=0.2f * frame_rate_compensation;
		if (p.y + 2 * TILE_HEIGHT >= initial_p.y) state=State::BreakDown;
	} else if (state == State::BreakDown) {
		if (velocity > 0.2f) velocity-=0.2f * frame_rate_compensation;
		if (velocity < 0.2f) velocity=0.2f;
		if (p.y >= initial_p.y) {
			state=State::Wait;
			next_state=2.0f;
			velocity=0.0f;
		}

	}
	p.y+=velocity * frame_rate_compensation;


	if (audio) audio->setPositional(p, 1800);

	if (current_state || state != State::Wait) {
		LightSystem& lights=GetGame().getLightSystem();
		light.x=p.x;
		light.y=p.y - 3 * TILE_HEIGHT;
		lights.addObjectLight(&light);
	}
}


void GreatElevator::handleCollision(Player* player, const Collision& collision)
{
	if (!current_state) return;
	if (collision.onFoot()) {
		//printf ("collision with player\n");
		player->setStandingOnObject(this);
		//player->y+=(velocity * collision.frame_rate_compensation);
		if (player->y > p.y) player->y=p.y;
		if (player->x < p.x - 3 * TILE_WIDTH) player->x=p.x - 3 * TILE_WIDTH;
		if (player->x > p.x + 3 * TILE_WIDTH) player->x=p.x + 3 * TILE_WIDTH;
	}
}

void GreatElevator::toggle(bool enable, Object* source)
{
	this->current_state=enable;
	if (current_state) next_state=0.0f;
	//ppl7::PrintDebug("GreatElevator::toggle, current_state=%d\n", (int)current_state);
}

void GreatElevator::trigger(Object* source)
{
	current_state=!current_state;
	if (current_state) next_state=0.0f;
	//ppl7::PrintDebug("GreatElevator::trigger, current_state=%d\n", (int)current_state);
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
	current_state=initial_state;
	enabled=true;
	return size;
}

}	// EOF namespace Decker::Objects
