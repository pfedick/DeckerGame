#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Tile::Tile(TileType type)
{
	for (int i=0;i<MAX_TILE_LAYER;i++) {
		this->tileset[i]=0;
		this->tileno[i]=0;
		this->origin_x[i]=0;
		this->origin_y[i]=0;
		this->occupation[i]=TileOccupation::OccupationNone;
	}
	this->type=type;
}

void Tile::setSprite(int z, int tileset, int tileno)
{
	if (z<0 || z>=MAX_TILE_LAYER) return;
	this->tileset[z]=tileset;
	this->tileno[z]=tileno;
}

void Tile::setType(TileType type)
{
	this->type=type;
}

void Tile::setOccupation(int z, TileOccupation o, int origin_x, int origin_y)
{
	if (z<0 || z>=MAX_TILE_LAYER) return;
	this->occupation[z]=o;
	this->origin_x[z]=origin_x;
	this->origin_y[z]=origin_y;
}
