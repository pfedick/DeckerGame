#include <stdio.h>
#include <stdlib.h>

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "widgets.h"


namespace Decker::ui {

GradientWidget::GradientWidget(int x, int y, int width, int height)
    : ppl7::tk::Widget(x, y, width, height)
{
    max_items=8;
    max_id=0;
    selected_id=0;
}

GradientWidget::~GradientWidget()
{


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
    ColorMap cm=getSorted();
    ppl7::grafix::Color c1, c2;



}

void GradientWidget::addItem(float age, const ppl7::grafix::Color& color)
{
    if (age < 0.0f) age=0.0f;
    if (age > 1.0f) age=1.0f;
    if (items.size() >= max_items) return;
    Item new_item;
    new_item.age=age;
    new_item.color=color;
    max_id++;
    items.insert(std::pair<size_t, Item>(max_id, new_item));
    selected_id=max_id;
    needsRedraw();

}

GradientWidget::ColorMap GradientWidget::getSorted() const
{
    GradientWidget::ColorMap cm;
    std::map<size_t, Item>::const_iterator it;
    for (it=items.begin();it != items.end();++it) {
        cm.insert(std::pair<float, ppl7::grafix::Color>(it->second.age, it->second.color));
    }
    return cm;
}

float GradientWidget::currentAge() const
{
    if (selected_id) {
        std::map<size_t, Item>::const_iterator it=items.find(selected_id);
        if (it != items.end()) return it->second.age;
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
    if (selected_id) {
        std::map<size_t, Item>::iterator it=items.find(selected_id);
        if (it != items.end()) {
            it->second.age=age;
            needsRedraw();
        }
    }
}

void GradientWidget::setCurrentColor(const ppl7::grafix::Color& color)
{
    if (selected_id) {
        std::map<size_t, Item>::iterator it=items.find(selected_id);
        if (it != items.end()) {
            it->second.color=color;
            needsRedraw();
        }
    }

}

void GradientWidget::paint(ppl7::grafix::Drawable& draw)
{
    const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
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
    ppl7::grafix::Drawable d=draw.getDrawable(11, 11, hw + 10, h - 11);
    drawCheckboard(d);
    drawGradient(d);



}

void GradientWidget::mouseDownEvent(ppl7::tk::MouseEvent* event)
{

}

void GradientWidget::mouseUpEvent(ppl7::tk::MouseEvent* event)
{

}

void GradientWidget::lostFocusEvent(ppl7::tk::FocusEvent* event)
{

}

void GradientWidget::mouseMoveEvent(ppl7::tk::MouseEvent* event)
{

}

}   // EOF namespace
