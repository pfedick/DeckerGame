#include <stdio.h>
#include <stdlib.h>

#include "ui.h"

namespace Decker::ui {
ColorSliderWidget::ColorSliderWidget(int x, int y, int width, int height, bool withAlphaChannel)
    : ppltk::Widget(x, y, width, height)
{
    color_red=NULL;
    color_green=NULL;
    color_blue=NULL;
    color_alpha=NULL;
    slider_red=NULL;
    slider_green=NULL;
    slider_blue=NULL;
    slider_alpha=NULL;
    color_preview=NULL;
    with_alphachannel=withAlphaChannel;
    setupUi();
}


void ColorSliderWidget::setupUi()
{
    destroyChilds();
    color_red=NULL;
    color_green=NULL;
    color_blue=NULL;
    slider_red=NULL;
    slider_green=NULL;
    slider_blue=NULL;
    color_preview=NULL;
    ppl7::grafix::Rect client=clientRect();
    int y1=0;
    int col1=0;
    int col2=col1 + 50;
    int col3=col2 + 80;
    int slider_width=client.width() - col3 - 10 - 3 * 35;
    int col4=col3 + slider_width + 10;
    int preview_height=3 * 35;
    if (with_alphachannel)preview_height+=35;
    color_preview=new ppltk::Frame(col4, y1, 3 * 35, preview_height);
    color_preview->setBackgroundColor(ppl7::grafix::Color(0, 0, 0, 255));
    addChild(color_preview);

    // red
    addChild(new ppltk::Label(col1, y1, 50, 30, "red:"));
    color_red=new ppltk::SpinBox(col2, y1, 70, 30, 0);
    color_red->setLimits(0, 255);
    color_red->setEventHandler(this);
    addChild(color_red);
    slider_red=new ppltk::HorizontalSlider(col3, y1, slider_width, 30);
    slider_red->setLimits(0, 255);
    slider_red->setEventHandler(this);
    addChild(slider_red);
    y1+=35;

    // green
    addChild(new ppltk::Label(col1, y1, 50, 30, "green:"));
    color_green=new ppltk::SpinBox(col2, y1, 70, 30, 0);
    color_green->setLimits(0, 255);
    color_green->setEventHandler(this);
    addChild(color_green);
    slider_green=new ppltk::HorizontalSlider(col3, y1, slider_width, 30);
    slider_green->setLimits(0, 255);
    slider_green->setEventHandler(this);
    addChild(slider_green);
    y1+=35;

    // blue
    addChild(new ppltk::Label(col1, y1, 50, 30, "blue:"));
    color_blue=new ppltk::SpinBox(col2, y1, 70, 30, 0);
    color_blue->setLimits(0, 255);
    color_blue->setEventHandler(this);
    addChild(color_blue);
    slider_blue=new ppltk::HorizontalSlider(col3, y1, slider_width, 30);
    slider_blue->setLimits(0, 255);
    slider_blue->setEventHandler(this);
    addChild(slider_blue);
    y1+=35;

    // alpha
    if (with_alphachannel) {
        addChild(new ppltk::Label(col1, y1, 50, 30, "alpha:"));
        color_alpha=new ppltk::SpinBox(col2, y1, 70, 30, 0);
        color_alpha->setLimits(0, 255);
        color_alpha->setEventHandler(this);
        addChild(color_alpha);
        slider_alpha=new ppltk::HorizontalSlider(col3, y1, slider_width, 30);
        slider_alpha->setLimits(0, 255);
        slider_alpha->setEventHandler(this);
        addChild(slider_alpha);
        y1+=35;
    }
}

void ColorSliderWidget::setColorPreviewSize(int w, int h)
{
    ppl7::grafix::Rect client=clientRect();
    int col1=0;
    int col2=col1 + 50;
    int col3=col2 + 80;
    int slider_width=client.width() - col3 - 10 - w;
    int col4=col3 + slider_width + 10;
    color_preview->setX(col4);
    color_preview->setSize(w, h);
    slider_red->setWidth(slider_width);
    slider_green->setWidth(slider_width);
    slider_blue->setWidth(slider_width);
    if (slider_alpha) slider_alpha->setWidth(slider_width);
    needsRedraw();
}

void ColorSliderWidget::setColor(const ppl7::grafix::Color& color)
{
    // we need a copy
    ppl7::grafix::Color c=color;
    //printf("setting Color to %d,%d,%d,%d\n", c.red(), c.green(), c.blue(), c.alpha());
    color_red->setValue(c.red());
    color_green->setValue(c.green());
    color_blue->setValue(c.blue());
    if (color_alpha) color_alpha->setValue(c.alpha());
}

ppl7::grafix::Color ColorSliderWidget::color() const
{
    int alpha=255;
    if (color_alpha) alpha=color_alpha->value();
    return ppl7::grafix::Color(color_red->value(), color_green->value(), color_blue->value(), alpha);
}



void ColorSliderWidget::updateColorPreview()
{
    if (color_preview && color_red && color_green && color_blue) {
        ppl7::grafix::Color color=ppl7::grafix::Color(color_red->value(), color_green->value(), color_blue->value(), 255);
        color_preview->setBackgroundColor(color);
        ppltk::Event new_event(ppltk::Event::ValueChanged);
        new_event.setWidget(this);
        /*
        printf("sending valueChangedEvent to Eventhandler\n");
        printf("color is now %d, %d, %d, %d\n", color.red(),
            color.green(), color.blue(), color.alpha());
        */
        EventHandler::valueChangedEvent(&new_event, 0);
    }
}

ppl7::String ColorSliderWidget::widgetType() const
{
    return "ColorSliderWidget";
}

void ColorSliderWidget::paint(ppl7::grafix::Drawable& draw)
{

}


void ColorSliderWidget::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    ppltk::Widget* widget=event->widget();
    if (widget == slider_red) {
        if (color_red && color_red->value() != slider_red->value()) color_red->setValue(slider_red->value());
        updateColorPreview();
    } else if (widget == slider_green) {
        if (color_green && color_green->value() != slider_green->value()) color_green->setValue(slider_green->value());
        updateColorPreview();
    } else if (widget == slider_blue) {
        if (color_blue && color_blue->value() != slider_blue->value()) color_blue->setValue(slider_blue->value());
        updateColorPreview();
    } else if (widget == slider_alpha) {
        if (color_alpha && color_alpha->value() != slider_alpha->value()) color_alpha->setValue(slider_alpha->value());
        updateColorPreview();
    } else if (widget == color_red) {
        if (slider_red && slider_red->value() != color_red->value()) slider_red->setValue(color_red->value());
        updateColorPreview();
    } else if (widget == color_green) {
        if (slider_green && slider_green->value() != color_green->value()) slider_green->setValue(color_green->value());
        updateColorPreview();
    } else if (widget == color_blue) {
        if (slider_blue && slider_blue->value() != color_blue->value()) slider_blue->setValue(color_blue->value());
        updateColorPreview();
    } else if (widget == color_alpha) {
        if (slider_alpha && slider_alpha->value() != color_alpha->value()) slider_alpha->setValue(color_alpha->value());
        updateColorPreview();
    }
}

