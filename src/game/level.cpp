#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"
#include "particle.h"



static double planeFactor[]={ 1.0f, 1.0f, 0.5f, 1.0f, 0.8f, 0.3f, 1.3f };

Level::Level()
{
	for (int i=0;i <= MAX_TILESETS;i++) {
		tileset[i]=NULL;
	}
	for (int i=0;i <= MAX_SPRITESETS;i++) {
		spriteset[i]=NULL;
	}
	objects=new Decker::Objects::ObjectSystem(&waynet);
	particles=new ParticleSystem();
	editMode=false;
	showSprites=true;
	showObjects=true;
	showParticles=true;
}

Level::~Level()
{
	clear();
	if (objects) delete objects;
	if (particles) delete particles;
}

void Level::clear()
{
	PlayerPlane.clear();
	FarPlane.clear();
	FrontPlane.clear();
	BackPlane.clear();
	MiddlePlane.clear();
	HorizonPlane.clear();
	NearPlane.clear();
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
	NearSprites[0].clear();
	NearSprites[1].clear();
	objects->clear();
	particles->clear();
	waynet.clear();
	params.clear();
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

void Level::setShowParticles(bool enabled)
{
	showParticles=enabled;
}

void Level::setTileset(int no, SpriteTexture* tileset)
{
	//printf ("Level::setTileset no=%d\n",no);
	if (no<0 || no>MAX_TILESETS) return;
	this->tileset[no]=tileset;
}

void Level::setSpriteset(int no, SpriteTexture* spriteset)
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
	NearSprites[0].setSpriteset(no, spriteset);
	NearSprites[1].setSpriteset(no, spriteset);

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
	NearPlane.create(width * 2, height * 2);
	palette.setDefaults();
}

void Level::setViewport(const ppl7::grafix::Rect& r)
{
	viewport=r;
}

Plane& Level::plane(int id)
{
	if (id == 0) return PlayerPlane;
	if (id == 1) return FrontPlane;
	if (id == 2) return FarPlane;
	if (id == 3) return BackPlane;
	if (id == 4) return MiddlePlane;
	if (id == 5) return HorizonPlane;
	if (id == 6) return NearPlane;
	return PlayerPlane;

}

SpriteSystem& Level::spritesystem(int plane, int layer)
{
	if (plane == 0) return PlayerSprites[layer];
	if (plane == 1) return FrontSprites[layer];
	if (plane == 2) return FarSprites[layer];
	if (plane == 3) return BackSprites[layer];
	if (plane == 4) return MiddleSprites[layer];
	if (plane == 5) return HorizonSprites[layer];
	if (plane == 6) return NearSprites[layer];

	return PlayerSprites[layer];
}


