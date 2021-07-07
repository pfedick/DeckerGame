#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"

Game::Game()
{
	tex_level_grid=NULL;
	wm=ppl7::tk::GetWindowManager();
	ppl7::tk::WidgetStyle s(ppl7::tk::WidgetStyle::Dark);
	wm->setWidgetStyle(s);
	Style=s;
	player=NULL;
	mainmenue=NULL;
	statusbar=NULL;
	tiles_selection=NULL;
	quitGame=false;
	worldIsMoving=false;
	tex_sky=NULL;
}

Game::~Game()
{
	if (player) delete player;
	if (tex_level_grid) sdl.destroyTexture(tex_level_grid);
	wm->destroyWindow(*this);
}

void Game::loadGrafix()
{
	resources.Sprite_George.load(sdl, "res/george.tex");
	//printf ("Sprites loaded: %d\n",Sprite_Charlie.numSprites());
	//resources.Tiles.load(sdl, "res/bricks.tex", ppl7::grafix::Color(230,220,0,255));
	resources.Tiles.load(sdl, "res/tiles.tex");
	resources.Cursor.load(sdl, "res/cursor.tex");
	resources.TileTypes.load(sdl, "res/tiletypes.tex");
	resources.uiTiles.load("res/tiles.tex");
	resources.uiTileTypes.load("res/tiletypes.tex");


	ppl7::grafix::Image img;
	img.load("res/sky2.png");
	tex_sky=sdl.createStreamingTexture(img.width(), img.height());
	ppl7::grafix::Drawable draw=sdl.lockTexture(tex_sky);
	draw.blt(img);
	sdl.unlockTexture(tex_sky);
}

void Game::createWindow()
{
	setFlags(ppl7::tk::Window::DefaultWindow);
	setWindowTitle("DeckerGame");
	setRGBFormat(ppl7::grafix::RGBFormat::A8R8G8B8);
	setBackgroundColor(ppl7::grafix::Color(0,0,0,0));
	ppl7::grafix::Size desktop=wm->desktopResolution();
	desktop.height-=80;
	setSize(desktop);
	wm->createWindow(*this);
	sdl.setRenderer((SDL_Renderer *)getRenderer());
	SDL_ShowCursor(SDL_DISABLE);

}

SDL_Renderer *Game::getSDLRenderer()
{
	return sdl.getRenderer();
}


void Game::initUi()
{
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	const ppl7::grafix::Size &desktop=clientSize();
	//ppl7::tk::Label *label;

	mainmenue=new Decker::ui::MainMenue(0,0,desktop.width,32, this);
	this->addChild(mainmenue);


	// Bottom Frame
	statusbar=new Decker::ui::StatusBar(0,desktop.height-32,desktop.width,32);
	this->addChild(statusbar);
	viewport.y1=33;
	viewport.y2=desktop.height-33;


}

void Game::init()
{
	createWindow();
	loadGrafix();

	desktopSize=sdl.getWindowSize();
	viewport=sdl.getClientWindow();
	initUi();
	PlayerCoords.x=desktopSize.width/2;
	PlayerCoords.y=desktopSize.height/2;

	gui_font.setName("Default");
	gui_font.setSize(12);
	gui_font.setBold(true);
	gui_font.setColor(ppl7::grafix::Color(255,255,255,255));
	gui_font.setBorderColor(ppl7::grafix::Color(0,0,0,0));
	gui_font.setShadowColor(ppl7::grafix::Color(0,0,0,0));
	gui_font.setOrientation(ppl7::grafix::Font::TOP);
	gui_font.setAntialias(true);
	if (player) delete player;
	player=new Player();
	//player->setGameWindow(sdl.getClientWindow());

	player->setSpriteResource(resources.Sprite_George);

	//level.create(255,255);
	level.load("level/test.lvl");
	level.setTileset(1, &resources.Tiles);
	level.setTileTypesSprites(&resources.TileTypes);
	//level.setRenderer

	showTilesSelection();


}

void Game::drawGrid()
{
	if (!tex_level_grid) {
		tex_level_grid=sdl.createStreamingTexture(desktopSize.width, desktopSize.height);
		ppl7::grafix::Drawable draw=sdl.lockTexture(tex_level_grid);
		ppl7::grafix::Color white(255,255,255,128);
		ppl7::grafix::Color black(0,0,0,128);

		draw.cls(0);
		for (int x=0;x<desktopSize.width;x+=64) {
			draw.line(x,0,x,desktopSize.height,black);
			draw.line(x+1,0,x+1,desktopSize.height,white);
		}
		for (int y=0;y<desktopSize.height;y+=64) {
			draw.line(0,y,desktopSize.width,y ,black);
			draw.line(0,y+1,desktopSize.width,y+1 ,white);
		}
		sdl.unlockTexture(tex_level_grid);
	}
	SDL_Renderer *renderer=sdl.getRenderer();
	SDL_Rect target;
	target.x=viewport.x1-(WorldCoords.x%64);
	target.y=viewport.y1-(WorldCoords.y%64);
	target.w=desktopSize.width;
	target.h=desktopSize.height;
	SDL_RenderCopy(renderer, tex_level_grid, NULL, &target);
}

