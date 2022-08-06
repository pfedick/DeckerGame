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


	this->addChild(new ppl7::tk::Label(0, y, 60, 30, "Cost:"));
	cost=new ppl7::tk::LineInput(60, y, 100, 30, "1");
	this->addChild(cost);
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


int WayNetEdit::getCost() const
{
	int value=cost->text().toInt();
	if (value < 1) value=1;
	if (value > 255) value=255;
	return value;
}



} //EOF namespace Decker::ui