void Level::load(const ppl7::String& Filename)
{
	clear();
	ppl7::File ff;
	ff.open(Filename, ppl7::File::READ);
	ppl7::ByteArray ba;
	ff.read(ba, 7);
	const char* buffer=ba.toCharPtr();
	if (memcmp(buffer, "Decker", 7) != 0) {
		printf("Invalid Fileformat\n");
		return;
	}
	while (!ff.eof()) {
		try {
			size_t bytes_read=ff.read(ba, 5);
			if (bytes_read != 5) break;
			buffer=ba.toCharPtr();
			size_t size=ppl7::Peek32(buffer);
			int id=ppl7::Peek8(buffer + 4);
			//printf ("load id=%d, size=%zd\n",id,size);
			if (size <= 5) continue;
			bytes_read=ff.read(ba, size - 5);
			if (bytes_read != size - 5) break;
			if (id == LevelChunkId::chunkLevelParameter) {
				params.load(ba);
			} else if (id == LevelChunkId::chunkColorPalette) {
				palette.load(ba);
			} else if (id == LevelChunkId::chunkPlayerPlane) {
				PlayerPlane.load(ba);
			} else if (id == LevelChunkId::chunkFrontPlane) {
				FrontPlane.load(ba);
			} else if (id == LevelChunkId::chunkFarPlane) {
				FarPlane.load(ba);
			} else if (id == LevelChunkId::chunkBackPlane) {
				BackPlane.load(ba);
			} else if (id == LevelChunkId::chunkMiddlePlane) {
				MiddlePlane.load(ba);
			} else if (id == LevelChunkId::chunkHorizonPlane) {
				HorizonPlane.load(ba);
			} else if (id == LevelChunkId::chunkNearPlane) {
				NearPlane.load(ba);
			} else if (id == LevelChunkId::chunkPlayerSpritesLayer0) {
				PlayerSprites[0].load(ba);
			} else if (id == LevelChunkId::chunkPlayerSpritesLayer1) {
				PlayerSprites[1].load(ba);
			} else if (id == LevelChunkId::chunkFrontSpritesLayer0) {
				FrontSprites[0].load(ba);
			} else if (id == LevelChunkId::chunkFrontSpritesLayer1) {
				FrontSprites[1].load(ba);
			} else if (id == LevelChunkId::chunkFarSpritesLayer0) {
				FarSprites[0].load(ba);
			} else if (id == LevelChunkId::chunkFarSpritesLayer1) {
				FarSprites[1].load(ba);
			} else if (id == LevelChunkId::chunkBackSpritesLayer0) {
				BackSprites[0].load(ba);
			} else if (id == LevelChunkId::chunkBackSpritesLayer1) {
				BackSprites[1].load(ba);
			} else if (id == LevelChunkId::chunkMiddleSpritesLayer0) {
				MiddleSprites[0].load(ba);
			} else if (id == LevelChunkId::chunkMiddleSpritesLayer1) {
				MiddleSprites[1].load(ba);
			} else if (id == LevelChunkId::chunkHorizonSpritesLayer0) {
				HorizonSprites[0].load(ba);
			} else if (id == LevelChunkId::chunkHorizonSpritesLayer1) {
				HorizonSprites[1].load(ba);
			} else if (id == LevelChunkId::chunkNearSpritesLayer0) {
				NearSprites[0].load(ba);
			} else if (id == LevelChunkId::chunkNearSpritesLayer1) {
				NearSprites[1].load(ba);
			} else if (id == LevelChunkId::chunkTileTypes) {
				TileTypeMatrix.load(ba);
			} else if (id == LevelChunkId::chunkObjects) {
				objects->load(ba);
			} else if (id == LevelChunkId::chunkWayNet) {
				waynet.load(ba);
			}
		}
		catch (const ppl7::EndOfFileException&) {
			break;
		}
	}
	//printf("Player plane is: %d x %d\n", PlayerPlane.getSize().width, PlayerPlane.getSize().height);
	if (PlayerPlane.getSize().isEmpty()) PlayerPlane.create(512, 256);
	if (HorizonPlane.getSize().isEmpty()) HorizonPlane.create(PlayerPlane.getSize().width, PlayerPlane.getSize().height);
	if (MiddlePlane.getSize().isEmpty()) MiddlePlane.create(PlayerPlane.getSize().width, PlayerPlane.getSize().height);
	if (FarPlane.getSize().isEmpty()) FarPlane.create(PlayerPlane.getSize().width, PlayerPlane.getSize().height);
	if (FrontPlane.getSize().isEmpty()) FrontPlane.create(PlayerPlane.getSize().width, PlayerPlane.getSize().height);
	if (BackPlane.getSize().isEmpty()) FrontPlane.create(PlayerPlane.getSize().width, PlayerPlane.getSize().height);
	if (NearPlane.getSize().isEmpty()) NearPlane.create(PlayerPlane.getSize().width * 2, PlayerPlane.getSize().height * 2);
	ff.close();

	if (PlayerPlane.getSize().width != params.width) params.width=PlayerPlane.getSize().width;
	if (PlayerPlane.getSize().height != params.height) params.height=PlayerPlane.getSize().height;
	/*
	// For performance-testing of sprite system
	for (int i=0;i<1000000;i++) {
		PlayerSprites[0].addSprite(ppl7::rand(0,50000), ppl7::rand(1000,50000), 0, 1, ppl7::rand(0,52*4), 1.0f);
	}
	*/
}

