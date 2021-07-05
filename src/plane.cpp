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
		setTile(0,y,0,1,4);
		setTile(29,y,0,1,4);
	}
	for (int x=1;x<30;x++) {
		setTile(x,0,0,1,ppl7::rand(4,7));
		setTile(x,12,0,1,ppl7::rand(8,11));
		setTile(x,13,0,1,ppl7::rand(4,7));
		setTile(x,14,0,1,ppl7::rand(4,7));
		setTile(x,15,0,1,ppl7::rand(4,7));
	}
}

void Plane::setTile(int x, int y, int z, int tileset, int tileno)
{
	if (x<0 || x>=width) return;
	if (y<0 || y>=height) return;
	if (z<0 || z>=3) return;
	if (tilematrix[y*width+x]==NULL) {
		tilematrix[y*width+x]=new Tile(tileset, tileno, z);
	} else {
		tilematrix[y*width+x]->set(tileset, tileno, z);
	}
}

void Plane::clearTile(int x, int y, int z)
{
	if (x<0 || x>=width) return;
	if (y<0 || y>=height) return;
	if (z<0 || z>=3) return;

	if (tilematrix[y*width+x]!=NULL) {
		tilematrix[y*width+x]->set(0, 0, z);
		tilematrix[y*width+x]->setType(Tile::NonBlocking);
	}
}

const Tile *Plane::get(int x, int y) const
{
	if (x<0 || x>=width || y<0 || y>=height) return NULL;
	return tilematrix[y*width+x];
}




