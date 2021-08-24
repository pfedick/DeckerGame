#include "decker.h"
#include "ui.h"
#include "objects.h"

namespace Decker::ui {


WayNetEdit::WayNetEdit(int x, int y, int width, int height, Game *game)
: ppl7::tk::Frame(x,y,width,height)
{
	setClientOffset(4,4,4,4);
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();

}


} //EOF namespace Decker::ui