void Level::save(const ppl7::String& Filename)
{
	ppl7::File ff;
	ff.open(Filename, ppl7::File::WRITE);
	char* buffer[20];
	memcpy(buffer, "Decker", 7);
	ff.write(buffer, 7);
	params.save(ff, LevelChunkId::chunkLevelParameter);
	palette.save(ff, LevelChunkId::chunkColorPalette);
	PlayerPlane.save(ff, LevelChunkId::chunkPlayerPlane);
	FrontPlane.save(ff, LevelChunkId::chunkFrontPlane);
	FarPlane.save(ff, LevelChunkId::chunkFarPlane);
	BackPlane.save(ff, LevelChunkId::chunkBackPlane);
	MiddlePlane.save(ff, LevelChunkId::chunkMiddlePlane);
	HorizonPlane.save(ff, LevelChunkId::chunkHorizonPlane);
	NearPlane.save(ff, LevelChunkId::chunkNearPlane);
	TileTypeMatrix.save(ff, LevelChunkId::chunkTileTypes);
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
	NearSprites[0].save(ff, LevelChunkId::chunkNearSpritesLayer0);
	NearSprites[1].save(ff, LevelChunkId::chunkNearSpritesLayer1);
	objects->save(ff, LevelChunkId::chunkObjects);
	waynet.save(ff, LevelChunkId::chunkWayNet);
	ff.close();

}

void Level::drawPlane(SDL_Renderer* renderer, const Plane& plane, const ppl7::grafix::Point& worldcoords) const
{
	//printf("viewport: x=%d, y=%d\n",viewport.x1, viewport.y1);
	int tiles_width=viewport.width() / TILE_WIDTH + 9;
	int tiles_height=viewport.height() / TILE_HEIGHT + 2;
	int offset_x=worldcoords.x % TILE_WIDTH;
	int offset_y=worldcoords.y % TILE_HEIGHT;
	int start_x=worldcoords.x / TILE_WIDTH - 7;
	int start_y=worldcoords.y / TILE_HEIGHT;
	int x1=viewport.x1 - offset_x - TILE_WIDTH * 7;
	int y1=viewport.y1 - offset_y + TILE_HEIGHT;

	std::map<int, ColorPaletteItem>::const_iterator cit;

	for (int z=0;z < MAX_TILE_LAYER;z++) {
		for (int y=tiles_height;y >= 0;y--) {
			for (int x=0;x < tiles_width;x++) {
				const Tile* tile=plane.get(x + start_x, y + start_y);
				if (tile) {
					//if (tile->layer[z].tileset>8) printf ("draw %d, %d\n",tile->layer[z].tileset, tile->layer[z].tileno);
					if (tile->layer[z].tileset <= MAX_TILESETS && tileset[tile->layer[z].tileset]) {
						//printf ("%d = %zd\n,",tile->tileset[z], tileset[tile->tileset[z]]);
						if (tile->layer[z].tileset > 1) {
							tileset[2]->draw(renderer, x1 + x * TILE_WIDTH, y1 + y * TILE_HEIGHT, tile->layer[z].tileno, palette.getColor(tile->layer[z].color_index));
						} else {
							tileset[tile->layer[z].tileset]->draw(renderer, x1 + x * TILE_WIDTH, y1 + y * TILE_HEIGHT, tile->layer[z].tileno);
						}
					}
				}
			}
		}
	}
}

