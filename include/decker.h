/*
 * decker.h
 *
 *  Created on: 03.10.2017
 *      Author: patrick
 */

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
	void startFrame();
	SDL_Renderer *getRenderer();
	void present();

	ppl7::grafix::Size getWindowSize() const;
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
			//ppl7::grafix::Rect r;
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
		//ppl7::AVLTree<int,SpriteTexture> TextureList;
		ppl7::AVLTree<int,SpriteIndexItem> SpriteList;

		void loadTexture(SDL &sdl, ppl7::PFPChunk *chunk);
		void loadIndex(ppl7::PFPChunk *chunk);

	public:
		Sprite();
		~Sprite();
		void load(SDL &sdl, const ppl7::String &filename);
		void load(SDL &sdl, ppl7::FileObject &ff);
		void clear();
		void draw(SDL_Renderer *renderer, int x, int y, int id) const;
		int numTextures() const;
		int numSprites() const;
		SDL_Texture *findTexture(int id) const;
};

class Tile
{

};


class Level
{
private:
	SDL_Renderer *renderer;
	Tile ****tiles;
	int width, height, layers;

	void clear();

public:
	Level();
	~Level();
	void create(int width, int height, int layers);
	void load(const ppl7::String &Filename);
	void save(const ppl7::String &Filename);
	void setRenderer(SDL_Renderer *renderer);
	void setTileSize(int width, int height);
	void draw(int layer, int x, int y);

};


class Game : private ppl7::tk::Window
{
private:
	SDL sdl;
	ppl7::tk::WindowManager *wm;
	//ppl7::tk::Window window;
	void loadGrafix();
	Sprite Sprite_Charlie;
	Sprite Bricks;
	Sprite Cursor;
	Level level;
	SDL_Texture* tex_level_grid;
	ppl7::grafix::Size desktopSize;
	ppl7::grafix::Font gui_font;
	ppl7::tk::Button *exit_button;
	Decker::StatusBar *statusbar;

	FPS fps;

	ppl7::grafix::Point WorldCoords;

	bool quitGame;

	void createWindow();
	void initUi();
	void drawGrid(int layer);

	void moveWorldWhenMouseIsInBorder(const ppl7::tk::MouseState &mouse);



public:
	Game();
	~Game();
	void init();
	void run();

	// EventHandler
	void quitEvent(ppl7::tk::Event *event);
	void closeEvent(ppl7::tk::Event *event);
	void mouseClickEvent(ppl7::tk::MouseEvent *event);
};




#endif /* INCLUDE_DECKER_H_ */
