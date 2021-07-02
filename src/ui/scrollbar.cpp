#include "decker.h"
#include "ui.h"

namespace Decker::ui {


Scrollbar::Scrollbar(int x, int y, int width, int height)
: ppl7::tk::Widget()
{
	this->create(x,y,width,height);
	up_button=NULL;
	down_button=NULL;
}

ppl7::String Scrollbar::widgetType() const
{
	return ppl7::String("Scrollbar");
}
void Scrollbar::paint(Drawable &draw)
{
	//Widget::paint(draw);
	draw.cls();
}

void Scrollbar::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
}



} //EOF namespace Decker

