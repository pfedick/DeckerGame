#include "decker.h"
#include "ui.h"

namespace Decker::ui {

SubWindow::SubWindow(int x, int y, int width, int height)
: ppl7::tk::Widget()
{
	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	ppl7::tk::WindowManager *wm=ppl7::tk::GetWindowManager();
	myBackground=style.windowBackgroundColor*1.2f;
	myBackground.setAlpha(230);
	create(x,y,width,height);
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	close_button=new ppl7::tk::Button(width-30,0,30,30,"");
	close_button->setIcon(wm->ButtonSymbols.getDrawable(0));
	close_button->setEventHandler(this);
	this->addChild(close_button);
	this->setClientOffset(2, 2, 2, 2);
}

SubWindow::~SubWindow()
{

}

const ppl7::String &SubWindow::windowTitle() const
{
	return WindowTitle;
}

void SubWindow::setWindowTitle(const ppl7::String &title)
{
	WindowTitle=title;
	needsRedraw();
}
const ppl7::grafix::Drawable &SubWindow::windowIcon() const
{
	return WindowIcon;
}

void SubWindow::setWindowIcon(const ppl7::grafix::Drawable &icon)
{
	WindowIcon=icon;
	needsRedraw();
}

const ppl7::grafix::Color &SubWindow::backgroundColor() const
{
	return myBackground;
}

void SubWindow::setBackgroundColor(const ppl7::grafix::Color &c)
{
	myBackground=c;
	needsRedraw();
}

void SubWindow::paint(ppl7::grafix::Drawable &draw)
{
	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	ppl7::grafix::Color light=style.buttonBackgroundColor*1.8f;
	ppl7::grafix::Color shadow=style.buttonBackgroundColor*0.4f;
	ppl7::grafix::Color black=ppl7::grafix::Color(0,0,0,255);
	ppl7::grafix::Color titlebg=style.buttonBackgroundColor*1.2f;
	titlebg.setAlpha(230);
	draw.cls(myBackground);
	draw.drawRect(0,0,draw.width(),draw.height(),black);
	draw.line(1,1,draw.width()-1,1,light);
	draw.line(1,1,1,draw.height()-1,light);
	draw.line(2,draw.height()-1,draw.width()-2,draw.height()-1,shadow);
	draw.line(draw.width()-1,2,draw.width()-1,draw.height()-1,shadow);
	draw.fillRect(3,3,draw.width()-3,33,titlebg);
}

void SubWindow::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	if (event->widget()==this) {
		move_start=event->p;
	}
}

void SubWindow::mouseMoveEvent(ppl7::tk::MouseEvent *event)
{
}

void SubWindow::mouseClickEvent(ppl7::tk::MouseEvent *event)
{
	if (event->widget()==close_button) {
		this->deleteLater();
	}
}

} //EOF namespace Decker::ui

