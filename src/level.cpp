#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"

Level::Level()
{
	for (int i=0;i<=MAX_TILESETS;i++) {
		tileset[i]=NULL;
	}
	for (int i=0;i<=MAX_SPRITESETS;i++) {
		spriteset[i]=NULL;
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

void Level::setSpriteset(int no, Sprite *spriteset)
{
	if (no<0 || no>MAX_SPRITESETS) return;
	this->spriteset[no]=spriteset;
	FarSprites[0].setSpriteset(no, spriteset);
	FarSprites[1].setSpriteset(no, spriteset);
	PlayerSprites[0].setSpriteset(no, spriteset);
	PlayerSprites[1].setSpriteset(no, spriteset);
	FrontSprites[0].setSpriteset(no, spriteset);
	FrontSprites[1].setSpriteset(no, spriteset);
}

void Level::setTileTypesSprites(Sprite *sprites)
{
	tiletypes=sprites;
}

void Level::create(int width, int height)
{
	clear();
	FarPlane.create(width, height);
	PlayerPlane.create(width, height);
	FrontPlane.create(width, height);
	FarSprites[0].clear();
	FarSprites[1].clear();
	PlayerSprites[0].clear();
	PlayerSprites[1].clear();
	FrontSprites[0].clear();
	FrontSprites[1].clear();
}

void Level::setViewport(const ppl7::grafix::Rect &r)
{
	viewport=r;
}

Plane &Level::plane(int id)
{
	if (id==0) return PlayerPlane;
	if (id==1) return FrontPlane;
	if (id==2) return FarPlane;
	return PlayerPlane;

}

SpriteSystem &Level::spritesystem(int plane, int layer)
{
	if (plane==0) return PlayerSprites[layer];
	if (plane==1) return FrontSprites[layer];
	if (plane==2) return FarSprites[layer];
	return PlayerSprites[layer];
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
			//printf ("load id=%d, size=%zd\n",id,size);
			if (size<=5) continue;
			bytes_read=ff.read(ba,size-5);
			if (bytes_read!=size-5) break;
			if (id==LevelChunkId::chunkPlayerPlane) {
				PlayerPlane.load(ba);
			} else if (id==LevelChunkId::chunkFrontPlane) {
				FrontPlane.load(ba);
			} else if (id==LevelChunkId::chunkFarPlane) {
				FarPlane.load(ba);
			} else if (id==LevelChunkId::chunkPlayerSpritesLayer0) {
				PlayerSprites[0].load(ba);
			} else if (id==LevelChunkId::chunkPlayerSpritesLayer1) {
				PlayerSprites[1].load(ba);
			} else if (id==LevelChunkId::chunkFrontSpritesLayer0) {
				FrontSprites[0].load(ba);
			} else if (id==LevelChunkId::chunkFrontSpritesLayer1) {
				FrontSprites[1].load(ba);
			} else if (id==LevelChunkId::chunkFarSpritesLayer0) {
				FarSprites[0].load(ba);
			} else if (id==LevelChunkId::chunkFarSpritesLayer1) {
				FarSprites[1].load(ba);
			}
		} catch (const ppl7::EndOfFileException &) {
			break;
		}
	}


	ff.close();
	/*
	// For performance-testing of sprite system
	for (int i=0;i<1000000;i++) {
		PlayerSprites[0].addSprite(ppl7::rand(0,50000), ppl7::rand(1000,50000), 0, 1, ppl7::rand(0,52*4), 1.0f);
	}
	*/
}

void Level::save(const ppl7::String &Filename)
{
	ppl7::File ff;
	ff.open(Filename, ppl7::File::WRITE);
	char *buffer[20];
	memcpy(buffer,"Decker",7);
	ff.write(buffer,7);
	PlayerPlane.save(ff,LevelChunkId::chunkPlayerPlane);
	FrontPlane.save(ff,LevelChunkId::chunkFrontPlane);
	FarPlane.save(ff,LevelChunkId::chunkFarPlane);
	PlayerSprites[0].save(ff, LevelChunkId::chunkPlayerSpritesLayer0);
	PlayerSprites[1].save(ff, LevelChunkId::chunkPlayerSpritesLayer1);
	FrontSprites[0].save(ff, LevelChunkId::chunkFrontSpritesLayer0);
	FrontSprites[1].save(ff, LevelChunkId::chunkFrontSpritesLayer1);
	FarSprites[0].save(ff, LevelChunkId::chunkFarSpritesLayer0);
	FarSprites[1].save(ff, LevelChunkId::chunkFarSpritesLayer1);
	ff.close();

}

void Level::drawPlane(SDL_Renderer *renderer, const Plane &plane, const ppl7::grafix::Point &worldcoords) const
{
	//printf("viewport: x=%d, y=%d\n",viewport.x1, viewport.y1);
	if (!plane.isVisible()) return;
	int tiles_width=viewport.width()/64+2;
	int tiles_height=viewport.height()/64+2;
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
	int tiles_width=viewport.width()/64+2;
	int tiles_height=viewport.height()/64+2;
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


void Level::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &worldcoords, Player *player)
{
	if (FarPlane.isVisible()) {
		FarSprites[0].draw(renderer, viewport,worldcoords*0.5f);
		drawPlane(renderer,FarPlane, worldcoords*0.5f);
		FarSprites[1].draw(renderer, viewport,worldcoords*0.5f);
	}
	if (PlayerPlane.isVisible()) {
		PlayerSprites[0].draw(renderer, viewport,worldcoords);
		drawPlane(renderer,PlayerPlane, worldcoords);
		PlayerSprites[1].draw(renderer, viewport,worldcoords);
		player->draw(renderer);
	}
	if (FrontPlane.isVisible()) {
		FrontSprites[0].draw(renderer, viewport,worldcoords);
		drawPlane(renderer,FrontPlane, worldcoords);
		FrontSprites[1].draw(renderer, viewport,worldcoords);
	}
}

void Level::updateVisibleSpriteLists(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport)
{
	FarSprites[0].updateVisibleSpriteList(worldcoords*0.5, viewport);
	FarSprites[1].updateVisibleSpriteList(worldcoords*0.5, viewport);
	PlayerSprites[0].updateVisibleSpriteList(worldcoords, viewport);
	PlayerSprites[1].updateVisibleSpriteList(worldcoords, viewport);
	FrontSprites[0].updateVisibleSpriteList(worldcoords, viewport);
	FrontSprites[1].updateVisibleSpriteList(worldcoords, viewport);
}

size_t Level::countSprites() const
{
	size_t total=0;
	total+=FarSprites[0].count();
	total+=FarSprites[1].count();
	total+=PlayerSprites[0].count();
	total+=PlayerSprites[1].count();
	total+=FrontSprites[0].count();
	total+=FrontSprites[1].count();
	return total;
}

size_t Level::countVisibleSprites() const
{
	size_t total=0;
	total+=FarSprites[0].countVisible();
	total+=FarSprites[1].countVisible();
	total+=PlayerSprites[0].countVisible();
	total+=PlayerSprites[1].countVisible();
	total+=FrontSprites[0].countVisible();
	total+=FrontSprites[1].countVisible();
	return total;
}




