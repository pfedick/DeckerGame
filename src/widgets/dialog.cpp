#include "decker.h"
#include "ui.h"

namespace Decker::ui {

Dialog::Dialog(int width, int height, int buttons)
	: ppl7::tk::Widget()
{
	setUseOwnDrawbuffer(true);
	setTopmost(true);
	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	ppl7::tk::WindowManager *wm=ppl7::tk::GetWindowManager();
	myBackground=style.windowBackgroundColor * 1.4f;
	myBackground.setAlpha(240);
	ppl7::tk::Window* gamewin=GetGameWindow();
	create((gamewin->width() - width) / 2, (gamewin->height() - height) / 2, width, height);

	ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
	ok_button=NULL;
	copy_button=NULL;
	paste_button=NULL;
	test_button=NULL;
	reset_button=NULL;
	close_button=NULL;
	int x=width - 2 - 16;
	int y=height - 40;

	close_button=new ppl7::tk::Button(width - 31, 2, 30, 30, "");
	close_button->setIcon(wm->ButtonSymbols.getDrawable(0));
	close_button->setEventHandler(this);
	ppl7::tk::Widget::addChild(close_button);


	int client_offset_y2=8;
	if (buttons & Buttons::OK) {
		ok_button=new ppl7::tk::Button((width - 80) / 2, y, 80, 30, "OK");
		ok_button->setIcon(gfx->Toolbar.getDrawable(24));
		ok_button->setEventHandler(this);
		ppl7::tk::Widget::addChild(ok_button);
		client_offset_y2=40 + 8;
	}
	if (buttons & Buttons::Paste) {
		paste_button=new ppl7::tk::Button(x - 32, y, 32, 32, "");
		paste_button->setIcon(gfx->Toolbar.getDrawable(38));
		paste_button->setEventHandler(this);
		ppl7::tk::Widget::addChild(paste_button);
		x-=32;
		client_offset_y2=40 + 8;
	}

	if (buttons & Buttons::Copy) {
		copy_button=new ppl7::tk::Button(x - 32, y, 32, 32, "");
		copy_button->setIcon(gfx->Toolbar.getDrawable(37));
		copy_button->setEventHandler(this);
		ppl7::tk::Widget::addChild(copy_button);
		x-=32;
		client_offset_y2=40 + 8;
	}

	x=18;
	if (buttons & Buttons::Reset) {
		reset_button=new ppl7::tk::Button(x, y, 80, 32, "Reset");
		reset_button->setIcon(gfx->Toolbar.getDrawable(34));
		reset_button->setEventHandler(this);
		ppl7::tk::Widget::addChild(reset_button);
		x+=82;
		client_offset_y2=40 + 8;
	}
	if (buttons & Buttons::Test) {
		test_button=new ppl7::tk::Button(x, y, 60, 32, "Test");
		reset_button->setIcon(gfx->Toolbar.getDrawable(63));
		test_button->setEventHandler(this);
		ppl7::tk::Widget::addChild(test_button);
		x+=62;
		client_offset_y2=40 + 8;
	}

	client_frame=new ppl7::tk::Frame(8, 40, this->width() - 16, this->height() - 40 - client_offset_y2, ppl7::tk::Frame::BorderStyle::NoBorder);
	client_frame->setBackgroundColor(myBackground);
	ppl7::tk::Widget::addChild(client_frame);

	this->setModal(true);
	//this->setClientOffset(8, 40, 8, client_offset_y2);
	drag_started=false;
}

Dialog::~Dialog()
{
	if (drag_started) {
		drag_started=false;
		ppl7::tk::GetWindowManager()->releaseMouse(this);
	}
}

void Dialog::addChild(Widget* w)
{
	client_frame->addChild(w);
}

void Dialog::removeChild(Widget* w)
{
	client_frame->removeChild(w);
}

void Dialog::destroyChilds()
{
	client_frame->destroyChilds();
}

ppl7::grafix::Rect Dialog::clientRect() const
{
	return client_frame->clientRect();
}

ppl7::grafix::Size Dialog::clientSize() const
{
	return client_frame->clientSize();
}



const ppl7::String& Dialog::windowTitle() const
{
	return WindowTitle;
}

void Dialog::setWindowTitle(const ppl7::String& title)
{
	WindowTitle=title;
	needsRedraw();
}
const ppl7::grafix::Drawable& Dialog::windowIcon() const
{
	return WindowIcon;
}

void Dialog::setWindowIcon(const ppl7::grafix::Drawable& icon)
{
	WindowIcon=icon;
	needsRedraw();
}

const ppl7::grafix::Color& Dialog::backgroundColor() const
{
	return myBackground;
}

void Dialog::setBackgroundColor(const ppl7::grafix::Color& c)
{
	myBackground=c;
	client_frame->setBackgroundColor(myBackground);
	needsRedraw();
}

void Dialog::paint(ppl7::grafix::Drawable& draw)
{
	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	//ppl7::grafix::Color light=style.buttonBackgroundColor*1.8f;
	ppl7::grafix::Color white(255, 255, 255, 255);
	ppl7::grafix::Color shadow=style.buttonBackgroundColor * 0.4f;
	ppl7::grafix::Color black=ppl7::grafix::Color(0, 0, 0, 255);
	ppl7::grafix::Color titlebg=style.buttonBackgroundColor * 1.2f;
	titlebg.setAlpha(240);
	draw.cls(myBackground);
	draw.drawRect(0, 0, draw.width(), draw.height(), black);
	draw.line(1, 1, draw.width() - 1, 1, white);
	//draw.line(2,2,draw.width()-2,2,white);
	draw.line(1, 1, 1, draw.height() - 1, white);
	//draw.line(2,2,2,draw.height()-2,white);
	draw.line(2, draw.height() - 1, draw.width() - 1, draw.height() - 1, shadow);
	draw.line(draw.width() - 1, 2, draw.width() - 1, draw.height() - 1, shadow);
	draw.fillRect(2, 2, draw.width() - 2, 33, titlebg);
	ppl7::grafix::Font myFont=style.buttonFont;
	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(ppl7::grafix::Font::TOP);

	int x=8;
	if (!WindowIcon.isEmpty()) {
		draw.bltAlpha(WindowIcon, 8, 3 + ((30 - WindowIcon.height()) >> 1));
		x+=WindowIcon.width() + 4;
	}

	ppl7::grafix::Size s=myFont.measure(WindowTitle);
	draw.print(myFont, x, 3 + ((30 - s.height) >> 1), WindowTitle);
}

void Dialog::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	ppl7::tk::Widget* widget=event->widget();
	if (widget == ok_button || widget == close_button) {
		this->deleteLater();
	} else if (copy_button && widget == copy_button) dialogButtonEvent(Buttons::Copy);
	else if (paste_button && widget == paste_button) dialogButtonEvent(Buttons::Paste);
	else if (reset_button && widget == reset_button) dialogButtonEvent(Buttons::Reset);
	else if (test_button && widget == test_button) dialogButtonEvent(Buttons::Test);
	else if (widget == this && event->p.y < 40) {
		drag_started=true;
		drag_start_pos=event->p;
		ppl7::tk::GetWindowManager()->grabMouse(this);
	}
}

void Dialog::mouseUpEvent(ppl7::tk::MouseEvent* event)
{
	if (drag_started) {
		drag_started=false;
		ppl7::tk::GetWindowManager()->releaseMouse(this);
	}
}

void Dialog::lostFocusEvent(ppl7::tk::FocusEvent* event)
{
	if (drag_started) {
		drag_started=false;
		ppl7::tk::GetWindowManager()->releaseMouse(this);
	}
}

void Dialog::mouseMoveEvent(ppl7::tk::MouseEvent* event)
{
	if (event->buttonMask & ppl7::tk::MouseEvent::MouseButton::Left) {
		if (drag_started) {
			ppl7::grafix::Point delta=event->p - drag_start_pos;
			//printf("delta: %d:%d\n", delta.x, delta.y);
			//printf("windowpos old: %d:%d, ", pos().x, pos().y);
			drag_start_pos=event->p - delta;
			ppl7::grafix::Point newpos=this->pos() + delta;
			this->setPos(newpos);


		}
	}
}


void Dialog::dialogButtonEvent(Dialog::Buttons button)
{

}


} //EOF namespace Decker::ui
