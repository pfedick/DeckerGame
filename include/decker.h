#ifndef INCLUDE_DECKER_H_
#define INCLUDE_DECKER_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "ui.h"
#include <map>


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
	//SDL_Window *window;
	SDL_Renderer *renderer;

	bool screensaver_enabled;



public:
	SDL();
	~SDL();
	/*
	void createWindow();
	void destroyWindow();
	void loop();
	*/
	void setRenderer(SDL_Renderer *r);
	ppl7::grafix::Drawable lockTexture(SDL_Texture *texture);
	void unlockTexture(SDL_Texture *texture);
	SDL_Texture *createTexture(const ppl7::grafix::Drawable &d);
	SDL_Texture *createStreamingTexture(int width, int height);
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
			SDL_Rect r;
			ppl7::grafix::Point Pivot;
			ppl7::grafix::Point Offset;

			SpriteIndexItem()
			{
				id=0;
				tex=NULL;
			}
			SpriteIndexItem(const SpriteIndexItem &other)
				:r(other.r), Pivot(other.Pivot), Offset(other.Offset)
			{
				id=other.id;
				tex=other.tex;
			}
		};
		std::map<int,SDL_Texture*> TextureMap;
		std::map<int,SpriteIndexItem> SpriteList;

		void loadTexture(SDL &sdl, ppl7::PFPChunk *chunk, const ppl7::grafix::Color &tint);
		void loadIndex(ppl7::PFPChunk *chunk);

	public:
		Sprite();
		~Sprite();
		void load(SDL &sdl, const ppl7::String &filename, const ppl7::grafix::Color &tint=ppl7::grafix::Color());
		void load(SDL &sdl, ppl7::FileObject &ff, const ppl7::grafix::Color &tint=ppl7::grafix::Color());
		void clear();
		void draw(SDL_Renderer *renderer, int x, int y, int id) const;
		int numTextures() const;
		int numSprites() const;
		SDL_Texture *findTexture(int id) const;
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
	Tile(int tileset, int tileno, int z=0, TileType type=Blocking);
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

	ppl7::grafix::Sprite uiTiles;
	ppl7::grafix::Sprite uiTilesNature;
	ppl7::grafix::Sprite uiTileTypes;

};


class Level
{
	friend class Game;
private:
	Plane FarPlane;
	Plane PlayerPlane;
	Plane FrontPlane;
	ppl7::grafix::Rect viewport;
	Sprite *tileset[10];
	Sprite *tiletypes;

	void clear();

public:
	Level();
	~Level();
	void setTileset(int no, Sprite *tileset);
	void setTileTypesSprites(Sprite *sprites);
	void create(int width, int height);
	void load(const ppl7::String &Filename);
	void save(const ppl7::String &Filename);
	void drawPlane(SDL_Renderer *renderer, const Plane &plane, const ppl7::grafix::Point &worldcoords) const;
	void drawTileTypes(SDL_Renderer *renderer, const ppl7::grafix::Point &worldcoords) const;
	void setViewport(const ppl7::grafix::Rect &r);
	Plane &plane(int id);
};


class Player;

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


	FPS fps;

	ppl7::grafix::Point WorldCoords;
	ppl7::grafix::Point WorldMoveStart;
	bool worldIsMoving;

	bool quitGame;

	void createWindow();
	void initUi();
	void updateUi(const ppl7::tk::MouseState &mouse);
	void drawGrid();
	void moveWorld(int offset_x, int offset_y);
	void moveWorldOnMouseClick(const ppl7::tk::MouseState &mouse);
	void handleMouseDrawInWorld(const ppl7::tk::MouseState &mouse);
	ppl7::grafix::Point PlayerCoords;

	Player *player;

public:
	Game();
	~Game();
	void init();
	void run();

	// EventHandler
	void quitEvent(ppl7::tk::Event *event);
	void closeEvent(ppl7::tk::Event *event);

	void showTilesSelection();

	SDL_Renderer *getSDLRenderer();

	void save();
	void load();



};




#endif /* INCLUDE_DECKER_H_ */
