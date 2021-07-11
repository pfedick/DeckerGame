#ifndef INCLUDE_DECKER_H_
#define INCLUDE_DECKER_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "ui.h"
#include <map>

#ifndef MAX_TILESETS
#define MAX_TILESETS 10
#endif


#ifndef MAX_SPRITESETS
#define MAX_SPRITESETS 10
#endif

#define STR_VALUE(arg)      #arg
#define EXCEPTION(name,inherit)	class name : public inherit { public: \
	name() throw() {}; \
	name(const char *msg, ...) throw() {  \
		va_list args; va_start(args, msg); copyText(msg,args); \
		va_end(args); } \
		virtual const char* what() const throw() { return (STR_VALUE(name)); } \
	};

EXCEPTION(InitializationFailed, ppl7::Exception);
EXCEPTION(SDLException, ppl7::Exception);

#define TILE_WIDTH 64
#define TILE_HEIGHT 64

class FPS
{
private:
	ppl7::ppl_time_t fps_start_time;
	int fps_frame_count;
	int fps;
public:
	FPS();
	int getFPS() const;
	void update();
};

class SDL
{
private:
	SDL_Renderer *renderer;
	bool screensaver_enabled;



public:
	SDL();
	~SDL();
	void setRenderer(SDL_Renderer *r);
	ppl7::grafix::Drawable lockTexture(SDL_Texture *texture);
	void unlockTexture(SDL_Texture *texture);
	SDL_Texture *createTexture(const ppl7::grafix::Drawable &d);
	SDL_Texture *createStreamingTexture(int width, int height);
	SDL_Texture *createRenderTargetTexture(int width, int height);
	void destroyTexture(SDL_Texture *texture);
	void startFrame(const ppl7::grafix::Color &background);
	SDL_Renderer *getRenderer();
	void present();

	ppl7::grafix::Size getWindowSize() const;
	ppl7::grafix::Rect getClientWindow() const;
};


class Sprite
{
private:
	class SpriteIndexItem
	{
	public:
		int id;
		SDL_Texture *tex;
		SDL_Texture *outlines;
		const ppl7::grafix::Drawable *drawable;
		SDL_Rect r;
		ppl7::grafix::Point Pivot;
		ppl7::grafix::Point Offset;

		SpriteIndexItem()
		{
			id=0;
			tex=outlines=NULL;
			drawable=NULL;
		}
		SpriteIndexItem(const SpriteIndexItem &other)
		:r(other.r), Pivot(other.Pivot), Offset(other.Offset)
		{
			id=other.id;
			tex=other.tex;
			outlines=other.outlines;
			drawable=other.drawable;
		}
	};
	std::map<int,SDL_Texture*> TextureMap;
	std::map<int,SDL_Texture*> OutlinesTextureMap;
	std::map<int,ppl7::grafix::Image> InMemoryTextureMap;
	std::map<int,SpriteIndexItem> SpriteList;

	bool bSDLBufferd;
	bool bMemoryBufferd;
	bool bOutlinesEnabled;
	bool bCollisionDetectionEnabled;

	void loadTexture(SDL &sdl, ppl7::PFPChunk *chunk, const ppl7::grafix::Color &tint);
	void loadIndex(ppl7::PFPChunk *chunk);
	void generateOutlines(SDL &sdl, int id, const ppl7::grafix::Image &src);
	SDL_Texture *findTexture(int id) const;
	SDL_Texture *findOutlines(int id) const;
	const ppl7::grafix::Drawable *findInMemoryTexture(int id) const;

public:
	Sprite();
	~Sprite();
	void load(SDL &sdl, const ppl7::String &filename, const ppl7::grafix::Color &tint=ppl7::grafix::Color());
	void load(SDL &sdl, ppl7::FileObject &ff, const ppl7::grafix::Color &tint=ppl7::grafix::Color());
	void clear();
	void draw(ppl7::grafix::Drawable &target, int x, int y, int id) const;
	void draw(SDL_Renderer *renderer, int x, int y, int id) const;
	void drawScaled(SDL_Renderer *renderer, int x, int y, int id, float scale_factor) const;
	void drawOutlines(SDL_Renderer *renderer, int x, int y, int id, float scale_factor) const;
	ppl7::grafix::Size spriteSize(int id, float scale_factor) const;
	ppl7::grafix::Rect spriteBoundary(int id, float scale_factor, int x, int y) const;
	const ppl7::grafix::Drawable getDrawable(int id) const;
	void enableMemoryBuffer(bool enabled);
	void enableSDLBuffer(bool enabled);
	void enableCollisionDetection(bool enabled);
	void enableOutlines(bool enabled);
	int numTextures() const;
	int numSprites() const;
};

