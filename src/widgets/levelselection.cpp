#include <stdio.h>
#include <stdlib.h>

#include "ppltk.h"
#include "widgets.h"



namespace Decker {
namespace ui {

LevelSelection::LevelSelection(int x, int y, int width, int height)
    : ppltk::Widget(x, y, width, height)
{
    setClientOffset(7, 7, 7, 7);
    scrollbar=new GameScrollbar(width - 77, 0, 70, height);
    scrollbar->setEventHandler(this);
    scrollbar->setVisibleItems((height - 14) / 210);
    addChild(scrollbar);
    current_start=0;

}

LevelSelection::~LevelSelection()
{
    for (auto it=item_list.begin();it != item_list.end();++it) {
        this->removeChild((*it));
        delete((*it));
    }
    item_list.clear();

}

void LevelSelection::addLevel(const LevelDescription& descr, bool hidden)
{
    LevelSelectionItem* item=new LevelSelectionItem(descr, hidden);
    item->setEventHandler(this);
    item_list.push_back(item);
    scrollbar->setSize(item_list.size());
    updateChildItems();
    needsRedraw();
}

void LevelSelection::updateChildItems()
{
    ppl7::grafix::Rect client=clientRect();
    int y=0;
    int c=0;
    for (auto it=item_list.begin();it != item_list.end();++it) {
        this->removeChild((*it));
        if (c >= scrollbar->position()) {
            (*it)->setPos(0, y);
            (*it)->setSize(client.width() - 70, 200);
            addChild((*it));
            y+=210;
        }
        c++;
    }
    needsRedraw();
}

void LevelSelection::paint(ppl7::grafix::Drawable& draw)
{
    ppl7::grafix::Color bg(20, 15, 5, 192);
    draw.cls(bg);
    ppl7::grafix::Color border(40, 30, 5, 192);
    for (int i=0;i < 6;i++) {
        draw.drawRect(i, i, draw.width() - i, draw.height() - i, border);
    }
}

void LevelSelection::valueChangedEvent(ppltk::Event* event, int)
{
    if (event->widget() == scrollbar) updateChildItems();
}

void LevelSelection::mouseWheelEvent(ppltk::MouseEvent* event)
{
    scrollbar->mouseWheelEvent(event);
}

void LevelSelection::mouseClickEvent(ppltk::MouseEvent* event)
{
    for (auto it=item_list.begin();it != item_list.end();++it) {
        if (event->widget() == (*it)) {
            ppl7::PrintDebug("Level clicked: %s\n", (const char*)(*it)->filename);
        }
    }
}

}	// EOF namespace ui
}	// EOF namespace Decker