void Level::drawNonePlayerPlane(SDL_Renderer* renderer, const Plane& plane, const SpriteSystem& sprites1, const SpriteSystem& sprites2, const ppl7::grafix::Point& worldcoords, Metrics& metrics)
{
	if (!plane.isVisible()) return;
	if (showSprites) {
		metrics.time_sprites.start();
		sprites1.draw(renderer, viewport, worldcoords);
		metrics.time_sprites.stop();
	}
	metrics.time_plane.start();
	drawPlane(renderer, plane, worldcoords);
	metrics.time_plane.stop();
	if (showSprites) {
		metrics.time_sprites.start();
		sprites2.draw(renderer, viewport, worldcoords);
		metrics.time_sprites.stop();
	}
}

void Level::drawParticles(SDL_Renderer* renderer, Particle::Layer layer, const ppl7::grafix::Point& worldcoords, Metrics& metrics)
{
	if (!showParticles) return;
	metrics.time_draw_particles.start();
	particles->draw(renderer, viewport, worldcoords, layer);
	metrics.time_draw_particles.stop();
}


void Level::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& worldcoords, Player* player, Metrics& metrics)
{
	drawNonePlayerPlane(renderer, HorizonPlane, HorizonSprites[0], HorizonSprites[1], worldcoords * planeFactor[5], metrics);
	drawNonePlayerPlane(renderer, FarPlane, FarSprites[0], FarSprites[1], worldcoords * planeFactor[2], metrics);
	drawNonePlayerPlane(renderer, MiddlePlane, MiddleSprites[0], MiddleSprites[1], worldcoords * planeFactor[4], metrics);
	drawParticles(renderer, Particle::Layer::BackplaneBack, worldcoords * planeFactor[3], metrics);
	drawNonePlayerPlane(renderer, BackPlane, BackSprites[0], BackSprites[1], worldcoords * planeFactor[3], metrics);
	drawParticles(renderer, Particle::Layer::BackplaneFront, worldcoords * planeFactor[3], metrics);

	if (PlayerPlane.isVisible()) {
		if (showSprites) {
			metrics.time_sprites.start();
			PlayerSprites[0].draw(renderer, viewport, worldcoords * planeFactor[0]);
			metrics.time_sprites.stop();
		}
		if (showObjects) {	// Objects behind Bricks
			metrics.time_objects.start();
			if (editMode)
				objects->drawEditMode(renderer, viewport, worldcoords * planeFactor[0], Decker::Objects::Object::Layer::BehindBricks);
			else
				objects->draw(renderer, viewport, worldcoords * planeFactor[0], Decker::Objects::Object::Layer::BehindBricks);
			metrics.time_objects.stop();
		}
		drawParticles(renderer, Particle::Layer::BehindBricks, worldcoords * planeFactor[0], metrics);
		metrics.time_plane.start();
		drawPlane(renderer, PlayerPlane, worldcoords * planeFactor[0]);
		metrics.time_plane.stop();
		if (showSprites) {
			metrics.time_sprites.start();
			PlayerSprites[1].draw(renderer, viewport, worldcoords * planeFactor[0]);
			metrics.time_sprites.stop();
		}

		// Behind Player
		drawParticles(renderer, Particle::Layer::BehindPlayer, worldcoords * planeFactor[0], metrics);
		metrics.time_objects.start();
		if (showObjects) {	// Objects behind Player
			if (editMode)
				objects->drawEditMode(renderer, viewport, worldcoords * planeFactor[0], Decker::Objects::Object::Layer::BehindPlayer);
			else
				objects->draw(renderer, viewport, worldcoords * planeFactor[0], Decker::Objects::Object::Layer::BehindPlayer);
		}
		// Player
		player->draw(renderer, viewport, worldcoords * planeFactor[0]);
		if (showObjects) {	// Objects before Player
			if (editMode)
				objects->drawEditMode(renderer, viewport, worldcoords * planeFactor[0], Decker::Objects::Object::Layer::BeforePlayer);
			else
				objects->draw(renderer, viewport, worldcoords * planeFactor[0], Decker::Objects::Object::Layer::BeforePlayer);
		}
		metrics.time_objects.stop();
		drawParticles(renderer, Particle::Layer::BeforePlayer, worldcoords * planeFactor[0], metrics);

	}
	drawParticles(renderer, Particle::Layer::FrontplaneBack, worldcoords * planeFactor[1], metrics);
	drawNonePlayerPlane(renderer, FrontPlane, FrontSprites[0], FrontSprites[1], worldcoords * planeFactor[1], metrics);
	drawParticles(renderer, Particle::Layer::FrontplaneFront, worldcoords * planeFactor[1], metrics);
	drawNonePlayerPlane(renderer, NearPlane, NearSprites[0], NearSprites[1], worldcoords * planeFactor[6], metrics);
}

