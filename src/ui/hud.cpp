#include <SDL.h>
#include "decker.h"
#include "hud.h"
#include "player.h"




GameHUD::GameHUD(SDL& sdl)
    : sdl(sdl)
{
    hud_texture=NULL;
    message_texture=NULL;
    value_health=0.0f;
    value_lifes=0.0f;
    value_points=0;
    value_energy=0.0f;
    number_batteries=0;
    oxygen_cooldown=0.0f;
    maxair=0.0f;
    editormode=false;
    visible=true;
    hud_size.setSize(1920, 100);
    my_viewport.setRect(0, 0, 1920, 1080);
    try {
        hud_texture=sdl.createStreamingTexture(hud_size.width, hud_size.height);
        //message_texture=sdl.createStreamingTexture(screen_size.width, 200);   // for the future?
    } catch (...) {
        // Do nothing
    }

    icons=new SpriteTexture();
    if (!icons) return;
    icons->enableSDLBuffer(false);
    icons->enableMemoryBuffer(true);
    icons->load(sdl, "res/hud.tex");

    redraw_needed=true;
    const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
    label_font=style.buttonFont;
    label_font.setName("NotoSansBlack");
    label_font.setBold(false);
    label_font.setSize(20);
    label_font.setOrientation(ppl7::grafix::Font::TOP);


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
    if (icons) delete icons;
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
    invalidate();
}

static int calculatePointDiff(int display, int player)
{

    if (display < player) {
        int pdiff=(player - display) / 30;
        if (pdiff < 1) pdiff=1;
        return pdiff;
    } else if (display > player) {
        int pdiff=(display - player) / 30;
        if (pdiff < 1) pdiff=1;
        return -pdiff;
    }
    return 0;
}



void GameHUD::updatePlayerStats(const Player* player)
{
    //invalidate();
    if (value_health != player->health) {
        value_health+=calculatePointDiff(value_health, player->health);
        redraw_needed=true;
    }
    if (value_energy != player->energylevel) {
        value_energy+=calculatePointDiff(value_energy, player->energylevel);
        redraw_needed=true;
    }
    maxair=player->maxair;
    if (value_oxygen != player->air * 100 / player->maxair) {
        value_oxygen=player->air * 100 / player->maxair;
        redraw_needed=true;
    }
    if (value_lifes != player->lifes) {
        value_lifes=player->lifes;
        redraw_needed=true;
    }
    if (number_batteries != player->powercells) {
        number_batteries=player->powercells;
        redraw_needed=true;
    }
    if (value_points != player->points) {
        value_points=player->points;
        redraw_needed=true;
    }
}

