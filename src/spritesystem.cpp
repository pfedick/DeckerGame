#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

SpriteSystem::SpriteSystem()
{

}

SpriteSystem::~SpriteSystem()
{

}


void SpriteSystem::addSprite(int x, int y, Sprite *sprite, int sprite_no, float sprite_scale)
{
	//printf ("x=%d, y=%d\n",x,y);
	SpriteSystem::Item item;
	item.sprite=sprite;
	item.x=x;
	item.y=y;
	item.sprite_no=sprite_no;
	item.scale=sprite_scale;
	sprite_list.push_back(item);

}

void SpriteSystem::updateVisibleSpriteList()
{

}

void SpriteSystem::draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	std::list<SpriteSystem::Item>::const_iterator it;
	for (it=sprite_list.begin();it!=sprite_list.end();++it) {
		(*it).sprite->drawScaled(renderer,(*it).x+viewport.x1-worldcoords.x,
				(*it).y+viewport.y1-worldcoords.y,
				(*it).sprite_no, (*it).scale);
	}
}

