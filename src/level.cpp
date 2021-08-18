#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"

static double planeFactor[]={1.0f, 1.0f, 0.5f, 1.0f, 0.8f, 0.3f};

Level::Level()
{
	for (int i=0;i<=MAX_TILESETS;i++) {
		tileset[i]=NULL;
	}
	for (int i=0;i<=MAX_SPRITESETS;i++) {
		spriteset[i]=NULL;
	}
	objects=new Decker::Objects::ObjectSystem();
	editMode=false;
	showSprites=true;
	showObjects=true;
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
	BackPlane.clear();
	MiddlePlane.clear();
	HorizonPlane.clear();
	TileTypeMatrix.clear();
	FarSprites[0].clear();
	FarSprites[1].clear();
	PlayerSprites[0].clear();
	PlayerSprites[1].clear();
	FrontSprites[0].clear();
	FrontSprites[1].clear();
	MiddleSprites[0].clear();
	MiddleSprites[1].clear();
	BackSprites[0].clear();
	BackSprites[1].clear();
	HorizonSprites[0].clear();
	HorizonSprites[1].clear();

	objects->clear();
}

void Level::setEditmode(bool enabled)
{
	editMode=enabled;
}

void Level::setShowSprites(bool enabled)
{
	showSprites=enabled;
}

void Level::setShowObjects(bool enabled)
{
	showObjects=enabled;
}

void Level::setTileset(int no, SpriteTexture *tileset)
{
	//printf ("Level::setTileset no=%d\n",no);
	if (no<0 || no>MAX_TILESETS) return;
	this->tileset[no]=tileset;
}

void Level::setSpriteset(int no, SpriteTexture *spriteset)
{
	if (no<0 || no>MAX_SPRITESETS) return;
	this->spriteset[no]=spriteset;
	FarSprites[0].setSpriteset(no, spriteset);
	FarSprites[1].setSpriteset(no, spriteset);
	PlayerSprites[0].setSpriteset(no, spriteset);
	PlayerSprites[1].setSpriteset(no, spriteset);
	FrontSprites[0].setSpriteset(no, spriteset);
	FrontSprites[1].setSpriteset(no, spriteset);
	BackSprites[0].setSpriteset(no, spriteset);
	BackSprites[1].setSpriteset(no, spriteset);
	MiddleSprites[0].setSpriteset(no, spriteset);
	MiddleSprites[1].setSpriteset(no, spriteset);
	HorizonSprites[0].setSpriteset(no, spriteset);
	HorizonSprites[1].setSpriteset(no, spriteset);

}

void Level::create(int width, int height)
{
	clear();
	TileTypeMatrix.create(width, height);
	FarPlane.create(width, height);
	PlayerPlane.create(width, height);
	FrontPlane.create(width, height);
	BackPlane.create(width, height);
	MiddlePlane.create(width, height);
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
	if (id==3) return BackPlane;
	if (id==4) return MiddlePlane;
	if (id==5) return HorizonPlane;
	return PlayerPlane;

}