class SpriteSystem
{
public:
	class Item
	{
	public:
		int id;
		int x;			// 4 Byte
		int y;			// 4 Byte
		int z;			// 2 Byte
		int sprite_set;	// 2 Byte
		int sprite_no;	// 2 Byte
		float scale;	// 4 Byte	==> 18 Byte
		ppl7::grafix::Rect boundary;
	};
private:
	ppl7::Mutex mutex;
	std::map<int, SpriteSystem::Item> sprite_list;
	std::map<uint64_t,SpriteSystem::Item> visible_sprite_map;
	Sprite *spriteset[MAX_SPRITESETS+1];
	bool bSpritesVisible;

public:
	SpriteSystem();
	~SpriteSystem();
	void clear();
	void addSprite(int x, int y, int z, int spriteset, int sprite_no, float sprite_scale);
	void setVisible(bool visible);
	bool isVisible() const;
	void setSpriteset(int no, Sprite *spriteset);
	void updateVisibleSpriteList(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport);
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void save(ppl7::FileObject &file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr &ba);
	SpriteSystem::Item findMatchingSprite(const ppl7::grafix::Point &p) const;

	void drawSelectedSpriteOutline(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords, int id);

	size_t count() const;
	size_t countVisible() const;
};

class Tile
{
public:
	enum TileType {
		NonBlocking,
		Blocking,
		SteepRampLeft,
		SteepRampRight,
		ShallowRampLeftLower,
		ShallowRampLeftUpper,
		ShallowRampRightUpper,
		ShallowRampRightLower,
		Ladder,
		Water
	};
	TileType type;
	int tileset[3];
	int tileno[3];

	Tile(TileType type=NonBlocking);
	Tile(int tileset, int tileno, int z=0, TileType type=NonBlocking);
	void setType(TileType type);
	void set(int tileset, int tileno, int z=0);
	int getTileset(int z);
	int getTileNo(int z);
};

class Plane
{
private:
	Tile **tilematrix;
	int width, height;
	//SpriteSystem spritessystem[2];
	bool bTilesVisible;

public:
	Plane();
	~Plane();
	void clear();
	void create(int width, int height);
	void setTile(int x, int y, int z, int tileset, int tileno);
	void setType(int x, int y,  Tile::TileType type);
	void clearTile(int x, int y, int z);
	const Tile *get(int x, int y) const;
	void save(ppl7::FileObject &file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr &ba);
	void setVisible(bool visible);
	bool isVisible() const;
};


class Resources
{
public:
	Sprite Sprite_George;
	Sprite Nature;
	Sprite Trees;
	Sprite Tiles;
	Sprite Tiles_Nature;
	Sprite Cursor;
	Sprite TileTypes;
	Sprite Sprites_Nature;

	//ppl7::grafix::Sprite uiTiles;
	//ppl7::grafix::Sprite uiTilesNature;
	//ppl7::grafix::Sprite uiTileTypes;
	Sprite uiSpritesNature;

};
class Player;

class Level
{
	friend class Game;
private:
	Plane FarPlane;
	Plane PlayerPlane;
	Plane FrontPlane;
	SpriteSystem FarSprites[2];
	SpriteSystem PlayerSprites[2];
	SpriteSystem FrontSprites[2];

