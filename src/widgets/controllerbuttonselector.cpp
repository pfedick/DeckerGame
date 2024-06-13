#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "widgets.h"
#include "translate.h"

namespace Decker {
namespace ui {


ControllerButtonSelector::ControllerButtonSelector(int x, int y, int width, int height, const ppl7::String& text)
{
    create(x, y, width, height);
    this->text=text;
    border_width=5;
    const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
    font=style.buttonFont;
    font.setName("NotoSansBlack");
    font.setBold(false);
    font.setSize(20);
    font.setOrientation(ppl7::grafix::Font::TOP);
    inputmode=false;
    button_id=-1;

}

void ControllerButtonSelector::setName(const ppl7::String& text)
{
    this->text=text;
    redrawRequired();
}

void ControllerButtonSelector::setControllerType(ControllerType type)
{
    controllertype=type;
}

void ControllerButtonSelector::setId(int id)
{
    button_id=id;
    if (controllertype == ControllerType::Axis || controllertype == ControllerType::Trigger)
        text=translate(GameController::getAxisName(id));
    else
        text=translate(GameController::getButtonName(id));
}

int ControllerButtonSelector::getId() const
{
    return button_id;
}

void ControllerButtonSelector::setInputmode()
{
    inputmode=true;
    needsRedraw();
    ppltk::WindowManager* wm=ppltk::GetWindowManager();
    wm->setGameControllerFocus(this);
    //ppl7::PrintDebugTime("ControllerButtonSelector::setInputmode\n");
}

void ControllerButtonSelector::setFontSize(int size)
{
    font.setSize(size);
}

void ControllerButtonSelector::paint(ppl7::grafix::Drawable& draw)
{
    ppl7::grafix::Color bg(20, 10, 0, 192);
    ppl7::grafix::Color border(128, 96, 0, 255);
    ppl7::grafix::Color fg(128, 128, 128, 255);
    if (hasFocus()) {
        bg.setColor(90, 70, 0, 192);
        border.setColor(255, 200, 0, 255);
        fg.setColor(255, 255, 255, 255);
    }
    if (inputmode) {
        bg.setColor(150, 70, 0, 192);
    }
    draw.fillRect(0, 0, draw.width(), draw.height(), bg);
    for (int i=0;i < border_width;i++) {
        draw.drawRect(i, i, draw.width() - i, draw.height() - i, border);
    }
    if (!inputmode) {
        font.setColor(fg);
        draw.print(font, border_width + 10, border_width, text);
    }
}

void ControllerButtonSelector::emmitValueChangedEvent()
{
    ppltk::Event ev(ppltk::Event::Type::ValueChanged);
    ev.setWidget(this);
    this->valueChangedEvent(&ev, button_id);
}

void ControllerButtonSelector::mouseDownEvent(ppltk::MouseEvent* event)
{
    setFocus();
    setInputmode();
}

void ControllerButtonSelector::gameControllerAxisMotionEvent(ppltk::GameControllerAxisEvent* event)
{
    if (!GetGame().controller.isOpen()) return;
    if (GetGame().controller.deadzone() > abs(event->value)) return;
    if (hasFocus() && controllertype == ControllerType::Axis && inputmode) {
        inputmode=false;
        setId(event->axis);
        emmitValueChangedEvent();
        needsRedraw();
    }
    if (hasFocus() && controllertype == ControllerType::Trigger && inputmode) {
        inputmode=false;
        setId(event->axis);
        emmitValueChangedEvent();
        needsRedraw();
    }
}

void ControllerButtonSelector::gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event)
{
    if (hasFocus() && controllertype == ControllerType::Button && inputmode) {
        inputmode=false;
        setId(event->button);
        emmitValueChangedEvent();
        needsRedraw();
    }
}





}
} // EOF namespace Decker::ui
