#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Plane::Plane()
{
	tilematrix=NULL;
	width=0;
	height=0;
	bTilesVisible=true;
}

Plane::~Plane()
{
	clear();
}

void Plane::clear()
{
	free(tilematrix);
	tilematrix=NULL;
	width=0;
	height=0;
}

void Plane::create(int width, int height)
{
	clear();
	this->width=width;
	this->height=height;
	tilematrix=(Tile**)calloc(1, sizeof(Tile*) * (width + 1) * (height + 1));
	for (int y=0;y < height;y++) {
		for (int x=0;x < width;x++) {
			tilematrix[y * width + x]=(Tile*)NULL;
		}
	}
}

ppl7::grafix::Size Plane::getSize() const
{
	return ppl7::grafix::Size(width, height);
}

void Plane::setTile(int x, int y, int z, int tileset, int tileno, bool showStuds)
{
	if (x < 0 || x >= width || y < 0 || y >= height || tilematrix == NULL) return;
	if (z < 0 || z >= MAX_TILESETS) return;
	if (tilematrix[y * width + x] == NULL) {
		tilematrix[y * width + x]=new Tile();
	}
	tilematrix[y * width + x]->setSprite(z, tileset, tileno, showStuds);
}

void Plane::setOccupation(int x, int y, int z, Tile::TileOccupation o, int origin_x, int origin_y)
{
	if (x < 0 || x >= width || y < 0 || y >= height || tilematrix == NULL) return;
	if (tilematrix[y * width + x] == NULL) {
		tilematrix[y * width + x]=new Tile();
	}
	tilematrix[y * width + x]->setOccupation(z, o, origin_x, origin_y);
}

void Plane::setBlockBackground(int x, int y, bool block)
{
	if (x < 0 || x >= width || y < 0 || y >= height || tilematrix == NULL) return;
	if (tilematrix[y * width + x] == NULL) {
		tilematrix[y * width + x]=new Tile();
	}
	tilematrix[y * width + x]->block_background=block;

}

void Plane::setOccupation(int x, int y, int z, const BrickOccupation::Matrix& matrix)
{
	BrickOccupation::Matrix::const_iterator it;
	for (it=matrix.begin();it != matrix.end();++it) {
		const BrickOccupation::Item& item=(*it);
		setOccupation(x + item.x, y - item.y, z, item.o, x, y);
	}
}

void Plane::clearOccupation(int x, int y, int z, const BrickOccupation::Matrix& matrix)
{
	BrickOccupation::Matrix::const_iterator it;
	for (it=matrix.begin();it != matrix.end();++it) {
		const BrickOccupation::Item& item=(*it);
		setOccupation(x + item.x, y - item.y, z, Tile::OccupationNone);
	}
	setOccupation(x, y, z, Tile::OccupationNone);
}


Tile::TileOccupation Plane::getOccupation(int x, int y, int z)
{
	if (x < 0 || x >= width || y < 0 || y >= height || tilematrix == NULL) return Tile::TileOccupation::OccupationNone;
	if (tilematrix[y * width + x] == NULL || z < 0 || z >= MAX_TILE_LAYER) return Tile::TileOccupation::OccupationNone;
	return tilematrix[y * width + x]->layer[z].occupation;
}

bool Plane::isOccupied(int x, int y, int z, const BrickOccupation::Matrix& matrix)
{
	BrickOccupation::Matrix::const_iterator it;
	for (it=matrix.begin();it != matrix.end();++it) {
		const BrickOccupation::Item& item=(*it);
		Tile::TileOccupation o=getOccupation(x + item.x, y - item.y, z);
		if (o) return true;
	}
	return false;
}



void Plane::clearTile(int x, int y, int z)
{
	if (x < 0 || x >= width || y < 0 || y >= height || tilematrix == NULL) return;
	if (z < 0 || z >= MAX_TILE_LAYER) return;
	if (tilematrix[y * width + x] != NULL) {
		tilematrix[y * width + x]->setSprite(z, 0, 0, true);
		tilematrix[y * width + x]->setOccupation(z, Tile::OccupationNone);
	}
}

