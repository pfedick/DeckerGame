#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"

static void FadeToBlack(SDL_Renderer *renderer, int fade_to_black)
{
	if (fade_to_black > 0) {
				SDL_BlendMode currentBlendMode;
				SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
				//SDL_BlendMode newBlendMode=SDL_BLENDMODE_BLEND;
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, fade_to_black);
				SDL_RenderFillRect(renderer, NULL);
				SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
	}
}

void Game::showStartScreen()
{
    world_widget->setVisible(false);
	world_widget->setEnabled(false);
	ppl7::grafix::Color black(128, 0, 0, 255);
	SDL_Renderer* renderer=sdl.getRenderer();
	StartScreen* widget=new StartScreen(sdl,0, 0, this->width(), this->height());
	this->addChild(widget);
	wm->setKeyboardFocus(widget);
	showUi(false);
	int fade_to_black=255;
	int fade_state=0;
	quitGame=false;
	startLevel("level/start.lvl");
	while (!quitGame) {
		wm->handleEvents();
		sdl.startFrame(black);
		ppl7::tk::MouseState mouse=wm->getMouseState();
		drawWorld(renderer);
		drawWidgets();
        resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
		if (fade_state==0) {
			fade_to_black-=2;
			if (fade_to_black<=0) {
                fade_state=1;
                fade_to_black=0;
            }
		}
        if (fade_to_black) FadeToBlack(renderer, fade_to_black);
		presentScreen();
		
	}
	this->removeChild(widget);
	delete widget;
}

StartScreen::StartScreen(SDL &s, int x, int y, int width, int height)
: sdl(s)
{
    create(x,y,width,height);
	
	TitleImage.load("res/game_title.png");
}

StartScreen::~StartScreen()
{

}

void StartScreen::paint(ppl7::grafix::Drawable &draw)
{
	ppl7::grafix::Color bg(20,0,0,96);
	draw.fillRect(0,0,draw.width(),TitleImage.height(),bg);
	printf("StartScreen::paint\n");
	draw.bltAlpha(TitleImage,0,0);
	printf("StartScreen::paint done\n");
}
