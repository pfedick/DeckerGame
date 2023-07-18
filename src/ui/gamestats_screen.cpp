#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"


GameStatsScreen::GameStatsScreen(Game& g, int x, int y, int width, int height, StatsScreenReason reason)
    : game(g)
{
    this->reason=reason;
    closeWindow=false;
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
    ppl7::grafix::Color bg(20, 10, 0, 192);
    ppl7::grafix::Color border(255, 200, 0, 255);
    ppl7::grafix::Color fg(128, 128, 128, 255);
    draw.cls(bg);
    for (int i=0;i < 6;i++) {
        draw.drawRect(i, i, draw.width() - i, draw.height() - i, border);
    }
    ppl7::String title=translate("Game over");
    if (reason == StatsScreenReason::LevelEnd) {
        title=translate("Level completed");
    }

}

void GameStatsScreen::signalContinue()
{
    closeWindow=true;
}

bool GameStatsScreen::continueSignaled() const
{
    return closeWindow;
}


void Game::showStatsScreen(StatsScreenReason reason)
{
    world_widget->setVisible(false);
    world_widget->setEnabled(false);
    ppl7::grafix::Color black(128, 0, 0, 255);
    SDL_Renderer* renderer=sdl.getRenderer();
    bool save_ui_state=this->showui;
    showUi(false);
    int fade_to_black=255;
    int fade_state=0;
    game_stats_screen=new GameStatsScreen(*this,
        100, 100, this->width() - 200, this->height() - 200, reason);
    this->addChild(game_stats_screen);
    this->needsRedraw();


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
        if (game_stats_screen->continueSignaled()) {
            if (fade_state == 1) {
                fade_state = 2;
                fade_to_black=0;
            } else if (fade_state == 3) break;
        }
    }
    delete game_stats_screen;
    game_stats_screen=NULL;
    world_widget->setVisible(true);
    world_widget->setEnabled(true);
    showUi(save_ui_state);
    enableControls(true);
}
