#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "audiopool.h"

namespace Decker::Objects {

Representation LaserBarrier::representation(Type::ObjectType type)
{
	if (type==Type::LaserBeamHorizontal) return Representation(Spriteset::GenericObjects, 213);
	return Representation(Spriteset::GenericObjects, 212);
}

LaserBarrier::LaserBarrier(Type::ObjectType type)
: Trap(type)
{
	sprite_set=Spriteset::GenericObjects;
	if (type==Type::LaserBeamHorizontal) {
		sprite_no=213;
		sprite_no_representation=213;
	} else {
		sprite_no=212;
		sprite_no_representation=212;
	}
	next_state=0.0f;
	state=ppl7::rand(0,1);
	pixelExactCollision=false;
	flicker=0;
	audio=NULL;
}

LaserBarrier::~LaserBarrier()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void LaserBarrier::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	if (state==0) return;
	if ((flicker&3)==0) return;
	if (type()==Type::LaserBeamHorizontal) {
		for (int i=start.x;i<end.x;i+=TILE_WIDTH)
			texture->draw(renderer,
					i+coords.x+16,
					start.y+coords.y,
					213);
	} else {
		for (int i=start.y;i<end.y;i+=TILE_HEIGHT)
			texture->draw(renderer,
					start.x+coords.x,
					i+coords.y+7+TILE_HEIGHT,
					212);
		/*
		SDL_SetRenderDrawColor(renderer,255,0,0,255);
		SDL_RenderDrawLine(renderer,start.x+coords.x, start.y+coords.y,
				end.x+coords.x, end.y+coords.y);
		SDL_Rect r;
		r.x=boundary.x1+coords.x;
		r.y=boundary.y1+coords.y;
		r.w=boundary.width();
		r.h=boundary.height();
		SDL_RenderDrawRect(renderer,&r);
		*/
	}

}

void LaserBarrier::update(double time, TileTypePlane &ttplane, Player &player)
{
	flicker++;
	if (next_state<time) {
		if (state==0) {
			AudioPool &pool=getAudioPool();
			if (!audio) {
				audio=pool.getInstance(AudioClip::electric);
				audio->setVolume(0.05f);
				audio->setPositional(p, 1600);
				audio->setLoop(true);
			}
			if (audio) {
				audio->setPositional(p, 1600);
				pool.playInstance(audio);
			}

			state=1;
			//next_state=time+(double)ppl7::rand(4000,8000)/1000.0f;
			next_state=time+1.0f;
			collisionDetection=true;
			if (type()==Type::LaserBeamHorizontal) {
				// find left end
				start.y=p.y;
				start.x=((int)p.x/TILE_WIDTH)*TILE_WIDTH;
				end=start;
				while (start.x>0 && ttplane.getType(start)!=TileType::Blocking)
					start.x-=TILE_WIDTH;
				start.x+=TILE_WIDTH;
				if (start.x<0)start.x=0;
				// find right end
				while (end.x<65000 && ttplane.getType(end)!=TileType::Blocking)
					end.x+=TILE_WIDTH;
				// update Boundary by Points
				boundary.x1=start.x;
				boundary.y1=start.y-15;
				boundary.x2=end.x;
				boundary.y2=end.y+15;
			} else {
				// find upper end
				start.x=p.x;
				start.y=((int)p.y/TILE_HEIGHT)*TILE_HEIGHT;
				end=start;
				while (start.y>0 && ttplane.getType(start)!=TileType::Blocking)
					start.y-=TILE_HEIGHT;
				start.y+=TILE_HEIGHT;
				if (start.y<0)start.y=0;
				// find lower end
				while (end.y<65000 && ttplane.getType(end)!=TileType::Blocking)
					end.y+=TILE_HEIGHT;
				//printf ("p=%d, start: %d, end: %d\n",p.y, start.y, end.y);
				// update Boundary by Points
				boundary.x1=start.x-15;
				boundary.y1=start.y;
				boundary.x2=end.x+15;
				boundary.y2=end.y;

			}
		} else {
			state=0;
			if (audio) {
				getAudioPool().stopInstace(audio);
			}
			//next_state=time+(double)ppl7::rand(2000,4000)/1000.f;
			next_state=time+1.0f;
			collisionDetection=false;
		}
	}

}

void LaserBarrier::handleCollision(Player *player, const Collision &collision)
{
	player->dropHealth(10);
}

}	// EOF namespace Decker::Objects
