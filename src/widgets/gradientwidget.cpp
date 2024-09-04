#include <stdio.h>
#include <stdlib.h>

#include <ppl7.h>
#include <ppl7-grafix.h>
#include "widgets.h"


namespace Decker::ui {

GradientWidget::Item::Item()
{
    age=0.0f;
}

GradientWidget::GradientWidget(int x, int y, int width, int height)
    : ppltk::Widget(x, y, width, height)
{
    //ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
    ppltk::WindowManager* wm=ppltk::GetWindowManager();
    max_items=8;
    max_id=0;
    selected_id=0;
    drag_started=false;
    drag_offset=0;
    gradient_y1=0;
    gradient_y2=0;
    add_item_button=new ppltk::Button(width - 32, 2, 30, 30, "", wm->Toolbar.getDrawable(43));
    add_item_button->setEventHandler(this);
    addChild(add_item_button);
    delete_item_button=new ppltk::Button(width - 32, height - 32, 30, 30, "", wm->Toolbar.getDrawable(44));
    delete_item_button->setEventHandler(this);
    addChild(delete_item_button);
}

GradientWidget::~GradientWidget()
{
    if (drag_started) {
        drag_started=false;
        ppltk::GetWindowManager()->releaseMouse(this);
    }

}

ppl7::String GradientWidget::widgetType() const
{
    return "GradientWidget";
}

void GradientWidget::clear()
{
    items.clear();
    max_id=0;
    selected_id=0;
    needsRedraw();
}

void GradientWidget::drawCheckboard(ppl7::grafix::Drawable& draw)
{
    if (draw.width() != gradient_bg.width() || draw.height() != gradient_bg.height()) {
        ppl7::grafix::Color color[2];
        color[0].set(0, 0, 0, 255);
        color[1].set(240, 240, 240, 255);
        gradient_bg.create(draw.width(), draw.height());
        int r=0;
        for (int y=0; y < gradient_bg.height();y+=16) {
            int c=r;
            for (int x=0; x < gradient_bg.width();x+=16) {
                gradient_bg.fillRect(x, y, x + 16, y + 16, color[c & 1]);
                c++;
            }
            r++;
        }
    }

    draw.blt(gradient_bg);
}

void GradientWidget::drawGradient(ppl7::grafix::Drawable& draw)
{
    if (draw.width() != gradient_vg.width() || draw.height() != gradient_vg.height()) {
        gradient_vg.create(draw.width(), draw.height());
    }
    gradient_vg.cls();
    std::list<Item> cl=getSortedList();
    if (!cl.empty()) {
        int h=draw.height();
        int w=draw.width();
        Item i1, i2;
        i1=cl.front();
        cl.pop_front();
        if (i1.age == 0.0f) {
            if (cl.size() > 0) {
                i2=cl.front();
                cl.pop_front();
            } else {
                i2=i1;
                i2.age=1.0;
            }
        } else {
            i2=i1;
            i1.age=0.0f;
        }
        float color_age_diff;
        color_age_diff=i2.age - i1.age;
        for (int y=0;y < h;y++) {
            float age=(float)y / h;
            if (age > i2.age) {
                i1=i2;
                if (cl.size() > 0) {
                    i2=cl.front();
                    cl.pop_front();
                } else {
                    i2.age=1.0f;
                }
                color_age_diff=i2.age - i1.age;
            }
            float n1=1.0f - ((age - i1.age) / color_age_diff);
            float n2=1.0f - ((i2.age - age) / color_age_diff);
            ppl7::grafix::Color c=multiplyWithAlpha(i1.color, n1) + multiplyWithAlpha(i2.color, n2);
            gradient_vg.line(16, y, w, y, c);
            c.setAlpha(255);
            gradient_vg.line(0, y, 16, y, c);
        }
    }
    draw.bltAlpha(gradient_vg);
}

void GradientWidget::addItem(float age, const ppl7::grafix::Color& color, float value)
{
    if (age < 0.0f) age=0.0f;
    if (age > 1.0f) age=1.0f;
    if (items.size() >= max_items) return;
    Item new_item;
    new_item.age=age;
    new_item.color=color;
    new_item.value=value;
    max_id++;
    items.insert(std::pair<size_t, Item>(max_id, new_item));
    selected_id=max_id;
    needsRedraw();

}

std::map<float, ppl7::grafix::Color> GradientWidget::getColorItems() const
{
    std::map<float, ppl7::grafix::Color> cm;
    std::map<size_t, Item>::const_iterator it;
    for (it=items.begin();it != items.end();++it) {
        cm.insert(std::pair<float, ppl7::grafix::Color>(it->second.age, it->second.color));
    }
    return cm;
}

std::map<float, float> GradientWidget::getValueItems() const
{
    std::map<float, float> cm;
    std::map<size_t, Item>::const_iterator it;
    for (it=items.begin();it != items.end();++it) {
        cm.insert(std::pair<float, float>(it->second.age, it->second.value));
    }
    return cm;
}


std::list<GradientWidget::Item> GradientWidget::getSortedList() const
{
    std::list<GradientWidget::Item> cl;
    std::map<size_t, Item>::const_iterator it;
    std::map<float, Item> sorted_list;
    for (it=items.begin();it != items.end();++it) {
        sorted_list.insert(std::pair<float, Item>(it->second.age, it->second));
    }
    std::map<float, Item>::const_iterator sit;
    for (sit=sorted_list.begin();sit != sorted_list.end();++sit) {
        cl.push_back(sit->second);
    }
    return cl;
}

float GradientWidget::currentAge() const
{
    if (selected_id) {
        std::map<size_t, Item>::const_iterator it=items.find(selected_id);
        if (it != items.end()) return it->second.age;
    }
    return 0.0f;
}

float GradientWidget::currentValue() const
{
    if (selected_id) {
        std::map<size_t, Item>::const_iterator it=items.find(selected_id);
        if (it != items.end()) return it->second.value;
    }
    return 0.0f;
}


ppl7::grafix::Color GradientWidget::currentColor() const
{
    if (selected_id) {
        std::map<size_t, Item>::const_iterator it=items.find(selected_id);
        if (it != items.end()) return it->second.color;
    }
    return ppl7::grafix::Color();
}

void GradientWidget::setCurrentAge(float age)
{
    if (age < 0.0f || age>1.0f) return;
    if (selected_id) {
        std::map<size_t, Item>::iterator it=items.find(selected_id);
        if (it != items.end()) {
            if (it->second.age != age) {
                it->second.age=age;
                needsRedraw();
            }
        }
    }
}

void GradientWidget::setCurrentValue(float value)
{
    if (selected_id) {
        std::map<size_t, Item>::iterator it=items.find(selected_id);
        if (it != items.end()) {
            if (it->second.value != value) {
                it->second.value=value;
                needsRedraw();
            }
        }
    }
}

void GradientWidget::setCurrentColor(const ppl7::grafix::Color& color)
{
    if (selected_id) {
        std::map<size_t, Item>::iterator it=items.find(selected_id);
        if (it != items.end()) {
            if (it->second.color != color) {
                it->second.color=color;
                needsRedraw();
            }
        }
    }
}

void GradientWidget::deleteCurrentItem()
{
    if (selected_id) {
        items.erase(selected_id);
        selected_id=-1;
        handler_pos.clear();
        if (items.size() > 0) selected_id=items.begin()->first;
        needsRedraw();
    }
}

void GradientWidget::drawHandlerItem(ppl7::grafix::Drawable& draw, int y, const ppl7::grafix::Color handler_color)
{
    ppl7::grafix::Color light(255, 255, 255, 255);
    ppl7::grafix::Color shadow(32, 32, 32, 255);

    draw.line(11, y, 80, y, light);
    draw.line(11, y + 1, 80, y + 1, shadow);
    draw.fillRect(82, y - 4, 120, y + 4, handler_color);
    draw.line(81, y - 4, 81, y + 5, light);
    draw.line(82, y - 4, 120, y - 4, light);
    draw.line(82, y + 5, 120, y + 5, shadow);
    draw.line(120, y - 4, 120, y + 4, shadow);
}

void GradientWidget::drawHandler(ppl7::grafix::Drawable& draw, int y1, int y2)
{
    handler_pos.clear();
    int selected_y=-1;
    int h=y2 - y1;

    std::map<size_t, Item>::const_iterator it;
    for (it=items.begin();it != items.end();++it) {
        int y=11 + it->second.age * (float)h;
        if (it->first == selected_id) {
            selected_y=y;
        } else {
            drawHandlerItem(draw, y, ppl7::grafix::Color(128, 128, 128, 255));
        }
        handler_pos.insert(std::pair<size_t, int>(it->first, y));
    }
    if (selected_y >= 0) drawHandlerItem(draw, selected_y, ppl7::grafix::Color(210, 210, 220, 255));
}

void GradientWidget::paint(ppl7::grafix::Drawable& draw)
{
    const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
    ppl7::grafix::Color bg=style.frameBackgroundColor * 0.9f;
    ppl7::grafix::Color light=style.frameBackgroundColor * 1.8f;
    ppl7::grafix::Color shadow=style.frameBackgroundColor * 0.4f;
    ppl7::grafix::Font font=style.labelFont;
    font.setColor(style.labelFontColor);
    font.setOrientation(ppl7::grafix::Font::Orientation::TOP);

    int w=draw.width() - 1;
    int h=draw.height() - 1;

    draw.cls(bg);
    draw.line(0, 0, w, 0, shadow);
    draw.line(0, 0, 0, h, shadow);
    draw.line(1, h, w, h, light);
    draw.line(w, 1, w, h, light);

    int hw=64;

    draw.line(9, 9, 11 + hw, 9, shadow);
    draw.line(9, 10, 9, h - 9, shadow);
    draw.line(11, h - 9, hw + 11, h - 9, light);
    draw.line(hw + 12, 10, hw + 12, h - 10, light);
    //draw.drawRect(10,10,10+hw,h-10,)
    gradient_y1=11;
    gradient_y2=h - 11;
    ppl7::grafix::Drawable d=draw.getDrawable(11, 11, hw + 10, h - 11);
    drawCheckboard(d);
    drawGradient(d);
    drawHandler(draw, 11, h - 11);
}

void GradientWidget::mouseDownEvent(ppltk::MouseEvent* event)
{
    std::map<size_t, int>::const_iterator it;
    if (event->widget() == add_item_button) {
        addItem(1.0f, currentColor());
        ppltk::Event ev(ppltk::Event::SelectionChanged);
        ev.setWidget(this);
        EventHandler::selectionChangedEvent(&ev);
        return;

    } else if (event->widget() == delete_item_button) {
        deleteCurrentItem();
        ppltk::Event ev(ppltk::Event::SelectionChanged);
        ev.setWidget(this);
        EventHandler::selectionChangedEvent(&ev);
        return;
    }
    if (event->p.x > 80 && event->p.x < 120) {
        if (selected_id >= 0) {
            it=handler_pos.find(selected_id);
            if (it != handler_pos.end()) {
                if (event->p.y >= it->second - 4 && event->p.y <= it->second + 4) {
                    drag_started=true;
                    drag_offset=event->p.y - it->second + 11;
                    drag_start_pos=event->p;
                    ppltk::GetWindowManager()->grabMouse(this);
                    return;
                }
            }
        }
        for (it=handler_pos.begin();it != handler_pos.end();++it) {
            if (event->p.y >= it->second - 4 && event->p.y <= it->second + 4) {
                selected_id=it->first;
                ppltk::Event ev(ppltk::Event::SelectionChanged);
                ev.setWidget(this);
                EventHandler::selectionChangedEvent(&ev);
                drag_started=true;
                drag_offset=event->p.y - it->second + 11;
                drag_start_pos=event->p;
                ppltk::GetWindowManager()->grabMouse(this);
                needsRedraw();
                return;
            }
        }
    }
}

void GradientWidget::mouseUpEvent(ppltk::MouseEvent* event)
{
    if (drag_started) {
        drag_started=false;
        ppltk::GetWindowManager()->releaseMouse(this);
    }
}

void GradientWidget::lostFocusEvent(ppltk::FocusEvent* event)
{
    if (drag_started) {
        drag_started=false;
        ppltk::GetWindowManager()->releaseMouse(this);
    }
}

void GradientWidget::mouseMoveEvent(ppltk::MouseEvent* event)
{
    if (event->buttonMask & ppltk::MouseEvent::MouseButton::Left) {
        if (drag_started) {
            float draw_range=gradient_y2 - gradient_y1;
            float age=(float)(event->p.y - drag_offset) / draw_range;
            setCurrentAge(age);
            ppltk::Event ev(ppltk::Event::ValueChanged);
            ev.setWidget(this);
            EventHandler::valueChangedEvent(&ev, (int)selected_id);
        }
    } else if (drag_started) {
        drag_started=false;
        ppltk::GetWindowManager()->releaseMouse(this);
    }
}

}   // EOF namespace
