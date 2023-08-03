#include "decker.h"
#include "ui.h"
#include "objects.h"
#include "waynet.h"

namespace Decker::ui {


WayNetEdit::WayNetEdit(int x, int y, int width, int height, Game* game)
	: ppl7::tk::Frame(x, y, width, height)
{
	setClientOffset(4, 4, 4, 4);
	this->game=game;
	//ppl7::grafix::Rect client=this->clientRect();
	this->addChild(new ppl7::tk::Label(0, 0, width, 30, "Way-Type:"));

	type_clear=new ppl7::tk::RadioButton(50, 30, width - 50, 30, "clear / invalid");
	this->addChild(type_clear);

	type_walk=new ppl7::tk::RadioButton(50, 60, width - 50, 30, "walk left/right");
	type_walk->setChecked(true);
	this->addChild(type_walk);

	type_jump_up=new ppl7::tk::RadioButton(50, 90, width - 50, 30, "jump up");
	this->addChild(type_jump_up);

	type_jump_left=new ppl7::tk::RadioButton(50, 120, width - 50, 30, "jump left");
	this->addChild(type_jump_left);

	type_jump_right=new ppl7::tk::RadioButton(50, 150, width - 50, 30, "jump right");
	this->addChild(type_jump_right);

	type_climb=new ppl7::tk::RadioButton(50, 180, width - 50, 30, "climp up/down");
	this->addChild(type_climb);

	type_go=new ppl7::tk::RadioButton(50, 210, width - 50, 30, "go left/right");
	this->addChild(type_go);
	y=240;

	ppl7::grafix::Rect client=clientRect();


	this->addChild(new ppl7::tk::Label(0, y, 60, 30, "Cost:"));
	cost=new ppl7::tk::DoubleHorizontalSlider(60, y, client.width() - 70, 30);
	cost->setEventHandler(this);
	cost->setLimits(0.0f, 10.0f);
	cost->enableSpinBox(true, 0.1, 3, 80);
	cost->setValue(1.0f);
	this->addChild(cost);
	y+=35;

	debug_mode=new ppl7::tk::Button(0, y, 100, 30, "Debug Mode");
	debug_mode->setCheckable(true);
	this->addChild(debug_mode);
	Waynet& waynet=Decker::Objects::GetObjectSystem()->getWaynet();
	wp_start=waynet.invalidPoint();
	wp_end=waynet.invalidPoint();

}


int WayNetEdit::getSelectedWayType() const
{
	if (type_walk->checked()) return Connection::Walk;
	if (type_jump_up->checked()) return Connection::JumpUp;
	if (type_jump_left->checked()) return Connection::JumpLeft;
	if (type_jump_right->checked()) return Connection::JumpRight;
	if (type_climb->checked()) return Connection::Climb;
	if (type_go->checked()) return Connection::Go;
	return Connection::Invalid;
}


float WayNetEdit::getCost() const
{
	float value=cost->value();
	if (value < 0.0f) value=0.0f;
	if (value > 10.0f) value=10.0f;
	return value;
}

bool WayNetEdit::debugMode() const
{
	return debug_mode->isChecked();
}

void WayNetEdit::setDebugStart(const WayPoint& wp)
{
	wp_start=wp;
	debugWaynet();
}

void WayNetEdit::setDebugEnd(const WayPoint& wp)
{
	wp_end=wp;
	debugWaynet();
}

void WayNetEdit::debugWaynet()
{
	Waynet& waynet=Decker::Objects::GetObjectSystem()->getWaynet();
	if (wp_start == waynet.invalidPoint() || wp_end == waynet.invalidPoint()) return;
	ppl7::PrintDebugTime("===========================================================================\n");
	ppl7::PrintDebugTime("searching for a way from %d:%d => %d:%d\n", wp_start.x, wp_start.y,
		wp_end.x, wp_end.y);

	std::list<Connection> waypoints;
	if (!waynet.findWay(waypoints, wp_start, wp_end)) {
		ppl7::PrintDebugTime("Found no way :-(\n");
	} else {

		std::list<Connection>::const_iterator it;
		for (it=waypoints.begin();it != waypoints.end();++it) {
			const WayPoint& wp1=waynet.getPoint((*it).source);
			const WayPoint& wp2=waynet.getPoint((*it).target);
			ppl7::PrintDebugTime("source: %3d(%5d:%5d), target: %3d(%5d:%5d), type: %-10s, cost: %0.3f\n",
				wp1.as,
				(*it).source.x, (*it).source.y,
				wp2.as,
				(*it).target.x, (*it).target.y,
				(*it).name(), (*it).cost);
		}
	}


}

} //EOF namespace Decker::ui
