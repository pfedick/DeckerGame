#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"
#include "objects.h"
#include "player.h"


GameStatsScreen::GameStatsScreen(Game& g, int x, int y, int width, int height, StatsScreenReason reason)
    : game(g)
{
    this->reason=reason;
    closeWindow=false;
    create(x, y, width, height);
    updateStyles();

    ppl7::String text=translate("Game over");
    if (reason == StatsScreenReason::LevelEnd) {
        text=translate("Level completed");
    }

    ppl7::tk::Label* label=new ppl7::tk::Label(10, 10, style_heading.size.width, style_heading.size.height, text);
    label->setFont(style_heading.font);
    ppl7::grafix::Size size=style_heading.font.measure(text);
    label->setPos((width - size.width) / 2, 10);
    this->addChild(label);


    text=translate("Press space to continue");
    size=style_anykey.font.measure(text);
    label=new ppl7::tk::Label(10, 10, size.width + 10, size.height + 10, text);
    label->setFont(style_anykey.font);

    //ppl7::PrintDebugTime("height: %d, size.height=%d, y=%d\n", height, size.height, height - size.height - 40);
    label->setPos((width - size.width) / 2, height - size.height - 10);
    this->addChild(label);
}


GameStatsScreen::~GameStatsScreen()
{

}

void GameStatsScreen::updateStyles()
{
    const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
    ppl7::grafix::Font font=style.buttonFont;
    font.setName("NotoSansBlack");
    font.setBold(false);
    font.setSize(12);
    font.setOrientation(ppl7::grafix::Font::TOP);
    font.setColor(ppl7::grafix::Color(255, 255, 255, 255));
    font.setBorderColor(ppl7::grafix::Color(0, 0, 0, 255));
    font.setDrawBorder(true);

    style_heading.font=font;
    style_heading.font.setSize(64);
    style_heading.size.setSize(width() - 20, 90);
    style_heading.total.setSize(width() - 20, 100);

    x1=20;
    x2=width() / 5 * 2;
    x3=width() / 5 * 3;
    style_label.font=font;
    style_label.font.setSize(40);

    style_anykey.font=font;
    style_anykey.font.setSize(20);

    if (height() < 600 || width() < 1280) {
        style_heading.font.setSize(40);
        style_heading.size.setSize(width() - 20, 70);
        style_heading.total.setSize(width() - 20, 80);

        style_label.font.setSize(20);
        style_anykey.font.setSize(16);
    }
    if (height() < 500 || width() < 1024) {
        style_heading.font.setSize(30);
        style_heading.size.setSize(width() - 20, 40);
        style_heading.total.setSize(width() - 20, 50);

        style_label.font.setName("NotoSans");
        style_label.font.setSize(16);
        style_label.font.setBold(true);
        style_anykey.font.setName("NotoSans");
        style_anykey.font.setBold(true);
        style_anykey.font.setSize(12);
    }
    y=10 + style_heading.font.size() * 2;

}


