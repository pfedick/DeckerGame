#include "decker.h"
#include "ui.h"

namespace Decker::ui {

CheckBox::CheckBox()
: ppl7::tk::Label()
{
	ischecked=false;
}

CheckBox::CheckBox(int x, int y, int width, int height, const String &text, bool checked) // @suppress("Class members should be properly initialized")
: ppl7::tk::Label(x,y,width,height,text)
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


void CheckBox::paint(Drawable &draw)
{
	const WidgetStyle &style=GetWidgetStyle();
	Drawable d=draw.getDrawable(16,0, draw.width()-16, draw.height());
	Label::paint(d);
	int y1=draw.height()/2-7;
	int y2=draw.height()/2+7;
	draw.drawRect(2,y1,16,y2,style.frameBorderColorLight);
	draw.drawRect(3,y1+1,15,y2-1,style.frameBorderColorLight);
	if (ischecked) draw.fillRect(5,y1+3,13,y2-3,this->color());
}

void CheckBox::mouseDownEvent(MouseEvent *event)
{
	ischecked=!ischecked;
	needsRedraw();
	ppl7::tk::Event ev(ppl7::tk::Event::Toggled);
	ev.setWidget(this);
	toggledEvent(&ev, ischecked);
}


} //EOF namespace Decker::ui

