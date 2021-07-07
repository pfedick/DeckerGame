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
	tilematrix=(Tile **)calloc(1,sizeof(Tile*)*(width+1)*(height+1));
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
			tilematrix[y*width+x]=(Tile *)NULL;
		}
	}
}

void Plane::setTile(int x, int y, int z, int tileset, int tileno)
{
	if (x<0 || x>=width || y<0 || y>=height || tilematrix==NULL) return;
	if (z<0 || z>=3) return;
	if (tilematrix[y*width+x]==NULL) {
		tilematrix[y*width+x]=new Tile(tileset, tileno, z);
	} else {
		tilematrix[y*width+x]->set(tileset, tileno, z);
	}
}

void Plane::setType(int x, int y, Tile::TileType type)
{
	if (x<0 || x>=width || y<0 || y>=height || tilematrix==NULL) return;
	if (tilematrix[y*width+x]==NULL) {
		tilematrix[y*width+x]=new Tile(type);
	} else {
		tilematrix[y*width+x]->setType(type);
	}
}

void Plane::clearTile(int x, int y, int z)
{
	if (x<0 || x>=width || y<0 || y>=height || tilematrix==NULL) return;
	if (z<0 || z>=3) return;
	if (tilematrix[y*width+x]!=NULL) {
		tilematrix[y*width+x]->set(0, 0, z);
		tilematrix[y*width+x]->setType(Tile::NonBlocking);
	}
}

const Tile *Plane::get(int x, int y) const
{
	if (x<0 || x>=width || y<0 || y>=height || tilematrix==NULL) return NULL;
	return tilematrix[y*width+x];
}

void Plane::save(ppl7::FileObject &file, unsigned char id) const
{
	if (tilematrix==NULL) return;
	unsigned char *buffer=(unsigned char*)malloc(width*height*17+9);
	ppl7::Poke32(buffer+0,0);
	ppl7::Poke8(buffer+4,id);
	ppl7::Poke16(buffer+5,width);
	ppl7::Poke16(buffer+7,height);
	size_t p=9;
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
			const Tile *t=tilematrix[y*width+x];
			if (t) {
				ppl7::Poke16(buffer+p,x);
				ppl7::Poke16(buffer+p+2,y);
				ppl7::Poke8(buffer+p+4,t->type);
				p+=5;
				for(int z=0;z<3;z++) {
					ppl7::Poke16(buffer+p,t->tileset[z]);
					ppl7::Poke16(buffer+p+2,t->tileno[z]);
					p+=4;
				}
			}
		}
	}
	ppl7::Poke32(buffer+0,p);
	file.write(buffer,p);
	free(buffer);
}

void Plane::load(const ppl7::ByteArrayPtr &ba)
{
	size_t p=0;
	const char *buffer=ba.toCharPtr();
	width=ppl7::Peek16(buffer);
	height=ppl7::Peek16(buffer+2);
	create(width, height);
	p+=4;
	while (p<ba.size()) {
		int x=ppl7::Peek16(buffer+p);
		int y=ppl7::Peek16(buffer+p+2);
		int type=ppl7::Peek8(buffer+p+4);
		setType(x,y,(Tile::TileType)type);
		p+=5;
		for(int z=0;z<3;z++) {
			int tileset=ppl7::Peek16(buffer+p);
			int tileno=ppl7::Peek16(buffer+p+2);
			setTile(x,y,z,tileset, tileno);
			p+=4;
		}
	}
}

Plane &Level::plane(int id)
{
	if (id==0) return PlayerPlane;
	if (id==1) return FrontPlane;
	if (id==2) return FarPlane;
	return PlayerPlane;

}