void GameStatsScreen::paint(ppl7::grafix::Drawable& draw)
{
    ppl7::grafix::Color bg(30, 10, 0, 255);
    ppl7::grafix::Color border(255, 200, 0, 255);
    ppl7::grafix::Color fg(128, 128, 128, 255);
    draw.cls(bg);
    int h=draw.height();
    int w=draw.width();
    for (int y=0;y < h;y++) {
        int hue=y * 200 / draw.height();
        if (reason == StatsScreenReason::PlayerDied) {
            bg.setColor(hue, 0, 0, 255);
        } else {
            bg.setColor(0, hue, 0, 255);
        }
        draw.line(0, y, w, y, bg);
    }
    for (int i=0;i < 6;i++) {
        draw.drawRect(i, i, draw.width() - i, draw.height() - i, border);
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

void GameStatsScreen::addResult(const ppl7::String& caption, int object_type, size_t max, size_t achived, size_t bonus)
{
    ppl7::grafix::Size size=style_label.font.measure(caption);

    Resources& res=getResources();
    int sprite_no=0;
    switch (object_type) {
    case Decker::Objects::Type::Crystal: sprite_no=0; break;
    case Decker::Objects::Type::Diamond: sprite_no=1; break;
    case Decker::Objects::Type::Coin: sprite_no=12; break;
    case Decker::Objects::Type::ExtraLife: sprite_no=40; break;
    case Decker::Objects::Type::TreasureChest: sprite_no=33; break;
    case Decker::Objects::Type::Apple: sprite_no=42; break;
    case Decker::Objects::Type::Cherry: sprite_no=45; break;
    default: sprite_no=0; break;
    }

    ppl7::tk::Label* label=new ppl7::tk::Label(x1, y, 72, 72);

    ppl7::grafix::Image img(128, 128);
    ppl7::grafix::Drawable dr=res.uiObjects.getDrawable(sprite_no);
    if (dr.width() > 72 || dr.height() > 72) {
        img.cls();
        img.blt(dr, (128 - dr.width()) / 2, (128 - dr.height()) / 2);
        ppl7::grafix::Image icon=img.scaled(72, 72, true, true);
        label->setIcon(icon);
    } else {
        label->setIcon(dr);
    }
    this->addChild(label);

    label=new ppl7::tk::Label(x1 + 128, y, size.width + 10, 72, caption);
    label->setFont(style_label.font);
    this->addChild(label);

    ppl7::String text;
    text.setf("%zd of %zd", achived, max, achived * 100 / max);
    size=style_label.font.measure(text);
    //style_label.font.setMonospace(true);
    label=new ppl7::tk::Label(x2, y, size.width + 10, 72, text);
    label->setFont(style_label.font);
    this->addChild(label);
    //style_label.font.setMonospace(false);

    text.setf("(%zd %%)", achived * 100 / max);
    if (bonus > 0) {
        text+=" " + translate("Bonus:");
        text.appendf(" %zd ", bonus);
        text+=translate("Points");
    }
    size=style_label.font.measure(text);
    label=new ppl7::tk::Label(x3, y, size.width + 10, 72, text);
    label->setFont(style_label.font);
    this->addChild(label);

    y+=72;

}

void GameStatsScreen::addTotal(const ppl7::String& caption, int points)
{
    ppl7::grafix::Size size=style_label.font.measure(caption);
    ppl7::tk::Label* label=new ppl7::tk::Label(10, y, size.width + 10, 72, caption);
    label->setFont(style_label.font);
    this->addChild(label);

    ppl7::String text;
    text.setf("%d", points);
    size=style_label.font.measure(text);
    //style_label.font.setMonospace(true);
    label=new ppl7::tk::Label(x2, y, size.width + 10, 72, text);
    label->setFont(style_label.font);
    this->addChild(label);
    y+=72;
}

void GameStatsScreen::addLine(const ppl7::String& caption)
{
    ppl7::grafix::Size size=style_label.font.measure(caption);
    ppl7::tk::Label* label=new ppl7::tk::Label(10, y, size.width + 10, 72, caption);
    label->setFont(style_label.font);
    this->addChild(label);
    y+=72;
}


void Game::showStatsScreen(StatsScreenReason reason)
{
    world_widget->setVisible(false);
    world_widget->setEnabled(false);
    ppl7::grafix::Color black(0, 0, 0, 255);
    SDL_Renderer* renderer=sdl.getRenderer();
    bool save_ui_state=this->showui;
    showUi(false);
    float fade_to_black=255.0f;
    int fade_state=0;
    game_stats_screen=new GameStatsScreen(*this,
        100, 100, this->width() - 200, this->height() - 200, reason);
    this->addChild(game_stats_screen);
    this->needsRedraw();
    LevelStats stats;
    level.getLevelStats(stats);

    enableControls(false);
    last_frame_time=0.0f;
    double next_result=0;
    int line=-1;
    int bonus=0;
    int total_bonus =0;
    while (1) {
        double now=ppl7::GetMicrotime();
        float frame_rate_compensation=1.0f;
        if (last_frame_time > 0.0f) {
            float frametime=now - last_frame_time;
            frame_rate_compensation=frametime / (1.0f / 60.0f);
        }
        last_frame_time=now;
        if (next_result < now && fade_state == 1) {
            if (line < 0) {
                game_stats_screen->addLine(translate("You collected:"));

            } else if (line == 0 && stats.getObjectCount(Decker::Objects::Type::Coin) > 0) {
                if (stats.getObjectCount(Decker::Objects::Type::Coin) == player->getObjectCount(Decker::Objects::Type::Coin)) {
                    bonus=(player->points * 0.1f);
                    total_bonus+=bonus;
                }
                game_stats_screen->addResult(translate("Coins:"), Decker::Objects::Type::Coin,
                    stats.getObjectCount(Decker::Objects::Type::Coin),
                    player->getObjectCount(Decker::Objects::Type::Coin), bonus);
            } else if (line == 1 && stats.getObjectCount(Decker::Objects::Type::Diamond) > 0) {
                if (stats.getObjectCount(Decker::Objects::Type::Diamond) == player->getObjectCount(Decker::Objects::Type::Diamond)) {
                    bonus=(player->points * 0.2f);
                    total_bonus+=bonus;
                }
                game_stats_screen->addResult(translate("Diamonds:"), Decker::Objects::Type::Diamond,
                    stats.getObjectCount(Decker::Objects::Type::Diamond),
                    player->getObjectCount(Decker::Objects::Type::Diamond), bonus);
            } else if (line == 2 && stats.getObjectCount(Decker::Objects::Type::Crystal) > 0) {
                if (stats.getObjectCount(Decker::Objects::Type::Crystal) == player->getObjectCount(Decker::Objects::Type::Crystal)) {
                    bonus=(player->points * 0.6f);
                    total_bonus+=bonus;
                }
                game_stats_screen->addResult(translate("Chrystals:"), Decker::Objects::Type::Crystal,
                    stats.getObjectCount(Decker::Objects::Type::Crystal),
                    player->getObjectCount(Decker::Objects::Type::Crystal), bonus);
            } else if (line == 3 && stats.getObjectCount(Decker::Objects::Type::TreasureChest) > 0) {
                if (stats.getObjectCount(Decker::Objects::Type::TreasureChest) == player->getObjectCount(Decker::Objects::Type::TreasureChest)) {
                    bonus=(player->points * 0.8f);
                    total_bonus+=bonus;
                }
                game_stats_screen->addResult(translate("Treasure Chests:"), Decker::Objects::Type::TreasureChest,
                    stats.getObjectCount(Decker::Objects::Type::TreasureChest),
                    player->getObjectCount(Decker::Objects::Type::TreasureChest), bonus);
            } else if (line == 4 && stats.getObjectCount(Decker::Objects::Type::ExtraLife) > 0) {
                game_stats_screen->addResult(translate("Extra Lives:"), Decker::Objects::Type::ExtraLife,
                    stats.getObjectCount(Decker::Objects::Type::ExtraLife),
                    player->getObjectCount(Decker::Objects::Type::ExtraLife));
            } else if (line == 5 && bonus > 0) {
                game_stats_screen->addTotal(translate("Bonus Points:"), total_bonus);
                player->points+=total_bonus;
            } else if (line == 6) {
                game_stats_screen->addTotal(translate("Total Points:"), player->points);
            }


            line++;
            next_result=now + 0.5f;
        }

        wm->handleEvents();
        sdl.startFrame(black);
        ppl7::tk::MouseState mouse=wm->getMouseState();
        drawWidgets();
        resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
        if (fade_state == 0) {
            fade_to_black-=2 * frame_rate_compensation;
            if (fade_to_black <= 0) {
                fade_state=1;
                fade_to_black=0.0f;
            }
        } else if (fade_state == 2) {
            fade_to_black+=5 * frame_rate_compensation;
            if (fade_to_black >= 255) {
                fade_state=3;
                fade_to_black=255.0f;
            }
        }
        if (fade_to_black) FadeToBlack(renderer, (int)fade_to_black);
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
