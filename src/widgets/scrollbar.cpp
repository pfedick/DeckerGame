#include "decker.h"
#include "ui.h"

namespace Decker::ui {


Scrollbar::Scrollbar(int x, int y, int width, int height) // @suppress("Class members should be properly initialized")
: ppl7::tk::Widget()
{
	this->create(x,y,width,height);
	setClientOffset(0,0,0,0);
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	ppl7::tk::WindowManager *wm=ppl7::tk::GetWindowManager();
	up_button=new ppl7::tk::Button(0,0,width-2,25);
	down_button=new ppl7::tk::Button(0,height-25,width-2,25);
	up_button->setIcon(wm->ButtonSymbols.getDrawable(3));
	down_button->setIcon(wm->ButtonSymbols.getDrawable(4));
	up_button->setEventHandler(this);
	down_button->setEventHandler(this);
	addChild(up_button);
	addChild(down_button);
	size=0;
	pos=0;
	visibleItems=0;
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
	if (position!=pos && position<size && position>=0) {
		pos=position;
		needsRedraw();
	}
}

void Scrollbar::setVisibleItems(int items)
{
	visibleItems=items;
	needsRedraw();
}

int Scrollbar::position() const
{
	return pos;
}

void Scrollbar::paint(ppl7::grafix::Drawable &draw)
{
	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	ppl7::grafix::Color light=style.buttonBackgroundColor*1.8f;
	ppl7::grafix::Color shadow=style.buttonBackgroundColor*0.4f;
	ppl7::grafix::Color shade1=style.buttonBackgroundColor*1.05f;
	ppl7::grafix::Color shade2=style.buttonBackgroundColor*0.85f;
	ppl7::grafix::Drawable indicator=draw.getDrawable(0, 25, draw.width(), draw.height()-26);
	int w=indicator.width()-1;
	//int h=indicator.height()-1;
	ppl7::grafix::Rect r1=indicator.rect();
	if (visibleItems>0 && visibleItems<size) {
		int pxi=indicator.height()/size;
		r1.y1=pos*pxi;
		r1.y2=r1.y1+visibleItems*pxi;
		if (r1.y2>=indicator.height()) r1.y2=indicator.height()-1;
	}

	ppl7::grafix::Color scrollarea=style.windowBackgroundColor*1.2f;
	indicator.cls(scrollarea);
	indicator.colorGradient(r1,shade1,shade2,1);
	indicator.line(0,r1.y1,w,r1.y1,light);
	indicator.line(0,r1.y1,0,r1.y2,light);
	indicator.line(0,r1.y2,w,r1.y2,shadow);
	indicator.line(w,r1.y1,w,r1.y2,shadow);

	//ppl7::tk::Widget::paint(draw);
	//draw.fillRect(0,y1,draw.width(),y2,style.frameBorderColorLight);
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

	} else if (event->widget()==down_button && pos<size-1) {
		pos++;
		needsRedraw();
		ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
		ev.setWidget(this);
		valueChangedEvent(&ev, pos);
	}
}


} //EOF namespace Decker

