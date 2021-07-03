#include "decker.h"
#include "ui.h"

namespace Decker::ui {


Scrollbar::Scrollbar(int x, int y, int width, int height)
: ppl7::tk::Widget()
{
	this->create(x,y,width,height);
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	WindowManager *wm=ppl7::tk::GetWindowManager();
	up_button=new ppl7::tk::Button(0,0,width-5,25);
	down_button=new ppl7::tk::Button(0,height-25,width-5,25);
	up_button->setIcon(wm->ButtonSymbols.getDrawable(3));
	down_button->setIcon(wm->ButtonSymbols.getDrawable(4));
	addChild(up_button);
	addChild(down_button);
}

ppl7::String Scrollbar::widgetType() const
{
	return ppl7::String("Scrollbar");
}
void Scrollbar::paint(Drawable &draw)
{
	//Widget::paint(draw);
	const WidgetStyle &style=GetWidgetStyle();
	Color scrollarea=style.windowBackgroundColor*1.2f;
	draw.cls(scrollarea);
}

void Scrollbar::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
}



} //EOF namespace Decker

