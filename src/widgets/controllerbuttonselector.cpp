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
    const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
    font=style.buttonFont;
    font.setName("NotoSansBlack");
    font.setBold(false);
    font.setSize(20);
    font.setOrientation(ppl7::grafix::Font::TOP);

}

void ControllerButtonSelector::setName(const ppl7::String& text)
{
    this->text=text;
    redrawRequired();
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
    draw.fillRect(0, 0, draw.width(), draw.height(), bg);
    for (int i=0;i < border_width;i++) {
        draw.drawRect(i, i, draw.width() - i, draw.height() - i, border);
    }
    font.setColor(fg);
    draw.print(font, border_width + 20, border_width + 10, text);
}

void ControllerButtonSelector::gameControllerAxisMotionEvent(ppl7::tk::GameControllerAxisEvent* event)
{

}

void ControllerButtonSelector::gameControllerButtonDownEvent(ppl7::tk::GameControllerButtonEvent* event)
{

}





}
} // EOF namespace Decker::ui
