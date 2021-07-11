#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

SpriteSystem::SpriteSystem()
{
	bSpritesVisible=true;
	for (int i=0;i<=MAX_SPRITESETS;i++) {
		spriteset[i]=NULL;
	}
}

SpriteSystem::~SpriteSystem()
{

}

void SpriteSystem::setVisible(bool visible)
{
	bSpritesVisible=visible;
}

bool SpriteSystem::isVisible() const
{
	return bSpritesVisible;
}

void SpriteSystem::clear()
{
	sprite_list.clear();
}

void SpriteSystem::setSpriteset(int no, Sprite *spriteset)
{
	if (no<0 || no>MAX_SPRITESETS) return;
	this->spriteset[no]=spriteset;
}

void SpriteSystem::addSprite(int x, int y, int spriteset, int sprite_no, float sprite_scale)
{
	//printf ("x=%d, y=%d\n",x,y);
	SpriteSystem::Item item;
	item.x=x;
	item.y=y;
	item.z=0;
	item.sprite_no=sprite_no;
	item.sprite_set=spriteset;
	item.scale=sprite_scale;
	item.width=0;
	item.height=0;
	sprite_list.push_back(item);

}

void SpriteSystem::updateVisibleSpriteList(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport)
{

}

void SpriteSystem::draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	if (!bSpritesVisible) return;
	std::list<SpriteSystem::Item>::const_iterator it;
	for (it=sprite_list.begin();it!=sprite_list.end();++it) {
		if ((*it).sprite_set<=MAX_SPRITESETS && spriteset[(*it).sprite_set]!=NULL) {
			spriteset[(*it).sprite_set]->drawScaled(renderer,
					(*it).x+viewport.x1-worldcoords.x,
					(*it).y+viewport.y1-worldcoords.y,
					(*it).sprite_no, (*it).scale);
		}
	}
}

void SpriteSystem::save(ppl7::FileObject &file, unsigned char id) const
{
	if (sprite_list.size()==0) return;
	unsigned char *buffer=(unsigned char*)malloc(sprite_list.size()*18+5);
	ppl7::Poke32(buffer+0,0);
	ppl7::Poke8(buffer+4,id);
	size_t p=5;
	std::list<SpriteSystem::Item>::const_iterator it;
	for (it=sprite_list.begin();it!=sprite_list.end();++it) {
		const SpriteSystem::Item &item=(*it);
		ppl7::Poke32(buffer+p,item.x);
		ppl7::Poke32(buffer+p+4,item.y);
		ppl7::Poke16(buffer+p+8,item.z);
		ppl7::Poke16(buffer+p+10,item.sprite_set);
		ppl7::Poke16(buffer+p+12,item.sprite_no);
		ppl7::PokeFloat(buffer+p+14,item.scale);
		p+=18;
	}
	ppl7::Poke32(buffer+0,p);
	file.write(buffer,p);
	free(buffer);
}

void SpriteSystem::load(const ppl7::ByteArrayPtr &ba)
{
	clear();
	size_t p=0;
	const char *buffer=ba.toCharPtr();
	while (p<ba.size()) {
		SpriteSystem::Item item;
		item.x=ppl7::Peek32(buffer+p);
		item.y=ppl7::Peek32(buffer+p+4);
		item.z=ppl7::Peek16(buffer+p+8);
		item.sprite_set=ppl7::Peek16(buffer+p+10);
		item.sprite_no=ppl7::Peek16(buffer+p+12);
		item.scale=ppl7::PeekFloat(buffer+p+14);
		item.width=0;
		item.height=0;
		sprite_list.push_back(item);
		p+=18;
	}
}


