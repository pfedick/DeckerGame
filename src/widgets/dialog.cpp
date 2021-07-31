#include "decker.h"
#include "ui.h"

namespace Decker::ui {

Dialog::Dialog(int width, int height)
: ppl7::tk::Widget()
{
	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	//ppl7::tk::WindowManager *wm=ppl7::tk::GetWindowManager();
	myBackground=style.windowBackgroundColor*1.4f;
	myBackground.setAlpha(240);
	ppl7::tk::Window *gamewin=GetGameWindow();
	create((gamewin->width()-640)/2, (gamewin->height()-480)/2,width,height);
	ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	close_button=new ppl7::tk::Button((width-80)/2,height-80,80,30,"OK");
	close_button->setIcon(gfx->Toolbar.getDrawable(24));
	close_button->setEventHandler(this);
	this->setModal(true);
	this->addChild(close_button);
	this->setClientOffset(2, 40, 2, 2);
}

Dialog::~Dialog()
{
}

const ppl7::String &Dialog::windowTitle() const
{
	return WindowTitle;
}

void Dialog::setWindowTitle(const ppl7::String &title)
{
	WindowTitle=title;
	needsRedraw();
}
const ppl7::grafix::Drawable &Dialog::windowIcon() const
{
	return WindowIcon;
}

void Dialog::setWindowIcon(const ppl7::grafix::Drawable &icon)
{
	WindowIcon=icon;
	needsRedraw();
}

const ppl7::grafix::Color &Dialog::backgroundColor() const
{
	return myBackground;
}

void Dialog::setBackgroundColor(const ppl7::grafix::Color &c)
{
	myBackground=c;
	needsRedraw();
}

void Dialog::paint(ppl7::grafix::Drawable &draw)
{
	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	//ppl7::grafix::Color light=style.buttonBackgroundColor*1.8f;
	ppl7::grafix::Color white(255,255,255,255);
	ppl7::grafix::Color shadow=style.buttonBackgroundColor*0.4f;
	ppl7::grafix::Color black=ppl7::grafix::Color(0,0,0,255);
	ppl7::grafix::Color titlebg=style.buttonBackgroundColor*1.2f;
	titlebg.setAlpha(240);
	draw.cls(myBackground);
	draw.drawRect(0,0,draw.width(),draw.height(),black);
	draw.line(1,1,draw.width()-1,1,white);
	//draw.line(2,2,draw.width()-2,2,white);
	draw.line(1,1,1,draw.height()-1,white);
	//draw.line(2,2,2,draw.height()-2,white);
	draw.line(2,draw.height()-1,draw.width()-1,draw.height()-1,shadow);
	draw.line(draw.width()-1,2,draw.width()-1,draw.height()-1,shadow);
	draw.fillRect(2,2,draw.width()-2,33,titlebg);
	ppl7::grafix::Font myFont=style.buttonFont;
	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(ppl7::grafix::Font::TOP);
	ppl7::grafix::Size s=myFont.measure(WindowTitle);
	draw.print(myFont,8,3+((30-s.height)>>1),WindowTitle);
}

void Dialog::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	if (event->widget()==close_button) {
		this->deleteLater();
	}
}



} //EOF namespace Decker::ui

