#include "decker.h"
#include "ui.h"

namespace Decker::ui {


Scrollbar::Scrollbar(int x, int y, int width, int height) // @suppress("Class members should be properly initialized")
: ppl7::tk::Widget()
{
	this->create(x,y,width,height);
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	ppl7::tk::WindowManager *wm=ppl7::tk::GetWindowManager();
	up_button=new ppl7::tk::Button(0,0,width-5,25);
	down_button=new ppl7::tk::Button(0,height-25,width-5,25);
	up_button->setIcon(wm->ButtonSymbols.getDrawable(3));
	down_button->setIcon(wm->ButtonSymbols.getDrawable(4));
	up_button->setEventHandler(this);
	down_button->setEventHandler(this);
	addChild(up_button);
	addChild(down_button);
	size=0;
	pos=0;
}

ppl7::String Scrollbar::widgetType() const
{
	return ppl7::String("Scrollbar");
}

void Scrollbar::setSize(int size)
{
	if (size!=this->size && size>=0 ) {
		this->size=size;
		if (pos>size) pos=size;
		needsRedraw();
	}
}

void Scrollbar::setPosition(int position)
{
	if (position!=pos && position<=size && position>=0) {
		pos=position;
		needsRedraw();
	}
}

int Scrollbar::position() const
{
	return pos;
}

void Scrollbar::paint(ppl7::grafix::Drawable &draw)
{
	//Widget::paint(draw);
	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	ppl7::grafix::Color scrollarea=style.windowBackgroundColor*1.2f;
	draw.cls(scrollarea);
}

void Scrollbar::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	//printf("Scrollbar::mouseDownEvent\n");
	if (event->widget()==up_button && pos>0) {
		pos--;
		needsRedraw();
		ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
		ev.setWidget(this);
		valueChangedEvent(&ev, pos);

	} else if (event->widget()==down_button && pos<size) {
		pos++;
		needsRedraw();
		ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
		ev.setWidget(this);
		valueChangedEvent(&ev, pos);
	}
}




} //EOF namespace Decker

