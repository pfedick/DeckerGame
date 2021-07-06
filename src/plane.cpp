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
	/*
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
	*/
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

void Plane::setType(int x, int y, Tile::TileType type)
{
	if (x<0 || x>=width) return;
	if (y<0 || y>=height) return;
	if (tilematrix[y*width+x]==NULL) {
		tilematrix[y*width+x]=new Tile(type);
	} else {
		tilematrix[y*width+x]->setType(type);
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
	if (x<0 || x>=width || y<0 || y>=height || tilematrix==NULL) return NULL;
	return tilematrix[y*width+x];
}

void Plane::save(ppl7::FileObject &file, unsigned char id) const
{
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
	//printf ("size=%zd\n", ba.size());
	width=ppl7::Peek16(buffer);
	height=ppl7::Peek16(buffer+2);
	create(width, height);
	//printf("Loading Plane %d x %d\n",width,height);
	p+=4;
	while (p<ba.size()) {
		//printf ("size=%zd, p=%zd, ", ba.size(),p);
		int x=ppl7::Peek16(buffer+p);
		int y=ppl7::Peek16(buffer+p+2);
		int type=ppl7::Peek8(buffer+p+4);
		setType(x,y,(Tile::TileType)type);
		p+=5;
		//printf ("tile %d:%d type=%d, ", x,y,type);
		for(int z=0;z<3;z++) {
			int tileset=ppl7::Peek16(buffer+p);
			int tileno=ppl7::Peek16(buffer+p+2);
			//printf ("z=%d, set=%d, no=%d, ",z,tileset,tileno);
			setTile(x,y,z,tileset, tileno);
			p+=4;
		}
		//printf("\n");
	}
}


