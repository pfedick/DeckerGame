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
    hud_size.setSize(1920, 110);
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

static float calculatePointDiff(float display, float player)
{

    if (display < player) {
        float pdiff=(player - display) / 30;
        if (pdiff < 1) pdiff=player - display;
        return pdiff;
    } else if (display > player) {
        float pdiff=(display - player) / 30;
        if (pdiff < 1) pdiff=display - player;
        return -pdiff;
    }
    return 0;
}



void GameHUD::updatePlayerStats(const Player* player)
{
    //invalidate();
    if (value_health != player->health) {
        int save=value_health;
        value_health+=calculatePointDiff(value_health, player->health);
        if (save != (int)value_health) {
            redraw_needed=true;
            //ppl7::PrintDebugTime("redraw health\n");
        }
    }
    if (value_energy != player->energylevel) {
        int save=value_energy;
        value_energy+=calculatePointDiff(value_energy, player->energylevel);
        if (save != (int)value_energy) {
            redraw_needed=true;
            //ppl7::PrintDebugTime("redraw energy\n");
        }
    }
    maxair=player->maxair;
    if (value_oxygen != player->air) {
        int save=value_oxygen;
        value_oxygen+=calculatePointDiff(value_oxygen, player->air);
        oxygen_cooldown=player->time + 3.0f;
        if (save != (int)value_oxygen) {
            //ppl7::PrintDebugTime("redraw oxygen\n");
            redraw_needed=true;
        }
    }
    if (value_lifes != player->lifes) {
        value_lifes=player->lifes;
        //ppl7::PrintDebug("redraw lifes\n");
        redraw_needed=true;
    }
    if (number_batteries != player->powercells) {
        number_batteries=player->powercells;
        redraw_needed=true;
        //ppl7::PrintDebug("redraw powercells\n");
    }
    if (value_points != player->points) {
        value_points=player->points;
        //ppl7::PrintDebug("redraw points\n");
        redraw_needed=true;
    }
}

