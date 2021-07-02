#include "decker.h"
#include "ui.h"

namespace Decker::ui {


TilesSelection::TilesSelection(int x, int y, int width, int height, Game *game, ppl7::grafix::Sprite *tiles)
: ppl7::tk::Frame(x,y,width,height)
{
	this->game=game;
	this->tiles=tiles;
	ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();

	ppl7::grafix::Rect client=this->clientRect();
	scrollbar=new Scrollbar(client.width()-30,client.top(),30,client.height());

	this->addChild(scrollbar);

}

void TilesSelection::paint(Drawable &draw)
{
	SDL_Renderer *renderer=game->getSDLRenderer();
	Frame::paint(draw);
	int x=0, y=0;
	for (int i=0;i<tiles->numSprites();i++) {
		tiles->draw(draw, x, y,i);
		x+=64;
		if (x>255) {
			x=0;
			y+=64;
		}

	}
	//draw.cls();
}

void TilesSelection::mouseClickEvent(ppl7::tk::MouseEvent *event)
{
}

} //EOF namespace Decker

