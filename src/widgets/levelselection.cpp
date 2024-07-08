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
    current_selection=0;

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
    LevelSelectionItem* item=new LevelSelectionItem(this, descr, hidden);
    item->setEventHandler(this);
    if (item_list.size() == 0) item->selected=true;
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
            //ppl7::PrintDebug("Level clicked: %s\n", (const char*)(*it)->filename);
            ppltk::Event event(ppltk::Event::Close);
            event.setWidget(this);
            this->getParent()->textChangedEvent(&event, (*it)->filename);

        }
    }
}

void LevelSelection::clearSelection()
{
    for (auto it=item_list.begin();it != item_list.end();++it) {
        (*it)->selected=false;
    }
    current_selection=0;
    needsRedraw();
}

void LevelSelection::selectionChangedEvent(ppltk::Event* event)
{
    int c=0;
    for (auto it=item_list.begin();it != item_list.end();++it) {
        if ((*it)->selected) current_selection=c;
        c++;
    }
}

void LevelSelection::setSelection(int item)
{
    if (item < 0) item=0;
    if (item >= (int)item_list.size()) item=(int)item_list.size() - 1;
    clearSelection();
    current_selection=item;
    int c=0;
    for (auto it=item_list.begin();it != item_list.end();++it) {
        if (c == current_selection) (*it)->selected=true;
        else (*it)->selected=false;
        c++;
    }
    scrollbar->makeVisible(item);
}

int LevelSelection::currentSelection() const
{
    return current_selection;
}

ppl7::String LevelSelection::getSelectedFilename() const
{
    for (auto it=item_list.begin();it != item_list.end();++it) {
        if ((*it)->selected) {
            return (*it)->filename;
        }
    }
    return ppl7::String();
}

}	// EOF namespace ui
}	// EOF namespace Decker
