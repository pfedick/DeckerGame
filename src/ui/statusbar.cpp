#include "ui.h"

namespace Decker::ui {

StatusBar::StatusBar(int x, int y, int width, int height)
: ppl7::tk::Frame(x,y,width,height)
{
	ppl7::tk::Label *label;
	label=new ppl7::tk::Label (0,0,40,32,"FPS:");
	this->addChild(label);

	fps_label=new ppl7::tk::Label (40,0,50,32,"?", ppl7::tk::Frame::Inset);
	ppl7::grafix::Font font_bold=fps_label->font();
	font_bold.setBold(true);
	font_bold.setAntialias(true);
	font_bold.setColor(ppl7::grafix::Color(0,0,0,255));
	fps_label->setFont(font_bold);
	this->addChild(fps_label);

	label=new ppl7::tk::Label (100,0,55,32,"Mouse:");
	this->addChild(label);

	mouse_coords=new ppl7::tk::Label (155,0,80,32,"?", ppl7::tk::Frame::Inset);
	mouse_coords->setFont(font_bold);
	this->addChild(mouse_coords);

	label=new ppl7::tk::Label (240,0,55,32,"Buttons:");
	this->addChild(label);
	mouse_buttons=new ppl7::tk::Label (300,0,30,32,"?", ppl7::tk::Frame::Inset);
	mouse_buttons->setFont(font_bold);
	this->addChild(mouse_buttons);

	label=new ppl7::tk::Label (340,0,55,32,"World:");
	this->addChild(label);
	world_coords=new ppl7::tk::Label (400,0,80,32,"?", ppl7::tk::Frame::Inset);
	world_coords->setFont(font_bold);
	this->addChild(world_coords);

	this->addChild(new ppl7::tk::Label (490,0,55,32,"Player:"));
	player_coords=new ppl7::tk::Label (550,0,80,32,"?", ppl7::tk::Frame::Inset);
	player_coords->setFont(font_bold);
	this->addChild(player_coords);

	this->addChild(new ppl7::tk::Label (640,0,140,32,"Sprites total / visible:"));
	sprite_count=new ppl7::tk::Label (780,0,120,32,"?", ppl7::tk::Frame::Inset);
	sprite_count->setFont(font_bold);
	this->addChild(sprite_count);


	label=new ppl7::tk::Label (width-112,0,50,32,"Zeit:");
	this->addChild(label);
	time_label=new ppl7::tk::Label (width-80,0,80,32,"?", ppl7::tk::Frame::Inset);
	time_label->setFont(font_bold);
	this->addChild(time_label);

	ppl7::tk::WindowManager *wm=ppl7::tk::GetWindowManager();
	wm->startTimer(this, 1000);
	timerEvent(NULL);

}

void StatusBar::setFps(int fps)
{
	fps_label->setText(ppl7::ToString("%d",fps));
}

void StatusBar::setMouse(const ppl7::tk::MouseState &mouse)
{
	mouse_coords->setText(ppl7::ToString("%d / %d",mouse.p.x, mouse.p.y));
	mouse_buttons->setText(ppl7::ToString("%d",mouse.buttonMask));
}

void StatusBar::setWorldCoords(const ppl7::grafix::Point &p)
{
	world_coords->setText(ppl7::ToString("%d / %d",p.x, p.y));
}

void StatusBar::setPlayerCoords(const ppl7::grafix::Point &p)
{
	player_coords->setText(ppl7::ToString("%d / %d",p.x, p.y));
}

void StatusBar::setSpriteCount(size_t total, size_t visible)
{
	sprite_count->setText(ppl7::ToString("%zd / %zd",total, visible));
}


void StatusBar::timerEvent(ppl7::tk::Event *event)
{
	ppl7::DateTime now=ppl7::DateTime::currentTime();
	time_label->setText(now.get("%H:%M:%S"));
}

} //EOF namespace Decker

