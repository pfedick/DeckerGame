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
	resources.uiTiles.load("res/tiles.tex");
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

	showTilesSelection();
}

void Game::init()
{
	createWindow();
	loadGrafix();
	initUi();
	desktopSize=sdl.getWindowSize();
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
	player->setGameWindow(sdl.getClientWindow());
	player->setSpriteResource(resources.Sprite_George);



}

void Game::drawGrid(int layer)
{
	static ppl7::grafix::Point LastPos;
	static int LastLayer;
	if (!tex_level_grid) {
		tex_level_grid=sdl.createStreamingTexture(desktopSize.width, desktopSize.height);
		LastLayer=-1;
	}

	if (layer!=LastLayer || LastPos!=WorldCoords) {
		ppl7::grafix::Drawable draw=sdl.lockTexture(tex_level_grid);
		draw.cls(0);
		ppl7::grafix::Color white(255,255,255,128);
		ppl7::grafix::Color black(0,0,0,128);
		int bricks_x=desktopSize.width/62+1;
		int brick_shift_y=bricks_x*4;
		int right=bricks_x*62;
		int wx=WorldCoords.x%62;
		int wy=WorldCoords.y%33;
		int pre_y=(brick_shift_y/33)*33;

		for (int y=32-wy-pre_y;y<desktopSize.height;y+=33) {
			draw.lineAA(0,y+1,right,y+brick_shift_y+1,black);
			draw.lineAA(0,y,right,y+brick_shift_y,white);
		}
		for (int x=0-wx;x<desktopSize.width;x+=62) {
			draw.line(x+31,32,x+31,desktopSize.height,black);
			draw.line(x+1,32,x+1,desktopSize.height,black);
			draw.line(x,32,x,desktopSize.height,white);
		}


		sdl.unlockTexture(tex_level_grid);
		LastLayer=layer;
		LastPos=WorldCoords;
	}
	SDL_Renderer *renderer=sdl.getRenderer();
	SDL_RenderCopy(renderer, tex_level_grid, NULL, NULL);
}

void Game::moveWorldWhenMouseIsInBorder(const ppl7::tk::MouseState &mouse)
{
	if (mouse.p.x<5 && WorldCoords.x>0) {
		WorldCoords.x-=4;
		if (WorldCoords.x<0) WorldCoords.x=0;
	}
	if (mouse.p.x>desktopSize.width-5 && WorldCoords.x<62000) {
		WorldCoords.x+=4;
		if (WorldCoords.x>62000) WorldCoords.x=62000;
	}
	if (mouse.p.y<5 && WorldCoords.y>0) {
		WorldCoords.y-=4;
		if (WorldCoords.y<0) WorldCoords.y=0;
	}
	if (mouse.p.y>desktopSize.height-5 && WorldCoords.y<62000) {
		WorldCoords.y+=4;
		if (WorldCoords.y>62000) WorldCoords.y=62000;
	}

}

void Game::run()
{
	SDL_Renderer *renderer=sdl.getRenderer();
	//SDL_Event event;
	quitGame=false;
	int c=0;
	while (!quitGame) {
		double now=ppl7::GetMicrotime();
		player->update(now);
		wm->handleEvents();
		fps.update();
		statusbar->setFps(fps.getFPS());
		ppl7::tk::MouseState mouse=wm->getMouseState();
		moveWorldWhenMouseIsInBorder(mouse);
		statusbar->setMouse(mouse);
		statusbar->setWorldCoords(WorldCoords);
		sdl.startFrame(Style.windowBackgroundColor);

		//Bricks.draw(renderer,400,400,7);
		/*
		for (int y=0;y<1080;y+=100) {
			for (int x=0;x<1920;x+=100) {
				Sprite_Charlie.draw(renderer,x,y,c);
			}
		}
		*/
		/*
		for (int y=1080;y>100;y-=38) {
			for (int x=0;x<1920;x+=64) {
				Bricks.draw(renderer,x,y,0);
			}
		}
		*/
		/*
		for (int i=0;i<10;i++) {
			Bricks.draw(renderer,400+i*64,656,0);
		}
		*/
		/*
		for (int i=0;i<8;i++) {
			Bricks.draw(renderer,431+i*62,656-35+i*4,1);
		}
		*/
		/*
		for (int y=100;y<1080;y+=200) {
			for (int x=0;x<1920;x+=200) {
				Sprite_George.draw(renderer,x,y,c);
			}
		}
		*/
		player->draw(renderer);
				/*
				 *
		//Sprite_George.draw(renderer,400,400,c);
		Sprite_George.draw(renderer,656,400,c+50);
		if (c%20<10)
			Sprite_George.draw(renderer,400+5*62,620,c%20+40);
		else
			Sprite_George.draw(renderer,400+5*62,620,c%20+80);
			*/
		c++;
		if (c>=50) c=0;
		//displayHUD();
		//SDL_RenderCopy(renderer, gui_tex, NULL, NULL);
		//drawGrid(0);
		mouse=wm->getMouseState();
		drawWidgets();
		resources.Cursor.draw(renderer,mouse.p.x,mouse.p.y,1);
		presentScreen();

		//WorldCoords.x+=1;
		//WorldCoords.y+=1;
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
	} else {
		tiles_selection=new Decker::ui::TilesSelection(0,33,300,statusbar->y()-2-33,this,&resources.uiTiles);
		this->addChild(tiles_selection);
	}
}