const Tile* Plane::get(int x, int y) const
{
	if (x < 0 || x >= width || y < 0 || y >= height || tilematrix == NULL) return NULL;
	return tilematrix[y * width + x];
}

int Plane::getTileNo(int x, int y, int z)
{
	if (z < 0 || z >= MAX_TILE_LAYER) return -1;
	const Tile* t=get(x, y);
	if (!t) return -1;
	return t->layer[z].tileno;
}

int Plane::getTileSet(int x, int y, int z)
{
	if (z < 0 || z >= MAX_TILE_LAYER) return -1;
	const Tile* t=get(x, y);
	if (!t) return -1;
	return t->layer[z].tileset;
}

ppl7::grafix::Point Plane::getOccupationOrigin(int x, int y, int z)
{
	const Tile* t=get(x, y);
	if (t == NULL || z < 0 || z >= MAX_TILE_LAYER) return ppl7::grafix::Point(-1, -1);
	return ppl7::grafix::Point(t->layer[z].origin_x, t->layer[z].origin_y);
}

void Plane::setVisible(bool visible)
{
	bTilesVisible=visible;
}

bool Plane::isVisible() const
{
	return bTilesVisible;
}

void Plane::save(ppl7::FileObject& file, unsigned char id) const
{
	if (tilematrix == NULL) return;
	unsigned char* buffer=(unsigned char*)malloc(9 + width * height * (MAX_TILE_LAYER * 10));
	ppl7::Poke32(buffer + 0, 0);
	ppl7::Poke8(buffer + 4, id);
	ppl7::Poke16(buffer + 5, width);
	ppl7::Poke16(buffer + 7, height);
	size_t p=9;
	for (int y=0;y < height;y++) {
		for (int x=0;x < width;x++) {
			const Tile* t=tilematrix[y * width + x];
			if (t) {
				ppl7::Poke16(buffer + p, x);
				ppl7::Poke16(buffer + p + 2, y);
				ppl7::Poke8(buffer + p + 4, (int)t->block_background);
				p+=5;
				for (int z=0;z < MAX_TILE_LAYER;z++) {
					ppl7::Poke16(buffer + p, t->layer[z].tileset);
					ppl7::Poke16(buffer + p + 2, t->layer[z].tileno);
					ppl7::Poke16(buffer + p + 4, t->layer[z].origin_x);
					ppl7::Poke16(buffer + p + 6, t->layer[z].origin_y);
					ppl7::Poke8(buffer + p + 8, t->layer[z].occupation);
					ppl7::Poke8(buffer + p + 9, t->layer[z].showStuds);
					p+=10;
				}
			}
		}
	}
	ppl7::Poke32(buffer + 0, p);
	file.write(buffer, p);
	free(buffer);
}

void Plane::load(const ppl7::ByteArrayPtr& ba)
{
	size_t p=0;
	const char* buffer=ba.toCharPtr();
	width=ppl7::Peek16(buffer);
	height=ppl7::Peek16(buffer + 2);
	//printf ("width: %d, height: %d\n",width,height);
	create(width, height);
	p+=4;
	while (p < ba.size()) {
		int x=ppl7::Peek16(buffer + p);
		int y=ppl7::Peek16(buffer + p + 2);
		bool block_background=(bool)ppl7::Peek8(buffer + p + 4);
		p+=5;
		for (int z=0;z < MAX_TILE_LAYER;z++) {
			int tileset=ppl7::Peek16(buffer + p);
			int tileno=ppl7::Peek16(buffer + p + 2);
			int origin_x=ppl7::Peek16(buffer + p + 4);
			int origin_y=ppl7::Peek16(buffer + p + 6);
			int occupation=ppl7::Peek8(buffer + p + 8);
			bool showStuds=ppl7::Peek8(buffer + p + 9);
			setTile(x, y, z, tileset, tileno, showStuds);
			setOccupation(x, y, z, (Tile::TileOccupation)occupation, origin_x, origin_y);
			p+=10;
		}
		setBlockBackground(x, y, block_background);
	}
}
