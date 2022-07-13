#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Tile::Tile()
{
	for (int z=0;z < MAX_TILE_LAYER;z++) {
		this->layer[z].tileset=0;
		this->layer[z].tileno=0;
		this->layer[z].origin_x=0;
		this->layer[z].origin_y=0;
		this->layer[z].occupation=TileOccupation::OccupationNone;
		this->layer[z].showStuds=true;
	}
	this->block_background=false;
}

void Tile::setSprite(int z, int tileset, int tileno, bool showStuds)
{
	if (z < 0 || z >= MAX_TILE_LAYER) return;
	this->layer[z].tileset=tileset;
	this->layer[z].tileno=tileno;
	this->layer[z].showStuds=showStuds;
}

void Tile::setOccupation(int z, TileOccupation o, int origin_x, int origin_y)
{
	if (z < 0 || z >= MAX_TILE_LAYER) return;
	this->layer[z].occupation=o;
	this->layer[z].origin_x=origin_x;
	this->layer[z].origin_y=origin_y;
}

bool Tile::hasSprite(int z) const
{
	if (z < 0 || z >= MAX_TILE_LAYER) return false;
	if (layer[z].tileset) return true;
	return false;
}

bool Tile::hasSprite() const
{
	for (int z=0;z < MAX_TILE_LAYER;z++) if (layer[z].tileset) return true;
	return false;
}