void GameHUD::resetPlayerStats(const Player* player)
{
    value_health=player->health;
    value_lifes=player->lifes;
    value_points=player->points;
    value_energy=player->energylevel;
    oxygen_cooldown=0.0f;

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

void GameHUD::drawProgressBar(ppl7::grafix::Drawable& draw, int x, int y, int width, int height, float value, const ppl7::grafix::Color& color)
{
    int w=width * value / 100 / 10;
    for (int i=0;i < w;i++) {
        icons->draw(draw, x + i * 10, y + 24, 6, color);
    }
}

void GameHUD::drawMiddlePart(ppl7::grafix::Drawable& draw)
{
    int y=10;

    if (number_batteries == 0) y+=(hud_size.height - 10 - 40) / 2;
    else y+=(hud_size.height - 10 - 80) / 2;


    label_font.setColor(ppl7::grafix::Color(210, 210, 210, 255));

    ppl7::String labeltext=translate("Lifes:");
    ppl7::grafix::Size s=label_font.measure(labeltext);
    int y_lifes=y;
    int maxwidth=s.width;
    draw.print(label_font, 800, y, labeltext);
    y+=40;
    int y_powercells=0;
    if (number_batteries > 0) {
        labeltext=translate("Batteries:");
        ppl7::grafix::Size s=label_font.measure(labeltext);
        y_powercells=y;
        if (s.width > maxwidth) maxwidth=s.width;
        draw.print(label_font, 800, y, labeltext);
    }

    int x=800 + 20 + maxwidth;

    for (int i=0;i < value_lifes;i++) {
        icons->draw(draw, x + i * 35, y_lifes + 25, 3);
    }

    for (int i=0;i < number_batteries;i++) {
        icons->draw(draw, x + i * 35, y_powercells + 25, 4);
    }

}

void GameHUD::drawPoints(ppl7::grafix::Drawable& draw)
{
    int y=0;

    label_font.setColor(ppl7::grafix::Color(210, 210, 210, 255));
    label_font.setBold(false);
    label_font.setSize(30);

    ppl7::String labeltext=translate("Points:");
    ppl7::grafix::Size s=label_font.measure(labeltext);
    y=(hud_size.height - s.height) / 2;
    draw.print(label_font, 1400, y, labeltext);
    int x=1400 + s.width + 10;

    label_font.setColor(ppl7::grafix::Color(255, 220, 0, 255));
    label_font.setSize(50);
    labeltext.setf("%d", value_points);
    s=label_font.measure(labeltext);
    y=(hud_size.height - s.height) / 2;
    draw.print(label_font, x, y, labeltext);


}

void GameHUD::redraw()
{
    //ppl7::PrintDebug("GameHUD::redraw\n");
    if (!hud_texture) {
        ppl7::PrintDebug("GameHUD::redraw => ERROR: no hud_texture!\n");
        return;
    }
    label_font.setBold(false);
    label_font.setSize(20);

    redraw_needed=false;
    ppl7::grafix::Color black(0, 0, 0, 255);
    ppl7::grafix::Color white(255, 255, 255, 255);
    label_font.setColor(ppl7::grafix::Color(210, 210, 210, 255));


    ppl7::grafix::Drawable draw=sdl.lockTexture(hud_texture);
    draw.cls(ppl7::grafix::Color(1, 1, 1, 128));
    draw.line(0, 0, hud_size.width, 0, black);
    draw.line(0, 1, hud_size.width, 1, white);
    draw.line(0, 2, hud_size.width, 2, white);

    int maxwidth=0;
    int y=5;
    ppl7::String labeltext=translate("Health:");
    ppl7::grafix::Size s=label_font.measure(labeltext);
    int lineheight=s.height;
    int y_health=y;
    maxwidth=s.width;
    icons->draw(draw, 20, y + lineheight, 0);
    draw.print(label_font, 40, y, labeltext);
    y+=lineheight;

    int y_energy=y;
    labeltext=translate("Energy:");
    s=label_font.measure(labeltext);
    if (s.width > maxwidth) maxwidth=s.width;
    icons->draw(draw, 20, y + lineheight, 2);
    draw.print(label_font, 40, y, labeltext);
    y+=lineheight;

    int y_oxygen=y;
    labeltext=translate("Oxygen:");
    s=label_font.measure(labeltext);
    if (s.width > maxwidth) maxwidth=s.width;
    icons->draw(draw, 20, y + lineheight, 1);
    draw.print(label_font, 40, y, labeltext);

    int w=700 - 50 - maxwidth;

    drawProgressBar(draw, 50 + maxwidth, y_health, w, 20, value_health, ppl7::grafix::Color(240, 15, 0, 255));
    drawProgressBar(draw, 50 + maxwidth, y_energy, w, 20, value_energy, ppl7::grafix::Color(15, 180, 20, 255));
    drawProgressBar(draw, 50 + maxwidth, y_oxygen, w, 20, value_oxygen, ppl7::grafix::Color(15, 30, 255, 255));


    label_font.setColor(ppl7::grafix::Color(255, 220, 0, 255));
    int x=50 + maxwidth + w + 5;
    ppl7::WideString value;
    value.setf("%d %%", (int)value_health);
    draw.print(label_font, x, y_health, value);

    value.setf("%d %%", (int)value_energy);
    draw.print(label_font, x, y_energy, value);

    value.setf("%d %%", (int)value_oxygen);
    draw.print(label_font, x, y_oxygen, value);

    //value.setf("%0.0f s", seconds_left);

    drawMiddlePart(draw);
    drawPoints(draw);



    sdl.unlockTexture(hud_texture);
}


void GameHUD::draw(SDL_Renderer* renderer, SDL_Texture* render_target, const SDL_Rect& render_rect)
{
    if (!visible) return;
    if (redraw_needed) redraw();
    //ppl7::PrintDebug("GameHUD::draw\n");
    SDL_Rect tr=render_rect;
    //tr.y=tr.h - hud_size.height;
    tr.y= my_viewport.y2 - hud_size.height;
    tr.h=hud_size.height;

    SDL_SetRenderTarget(renderer, render_target);
    SDL_RenderCopy(renderer, hud_texture, NULL, &tr);
}
