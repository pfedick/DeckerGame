#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Level::Level()
{
	for (int i=0;i<10;i++) {
		tileset[i]=NULL;
	}
}

Level::~Level()
{
	clear();
}

void Level::clear()
{
	PlayerPlane.clear();
}

void Level::setTileset(int no, Sprite *tileset)
{
	if (no<0 || no>9) return;
	this->tileset[no]=tileset;
}

void Level::create(int width, int height)
{
	clear();
	PlayerPlane.create(width, height, TILE_WIDTH, TILE_HEIGHT);

}

void Level::setViewport(const ppl7::grafix::Rect &r)
{
	viewport=r;
}


void Level::load(const ppl7::String &Filename)
{

}

void Level::save(const ppl7::String &Filename)
{

}

void Level::drawPlayerPlane(SDL_Renderer *renderer, const ppl7::grafix::Point &playercoords)
{
	//printf("viewport: x=%d, y=%d\n",viewport.x1, viewport.y1);
	for (int y=0;y<20;y++) {
		for (int x=0;x<30;x++) {
			const Tile *tile=PlayerPlane.get(x,y);
			if (tile) {
				for (int z=0;z<3;z++) {
					if (tile->tileset[z] && tileset[tile->tileset[z]]) {
						//printf ("%d = %zd\n,",tile->tileset[z], tileset[tile->tileset[z]]);
						tileset[tile->tileset[z]]->draw(renderer,viewport.x1+x*64,viewport.y1+y*64,tile->tileno[z]);
					}
				}
			}
		}
	}

}

