#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Game::Game()
{
	tex_level_grid=NULL;
	wm=ppl7::tk::GetWindowManager();
}

Game::~Game()
{
	if (tex_level_grid) sdl.destroyTexture(tex_level_grid);
	wm->destroyWindow(*this);
}

void Game::loadGrafix()
{
	Sprite_Charlie.load(sdl, "res/charlie.tex");
	//printf ("Sprites loaded: %d\n",Sprite_Charlie.numSprites());
	Bricks.load(sdl, "res/bricks.tex");
	Cursor.load(sdl, "res/cursor.tex");
}

void Game::createWindow()
{
	setFlags(ppl7::tk::Window::DefaultFullscreen);
	setWindowTitle("DeckerGame");
	setRGBFormat(ppl7::grafix::RGBFormat::A8R8G8B8);
	setBackgroundColor(ppl7::grafix::Color(0,0,0,0));
	ppl7::grafix::Size desktop=wm->desktopResolution();
	setSize(desktop);
	wm->createWindow(*this);
	sdl.setRenderer((SDL_Renderer *)getRenderer());

}

void Game::initUi()
{
	ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	ppl7::grafix::Size desktop=wm->desktopResolution();
	ppl7::tk::Label *label;

	ppl7::tk::Frame *title_frame=new ppl7::tk::Frame(0,0,desktop.width,32);
	this->addChild(title_frame);

	exit_button=new ppl7::tk::Button(desktop.width-100,0,100,32,"Exit");
	exit_button->setIcon(gfx->Toolbar.getDrawable(68));
	exit_button->setEventHandler(this);
	title_frame->addChild(exit_button);

	// Bottom Frame
	statusbar=new Decker::StatusBar(0,desktop.height-32,desktop.width,32);
	this->addChild(statusbar);
}

void Game::init()
{
	createWindow();
	initUi();
	desktopSize=sdl.getWindowSize();
	loadGrafix();
	gui_font.setName("Default");
	gui_font.setSize(12);
	gui_font.setBold(true);
	gui_font.setColor(ppl7::grafix::Color(255,255,255,255));
	gui_font.setBorderColor(ppl7::grafix::Color(0,0,0,0));
	gui_font.setShadowColor(ppl7::grafix::Color(0,0,0,0));
	gui_font.setOrientation(ppl7::grafix::Font::TOP);
	gui_font.setAntialias(true);

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
		wm->handleEvents();
		fps.update();
		statusbar->setFps(fps.getFPS());
		ppl7::tk::MouseState mouse=wm->getMouseState();
		moveWorldWhenMouseIsInBorder(mouse);
		statusbar->setMouse(mouse);
		statusbar->setWorldCoords(WorldCoords);
		sdl.startFrame();

		Bricks.draw(renderer,400,400,7);
		/*
		for (int y=0;y<1080;y+=100) {
			for (int x=0;x<1920;x+=100) {
				Sprite_Charlie.draw(renderer,x,y,c);
			}
		}
		*/

		for (int i=0;i<10;i++) {
			Bricks.draw(renderer,400+i*62,656+i*4,10);
		}
		for (int i=0;i<8;i++) {
			Bricks.draw(renderer,431+i*62,656-35+i*4,1);
		}
		Sprite_Charlie.draw(renderer,400,400,c);
		Sprite_Charlie.draw(renderer,656,400,c+50);
		if (c%20<10)
			Sprite_Charlie.draw(renderer,400+5*62,620,c%20+40);
		else
			Sprite_Charlie.draw(renderer,400+5*62,620,c%20+80);
		c++;
		if (c>=40) c=0;
		//displayHUD();
		//SDL_RenderCopy(renderer, gui_tex, NULL, NULL);
		drawGrid(0);
		mouse=wm->getMouseState();
		Cursor.draw(renderer,mouse.p.x,mouse.p.y,0);
		drawWidgets();
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

void Game::mouseClickEvent(ppl7::tk::MouseEvent *event)
{
	if (event->widget()==exit_button) {
		quitGame=true;
	}
}



