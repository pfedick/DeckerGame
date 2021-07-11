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

void SpriteSystem::addSprite(int x, int y, int z, int spriteset, int sprite_no, float sprite_scale)
{
	//printf ("x=%d, y=%d\n",x,y);
	SpriteSystem::Item item;
	item.id=(int)sprite_list.size()+1;
	item.x=x;
	item.y=y;
	item.z=z;
	item.sprite_no=sprite_no;
	item.sprite_set=spriteset;
	item.scale=sprite_scale;
	if (item.sprite_set<=MAX_SPRITESETS && this->spriteset[item.sprite_set]!=NULL) {
		item.boundary=this->spriteset[item.sprite_set]->spriteBoundary(sprite_no,sprite_scale,x,y);
	}
	sprite_list.insert(std::pair<int,SpriteSystem::Item>(item.id,item));

}

void SpriteSystem::updateVisibleSpriteList(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport)
{
	if (!bSpritesVisible) return;
	visible_sprite_map.clear();
	std::map<int, SpriteSystem::Item>::const_iterator it;
	int width=viewport.width();
	int height=viewport.height();
	for (it=sprite_list.begin();it!=sprite_list.end();++it) {
		const SpriteSystem::Item &item=(it->second);
		if (item.sprite_set<=MAX_SPRITESETS && spriteset[item.sprite_set]!=NULL) {
			int x=item.x-worldcoords.x;
			int y=item.y-worldcoords.y;
			if (x+item.boundary.width()>0 && y+item.boundary.height()>0
					&& x-item.boundary.width()<width && y-item.boundary.height()<height ) {
				uint64_t id=((uint64_t)item.z<<32&0x0000ffff00000000)|(uint64_t)(item.y<<16)|(uint64_t)item.x;
				visible_sprite_map.insert(std::pair<uint64_t,SpriteSystem::Item>(id,item));
			}
		}
	}

}

size_t SpriteSystem::count() const
{
	return sprite_list.size();
}

size_t SpriteSystem::countVisible() const
{
	return visible_sprite_map.size();
}



void SpriteSystem::draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	if (!bSpritesVisible) return;
	std::map<uint64_t,SpriteSystem::Item>::const_iterator it;
	for (it=visible_sprite_map.begin();it!=visible_sprite_map.end();++it) {
		const SpriteSystem::Item &item=(it->second);
		if (item.sprite_set<=MAX_SPRITESETS && spriteset[item.sprite_set]!=NULL) {
			spriteset[item.sprite_set]->drawScaled(renderer,
					item.x+viewport.x1-worldcoords.x,
					item.y+viewport.y1-worldcoords.y,
					item.sprite_no, item.scale);
		}
	}
	/*

	std::list<SpriteSystem::Item>::const_iterator it;
	for (it=sprite_list.begin();it!=sprite_list.end();++it) {
		if ((*it).sprite_set<=MAX_SPRITESETS && spriteset[(*it).sprite_set]!=NULL) {
			spriteset[(*it).sprite_set]->drawScaled(renderer,
					(*it).x+viewport.x1-worldcoords.x,
					(*it).y+viewport.y1-worldcoords.y,
					(*it).sprite_no, (*it).scale);
		}
	}
	*/

}

void SpriteSystem::drawSelectedSpriteOutline(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords, int id)
{
	if (!bSpritesVisible) return;
	std::map<int,SpriteSystem::Item>::const_iterator it;
	it=sprite_list.find(id);
	if (it!=sprite_list.end()) {
		const SpriteSystem::Item &item=(it->second);
		//printf ("found sprite to draw: %d\n",item.id);
		if (item.sprite_set<=MAX_SPRITESETS && spriteset[item.sprite_set]!=NULL) {
			//printf ("----: %d\n",item.id);
			spriteset[item.sprite_set]->drawOutlines(renderer,
					item.x+viewport.x1-worldcoords.x,
					item.y+viewport.y1-worldcoords.y,
					item.sprite_no, item.scale);
		}
	}
}

int SpriteSystem::findMatchingSprite(const ppl7::grafix::Point &p) const
{
	if (!bSpritesVisible) return -1;
	int bestmatch=-1;
	//printf ("Try to find sprite\n");
	std::map<uint64_t,SpriteSystem::Item>::const_iterator it;
	for (it=visible_sprite_map.begin();it!=visible_sprite_map.end();++it) {
		const SpriteSystem::Item &item=(it->second);
		/*
		printf ("checking: x=%d, y=%d, item.id=%d, item.x=%d, item.y=%d, item.w=%d, item.h=%d\n",
				p.x,p.y,item.id, item.boundary.x1, item.boundary.y1,
				item.boundary.width(), item.boundary.height());
		*/
		if (p.inside(item.boundary)) {
			//printf ("possible match: %d\n", item.id);
			bestmatch=item.id;
		}
	}
	return bestmatch;
}

void SpriteSystem::save(ppl7::FileObject &file, unsigned char id) const
{
	if (sprite_list.size()==0) return;
	unsigned char *buffer=(unsigned char*)malloc(sprite_list.size()*18+5);
	ppl7::Poke32(buffer+0,0);
	ppl7::Poke8(buffer+4,id);
	size_t p=5;
	std::map<int, SpriteSystem::Item>::const_iterator it;
	for (it=sprite_list.begin();it!=sprite_list.end();++it) {
		const SpriteSystem::Item &item=(it->second);
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
		addSprite(ppl7::Peek32(buffer+p),
				ppl7::Peek32(buffer+p+4),
				ppl7::Peek16(buffer+p+8),
				ppl7::Peek16(buffer+p+10),
				ppl7::Peek16(buffer+p+12),
				ppl7::PeekFloat(buffer+p+14));
		p+=18;
	}
}