void Level::updateVisibleSpriteLists(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
	FarSprites[0].updateVisibleSpriteList(worldcoords * planeFactor[2], viewport);
	FarSprites[1].updateVisibleSpriteList(worldcoords * planeFactor[2], viewport);
	MiddleSprites[0].updateVisibleSpriteList(worldcoords * planeFactor[4], viewport);
	MiddleSprites[1].updateVisibleSpriteList(worldcoords * planeFactor[4], viewport);
	BackSprites[0].updateVisibleSpriteList(worldcoords * planeFactor[3], viewport);
	BackSprites[1].updateVisibleSpriteList(worldcoords * planeFactor[3], viewport);
	PlayerSprites[0].updateVisibleSpriteList(worldcoords * planeFactor[0], viewport);
	PlayerSprites[1].updateVisibleSpriteList(worldcoords * planeFactor[0], viewport);
	FrontSprites[0].updateVisibleSpriteList(worldcoords * planeFactor[1], viewport);
	FrontSprites[1].updateVisibleSpriteList(worldcoords * planeFactor[1], viewport);
	HorizonSprites[0].updateVisibleSpriteList(worldcoords * planeFactor[5], viewport);
	HorizonSprites[1].updateVisibleSpriteList(worldcoords * planeFactor[5], viewport);
	NearSprites[0].updateVisibleSpriteList(worldcoords * planeFactor[6], viewport);
	NearSprites[1].updateVisibleSpriteList(worldcoords * planeFactor[6], viewport);
	//objects->updateVisibleObjectList(worldcoords * planeFactor[0], viewport);
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
	total+=NearSprites[0].count();
	total+=NearSprites[1].count();
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
	if (NearPlane.isVisible()) {
		total+=NearSprites[0].countVisible();
		total+=NearSprites[1].countVisible();
	}

	return total;
}

