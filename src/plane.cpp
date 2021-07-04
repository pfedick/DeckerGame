#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Plane::Plane()
{
	tilematrix=NULL;

}

Plane::~Plane()
{
	clear();
}

void Plane::clear()
{
	free(tilematrix);
	tilematrix=NULL;
}

void Plane::create(int width, int height, int tile_width, int tile_height)
{
	clear();
	this->width=width;
	this->height=height;
	tilematrix=(Tile **)calloc(1,sizeof(Tile*)*(width+1)*(height+1));
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
			tilematrix[y*width+x]=(Tile *)NULL;
		}
	}

	for (int y=0;y<20;y++) {
		setTile(0,y,1,4);
		setTile(29,y,1,4);
	}
	for (int x=1;x<30;x++) {
		setTile(x,0,1,ppl7::rand(4,7));
		setTile(x,12,1,ppl7::rand(8,11));
		setTile(x,13,1,ppl7::rand(4,7));
		setTile(x,14,1,ppl7::rand(4,7));
		setTile(x,15,1,ppl7::rand(4,7));
	}
}

void Plane::setTile(int x, int y, int tileset, int tileno)
{
	if (x<0 || x>=width) return;
	if (y<0 || y>=height) return;
	if (tilematrix[y*width+x]==NULL) {
		tilematrix[y*width+x]=new Tile(tileset, tileno);
	} else {
		tilematrix[y*width+x]->set(tileset, tileno);
	}
}

const Tile *Plane::get(int x, int y) const
{
	if (x<0 || x>=width || y<0 || y>=height) return NULL;
	return tilematrix[y*width+x];
}




