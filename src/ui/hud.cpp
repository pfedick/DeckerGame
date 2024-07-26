#include <SDL.h>
#include "hud.h"



GameHUD::GameHUD(SDL& sdl)
    : sdl(sdl)
{
    hud_texture=NULL;
    message_texture=NULL;
    value_health=0;
    value_lifes=0;
    value_points=0;
    value_energy=0;
    number_batteries=0;
    oxygen_cooldown=0.0f;
    editormode=false;
    visible=true;
    hud_size.setSize(1920, 110);
    my_viewport.setRect(0, 0, 1920, 1080);
    try {
        hud_texture=sdl.createStreamingTexture(hud_size.width, hud_size.height);
        //message_texture=sdl.createStreamingTexture(screen_size.width, 200);   // for the future?
    } catch (...) {
        // Do nothing
    }
    redraw_needed=true;
}

GameHUD::~GameHUD()
{
    if (hud_texture) {
        sdl.destroyTexture(hud_texture);
        hud_texture=NULL;
    }
    if (message_texture) {
        sdl.destroyTexture(message_texture);
        message_texture=NULL;
    }
}

void GameHUD::invalidate()
{
    redraw_needed=true;
}

void GameHUD::setViewport(const ppl7::grafix::Rect& viewport)
{
    if (my_viewport != viewport) {
        my_viewport=viewport;
    }
}

const ppl7::grafix::Rect& GameHUD::viewport() const
{
    return my_viewport;

}

void GameHUD::setEditorMode(bool enable)
{
    editormode=true;
}

bool GameHUD::isEditorMode() const
{
    return editormode;
}

void GameHUD::retranslateUi()
{

}

void GameHUD::updatePlayerStats(const Player* player)
{
    //invalidate();
}

void GameHUD::resetPlayerStats(const Player* player)
{
    invalidate();
}

void GameHUD::setVisible(bool visible)
{
    this->visible=visible;
}
bool GameHUD::isVisible() const
{
    return visible;
}

void GameHUD::redraw()
{
    ppl7::PrintDebug("GameHUD::redraw\n");
    if (!hud_texture) {
        ppl7::PrintDebug("GameHUD::redraw => ERROR: no hud_texture!\n");
        return;
    }
    redraw_needed=false;

    ppl7::grafix::Drawable draw=sdl.lockTexture(hud_texture);
    draw.cls(ppl7::grafix::Color(63, 0, 0, 90));


    sdl.unlockTexture(hud_texture);
}


void GameHUD::draw(SDL_Renderer* renderer, SDL_Texture* render_target, const SDL_Rect& render_rect)
{
    if (!visible) return;
    if (redraw_needed) redraw();
    //ppl7::PrintDebug("GameHUD::draw\n");
    SDL_Rect tr=render_rect;
    tr.y=tr.h - hud_size.height;
    tr.y-=120;
    tr.h=hud_size.height;

    SDL_SetRenderTarget(renderer, render_target);
    SDL_RenderCopy(renderer, hud_texture, NULL, &tr);
}
