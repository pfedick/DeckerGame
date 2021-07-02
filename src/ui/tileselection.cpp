#include "decker.h"
#include "ui.h"

namespace Decker::ui {


TilesSelection::TilesSelection(int x, int y, int width, int height, Game *game, ::Sprite *tiles)
: ppl7::tk::Frame(x,y,width,height)
{
	this->game=game;
	this->tiles=tiles;
	ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
}

void TilesSelection::mouseClickEvent(ppl7::tk::MouseEvent *event)
{
}

} //EOF namespace Decker

