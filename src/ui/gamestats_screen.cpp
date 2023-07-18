#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"


GameStatsScreen::GameStatsScreen(Game& g, int x, int y, int width, int height, Reason reason)
    : game(g)
{
    this->reason=reason;
    create(x, y, width, height);
}


GameStatsScreen::~GameStatsScreen()
{

}

void GameStatsScreen::setupUi()
{
    this->destroyChilds();
}

void GameStatsScreen::paint(ppl7::grafix::Drawable& draw)
{

}


static void FadeToBlack(SDL_Renderer* renderer, int fade_to_black)
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


void Game::showStatsScreen()
{
    //world_widget->setVisible(false);
    //world_widget->setEnabled(false);
    ppl7::grafix::Color black(128, 0, 0, 255);
    SDL_Renderer* renderer=sdl.getRenderer();
    showUi(false);
    int fade_to_black=255;
    int fade_state=0;
    enableControls(false);
    last_frame_time=0.0f;
    while (1) {
        double now=ppl7::GetMicrotime();
        float frame_rate_compensation=1.0f;
        if (last_frame_time > 0.0f) {
            float frametime=now - last_frame_time;
            frame_rate_compensation=frametime / (1.0f / 60.0f);
        }
        last_frame_time=now;

        wm->handleEvents();
        sdl.startFrame(black);
        ppl7::tk::MouseState mouse=wm->getMouseState();
        drawWidgets();
        resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
        if (fade_state == 0) {
            fade_to_black-=2;
            if (fade_to_black <= 0) {
                fade_state=1;
                fade_to_black=0;
            }
        } else if (fade_state == 2) {
            fade_to_black+=5;
            if (fade_to_black >= 255) {
                fade_state=3;
                fade_to_black=255;
            }
        }
        if (fade_to_black) FadeToBlack(renderer, fade_to_black);
        presentScreen();
        if (quitGame == true && fade_state == 1) {
            fade_state = 2;
            fade_to_black=0;
        } else if (quitGame == true && fade_state == 3) {
            break;
        }
    }

}
