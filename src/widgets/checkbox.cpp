#include "decker.h"
#include "ui.h"

namespace Decker::ui {

CheckBox::CheckBox()
	: ppl7::tk::Label()
{
	ischecked=false;
}

CheckBox::CheckBox(int x, int y, int width, int height, const ppl7::String& text, bool checked) // @suppress("Class members should be properly initialized")
	: ppl7::tk::Label(x, y, width, height, text)
{
	ischecked=checked;
}

CheckBox::~CheckBox()
{

}

ppl7::String CheckBox::widgetType() const
{
	return ppl7::String("CheckBox");
}

bool CheckBox::checked() const
{
	return ischecked;
}

void CheckBox::setChecked(bool checked)
{
	ischecked=checked;
	needsRedraw();
}


void CheckBox::paint(ppl7::grafix::Drawable& draw)
{
	int s=draw.height() * 3 / 5;
	int sh=s / 2;

	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	ppl7::grafix::Drawable d=draw.getDrawable(s + (s / 3), 0, draw.width() - (s + s / 3), draw.height());
	Label::paint(d);
	int y1=draw.height() / 2 - sh;
	int y2=draw.height() / 2 + sh;
	draw.drawRect(2, y1, 2 + s, y2, style.frameBorderColorLight);
	draw.drawRect(3, y1 + 1, 1 + s, y2 - 1, style.frameBorderColorLight);
	if (ischecked) draw.fillRect(5, y1 + 3, 2 + s - 3, y2 - 3, this->color());
}

void CheckBox::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	ischecked=!ischecked;
	needsRedraw();
	ppl7::tk::Event ev(ppl7::tk::Event::Toggled);
	ev.setWidget(this);
	toggledEvent(&ev, ischecked);
}


} //EOF namespace Decker::ui
