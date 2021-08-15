#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

Floater::Floater(Type::ObjectType type)
: Object(type)
{
	sprite_no_representation=115;
	direction=0;
	collisionDetection=true;
	next_animation=0.0f;
	next_state=ppl7::GetMicrotime()+5.0f;
	state=0;
	flame_sprite1=152;
	flame_sprite2=152;
	audio=NULL;
	initial_state=enabled;
	current_state=enabled;
}

Floater::~Floater()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}


void Floater::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (current_state==false && state==0) return;
	AudioPool &pool=getAudioPool();
	if (!audio) {
		audio=pool.getInstance(AudioClip::thruster);
		audio->setVolume(0.3f);
		audio->setPositional(p, 1800);
		audio->setLoop(true);
	}
	audio->setPositional(p, 1800);
	if (direction==0) {		// horizontal
		if (next_state<time && (state==0 || state==2)) {
			state++;
			pool.playInstance(audio);
		} else if (state==1) {
			velocity.x=4;
			velocity.y=0;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x+64, p.y));
			if (t1==TileType::Blocking || t1==TileType::EnemyBlocker) {
				velocity.setPoint(0,0);
				state=2;
				next_state=time+5.0f;
				pool.stopInstace(audio);
			}
			if (time>next_animation) {
				flame_sprite1=ppl7::rand(152,161);
				next_animation=time+0.06f;
			}
		} else if (state==3) {
			velocity.x=-4;
			velocity.y=0;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x-64, p.y));
			if (t1==TileType::Blocking || t1==TileType::EnemyBlocker) {
				velocity.setPoint(0,0);
				state=0;
				pool.stopInstace(audio);
				next_state=time+5.0f;
			}
			if (time>next_animation) {
				flame_sprite1=ppl7::rand(162,171);
				next_animation=time+0.06f;
			}
		}
	} else { // vertical
		if (next_state<time && (state==0 || state==2)) {
			state++;
			pool.playInstance(audio);
		} else if (state==1) {
			velocity.x=0;
			velocity.y=4;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y+64));
			if (t1==TileType::Blocking || t1==TileType::EnemyBlocker) {
				velocity.setPoint(0,0);
				state=2;
				next_state=time+5.0f;
				pool.stopInstace(audio);
			}
			if (time>next_animation) {
				flame_sprite1=ppl7::rand(172,181);
				flame_sprite2=ppl7::rand(172,181);
				next_animation=time+0.06f;
			}
		} else if (state==3) {
			velocity.x=0;
			velocity.y=-4;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y-64));
			if (t1==TileType::Blocking || t1==TileType::EnemyBlocker) {
				velocity.setPoint(0,0);
				state=0;
				pool.stopInstace(audio);
				next_state=time+5.0f;
			}
			if (time>next_animation) {
				flame_sprite1=ppl7::rand(172,181);
				flame_sprite2=ppl7::rand(172,181);
				next_animation=time+0.06f;
			}
		}
	}
}

void Floater::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	if (direction==0) {
		if (state==1) texture->draw(renderer,
				p.x+coords.x-128-32,
				p.y+coords.y,
				flame_sprite1);
		else if (state==3) texture->draw(renderer,
				p.x+coords.x+128+32,
				p.y+coords.y,
				flame_sprite1);
	} else if (direction==1 && (state==1 || state==3)) {
		texture->draw(renderer,
				p.x+coords.x-16,
				p.y+coords.y+112,
				flame_sprite1);
		texture->draw(renderer,
				p.x+coords.x+16,
				p.y+coords.y+112,
				flame_sprite2);
	}
	Object::draw(renderer, coords);
}

void Floater::handleCollision(Player *player, const Collision &collision)
{
	if (collision.onFoot()) {
		//printf ("collision with floater\n");
		player->setStandingOnObject(this);
		player->x+=velocity.x;
		player->y+=velocity.y;
	}
}

void Floater::toggle(bool enable, Object *source)
{
	current_state=enable;
	//printf ("Floater::toggle %d: %d\n", id, (int)enable);
}


Representation FloaterHorizontal::representation()
{
	return Representation(Spriteset::GenericObjects, 115);
}


Representation FloaterVertical::representation()
{
	return Representation(Spriteset::GenericObjects, 116);
}

FloaterHorizontal::FloaterHorizontal()
: Floater(Type::FloaterHorizontal)
{
	sprite_no_representation=115;
	sprite_no=115;
	direction=0;
}


FloaterVertical::FloaterVertical()
: Floater(Type::FloaterVertical)
{
	sprite_no_representation=116;
	sprite_no=116;
	direction=1;
}




}	// EOF namespace Decker::Objects