SpriteSystem &Level::spritesystem(int plane, int layer)
{
	if (plane==0) return PlayerSprites[layer];
	if (plane==1) return FrontSprites[layer];
	if (plane==2) return FarSprites[layer];
	if (plane==3) return BackSprites[layer];
	if (plane==4) return MiddleSprites[layer];
	if (plane==5) return HorizonSprites[layer];

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
			} else if (id==LevelChunkId::chunkBackPlane) {
				BackPlane.load(ba);
			} else if (id==LevelChunkId::chunkMiddlePlane) {
				MiddlePlane.load(ba);
			} else if (id==LevelChunkId::chunkHorizonPlane) {
				HorizonPlane.load(ba);
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
			} else if (id==LevelChunkId::chunkBackSpritesLayer0) {
				BackSprites[0].load(ba);
			} else if (id==LevelChunkId::chunkBackSpritesLayer1) {
				BackSprites[1].load(ba);
			} else if (id==LevelChunkId::chunkMiddleSpritesLayer0) {
				MiddleSprites[0].load(ba);
			} else if (id==LevelChunkId::chunkMiddleSpritesLayer1) {
				MiddleSprites[1].load(ba);
			} else if (id==LevelChunkId::chunkHorizonSpritesLayer0) {
				HorizonSprites[0].load(ba);
			} else if (id==LevelChunkId::chunkHorizonSpritesLayer1) {
				HorizonSprites[1].load(ba);
			} else if (id==LevelChunkId::chunkTileTypes) {
				TileTypeMatrix.load(ba);
			} else if (id==LevelChunkId::chunkObjects) {
				objects->load(ba);
			}
		} catch (const ppl7::EndOfFileException &) {
			break;
		}
	}

	//BackPlane.create(PlayerPlane.width, PlayerPlane.height);
	//MiddlePlane.create(PlayerPlane.width, PlayerPlane.height);
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
	BackPlane.save(ff,LevelChunkId::chunkBackPlane);
	MiddlePlane.save(ff,LevelChunkId::chunkMiddlePlane);
	HorizonPlane.save(ff,LevelChunkId::chunkHorizonPlane);
	TileTypeMatrix.save(ff,LevelChunkId::chunkTileTypes);
	PlayerSprites[0].save(ff, LevelChunkId::chunkPlayerSpritesLayer0);
	PlayerSprites[1].save(ff, LevelChunkId::chunkPlayerSpritesLayer1);
	FrontSprites[0].save(ff, LevelChunkId::chunkFrontSpritesLayer0);
	FrontSprites[1].save(ff, LevelChunkId::chunkFrontSpritesLayer1);
	FarSprites[0].save(ff, LevelChunkId::chunkFarSpritesLayer0);
	FarSprites[1].save(ff, LevelChunkId::chunkFarSpritesLayer1);
	FarSprites[0].save(ff, LevelChunkId::chunkFarSpritesLayer0);
	FarSprites[1].save(ff, LevelChunkId::chunkFarSpritesLayer1);
	BackSprites[0].save(ff, LevelChunkId::chunkBackSpritesLayer0);
	BackSprites[1].save(ff, LevelChunkId::chunkBackSpritesLayer1);
	MiddleSprites[0].save(ff, LevelChunkId::chunkMiddleSpritesLayer0);
	MiddleSprites[1].save(ff, LevelChunkId::chunkMiddleSpritesLayer1);
	HorizonSprites[0].save(ff, LevelChunkId::chunkHorizonSpritesLayer0);
	HorizonSprites[1].save(ff, LevelChunkId::chunkHorizonSpritesLayer1);
	objects->save(ff,LevelChunkId::chunkObjects);
	ff.close();

}

void Level::drawPlane(SDL_Renderer *renderer, const Plane &plane, const ppl7::grafix::Point &worldcoords) const
{
	//printf("viewport: x=%d, y=%d\n",viewport.x1, viewport.y1);
	if (!plane.isVisible()) return;
	int tiles_width=viewport.width()/TILE_WIDTH+9;
	int tiles_height=viewport.height()/TILE_HEIGHT+2;
	int offset_x=worldcoords.x%TILE_WIDTH;
	int offset_y=worldcoords.y%TILE_HEIGHT;
	int start_x=worldcoords.x/TILE_WIDTH-7;
	int start_y=worldcoords.y/TILE_HEIGHT;
	int x1=viewport.x1-offset_x-TILE_WIDTH*7;
	int y1=viewport.y1-offset_y+TILE_HEIGHT;

	for (int z=0;z<MAX_TILE_LAYER;z++) {
		for (int y=tiles_height;y>=0;y--) {
			for (int x=0;x<tiles_width;x++) {
				const Tile *tile=plane.get(x+start_x,y+start_y);
				if (tile) {
					//if (tile->layer[z].tileset>8) printf ("draw %d, %d\n",tile->layer[z].tileset, tile->layer[z].tileno);
					if (tile->layer[z].tileset<=MAX_TILESETS && tileset[tile->layer[z].tileset]) {
						//printf ("%d = %zd\n,",tile->tileset[z], tileset[tile->tileset[z]]);
						tileset[tile->layer[z].tileset]->draw(renderer,x1+x*TILE_WIDTH,y1+y*TILE_HEIGHT,tile->layer[z].tileno);
					}
				}
			}
		}
	}
}