void Game::moveWorld(int offset_x, int offset_y)
{
	if (offset_x==0 && offset_y==0) return;
	WorldCoords.x+=offset_x;
	WorldCoords.y+=offset_y;
	if (WorldCoords.x<0) WorldCoords.x=0;
	if (WorldCoords.x>62000) WorldCoords.x=62000;
	if (WorldCoords.y<0) WorldCoords.y=0;
	if (WorldCoords.y>62000) WorldCoords.y=62000;
}

void Game::moveWorldOnMouseClick(const ppl7::tk::MouseState &mouse)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (worldIsMoving) {
		if (mouse.buttonMask == ppl7::tk::MouseState::Middle || ((mouse.buttonMask == ppl7::tk::MouseState::Left) && state[SDL_SCANCODE_LSHIFT])) {
			//printf("Move\n");
			moveWorld(WorldMoveStart.x-mouse.p.x, WorldMoveStart.y-mouse.p.y);
			WorldMoveStart=mouse.p;
		} else {
			worldIsMoving=false;
			//printf("End\n");
		}
	} else {
		//printf("mouse.buttonMask=%d\n", mouse.button);
		if (mouse.buttonMask == ppl7::tk::MouseState::Middle || ((mouse.buttonMask == ppl7::tk::MouseState::Left) && state[SDL_SCANCODE_LSHIFT])) {
			//printf("Start\n");
			worldIsMoving=true;
			WorldMoveStart=mouse.p;
		} else {
			worldIsMoving=false;;
		}

	}
}

void Game::updateUi(const ppl7::tk::MouseState &mouse)
{
	fps.update();
	statusbar->setFps(fps.getFPS());
	statusbar->setMouse(mouse);
	statusbar->setWorldCoords(WorldCoords);
	statusbar->setPlayerCoords(PlayerCoords);
}

void Game::run()
{
	SDL_Renderer *renderer=sdl.getRenderer();
	quitGame=false;
	while (!quitGame) {
		double now=ppl7::GetMicrotime();
		player->update(now);
		wm->handleEvents();
		ppl7::tk::MouseState mouse=wm->getMouseState();
		updateUi(mouse);
		// Handle Mouse events inside World
		if (mouse.p.inside(viewport)) {
			moveWorldOnMouseClick(mouse);
			handleMouseDrawInWorld(mouse);
		}
		sdl.startFrame(Style.windowBackgroundColor);
		level.setViewport(viewport);
		player->setGameWindow(viewport);
		SDL_Rect target;
		target.x=viewport.x1;
		target.y=viewport.y1;
		target.w=desktopSize.width;
		target.h=desktopSize.height;
		SDL_Rect source;
		ppl7::grafix::Point c=WorldCoords*0.1f;
		source.x=c.x;
		source.y=c.y;
		source.w=desktopSize.width;
		source.h=desktopSize.height;
		SDL_RenderCopy(renderer, tex_sky, &source, &target);

		// Draw Planes and Sprites
		level.drawPlane(renderer,level.FarPlane, WorldCoords*0.5f);
		level.drawPlane(renderer,level.PlayerPlane, WorldCoords);
		player->draw(renderer);
		level.drawPlane(renderer,level.FrontPlane, WorldCoords);
		if (mainmenue->showTileTypes()) level.drawTileTypes(renderer, WorldCoords);
		// Grid
		if (mainmenue->showGrid()) drawGrid();

		// Widgets
		drawWidgets();
		// Mouse
		resources.Cursor.draw(renderer,mouse.p.x,mouse.p.y,1);
		presentScreen();
	}
}

void Game::quitEvent(ppl7::tk::Event *e)
{
	quitGame=true;
}

void Game::closeEvent(ppl7::tk::Event *e)
{
	quitGame=true;
}


void Game::showTilesSelection()
{
	if (tiles_selection) {
		this->removeChild(tiles_selection);
		delete(tiles_selection);
		tiles_selection=NULL;
		viewport.x1=0;
	} else {
		tiles_selection=new Decker::ui::TilesSelection(0,33,300,statusbar->y()-2-33,this,&resources.uiTiles);
		this->addChild(tiles_selection);
		viewport.x1=300;
	}
}

void Game::handleMouseDrawInWorld(const ppl7::tk::MouseState &mouse)
{
	if (!tiles_selection) return;
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_LSHIFT]) return;
	ppl7::grafix::Point coords=WorldCoords*1.0f;
	int x=(mouse.p.x-viewport.x1+coords.x)/64;
	int y=(mouse.p.y-viewport.y1+coords.y)/64;

	int selectedTile=tiles_selection->selectedTile();

	if (mouse.buttonMask==ppl7::tk::MouseState::Left) {
		level.PlayerPlane.setTile(x,y,0,1,selectedTile);
	} else if (mouse.buttonMask==ppl7::tk::MouseState::Right) {
		level.PlayerPlane.clearTile(x,y,0);
	}
}

void Game::save()
{
	level.save("level/test.lvl");
}

void Game::load()
{
	printf ("Game::load\n");
	level.load("level/test.lvl");
}


