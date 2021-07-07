#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Level::Level()
{
	for (int i=0;i<10;i++) {
		tileset[i]=NULL;
	}
	tiletypes=NULL;
}

Level::~Level()
{
	clear();
}

void Level::clear()
{
	PlayerPlane.clear();
	FarPlane.clear();
	FrontPlane.clear();
}

void Level::setTileset(int no, Sprite *tileset)
{
	if (no<0 || no>9) return;
	this->tileset[no]=tileset;
}

void Level::setTileTypesSprites(Sprite *sprites)
{
	tiletypes=sprites;
}

void Level::create(int width, int height)
{
	clear();
	PlayerPlane.create(width, height);

}

void Level::setViewport(const ppl7::grafix::Rect &r)
{
	viewport=r;
}


void Level::load(const ppl7::String &Filename)
{
	clear();
	ppl7::File ff;
	ff.open(Filename, ppl7::File::READ);
	ppl7::ByteArray ba;
	ff.read(ba,7);
	const char *buffer=ba.toCharPtr();
	if (memcmp(buffer,"Decker",7)!=0) {
		printf("Invalid Fileformat\n");
		return;
	}
	while(!ff.eof()) {
		try {
			size_t bytes_read=ff.read(ba,5);
			if (bytes_read!=5) break;
			buffer=ba.toCharPtr();
			size_t size=ppl7::Peek32(buffer);
			int id=ppl7::Peek8(buffer+4);
			bytes_read=ff.read(ba,size-5);
			if (bytes_read!=size-5) break;
			if (id==1) {
				PlayerPlane.load(ba);
			} else if (id==2) {
				FrontPlane.load(ba);
			} else if (id==3) {
				FarPlane.load(ba);
			}
		} catch (const ppl7::EndOfFileException &) {
			break;
		}
	}
	//FrontPlane.create(256,256);
	//FarPlane.create(256,256);

	ff.close();
}

void Level::save(const ppl7::String &Filename)
{
	ppl7::File ff;
	ff.open(Filename, ppl7::File::WRITE);
	char *buffer[20];
	memcpy(buffer,"Decker",7);
	ff.write(buffer,7);
	PlayerPlane.save(ff,1);
	FrontPlane.save(ff,2);
	FarPlane.save(ff,3);

	ff.close();

}

void Level::drawPlane(SDL_Renderer *renderer, const Plane &plane, const ppl7::grafix::Point &worldcoords) const
{
	//printf("viewport: x=%d, y=%d\n",viewport.x1, viewport.y1);
	int tiles_width=viewport.width()/64+1;
	int tiles_height=viewport.height()/64+1;
	int offset_x=worldcoords.x%64;
	int offset_y=worldcoords.y%64;
	int start_x=worldcoords.x/64;
	int start_y=worldcoords.y/64;
	int x1=viewport.x1-offset_x;
	int y1=viewport.y1-offset_y;

	for (int y=0;y<tiles_height;y++) {
		for (int x=0;x<tiles_width;x++) {
			const Tile *tile=plane.get(x+start_x,y+start_y);
			if (tile) {
				for (int z=0;z<3;z++) {
					if (tile->tileset[z]<10 && tileset[tile->tileset[z]]) {
						//printf ("%d = %zd\n,",tile->tileset[z], tileset[tile->tileset[z]]);
						tileset[tile->tileset[z]]->draw(renderer,x1+x*64,y1+y*64,tile->tileno[z]);
					}
				}
			}
		}
	}
}

void Level::drawTileTypes(SDL_Renderer *renderer, const ppl7::grafix::Point &worldcoords) const
{
	//printf("viewport: x=%d, y=%d\n",viewport.x1, viewport.y1);
	if (!tiletypes) return;
	int tiles_width=viewport.width()/64+1;
	int tiles_height=viewport.height()/64+1;
	int offset_x=worldcoords.x%64;
	int offset_y=worldcoords.y%64;
	int start_x=worldcoords.x/64;
	int start_y=worldcoords.y/64;
	int x1=viewport.x1-offset_x;
	int y1=viewport.y1-offset_y;

	for (int y=0;y<tiles_height;y++) {
		for (int x=0;x<tiles_width;x++) {
			const Tile *tile=PlayerPlane.get(x+start_x,y+start_y);
			if (tile!=NULL && tile->type>0) {
				tiletypes->draw(renderer,x1+x*64,y1+y*64,tile->type);
			}
		}
	}
}



