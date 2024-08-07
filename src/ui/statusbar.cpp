#include "ui.h"
#include "version.h"

namespace Decker::ui {

StatusBar::StatusBar(int x, int y, int width, int height)
	: ppltk::Frame(x, y, width, height)
{
	setupUi();
	ppltk::WindowManager* wm=ppltk::GetWindowManager();
	timer_id=wm->startTimer(this, 1000);
	timerEvent(NULL);
}

StatusBar::~StatusBar()
{
	ppltk::WindowManager* wm=ppltk::GetWindowManager();
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
	ppltk::Label* label;
	label=new ppltk::Label(x, 0, 40, 32, "FPS:");
	this->addChild(label);
	x+=40;

	fps_label=new ppltk::Label(x, 0, 50, 32, "?", ppltk::Frame::Inset);
	ppl7::grafix::Font font_bold=fps_label->font();
	font_bold.setBold(true);
	font_bold.setAntialias(true);
	font_bold.setColor(ppl7::grafix::Color(0, 0, 0, 255));
	fps_label->setFont(font_bold);
	this->addChild(fps_label);
	x+=55;

	this->addChild(new ppltk::Label(x, 0, 50, 32, "Load:"));
	x+=50;
	load_label=new ppltk::Label(x, 0, 60, 32, "?", ppltk::Frame::Inset);
	load_label->setFont(font_bold);
	this->addChild(load_label);
	x+=65;

	this->addChild(new ppltk::Label(x, 0, 80, 32, "FrameTime:"));
	x+=80;
	frametime_label=new ppltk::Label(x, 0, 80, 32, "?", ppltk::Frame::Inset);
	frametime_label->setFont(font_bold);
	this->addChild(frametime_label);
	x+=85;

	this->addChild(new ppltk::Label(x, 0, 55, 32, "Mouse:"));
	x+=55;

	mouse_coords=new ppltk::Label(x, 0, 80, 32, "?", ppltk::Frame::Inset);
	mouse_coords->setFont(font_bold);
	this->addChild(mouse_coords);
	x+=85;

	this->addChild(new ppltk::Label(x, 0, 65, 32, "Buttons:"));
	x+=65;
	mouse_buttons=new ppltk::Label(x, 0, 30, 32, "?", ppltk::Frame::Inset);
	mouse_buttons->setFont(font_bold);
	this->addChild(mouse_buttons);
	x+=35;

	this->addChild(new ppltk::Label(x, 0, 55, 32, "World:"));
	x+=55;
	world_coords=new ppltk::Label(x, 0, 110, 32, "?", ppltk::Frame::Inset);
	world_coords->setFont(font_bold);
	this->addChild(world_coords);
	x+=115;

	this->addChild(new ppltk::Label(x, 0, 55, 32, "Player:"));
	x+=55;
	player_coords=new ppltk::Label(x, 0, 110, 32, "?", ppltk::Frame::Inset);
	player_coords->setFont(font_bold);
	this->addChild(player_coords);
	x+=115;

	this->addChild(new ppltk::Label(x, 0, 100, 32, "Player state:"));
	x+=100;
	player_state=new ppltk::Label(x, 0, 300, 32, "?", ppltk::Frame::Inset);
	player_state->setFont(font_bold);
	this->addChild(player_state);
	x+=305;

	this->addChild(new ppltk::Label(x, 0, 110, 32, "selected object:"));
	x+=110;
	object_id=new ppltk::Label(x, 0, 60, 32, "-", ppltk::Frame::Inset);
	object_id->setFont(font_bold);
	this->addChild(object_id);
	x+=65;

	this->addChild(new ppltk::Label(width() - 125, 0, 45, 32, "Time:"));
	time_label=new ppltk::Label(width() - 80, 0, 80, 32, "?", ppltk::Frame::Inset);
	time_label->setFont(font_bold);
	this->addChild(time_label);

	this->addChild(new ppltk::Label(width() - 300, 0, 60, 32, "Version:"));
	version_label=new ppltk::Label(width() - 240, 0, 100, 32,
		ppl7::ToString("%s.%s", DECKER_VERSION, DECKER_REVSION),
		ppltk::Frame::Inset);
	version_label->setFont(font_bold);
	this->addChild(version_label);


}

void StatusBar::setFps(int fps)
{
	fps_label->setText(ppl7::ToString("%d", fps));
}

void StatusBar::setLoad(float load)
{
	load_label->setText(ppl7::ToString("%5.1f%%", load));
}

void StatusBar::setFrameTime(float time)
{
	frametime_label->setText(ppl7::ToString("%0.3f ms", time));
}

void StatusBar::setMouse(const ppltk::MouseState& mouse)
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

void StatusBar::timerEvent(ppltk::Event* event)
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