bool Level::findSprite(const ppl7::grafix::Point& p, const ppl7::grafix::Point& worldcoords, SpriteSystem::Item& item, int& plane, int& layer) const
{
	if (NearPlane.isVisible()) {
		ppl7::grafix::Point coords=p + worldcoords * planeFactor[6];
		if (NearSprites[1].findMatchingSprite(coords, item)) {
			plane=6;
			layer=1;
			return true;
		}
		if (NearSprites[0].findMatchingSprite(coords, item)) {
			plane=6;
			layer=0;
			return true;
		}
	}
	if (FrontPlane.isVisible()) {
		ppl7::grafix::Point coords=p + worldcoords * planeFactor[1];
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
		ppl7::grafix::Point coords=p + worldcoords * planeFactor[0];
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
		ppl7::grafix::Point coords=p + worldcoords * planeFactor[3];
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
		ppl7::grafix::Point coords=p + worldcoords * planeFactor[4];
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
		ppl7::grafix::Point coords=p + worldcoords * planeFactor[2];
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
		ppl7::grafix::Point coords=p + worldcoords * planeFactor[5];
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

size_t Level::tileCount() const
{
	size_t count=FarPlane.tileCount();
	count+=PlayerPlane.tileCount();
	count+=FrontPlane.tileCount();
	count+=BackPlane.tileCount();
	count+=MiddlePlane.tileCount();
	count+=HorizonPlane.tileCount();
	count+=NearPlane.tileCount();
	return count;
}

ppl7::grafix::Rect Level::getOccupiedArea() const
{
	ppl7::grafix::Rect r;

	return r;

}

ppl7::grafix::Rect Level::getOccupiedAreaFromTileTypePlane() const
{
	return TileTypeMatrix.getOccupiedArea();
}




LevelParameter::LevelParameter()
{
	clear();
}

void LevelParameter::clear()
{
	width=0;
	height=0;
	randomSong=true;
	backgroundType=Background::Type::Color;
	BackgroundColor.setColor(32, 32, 64, 255);
	Name.clear();
	InitialSong.clear();
	SongPlaylist.clear();
	BackgroundImage.clear();
}


static void storeParameters(ppl7::AssocArray& a, const LevelParameter& params)
{
	a.clear();
	a.setf("level_width", "%d", params.width);
	a.setf("level_height", "%d", params.height);
	a.set("level_name", params.Name);
	a.set("initial_song", params.InitialSong);
	if (params.randomSong) a.set("randomSong", "true");
	else a.set("randomSong", "false");
	std::vector<ppl7::String>::const_iterator it;
	for (it=params.SongPlaylist.begin();it != params.SongPlaylist.end();++it) {
		a.set("additional_playlist/[]", (*it));
	}
	if (params.backgroundType == Background::Type::Image)
		a.set("background_type", "image");
	else if (params.backgroundType == Background::Type::Color)
		a.set("background_type", "color");

	a.set("BackgroundImage", params.BackgroundImage);
	a.setf("BackgroundColor", "%d,%d,%d,%d", params.BackgroundColor.red(),
		params.BackgroundColor.green(),
		params.BackgroundColor.blue(),
		params.BackgroundColor.alpha()
	);
	//a.list();

}

size_t LevelParameter::size() const
{
	ppl7::AssocArray a;
	storeParameters(a, *this);
	return a.size();
}

void LevelParameter::save(ppl7::File& ff, int chunk_id) const
{
	ppl7::AssocArray a;
	storeParameters(a, *this);
	ppl7::ByteArray ba;
	a.exportBinary(ba);
	unsigned char buffer[5];
	ppl7::Poke32(buffer + 0, ba.size() + 5);
	ppl7::Poke8(buffer + 4, chunk_id);
	ff.write(buffer, 5);
	ff.write(ba.ptr(), ba.size());
}

void LevelParameter::load(const ppl7::ByteArrayPtr& ba)
{
	clear();
	ppl7::AssocArray a;
	ppl7::String Default, Tmp;
	a.importBinary(ba);
	Default="";
	width=a.getInt("level_width", width);
	height=a.getInt("level_height", height);
	Name=a.getString("level_name", Default);
	InitialSong=a.getString("initial_song", Default);
	BackgroundImage=a.getString("BackgroundImage", Default);
	Default="true";
	randomSong=a.getString("randomSong", Default).toBool();
	Default="color";
	Tmp=a.getString("background_type", Default);
	if (Tmp == "color") backgroundType=Background::Type::Color;
	else if (Tmp == "image") backgroundType=Background::Type::Image;
	if (a.exists("BackgroundColor")) {
		Tmp=a.getString("BackgroundColor");
		ppl7::Array Tok(Tmp, ",");
		BackgroundColor.setRed(Tok[0].toInt());
		BackgroundColor.setGreen(Tok[1].toInt());
		BackgroundColor.setBlue(Tok[2].toInt());
		BackgroundColor.setAlpha(Tok[3].toInt());
	}
	if (a.exists("additional_playlist")) {
		ppl7::AssocArray& songlist=a.getAssocArray("additional_playlist");
		ppl7::AssocArray::const_iterator it;
		for (it=songlist.begin();it != songlist.end();++it) {
			SongPlaylist.push_back((*it).second->toString());
		}
	}

	//a.list("level::parameter::load");
}
