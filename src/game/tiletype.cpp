#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>


TileTypePlane::TileTypePlane()
{
	tiletypes=NULL;
	tilematrix=NULL;
	width=height=0;
}

TileTypePlane::~TileTypePlane()
{
	clear();
}

void TileTypePlane::clear()
{
	free(tilematrix);
	tilematrix=NULL;
	width=0;
	height=0;
}

void TileTypePlane::create(int width, int height)
{
	clear();
	this->width=width;
	this->height=height;
	tilematrix=(TileType::Type*)calloc(1, sizeof(TileType::Type) * (width + 1) * (height + 1));
	for (int y=0;y < height;y++) {
		for (int x=0;x < width;x++) {
			tilematrix[y * width + x]=TileType::Type::NonBlocking;
		}
	}
}

void TileTypePlane::setType(int x, int y, TileType::Type type)
{
	if (x < 0 || x >= width || y < 0 || y >= height || tilematrix == NULL) return;
	tilematrix[y * width + x]=type;
}

TileType::Type TileTypePlane::getType(int x, int y) const
{
	if (x < 0 || x >= width || y < 0 || y >= height || tilematrix == NULL) return TileType::Type::NonBlocking;
	return tilematrix[y * width + x];
}

TileType::Type TileTypePlane::getType(const ppl7::grafix::Point& player) const
{
	int tx=player.x / TILE_WIDTH;
	int ty=player.y / TILE_HEIGHT;
	return getType(tx, ty);
}

int TileTypePlane::getPlayerGround(const ppl7::grafix::Point& player) const
{
	int tx=player.x / TILE_WIDTH;
	int ty=player.y / TILE_HEIGHT;
	TileType::Type type=getType(tx, ty);
	if (type != TileType::Type::NonBlocking) {
		if (type == TileType::Type::SteepRampLeft) {
			//int x=player.x%TILE_WIDTH;
		}
	}
	type=getType(tx, ty + 1);
	if (type != TileType::Type::NonBlocking) {

	}
	return 0;
}

void TileTypePlane::setTileTypesSprites(SpriteTexture* sprites)
{
	this->tiletypes=sprites;
}

void TileTypePlane::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	if (!tiletypes) return;
	int tiles_width=viewport.width() / TILE_WIDTH + 2;
	int tiles_height=viewport.height() / TILE_HEIGHT + 2;
	int offset_x=worldcoords.x % TILE_WIDTH;
	int offset_y=worldcoords.y % TILE_HEIGHT;
	int start_x=worldcoords.x / TILE_WIDTH;
	int start_y=worldcoords.y / TILE_HEIGHT;
	int x1=viewport.x1 - offset_x;
	int y1=viewport.y1 - offset_y;

	for (int y=0;y < tiles_height;y++) {
		for (int x=0;x < tiles_width;x++) {
			TileType::Type type=getType(x + start_x, y + start_y);
			if (type > 0) {
				tiletypes->draw(renderer, x1 + x * TILE_WIDTH, y1 + y * TILE_HEIGHT, type);
			}
		}
	}
}

void TileTypePlane::save(ppl7::FileObject& file, unsigned char id) const
{
	// We only save tiles with type>0
	if (tilematrix == NULL) return;
	unsigned char* buffer=(unsigned char*)malloc(9 + (width * height * 5));
	ppl7::Poke32(buffer + 0, 0);
	ppl7::Poke8(buffer + 4, id);
	ppl7::Poke16(buffer + 5, width);
	ppl7::Poke16(buffer + 7, height);
	size_t p=9;
	for (int y=0;y < height;y++) {
		for (int x=0;x < width;x++) {
			TileType::Type type=tilematrix[y * width + x];
			if ((int)type > 0) {
				ppl7::Poke16(buffer + p, x);
				ppl7::Poke16(buffer + p + 2, y);
				ppl7::Poke8(buffer + p + 4, (int)type);
				p+=5;
			}
		}
	}
	ppl7::Poke32(buffer + 0, p);
	file.write(buffer, p);
	free(buffer);
}

void TileTypePlane::load(const ppl7::ByteArrayPtr& ba)
{
	size_t p=0;
	const char* buffer=ba.toCharPtr();
	width=ppl7::Peek16(buffer);
	height=ppl7::Peek16(buffer + 2);
	create(width, height);
	p+=4;
	while (p < ba.size()) {
		int x=ppl7::Peek16(buffer + p);
		int y=ppl7::Peek16(buffer + p + 2);
		int type=ppl7::Peek8(buffer + p + 4);
		setType(x, y, (TileType::Type)type);
		p+=5;
	}
}


ppl7::grafix::Rect TileTypePlane::getOccupiedArea() const
{
	ppl7::grafix::Rect r;
	if (tilematrix) {
		size_t count=0;
		for (int y=0;y < height;y++) {
			for (int x=0;x < width;x++) {
				TileType::Type type=tilematrix[y * width + x];
				if ((int)type > 0) {
					if (!count) {
						r.x1=x;
						r.y1=y;
						r.x2=x;
						r.y2=y;
					} else {
						if (x > r.x2) r.x2=x;
						if (x < r.x1) r.x1=x;
						if (y > r.y2) r.y2=y;
						if (y < r.y1) r.y1=y;
					}
					count++;
				}
			}
		}
	}
	return r;
}