void Level::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &worldcoords, Player *player)
{
	if (HorizonPlane.isVisible()) {
		if (showSprites) HorizonSprites[0].draw(renderer, viewport,worldcoords*planeFactor[5]);
		drawPlane(renderer,HorizonPlane, worldcoords*planeFactor[5]);
		if (showSprites) HorizonSprites[1].draw(renderer, viewport,worldcoords*planeFactor[5]);
	}

	if (FarPlane.isVisible()) {
		if (showSprites) FarSprites[0].draw(renderer, viewport,worldcoords*planeFactor[2]);
		drawPlane(renderer,FarPlane, worldcoords*planeFactor[2]);
		if (showSprites) FarSprites[1].draw(renderer, viewport,worldcoords*planeFactor[2]);
	}
	if (MiddlePlane.isVisible()) {
		if (showSprites) MiddleSprites[0].draw(renderer, viewport,worldcoords*planeFactor[4]);
		drawPlane(renderer,MiddlePlane, worldcoords*planeFactor[4]);
		if (showSprites) MiddleSprites[1].draw(renderer, viewport,worldcoords*planeFactor[4]);
	}
	if (BackPlane.isVisible()) {
		if (showSprites) BackSprites[0].draw(renderer, viewport,worldcoords*planeFactor[3]);
		drawPlane(renderer,BackPlane, worldcoords*planeFactor[3]);
		if (showSprites) BackSprites[1].draw(renderer, viewport,worldcoords*planeFactor[3]);
	}
	if (PlayerPlane.isVisible()) {
		if (showSprites) PlayerSprites[0].draw(renderer, viewport,worldcoords*planeFactor[0]);
		drawPlane(renderer,PlayerPlane, worldcoords*planeFactor[0]);
		if (showSprites) PlayerSprites[1].draw(renderer, viewport,worldcoords*planeFactor[0]);
		player->draw(renderer, viewport, worldcoords*planeFactor[0]);
		if (showObjects) {
			if (editMode)
				objects->drawEditMode(renderer, viewport, worldcoords*planeFactor[0]);
			else
				objects->draw(renderer, viewport, worldcoords*planeFactor[0]);
		}

	}
	if (FrontPlane.isVisible()) {
		if (showSprites) FrontSprites[0].draw(renderer, viewport,worldcoords*planeFactor[1]);
		drawPlane(renderer,FrontPlane, worldcoords*planeFactor[1]);
		if (showSprites) FrontSprites[1].draw(renderer, viewport,worldcoords*planeFactor[1]);
	}
}

void Level::updateVisibleSpriteLists(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport)
{
	FarSprites[0].updateVisibleSpriteList(worldcoords*planeFactor[2], viewport);
	FarSprites[1].updateVisibleSpriteList(worldcoords*planeFactor[2], viewport);
	MiddleSprites[0].updateVisibleSpriteList(worldcoords*planeFactor[4], viewport);
	MiddleSprites[1].updateVisibleSpriteList(worldcoords*planeFactor[4], viewport);
	BackSprites[0].updateVisibleSpriteList(worldcoords*planeFactor[3], viewport);
	BackSprites[1].updateVisibleSpriteList(worldcoords*planeFactor[3], viewport);
	PlayerSprites[0].updateVisibleSpriteList(worldcoords*planeFactor[0], viewport);
	PlayerSprites[1].updateVisibleSpriteList(worldcoords*planeFactor[0], viewport);
	FrontSprites[0].updateVisibleSpriteList(worldcoords*planeFactor[1], viewport);
	FrontSprites[1].updateVisibleSpriteList(worldcoords*planeFactor[1], viewport);
	HorizonSprites[0].updateVisibleSpriteList(worldcoords*planeFactor[5], viewport);
	HorizonSprites[1].updateVisibleSpriteList(worldcoords*planeFactor[5], viewport);
	objects->updateVisibleObjectList(worldcoords*planeFactor[0], viewport);
}

