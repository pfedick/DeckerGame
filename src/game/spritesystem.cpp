#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

SpriteSystem::Item::Item()
{
	spritesystem=NULL;
	texture=NULL;
	id=0;
	x=0;
	y=0;
	z=0;
	sprite_set=0;
	sprite_no=0;
	scale=1.0f;
	color_index=0;
}

SpriteSystem::SpriteSystem(const ColorPalette& palette)
	: palette(palette)
{
	bSpritesVisible=true;
	for (int i=0;i <= MAX_SPRITESETS;i++) {
		spriteset[i]=NULL;
	}
	maxid=0;
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
	maxid=0;
}

void SpriteSystem::setSpriteset(int no, SpriteTexture* spriteset)
{
	if (no<0 || no>MAX_SPRITESETS) return;
	this->spriteset[no]=spriteset;
}

void SpriteSystem::addSprite(int x, int y, int z, int spriteset, int sprite_no, float sprite_scale, uint32_t color_index)
{
	//printf ("x=%d, y=%d\n",x,y);
	SpriteSystem::Item item;
	maxid++;
	item.id=maxid;
	item.x=x;
	item.y=y;
	item.z=z;
	item.sprite_no=sprite_no;
	item.sprite_set=spriteset;
	item.scale=sprite_scale;
	item.spritesystem=this;
	item.color_index=color_index;
	if (item.sprite_set <= MAX_SPRITESETS && this->spriteset[item.sprite_set] != NULL) {
		item.texture=this->spriteset[item.sprite_set];
		item.boundary=this->spriteset[item.sprite_set]->spriteBoundary(sprite_no, sprite_scale, x, y);
	}
	sprite_list.insert(std::pair<int, SpriteSystem::Item>(item.id, item));

}

void SpriteSystem::updateVisibleSpriteList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
	if (!bSpritesVisible) return;
	visible_sprite_map.clear();
	std::map<int, SpriteSystem::Item>::const_iterator it;
	int width=viewport.width();
	int height=viewport.height();
	for (it=sprite_list.begin();it != sprite_list.end();++it) {
		const SpriteSystem::Item& item=(it->second);
		if (item.texture) {
			int x=item.x - worldcoords.x;
			int y=item.y - worldcoords.y;
			if (x + item.boundary.width() > 0 && y + item.boundary.height() > 0
				&& x - item.boundary.width() < width && y - item.boundary.height() < height) {
				uint64_t id=((uint64_t)item.z << 32 & 0x0000ffff00000000) | (uint64_t)(item.y << 16) | (uint64_t)item.x;
				visible_sprite_map.insert(std::pair<uint64_t, const SpriteSystem::Item&>(id, item));
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



void SpriteSystem::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	if (!bSpritesVisible) return;
	std::map<uint64_t, const SpriteSystem::Item&>::const_iterator it;
	for (it=visible_sprite_map.begin();it != visible_sprite_map.end();++it) {
		const SpriteSystem::Item& item=(it->second);
		if (item.texture) {
			item.texture->drawScaled(renderer,
				item.x + viewport.x1 - worldcoords.x,
				item.y + viewport.y1 - worldcoords.y,
				item.sprite_no, item.scale,
				palette.getColor(item.color_index));
		}
	}
}

void SpriteSystem::drawSelectedSpriteOutline(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id)
{
	if (!bSpritesVisible) return;
	std::map<int, SpriteSystem::Item>::const_iterator it;
	it=sprite_list.find(id);
	if (it != sprite_list.end()) {
		const SpriteSystem::Item& item=(it->second);
		if (item.texture) {
			item.texture->drawOutlines(renderer,
				item.x + viewport.x1 - worldcoords.x,
				item.y + viewport.y1 - worldcoords.y,
				item.sprite_no, item.scale);
		}
	}
}

void SpriteSystem::deleteSprite(int id)
{
	std::map<int, SpriteSystem::Item>::const_iterator it;
	it=sprite_list.find(id);
	if (it != sprite_list.end()) {
		sprite_list.erase(it);
	}
}

void SpriteSystem::modifySprite(const SpriteSystem::Item& item)
{
	std::map<int, SpriteSystem::Item>::iterator it;
	it=sprite_list.find(item.id);
	if (it != sprite_list.end()) {
		SpriteSystem::Item& intitem=(it->second);
		intitem.x=item.x;
		intitem.y=item.y;
		intitem.z=item.z;
		intitem.scale=item.scale;
		if (intitem.texture) {
			intitem.boundary=intitem.texture->spriteBoundary(intitem.sprite_no,
				intitem.scale, intitem.x, intitem.y);
		}
	}
}


bool SpriteSystem::findMatchingSprite(const ppl7::grafix::Point& p, SpriteSystem::Item& sprite) const
{
	bool found_match=false;
	sprite.id=-1;
	if (!bSpritesVisible) return false;
	//printf ("Try to find sprite\n");
	std::map<uint64_t, const SpriteSystem::Item&>::const_iterator it;
	for (it=visible_sprite_map.begin();it != visible_sprite_map.end();++it) {
		const SpriteSystem::Item& item=(it->second);
		if (p.inside(item.boundary)) {
			//printf ("possible match: %d\n", item.id);
			if (item.texture) {
				const ppl7::grafix::Drawable draw=item.texture->getDrawable(item.sprite_no);
				if (draw.width()) {
					int x=p.x - item.boundary.x1;
					int y=p.y - item.boundary.y1;
					ppl7::grafix::Color c=draw.getPixel(x / item.scale, y / item.scale);
					if (c.alpha() > 40) {
						sprite=item;
						found_match=true;
					}
				}
			}
		}
	}
	return found_match;
}

void SpriteSystem::save(ppl7::FileObject& file, unsigned char id) const
{
	if (sprite_list.size() == 0) return;
	unsigned char* buffer=(unsigned char*)malloc(sprite_list.size() * 18 + 6);
	ppl7::Poke32(buffer + 0, 0);
	ppl7::Poke8(buffer + 4, id);
	ppl7::Poke8(buffer + 5, 1);		// Version
	size_t p=6;
	std::map<int, SpriteSystem::Item>::const_iterator it;
	for (it=sprite_list.begin();it != sprite_list.end();++it) {
		const SpriteSystem::Item& item=(it->second);
		ppl7::Poke32(buffer + p, item.x);
		ppl7::Poke32(buffer + p + 4, item.y);
		ppl7::Poke8(buffer + p + 8, item.z);
		ppl7::Poke8(buffer + p + 9, item.color_index);
		ppl7::Poke16(buffer + p + 10, item.sprite_set);
		ppl7::Poke16(buffer + p + 12, item.sprite_no);
		ppl7::PokeFloat(buffer + p + 14, item.scale);
		p+=18;
	}
	ppl7::Poke32(buffer + 0, p);
	file.write(buffer, p);
	free(buffer);
}

void SpriteSystem::load(const ppl7::ByteArrayPtr& ba)
{
	clear();
	const char* buffer=ba.toCharPtr();
	int version=ppl7::Peek8(buffer);
	size_t p=1;
	if (version == 1) {
		while (p < ba.size()) {
			addSprite(ppl7::Peek32(buffer + p),
				ppl7::Peek32(buffer + p + 4),
				ppl7::Peek8(buffer + p + 8),
				ppl7::Peek16(buffer + p + 10),
				ppl7::Peek16(buffer + p + 12),
				ppl7::PeekFloat(buffer + p + 14),
				ppl7::Peek8(buffer + p + 9));
			p+=18;
		}
	} else {
		printf("Can't load SpriteSystem, unknown version! [%d]\n", version);

	}
}