void GameHUD::resetPlayerStats(const Player* player)
{
    value_health=player->health;
    value_lifes=player->lifes;
    value_points=player->points;
    value_energy=player->energylevel;
    value_oxygen=player->air;
    maxair=player->maxair;
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

static void drawRect(ppl7::grafix::Drawable& draw)
{
    draw.cls(ppl7::grafix::Color(1, 1, 1, 128));
    ppl7::grafix::Color black(0, 0, 0, 255);
    ppl7::grafix::Color white(255, 255, 255, 255);
    draw.drawRect(0, 0, draw.width(), draw.height(), black);
    draw.drawRect(1, 1, draw.width() - 1, draw.height() - 1, black);
    draw.drawRect(2, 2, draw.width() - 2, draw.height() - 2, white);
    draw.drawRect(3, 3, draw.width() - 3, draw.height() - 3, white);
}

void GameHUD::drawLeftPart(ppl7::grafix::Drawable& draw)
{
    drawRect(draw);
    ppl7::grafix::Color black(0, 0, 0, 255);
    ppl7::grafix::Color white(255, 255, 255, 255);
    label_font.setColor(ppl7::grafix::Color(210, 210, 210, 255));
    label_font.setBold(false);
    label_font.setSize(20);

    bool showOxygen=false;
    bool showEnergy=false;
    if (value_oxygen<maxair || oxygen_cooldown>time) showOxygen=true;
    if (number_batteries > 0 || value_energy < 100.0f) showEnergy=true;

    int count_items=1;
    if (showOxygen) count_items++;
    if (showEnergy) count_items++;

    int maxwidth=0;
    int y=8;

    if (count_items == 1) y=40;
    if (count_items == 2) y=20;



    ppl7::String labeltext=translate("Health:");
    ppl7::grafix::Size s=label_font.measure(labeltext);
    int lineheight=30;
    int y_health=y;
    maxwidth=s.width;
    icons->draw(draw, 25, y + lineheight, 0);
    draw.print(label_font, 50, y, labeltext);
    y+=lineheight;

    int y_energy=y;
    if (showEnergy) {
        labeltext=translate("Energy:");
        s=label_font.measure(labeltext);
        if (s.width > maxwidth) maxwidth=s.width;
        icons->draw(draw, 25, y + lineheight, 2);
        draw.print(label_font, 50, y, labeltext);
        y+=lineheight;
    }

    int y_oxygen=y;
    if (showOxygen) {
        labeltext=translate("Oxygen:");
        s=label_font.measure(labeltext);
        if (s.width > maxwidth) maxwidth=s.width;
        icons->draw(draw, 25, y + lineheight, 1);
        draw.print(label_font, 50, y, labeltext);
    }

    int w=draw.width() - 90 - 60 - maxwidth;

    drawProgressBar(draw, 60 + maxwidth, y_health, w, 20, value_health, ppl7::grafix::Color(220, 15, 0, 255));
    if (showEnergy) drawProgressBar(draw, 60 + maxwidth, y_energy, w, 20, value_energy, ppl7::grafix::Color(15, 150, 20, 255));
    if (showOxygen) drawProgressBar(draw, 60 + maxwidth, y_oxygen, w, 20, value_oxygen * 100.0f / maxair, ppl7::grafix::Color(15, 60, 220, 255));


    label_font.setColor(ppl7::grafix::Color(255, 220, 0, 255));
    int x=70 + maxwidth + w;
    ppl7::WideString value;
    value.setf("%d %%", (int)value_health);
    draw.print(label_font, x, y_health, value);

    if (showEnergy) {
        value.setf("%d %%", (int)value_energy);
        draw.print(label_font, x, y_energy, value);
    }

    if (showOxygen) {
        value.setf("%d s", (int)value_oxygen);
        draw.print(label_font, x, y_oxygen, value);
    }

    //value.setf("%0.0f s", seconds_left);

}

void GameHUD::drawMiddlePart(ppl7::grafix::Drawable& draw)
{
    drawRect(draw);
    int y=10;


    label_font.setColor(ppl7::grafix::Color(210, 210, 210, 255));
    label_font.setBold(false);
    label_font.setSize(30);

    ppl7::String labeltext=translate("Lifes:");
    ppl7::grafix::Size s=label_font.measure(labeltext);

    if (number_batteries == 0) y=(hud_size.height - s.height) / 2;
    else y=(hud_size.height - 2 * s.height) / 2;



    int y_lifes=y;
    int maxwidth=s.width;
    draw.print(label_font, 10, y, labeltext);
    y+=40;
    int y_powercells=0;
    if (number_batteries > 0) {
        labeltext=translate("Batteries:");
        ppl7::grafix::Size s=label_font.measure(labeltext);
        y_powercells=y;
        if (s.width > maxwidth) maxwidth=s.width;
        draw.print(label_font, 10, y, labeltext);
    }

    int x=10 + 20 + maxwidth;
    for (int i=0;i < value_lifes;i++) {
        if (x + i * 30 + 20 < draw.width()) icons->draw(draw, x + i * 30, y_lifes + 31, 3);
    }

    for (int i=0;i < number_batteries;i++) {
        if (x + i * 30 + 20 < draw.width()) icons->draw(draw, x + i * 30, y_powercells + 31, 4);
    }

}

void GameHUD::drawPoints(ppl7::grafix::Drawable& draw)
{
    drawRect(draw);
    int y=0;

    label_font.setColor(ppl7::grafix::Color(210, 210, 210, 255));
    label_font.setBold(false);
    label_font.setSize(30);

    ppl7::String labeltext=translate("Points:");
    ppl7::grafix::Size s=label_font.measure(labeltext);
    y=(hud_size.height - s.height) / 2;
    draw.print(label_font, 15, y, labeltext);
    int x=15 + s.width + 10;

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
    redraw_needed=false;
    ppl7::grafix::Drawable draw=sdl.lockTexture(hud_texture);

    if (my_viewport != last_drawn_viewport) draw.cls();


    ppl7::grafix::Drawable fragment;
    if (my_viewport.left() > 0) {
        fragment=draw.getDrawable(my_viewport.left() + 10, 0, my_viewport.left() + 600, 110);
        drawLeftPart(fragment);
        fragment=draw.getDrawable(my_viewport.left() + 650, 0, 1400, 110);
        drawMiddlePart(fragment);
        fragment=draw.getDrawable(1450, 0, 1910, 110);
        drawPoints(fragment);
    } else {
        fragment=draw.getDrawable(10, 0, 800, 110);
        drawLeftPart(fragment);
        fragment=draw.getDrawable(850, 0, 1400, 110);
        drawMiddlePart(fragment);
        fragment=draw.getDrawable(1450, 0, 1910, 110);
        drawPoints(fragment);
    }
    last_drawn_viewport=my_viewport;

    sdl.unlockTexture(hud_texture);

}


void GameHUD::draw(SDL_Renderer* renderer, SDL_Texture* render_target, const SDL_Rect& render_rect)
{
    if (!visible) return;
    time=ppl7::GetMicrotime();
    if (oxygen_cooldown < time && oxygen_cooldown != 0.0f) {
        ppl7::PrintDebug("Oxygen Cooldown\n");
        redraw_needed=true;
        oxygen_cooldown=0.0f;
    }
    if (redraw_needed || last_drawn_viewport != my_viewport) redraw();
    //ppl7::PrintDebug("GameHUD::draw\n");
    SDL_Rect tr;
    //tr.y=tr.h - hud_size.height;
    tr.x=0;
    tr.w=1920;
    tr.y= my_viewport.y2 - hud_size.height - 5;
    tr.h=hud_size.height;
    //ppl7::PrintDebug("my_viewport.y2=%d\n", my_viewport.y2);

    SDL_SetRenderTarget(renderer, render_target);
    SDL_RenderCopy(renderer, hud_texture, NULL, &tr);
}
