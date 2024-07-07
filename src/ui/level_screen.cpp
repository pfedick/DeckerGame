#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"

LevelSelectScreen::LevelSelectScreen(Game& g, int x, int y, int width, int height)
    : game(g)
{
    create(x, y, width, height);
    setClientOffset(10, 10, 10, 10);
    ppltk::GetWindowManager()->setGameControllerFocus(this);
    ppltk::GetWindowManager()->setKeyboardFocus(this);

    levelselection=new Decker::ui::LevelSelection(300, 0, width - 300, height - 20);

    addChild(levelselection);

    back_button=new Decker::ui::GameMenuArea(0, height - 100, 280, 80, translate("Back (ESC)"));
    back_button->setEventHandler(this);
    addChild(back_button);

    std::list<LevelDescription> level_list;
    getLevelList(level_list);
    for (auto it=level_list.begin();it != level_list.end();++it) {
        if (it->visibleInLevelSelection) levelselection->addLevel((*it), false);
    }
    for (auto it=level_list.begin();it != level_list.end();++it) {
        if (!it->visibleInLevelSelection) levelselection->addLevel((*it), false);
    }


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
    if (event->widget() == back_button) {
        back_button->setSelected(true);
        //levelselection->setSelected(false);
    }
}

void LevelSelectScreen::mouseLeaveEvent(ppltk::MouseEvent* event)
{
    if (event->widget() == back_button) {
        back_button->setSelected(false);
    }
}

void LevelSelectScreen::mouseClickEvent(ppltk::MouseEvent* event)
{
    if (event->widget() == back_button) {
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        this->getParent()->closeEvent(&event);
    }
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
