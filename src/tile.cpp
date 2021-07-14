#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Tile::Tile()
{
	for (int i=0;i<MAX_TILE_LAYER;i++) {
		this->tileset[i]=0;
		this->tileno[i]=0;
		this->origin_x[i]=0;
		this->origin_y[i]=0;
		this->occupation[i]=TileOccupation::OccupationNone;
	}
	this->block_background=false;
}

void Tile::setSprite(int z, int tileset, int tileno, bool showStuds)
{
	if (z<0 || z>=MAX_TILE_LAYER) return;
	this->tileset[z]=tileset;
	this->tileno[z]=tileno;
	this->showStuds[z]=showStuds;
}

void Tile::setOccupation(int z, TileOccupation o, int origin_x, int origin_y)
{
	if (z<0 || z>=MAX_TILE_LAYER) return;
	this->occupation[z]=o;
	this->origin_x[z]=origin_x;
	this->origin_y[z]=origin_y;
}
