#include "ui.h"
#include "version.h"

namespace Decker::ui {

StatusBar::StatusBar(int x, int y, int width, int height)
	: ppl7::tk::Frame(x, y, width, height)
{
	setupUi();
	ppl7::tk::WindowManager* wm=ppl7::tk::GetWindowManager();
	timer_id=wm->startTimer(this, 1000);
	timerEvent(NULL);
}

StatusBar::~StatusBar()
{
	ppl7::tk::WindowManager* wm=ppl7::tk::GetWindowManager();
	wm->removeTimer(timer_id);
}

void StatusBar::resize(int x, int y, int width, int height)
{
	this->setPos(x, y);
	this->setSize(width, height);
	this->destroyChilds();
	setupUi();
	needsRedraw();
}

void StatusBar::setupUi()
{
	int x=0;
	ppl7::tk::Label* label;
	label=new ppl7::tk::Label(x, 0, 40, 32, "FPS:");
	this->addChild(label);
	x+=40;

	fps_label=new ppl7::tk::Label(x, 0, 50, 32, "?", ppl7::tk::Frame::Inset);
	ppl7::grafix::Font font_bold=fps_label->font();
	font_bold.setBold(true);
	font_bold.setAntialias(true);
	font_bold.setColor(ppl7::grafix::Color(0, 0, 0, 255));
	fps_label->setFont(font_bold);
	this->addChild(fps_label);
	x+=55;

	this->addChild(new ppl7::tk::Label(x, 0, 55, 32, "Mouse:"));
	x+=55;

	mouse_coords=new ppl7::tk::Label(x, 0, 80, 32, "?", ppl7::tk::Frame::Inset);
	mouse_coords->setFont(font_bold);
	this->addChild(mouse_coords);
	x+=85;

	this->addChild(new ppl7::tk::Label(x, 0, 65, 32, "Buttons:"));
	x+=65;
	mouse_buttons=new ppl7::tk::Label(x, 0, 30, 32, "?", ppl7::tk::Frame::Inset);
	mouse_buttons->setFont(font_bold);
	this->addChild(mouse_buttons);
	x+=35;

	this->addChild(new ppl7::tk::Label(x, 0, 55, 32, "World:"));
	x+=55;
	world_coords=new ppl7::tk::Label(x, 0, 110, 32, "?", ppl7::tk::Frame::Inset);
	world_coords->setFont(font_bold);
	this->addChild(world_coords);
	x+=115;

	this->addChild(new ppl7::tk::Label(x, 0, 55, 32, "Player:"));
	x+=55;
	player_coords=new ppl7::tk::Label(x, 0, 110, 32, "?", ppl7::tk::Frame::Inset);
	player_coords->setFont(font_bold);
	this->addChild(player_coords);
	x+=115;

	this->addChild(new ppl7::tk::Label(x, 0, 100, 32, "Player state:"));
	x+=100;
	player_state=new ppl7::tk::Label(x, 0, 300, 32, "?", ppl7::tk::Frame::Inset);
	player_state->setFont(font_bold);
	this->addChild(player_state);
	x+=305;

	this->addChild(new ppl7::tk::Label(x, 0, 110, 32, "selected object:"));
	x+=110;
	object_id=new ppl7::tk::Label(x, 0, 60, 32, "-", ppl7::tk::Frame::Inset);
	object_id->setFont(font_bold);
	this->addChild(object_id);
	x+=65;

	this->addChild(new ppl7::tk::Label(width() - 125, 0, 45, 32, "Time:"));
	time_label=new ppl7::tk::Label(width() - 80, 0, 80, 32, "?", ppl7::tk::Frame::Inset);
	time_label->setFont(font_bold);
	this->addChild(time_label);

	this->addChild(new ppl7::tk::Label(width() - 300, 0, 60, 32, "Version:"));
	version_label=new ppl7::tk::Label(width() - 240, 0, 100, 32,
		ppl7::ToString("%s.%s", DECKER_VERSION, DECKER_REVSION),
		ppl7::tk::Frame::Inset);
	version_label->setFont(font_bold);
	this->addChild(version_label);


}

void StatusBar::setFps(int fps)
{
	fps_label->setText(ppl7::ToString("%d", fps));
}

void StatusBar::setMouse(const ppl7::tk::MouseState& mouse)
{
	mouse_coords->setText(ppl7::ToString("%d / %d", mouse.p.x, mouse.p.y));
	mouse_buttons->setText(ppl7::ToString("%d", mouse.buttonMask));
}

void StatusBar::setWorldCoords(const ppl7::grafix::Point& p)
{
	world_coords->setText(ppl7::ToString("%d / %d", p.x, p.y));
}

void StatusBar::setPlayerCoords(const ppl7::grafix::Point& p)
{
	player_coords->setText(ppl7::ToString("%d / %d", p.x, p.y));
}

void StatusBar::timerEvent(ppl7::tk::Event* event)
{
	ppl7::DateTime now=ppl7::DateTime::currentTime();
	time_label->setText(now.get("%H:%M:%S"));
}

void StatusBar::setPlayerState(const ppl7::String& state)
{
	player_state->setText(state);
}

void StatusBar::setSelectedObject(int id)
{
	if (id >= 0) {
		object_id->setText(ppl7::ToString("%d", id));
	} else {
		object_id->setText("-");
	}
}
} //EOF namespace Decker
