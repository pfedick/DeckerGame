#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Tile::Tile(int tileset, int tileno, int z, TileType type)
{
	for (int i=0;i<3;i++) {
		this->tileset[i]=0;
		this->tileno[i]=0;
	}
	this->type=type;
	if (z<0 || z>2) return;
	this->tileset[z]=tileset;
	this->tileno[z]=tileno;
}

void Tile::set(int tileset, int tileno, int z)
{
	if (z<0 || z>2) return;
	this->tileset[z]=tileset;
	this->tileno[z]=tileno;
}

void Tile::setType(TileType type)
{
	this->type=type;
}

