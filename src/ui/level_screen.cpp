#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"

LevelSelectScreen::LevelSelectScreen(Game& g, int x, int y, int width, int height)
    : game(g)
{
    create(x, y, width, height);
    ppltk::GetWindowManager()->setGameControllerFocus(this);
    ppltk::GetWindowManager()->setKeyboardFocus(this);


}


LevelSelectScreen::~LevelSelectScreen()
{

}


void LevelSelectScreen::paint(ppl7::grafix::Drawable& draw)
{
    ppl7::grafix::Color bg(20, 10, 0, 192);
    ppl7::grafix::Color border(255, 200, 0, 255);
    ppl7::grafix::Color fg(128, 128, 128, 255);
    draw.cls(bg);
    return;
    for (int i=0;i < 6;i++) {
        draw.drawRect(i, i, draw.width() - i, draw.height() - i, border);
    }
}


void LevelSelectScreen::keyDownEvent(ppltk::KeyEvent* event)
{
    int key=event->key;
    if (key == ppltk::KeyEvent::KEY_ESCAPE) {
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        this->getParent()->closeEvent(&event);
    }
}

void LevelSelectScreen::mouseEnterEvent(ppltk::MouseEvent* event)
{

}

void LevelSelectScreen::mouseClickEvent(ppltk::MouseEvent* event)
{

}

void LevelSelectScreen::gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event)
{
    GameControllerMapping::Button b=game.controller.mapping.getButton(event);
    if (b == GameControllerMapping::Button::Menu) {
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        this->getParent()->closeEvent(&event);
    }

}

void LevelSelectScreen::gameControllerAxisMotionEvent(ppltk::GameControllerAxisEvent* event)
{

}

void LevelSelectScreen::gameControllerDeviceAdded(ppltk::GameControllerEvent* event)
{
    GetGame().gameControllerDeviceAdded(event);
}

void LevelSelectScreen::gameControllerDeviceRemoved(ppltk::GameControllerEvent* event)
{
    GetGame().gameControllerDeviceRemoved(event);
}
