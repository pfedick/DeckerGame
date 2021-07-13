#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ppl7-grafix.h>

BrickOccupation::Item::Item(int x, int y, Tile::TileOccupation o)
{
	this->x=x;
	this->y=y;
	this->o=o;
}


void BrickOccupation::createFromSpriteTexture(const SpriteTexture &tex, int brick_width, int brick_height)
{
	tiles.clear();
	for (int i=0;i<tex.numSprites();i++) {
		createFromImage(i,tex.getDrawable(i), brick_width, brick_height);
	}
}

void BrickOccupation::createFromImage(int id, const ppl7::grafix::Drawable &img, int brick_width, int brick_height)
{
	int height=img.height();
	int plate_height=brick_height/3-1;
	int rows=img.height()/brick_height;
	int studs=img.width()/brick_width;
	int hw=brick_width/2;
	if (!rows) rows=1;

	//printf ("Find occupation of brick %d, ",id);
	//printf ("img(%d:%d), studs(%d:%d)\n",img.width(),img.height(),studs,rows);
	BrickOccupation::Matrix m;
	ppl7::grafix::Color pixel;
	for (int row=0;row<rows;row++) {
		for (int stud=0;stud<studs;stud++) {
			int occupation=0;
			pixel=img.getPixel(stud*brick_width+hw,height-row*brick_height-plate_height);
			if (pixel.alpha()>200) occupation|=Tile::TileOccupation::OccupationPlate0;
			pixel=img.getPixel(stud*brick_width+hw,height-row*brick_height-plate_height*2);
			if (pixel.alpha()>200) occupation|=Tile::TileOccupation::OccupationPlate1;
			pixel=img.getPixel(stud*brick_width+hw,height-row*brick_height-plate_height*3);
			if (pixel.alpha()>200) occupation|=Tile::TileOccupation::OccupationPlate2;
			if (occupation) {
				BrickOccupation::Item item(stud,row,(Tile::TileOccupation)occupation);
				m.push_back(item);
				//printf ("stud(%d:%d)=%d ",stud,row,(int)occupation);
			}
		}
	}
	if (m.size()) {
		tiles.insert(std::pair<int,BrickOccupation::Matrix>(id,m));
	}
	//printf("\n");
}

void BrickOccupation::set(int id, const BrickOccupation::Matrix &matrix)
{
	tiles.insert(std::pair<int,BrickOccupation::Matrix>(id,matrix));
}

const BrickOccupation::Matrix &BrickOccupation::get(int id)
{
	std::map<int,BrickOccupation::Matrix>::const_iterator it;
	it=tiles.find(id);
	if (it!=tiles.end()) return (it->second);
	return empty;
}
