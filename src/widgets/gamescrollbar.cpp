#include <stdio.h>
#include <stdlib.h>

#include "ppltk.h"
#include "widgets.h"


namespace Decker {
namespace ui {

GameScrollbar::GameScrollbar(int x, int y, int width, int height) // @suppress("Class members should be properly initialized")
    : ppltk::Widget()
{
    this->create(x, y, width, height);
    setClientOffset(0, 0, 0, 0);
    size=0;
    pos=0;
    numVisibleItems=0;
    drag_started=false;
    drag_offset=0;
}

GameScrollbar::~GameScrollbar()
{
    if (drag_started) {
        drag_started=false;
        ppltk::GetWindowManager()->releaseMouse(this);
    }
}

ppl7::String GameScrollbar::widgetType() const
{
    return ppl7::String("Decker::ui::GameScrollbar");
}

void GameScrollbar::setSize(int size)
{
    if (size != this->size && size >= 0) {
        this->size=size;
        if (pos > size) pos=size;
        needsRedraw();
    }
}

void GameScrollbar::setPosition(int position)
{
    if (position != pos && position < size && position >= 0) {
        pos=position;
        needsRedraw();
        ppltk::Event ev(ppltk::Event::ValueChanged);
        ev.setWidget(this);
        valueChangedEvent(&ev, pos);
    }
}

void GameScrollbar::setVisibleItems(int items)
{
    numVisibleItems=items;
    needsRedraw();
}

int GameScrollbar::visibleItems() const
{
    return numVisibleItems;
}

int GameScrollbar::position() const
{
    return pos;
}

void GameScrollbar::paint(ppl7::grafix::Drawable& draw)
{
    const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
    ppl7::grafix::Color light=style.buttonBackgroundColor * 1.8f;
    ppl7::grafix::Color shadow=style.buttonBackgroundColor * 0.4f;
    ppl7::grafix::Color shade1=style.buttonBackgroundColor * 1.05f;
    ppl7::grafix::Color shade2=style.buttonBackgroundColor * 0.85f;
    ppl7::grafix::Drawable indicator=draw;
    int w=indicator.width() - 1;
    //int h=indicator.height()-1;
    ppl7::grafix::Rect r1=indicator.rect();

    if (numVisibleItems > 0 && numVisibleItems < size) {
        float pxi=(float)indicator.height() / (float)size;
        int visible=pxi * numVisibleItems;
        if (visible < 25) visible=25;
        int unvisible=indicator.height() - visible;
        r1.y1=pos * unvisible / (size - numVisibleItems);
        r1.y2=r1.y1 + visible;
        /*
        ppl7::PrintDebug("height: %d, pxi=%0.3f, size=%d, numVisibleItems=%d, visible=%d, unvisible=%d, y1=%d, y2=%d\n",
            indicator.height(),pxi,size,numVisibleItems, visible, unvisible,r1.y1,r1.y2);
        */
        if (r1.y2 >= indicator.height() - 1) r1.y2=indicator.height() - 2;


    }
    slider_pos=r1;
    slider_pos.y1+=23;
    slider_pos.y2+=23;

    ppl7::grafix::Color scrollarea=style.windowBackgroundColor * 1.2f;
    indicator.cls(scrollarea);
    indicator.colorGradient(r1, shade1, shade2, 1);
    indicator.line(0, r1.y1, w, r1.y1, light);
    indicator.line(0, r1.y1, 0, r1.y2, light);
    indicator.line(0, r1.y2, w, r1.y2, shadow);
    indicator.line(w, r1.y1, w, r1.y2, shadow);

    //ppltk::Widget::paint(draw);
    //draw.fillRect(0,y1,draw.width(),y2,style.frameBorderColorLight);
}

void GameScrollbar::mouseDownEvent(ppltk::MouseEvent* event)
{
    if (event->buttonMask & ppltk::MouseEvent::MouseButton::Left) {
        if (event->p.inside(slider_pos)) {
            //ppl7::PrintDebug("HorizontalSlider::mouseDownEvent: %d, %d\n", event->p.x, event->p.y);
            drag_started=true;
            drag_offset=event->p.y - slider_pos.y1;
            drag_start_pos=event->p;
            ppltk::GetWindowManager()->grabMouse(this);
        } else if (event->p.y < slider_pos.y1 && pos>0) {
            int d=numVisibleItems - 1;
            if (d < 1) d=1;
            pos-=d;
            if (pos < 0) pos=0;
            needsRedraw();
            ppltk::Event ev(ppltk::Event::ValueChanged);
            ev.setWidget(this);
            valueChangedEvent(&ev, pos);
        } else if (event->p.y > slider_pos.y2 && pos < size - numVisibleItems) {
            int d=numVisibleItems - 1;
            if (d < 1) d=1;
            pos+=d;
            if (pos >= size) pos=size - numVisibleItems;
            if (pos < 0) pos=0;
            needsRedraw();
            ppltk::Event ev(ppltk::Event::ValueChanged);
            ev.setWidget(this);
            valueChangedEvent(&ev, pos);
        }
    }
}

void GameScrollbar::lostFocusEvent(ppltk::FocusEvent* event)
{
    if (drag_started) {
        drag_started=false;
        ppltk::GetWindowManager()->releaseMouse(this);
    }
}

void GameScrollbar::mouseUpEvent(ppltk::MouseEvent* event)
{
    if (drag_started) {
        drag_started=false;
        ppltk::GetWindowManager()->releaseMouse(this);
    }
}

void GameScrollbar::mouseMoveEvent(ppltk::MouseEvent* event)
{
    if (event->buttonMask & ppltk::MouseEvent::MouseButton::Left) {
        if (drag_started) {
            int draw_range=height() - 46;
            int64_t v=(event->p.y - drag_offset) * size / draw_range;
            if (v >= size - numVisibleItems) v=size - numVisibleItems;
            if (v < 0) v=0;
            pos=v;
            needsRedraw();
            ppltk::Event ev(ppltk::Event::ValueChanged);
            ev.setWidget(this);
            valueChangedEvent(&ev, pos);

        }
    } else if (drag_started) {
        drag_started=false;
        ppltk::GetWindowManager()->releaseMouse(this);
    }
}

void GameScrollbar::mouseWheelEvent(ppltk::MouseEvent* event)
{
    int d=1;
    if (event->keyModifier & ppltk::KeyEvent::KEYMOD_LEFTALT) { //TODO
        d=numVisibleItems - 1;
        if (d < 1) d=1;
    }

    if (event->wheel.y < 0 && pos < size - 1) {
        pos+=d;
        if (pos >= size - numVisibleItems) pos=size - numVisibleItems;
        if (pos < 0) pos=0;
        needsRedraw();
        ppltk::Event ev(ppltk::Event::ValueChanged);
        ev.setWidget(this);
        valueChangedEvent(&ev, pos);
    } else if (event->wheel.y > 0 && pos > 0) {
        pos-=d;
        if (pos < 0) pos=0;
        needsRedraw();
        ppltk::Event ev(ppltk::Event::ValueChanged);
        ev.setWidget(this);
        valueChangedEvent(&ev, pos);

    }
}

void GameScrollbar::makeVisible(int position)
{
    if (position > size) position=size - 1;
    if (position < 0) position=0;
    if (position >= pos && position < pos + numVisibleItems) return;
    while (position >= pos + numVisibleItems) pos++;
    while (position < pos) pos--;

    needsRedraw();
    ppltk::Event ev(ppltk::Event::ValueChanged);
    ev.setWidget(this);
    valueChangedEvent(&ev, pos);
}

}	// EOF namespace ui
}	// EOF namespace Decker
