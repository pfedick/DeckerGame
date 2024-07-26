#ifndef INCLUDE_HUD_H_
#define INCLUDE_HUD_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "decker_sdl.h"


class Player;
class SpriteTexture;

class GameHUD
{
private:
    int value_health, value_lifes, value_points;
    int value_energy;
    int number_batteries;
    double oxygen_cooldown;
    bool editormode;
    bool redraw_needed;
    bool visible;
    ppl7::grafix::Rect my_viewport;
    SDL_Texture* hud_texture;
    SDL_Texture* message_texture;
    ppl7::grafix::Size hud_size;
    SDL& sdl;
    SpriteTexture* icons;

    void invalidate();
    void redraw();

public:
    GameHUD(SDL& sdl);
    ~GameHUD();
    void setViewport(const ppl7::grafix::Rect& viewport);
    const ppl7::grafix::Rect& viewport() const;
    void retranslateUi();

    void setEditorMode(bool enable);
    bool isEditorMode() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void updatePlayerStats(const Player* player);
    void resetPlayerStats(const Player* player);

    void draw(SDL_Renderer* renderer, SDL_Texture* render_target, const SDL_Rect& render_rect);
};

#endif
