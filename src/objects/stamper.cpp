#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation StamperVertical::representation()
{
	return Representation(Spriteset::StamperVertical, 5);
}


StamperVertical::StamperVertical()
:Trap(Type::ObjectType::StamperVertical)
{
	sprite_set=Spriteset::StamperVertical;
	sprite_no=0;
	collisionDetection=true;
	visibleAtPlaytime=true;
	sprite_no_representation=5;
	pixelExactCollision=true;
	//save_size+=8;
	next_state=0.0f;
	state=0;
	if (ppl7::rand(0,1)==1) {
		state=2;
		sprite_no=5;
	}
	time_active=ppl7::randf(0.2f, 0.5f);
	time_inactive=ppl7::randf(0.2f, 0.5f);
	auto_intervall=true;
	next_animation=0.0f;
}

/*
void StamperVertical::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	int starty=p.y-7*TILE_HEIGHT;
	SDL_Rect r;
	r.x=coords.x+p.x-TILE_WIDTH*2;
	r.y=coords.y+starty;
	r.w=TILE_WIDTH*4;
	r.h=7*TILE_HEIGHT;
	texture->

	SDL_SetRenderDrawColor(renderer,255,0,0,255);
	SDL_RenderFillRect(renderer,&r);

}
*/


void StamperVertical::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (time>next_animation) {
		next_animation=time+0.07f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite!=sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}

	if (state==0 && time>next_state) {
		state=1;
		animation.startSequence(0, 5, false, 5);
	} else if (state==1 && animation.isFinished()==true) {
		state=2;
		next_state=time+time_active;
	} else if (state==2 && time>next_state) {
		state=3;
		animation.startSequence(6, 19, false, 19);
	} else if (state==3 && animation.isFinished()==true) {
		state=0;
		next_state=time+time_inactive;
	}
}



void StamperVertical::handleCollision(Player *player, const Collision &collision)
{
	if (collision.objectLeft() || collision.objectRight()) {
		player->setZeroVelocity();
	} else {
		player->dropHealth(100, Player::Smashed);
	}
}

void StamperVertical::toggle(bool enabled, Object *source)
{

}

size_t StamperVertical::save(unsigned char *buffer, size_t size)
{
	if (size<save_size) return 0;

	size_t bytes=Object::save(buffer,size);
	/*
	ppl7::Poke16(buffer+bytes,sample_id);
	ppl7::Poke16(buffer+bytes+2,max_distance);
	ppl7::PokeFloat(buffer+bytes+4,volume);
	*/
	return bytes;
}

bool StamperVertical::load(const unsigned char *buffer, size_t size)
{
	if (!Object::load(buffer,size)) return false;
	/*
	sample_id=0;
	volume=1.0f;
	if (size<save_size) return false;
	sample_id=ppl7::Peek16(buffer+9);
	max_distance=ppl7::Peek16(buffer+11);
	volume=ppl7::PeekFloat(buffer+13);
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
	*/
	return true;
}

void StamperVertical::openUi()
{
	/*
	SpeakerDialog *dialog=new SpeakerDialog(this);
	GetGameWindow()->addChild(dialog);
	*/
}



}	// EOF namespace Decker::Objects
