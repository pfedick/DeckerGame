#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Level::Level()
{
	renderer=NULL;
	tiles=NULL;
	width=height=layers=0;
}

Level::~Level()
{
	clear();
}

void Level::clear()
{
	if (!tiles) return;
	for (int l=0;l<layers;l++) {
		for (int y=0;y<height;y++) {
			for (int x=0;x<width;x++) {
				if (tiles[l][x][y]!=NULL) {
					delete tiles[l][x][y];
				}
			}
		}
	}
	free(tiles);
	tiles=NULL;
}

void Level::create(int width, int height, int layers)
{
	clear();
	tiles=(Tile ****)calloc(layers*width*height,sizeof(Tile*));
	this->width=width;
	this->height=height;
	this->layers=layers;
}

void Level::load(const ppl7::String &Filename)
{

}

void Level::save(const ppl7::String &Filename)
{

}

void Level::setRenderer(SDL_Renderer *renderer)
{
	this->renderer=renderer;
}