void ColorSliderWidget::keyDownEvent(ppltk::KeyEvent* event)
{
    //printf("keyDownEvent: %d, modifier: %04x\n", event->key, event->modifier);
    ppltk::WindowManager* wm=ppltk::GetWindowManager();
    ppltk::Widget* widget=event->widget();
    if ((event->key == ppltk::KeyEvent::KEY_TAB || event->key == ppltk::KeyEvent::KEY_RETURN)
        && (event->modifier & ppltk::KeyEvent::KEYMOD_SHIFT) == 0) {
           // Tab forward
        if (widget == color_red) wm->setKeyboardFocus(color_green);
        else if (widget == color_green) wm->setKeyboardFocus(color_blue);
        else if (widget == color_blue) {
            ppltk::KeyEvent new_event=(*event);
            new_event.setWidget(this);
            EventHandler::keyDownEvent(&new_event);
        }

    } else if ((event->key == ppltk::KeyEvent::KEY_TAB || event->key == ppltk::KeyEvent::KEY_RETURN)
        && (event->modifier & ppltk::KeyEvent::KEYMOD_SHIFT) != 0) {
           // Tab backward
        if (widget == color_blue) wm->setKeyboardFocus(color_green);
        else if (widget == color_green) wm->setKeyboardFocus(color_red);
        else if (widget == color_red) {
            ppltk::KeyEvent new_event=(*event);
            new_event.setWidget(this);
            EventHandler::keyDownEvent(&new_event);
        }
    }
}



} // EOF namespace