size_t Level::countSprites() const
{
	size_t total=0;
	total+=FarSprites[0].count();
	total+=FarSprites[1].count();
	total+=MiddleSprites[0].count();
	total+=MiddleSprites[1].count();
	total+=BackSprites[0].count();
	total+=BackSprites[1].count();
	total+=PlayerSprites[0].count();
	total+=PlayerSprites[1].count();
	total+=FrontSprites[0].count();
	total+=FrontSprites[1].count();
	total+=HorizonSprites[0].count();
	total+=HorizonSprites[1].count();
	return total;
}

size_t Level::countVisibleSprites() const
{
	size_t total=0;
	if (FarPlane.isVisible()) {
		total+=FarSprites[0].countVisible();
		total+=FarSprites[1].countVisible();
	}
	if (MiddlePlane.isVisible()) {
		total+=MiddleSprites[0].countVisible();
		total+=MiddleSprites[1].countVisible();
	}
	if (BackPlane.isVisible()) {
		total+=BackSprites[0].countVisible();
		total+=BackSprites[1].countVisible();
	}
	if (PlayerPlane.isVisible()) {
		total+=PlayerSprites[0].countVisible();
		total+=PlayerSprites[1].countVisible();
	}
	if (FrontPlane.isVisible()) {
		total+=FrontSprites[0].countVisible();
		total+=FrontSprites[1].countVisible();
	}
	if (HorizonPlane.isVisible()) {
		total+=HorizonSprites[0].countVisible();
		total+=HorizonSprites[1].countVisible();
	}
	return total;
}

bool Level::findSprite(const ppl7::grafix::Point &p, const ppl7::grafix::Point &worldcoords, SpriteSystem::Item &item, int &plane, int &layer) const
{
	if (FrontPlane.isVisible()) {
		ppl7::grafix::Point coords=p+worldcoords*planeFactor[1];
		if (FrontSprites[1].findMatchingSprite(coords, item)) {
			plane=1;
			layer=1;
			return true;
		}
		if (FrontSprites[0].findMatchingSprite(coords, item)) {
			plane=1;
			layer=0;
			return true;
		}
	}
	if (PlayerPlane.isVisible()) {
		ppl7::grafix::Point coords=p+worldcoords*planeFactor[0];
		if (PlayerSprites[1].findMatchingSprite(coords, item)) {
			plane=0;
			layer=1;
			return true;
		}
		if (PlayerSprites[0].findMatchingSprite(coords, item)) {
			plane=0;
			layer=0;
			return true;
		}
	}
	if (BackPlane.isVisible()) {
		ppl7::grafix::Point coords=p+worldcoords*planeFactor[3];
		if (BackSprites[1].findMatchingSprite(coords, item)) {
			plane=3;
			layer=1;
			return true;
		}
		if (BackSprites[0].findMatchingSprite(coords, item)) {
			plane=3;
			layer=0;
			return true;
		}
	}
	if (MiddlePlane.isVisible()) {
		ppl7::grafix::Point coords=p+worldcoords*planeFactor[4];
		if (MiddleSprites[1].findMatchingSprite(coords, item)) {
			plane=4;
			layer=1;
			return true;
		}
		if (MiddleSprites[0].findMatchingSprite(coords, item)) {
			plane=4;
			layer=0;
			return true;
		}
	}
	if (FarPlane.isVisible()) {
		ppl7::grafix::Point coords=p+worldcoords*planeFactor[2];
		if (FarSprites[1].findMatchingSprite(coords, item)) {
			plane=2;
			layer=1;
			return true;
		}
		if (FarSprites[0].findMatchingSprite(coords, item)) {
			plane=2;
			layer=0;
			return true;
		}
	}
	if (HorizonPlane.isVisible()) {
		ppl7::grafix::Point coords=p+worldcoords*planeFactor[5];
		if (HorizonSprites[1].findMatchingSprite(coords, item)) {
			plane=5;
			layer=1;
			return true;
		}
		if (HorizonSprites[0].findMatchingSprite(coords, item)) {
			plane=5;
			layer=0;
			return true;
		}
	}
	return false;
}