	ppl7::grafix::Rect viewport;
	Sprite *tileset[MAX_TILESETS+1];
	Sprite *spriteset[MAX_SPRITESETS+1];
	Sprite *tiletypes;
	void clear();

	enum LevelChunkId {
		chunkPlayerPlane=1,
		chunkFrontPlane=2,
		chunkFarPlane=3,
		chunkPlayerSpritesLayer0=10,
		chunkPlayerSpritesLayer1=11,
		chunkFrontSpritesLayer0=12,
		chunkFrontSpritesLayer1=13,
		chunkFarSpritesLayer0=14,
		chunkFarSpritesLayer1=15,
	};

public:
	Level();
	~Level();
	void setTileset(int no, Sprite *tileset);
	void setTileTypesSprites(Sprite *sprites);
	void setSpriteset(int no, Sprite *spriteset);
	void create(int width, int height);
	void load(const ppl7::String &Filename);
	void save(const ppl7::String &Filename);
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Point &worldcoords, Player *player);
	void drawPlane(SDL_Renderer *renderer, const Plane &plane, const ppl7::grafix::Point &worldcoords) const;
	void drawTileTypes(SDL_Renderer *renderer, const ppl7::grafix::Point &worldcoords) const;
	void setViewport(const ppl7::grafix::Rect &r);
	Plane &plane(int id);
	SpriteSystem &spritesystem(int plane, int layer);
	void updateVisibleSpriteLists(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport);

	size_t countSprites() const;
	size_t countVisibleSprites() const;

};



class Game : private ppl7::tk::Window
{
private:
	SDL sdl;
	ppl7::tk::WindowManager *wm;
	ppl7::tk::WidgetStyle Style;
	//ppl7::tk::Window window;
	void loadGrafix();
	Resources resources;
	Level level;
	SDL_Texture* tex_level_grid;
	SDL_Texture* tex_sky;
	ppl7::grafix::Size desktopSize;
	ppl7::grafix::Font gui_font;
	ppl7::grafix::Rect viewport;
	Decker::ui::MainMenue *mainmenue;
	Decker::ui::StatusBar *statusbar;
	Decker::ui::TilesSelection *tiles_selection;
	Decker::ui::TileTypeSelection *tiletype_selection;
	Decker::ui::SpriteSelection *sprite_selection;
	Decker::ui::WorldWidget *world_widget;

	FPS fps;

	ppl7::grafix::Point WorldCoords;
	ppl7::grafix::Point WorldMoveStart;
	bool worldIsMoving;

	bool quitGame;

	void createWindow();
	void initUi();
	void updateUi(const ppl7::tk::MouseState &mouse);
	void drawGrid();
	void drawSelectedSprite(SDL_Renderer *renderer, const ppl7::grafix::Point &mouse);
	void moveWorld(int offset_x, int offset_y);
	void moveWorldOnMouseClick(const ppl7::tk::MouseState &mouse);
	void handleMouseDrawInWorld(const ppl7::tk::MouseState &mouse);
	void closeTileTypeSelection();
	void closeTileSelection();
	void closeSpriteSelection();
	void selectSprite(const ppl7::grafix::Point &mouse);

	ppl7::grafix::Point PlayerCoords;

	Player *player;

	enum spriteMode {
		spriteModeDraw,
		SpriteModeEdit
	};
	spriteMode sprite_mode;
	int selected_sprite_id;
	SpriteSystem *selected_sprite_system;

public:
	Game();
	~Game();
	void init();
	void run();

	// EventHandler
	void quitEvent(ppl7::tk::Event *event);
	void closeEvent(ppl7::tk::Event *event);
	void mouseClickEvent(ppl7::tk::MouseEvent *event);
	void mouseDownEvent(ppl7::tk::MouseEvent *event);
	void mouseWheelEvent(ppl7::tk::MouseEvent *event);

	void showTilesSelection();
	void showTileTypeSelection();
	void showSpriteSelection();

	SDL_Renderer *getSDLRenderer();

	void save();
	void load();



};




#endif /* INCLUDE_DECKER_H_ */
