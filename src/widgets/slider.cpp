#include "decker.h"
#include "ui.h"

namespace Decker::ui {


Slider::Slider(int x, int y, int width, int height)
    : ppl7::tk::Widget()
{
    create(x, y, width, height);
    setClientOffset(0, 0, 0, 0);
    min=0;
    max=100;
    current_value=0;
    my_steps=20;
}


void Slider::setMinimum(int value)
{
    min=value;
}

void Slider::setMaximum(int value)
{
    max=value;
}

void Slider::setSteps(int value)
{
    my_steps=value;
}

void Slider::setDimension(int min, int max)
{
    this->min=min;
    this->max=max;
}

void Slider::setValue(int value)
{
    if (value >= min && value <= max) current_value=value;
    if (value < min) current_value=min;
    if (value > max) current_value=max;
    parentMustRedraw();
    needsRedraw();
}

int Slider::value() const
{
    return current_value;
}

int Slider::minimum() const
{
    return min;
}

int Slider::maximum() const
{
    return max;
}

int Slider::steps() const
{
    return my_steps;
}

int Slider::stepSize() const
{
    int step=(max - min) / my_steps;
    if (step < 1) step=1;
    return step;
}


HorizontalSlider::HorizontalSlider(int x, int y, int width, int height)
    : Slider(x, y, width, height)
{
    drag_started=false;
}

HorizontalSlider::~HorizontalSlider()
{
    if (drag_started) {
        drag_started=false;
        ppl7::tk::GetWindowManager()->releaseMouse(this);
    }
}


void HorizontalSlider::paint(ppl7::grafix::Drawable& draw)
{
    const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();


    int y1=draw.height() * 1 / 5;
    int y2=draw.height() * 4 / 5;
    int h=y2 - y1;
    int draw_range=draw.width() - h;
    int slider_range=maximum() - minimum();
    int x1=(value() - minimum()) * draw_range / slider_range;
    int x2=x1 + h;
    slider_pos.setRect(x1, y1, h, h);


    y1=draw.height() * 2 / 5;
    y2=draw.height() * 3 / 5;

    draw.fillRect(0, y1, draw.width(), y2, style.buttonBackgroundColor);
    draw.fillRect(0, y1, x1, y2, style.sliderHighlightColor);
    draw.drawRect(0, y1, draw.width(), y2, style.frameBorderColorLight);

    y1=draw.height() * 1 / 5;
    y2=draw.height() * 4 / 5;


    draw.fillRect(x1, y1, x2, y2, style.windowBackgroundColor);
    draw.drawRect(x1, y1, x2, y2, style.frameBorderColorLight);


}

void HorizontalSlider::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
    if (event->buttonMask && ppl7::tk::MouseEvent::MouseButton::Left) {
        //printf("HorizontalSlider::mouseDownEvent: %d, %d\n", event->p.x, event->p.y);
        if (event->p.inside(slider_pos)) {
            drag_started=true;
            drag_offset=event->p.x - slider_pos.x1;
            drag_start_pos=event->p;
            ppl7::tk::GetWindowManager()->grabMouse(this);
        } else if (event->p.x < slider_pos.x1) {
            setValue(value() - stepSize());
            ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
            ev.setWidget(this);
            valueChangedEvent(&ev, value());
        } else if (event->p.x > slider_pos.x2) {
            setValue(value() + stepSize());
            ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
            ev.setWidget(this);
            valueChangedEvent(&ev, value());
        }
    }
}

void HorizontalSlider::lostFocusEvent(ppl7::tk::FocusEvent* event)
{
    if (drag_started) {
        drag_started=false;
        ppl7::tk::GetWindowManager()->releaseMouse(this);
    }
}

void HorizontalSlider::mouseUpEvent(ppl7::tk::MouseEvent* event)
{
    if (drag_started) {
        drag_started=false;
        ppl7::tk::GetWindowManager()->releaseMouse(this);
    }
}

void HorizontalSlider::mouseMoveEvent(ppl7::tk::MouseEvent* event)
{
    if (event->buttonMask && ppl7::tk::MouseEvent::MouseButton::Left) {
        if (drag_started) {
            int y1=height() * 1 / 5;
            int y2=height() * 4 / 5;
            int h=y2 - y1;
            int draw_range=width() - h;
            int v=minimum() + (event->p.x - drag_offset) * (maximum() - minimum()) / draw_range;
            setValue(v);
            ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
            ev.setWidget(this);
            valueChangedEvent(&ev, value());

        }
    } else if (drag_started) {
        drag_started=false;
        ppl7::tk::GetWindowManager()->releaseMouse(this);
    }

}

void HorizontalSlider::mouseWheelEvent(ppl7::tk::MouseEvent* event)
{
    if (event->wheel.y < 0 && value()>minimum()) {
        setValue(value() - stepSize());
        ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
        ev.setWidget(this);
        valueChangedEvent(&ev, value());
    } else if (event->wheel.y > 0 && value() < maximum()) {
        setValue(value() + stepSize());
        ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
        ev.setWidget(this);
        valueChangedEvent(&ev, value());
    }
}

}   // EOF namespace
