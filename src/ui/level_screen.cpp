#include <stdio.h>
#include <stdlib.h>
#include <map>

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

    std::multimap<int, LevelDescription> sorted_list;
    for (auto it=level_list.begin();it != level_list.end();++it) {
        if (it->visibleInLevelSelection) {
            //ppl7::PrintDebug("visible: %s\n", (const char*)it->Filename);
            sorted_list.insert(std::pair<int, LevelDescription>(it->levelSort, (*it)));
        }
    }

    // Part of Story
    for (auto it=sorted_list.begin();it != sorted_list.end();++it) {
        //ppl7::PrintDebug("%s\n", (const char*)it->second.Filename);
        if (it->second.partOfStory) levelselection->addLevel(it->second, false);
    }

    // Everything else
    for (auto it=sorted_list.begin();it != sorted_list.end();++it) {
        if (!it->second.partOfStory) levelselection->addLevel(it->second, false);
    }


    /*
    for (auto it=level_list.begin();it != level_list.end();++it) {
        if (!it->visibleInLevelSelection) levelselection->addLevel((*it), false);
    }
    */
    levelselection->setSelection(0);


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
    } else if (key == ppltk::KeyEvent::KEY_DOWN) {
        levelselection->setSelection(levelselection->currentSelection() + 1);
    } else if (key == ppltk::KeyEvent::KEY_UP) {
        levelselection->setSelection(levelselection->currentSelection() - 1);
    } else if (key == ppltk::KeyEvent::KEY_RETURN) {
        selectedLevelFilename=levelselection->getSelectedFilename();
        if (selectedLevelFilename.notEmpty()) {
            ppltk::Event event(ppltk::Event::Close);
            event.setWidget(this);
            this->getParent()->closeEvent(&event);
        }
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
    } else if (b == GameControllerMapping::Button::MenuDown) {
        levelselection->setSelection(levelselection->currentSelection() + 1);
    } else if (b == GameControllerMapping::Button::MenuUp) {
        levelselection->setSelection(levelselection->currentSelection() - 1);
    } else if (b == GameControllerMapping::Button::Action) {
        selectedLevelFilename=levelselection->getSelectedFilename();
        if (selectedLevelFilename.notEmpty()) {
            ppltk::Event event(ppltk::Event::Close);
            event.setWidget(this);
            this->getParent()->closeEvent(&event);
        }
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


void LevelSelectScreen::textChangedEvent(ppltk::Event* event, const ppl7::String& text)
{
    if (event->widget() == levelselection) {
        selectedLevelFilename=text;
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        this->getParent()->closeEvent(&event);
    }
}


ppl7::String LevelSelectScreen::selectedLevel() const
{
    return selectedLevelFilename;
}
