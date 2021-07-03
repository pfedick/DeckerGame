#include "decker.h"
#include "ui.h"

namespace Decker::ui {

TilesFrame::TilesFrame(int x, int y, int width, int height, Game *game, ppl7::grafix::Sprite *tiles)
: ppl7::tk::Frame(x,y,width,height)
{
	this->game=game;
	this->tiles=tiles;
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	setBorderStyle(ppl7::tk::Frame::Inset);
	setBackgroundColor(ppl7::grafix::Color(32,32,32,255));
	ppl7::grafix::Rect client=this->clientRect();
	scrollbar=new Scrollbar(width-30,0,29,client.height());
	scrollbar->setName("tiles-scrollbar");
	selectedTile=0;
	this->addChild(scrollbar);

}

void TilesFrame::paint(Drawable &draw)
{
	Frame::paint(draw);
	int w=width()-1;
	int h=height()-1;
	//Drawable client=clientDrawable(draw);
	// ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	//SDL_Renderer *renderer=game->getSDLRenderer();
	ppl7::grafix::Color white(245,245,242,255);
	int x=0, y=0;
	for (int i=0;i<tiles->numSprites();i++) {
		tiles->draw(draw, 2+x, 66+y,i);
		if (i==selectedTile) {
			draw.drawRect(2+x,2+y,66+x,66+y,white);
			draw.drawRect(3+x,3+y,65+x,65+y,white);
		}
		x+=64;
		if (x>255) {
			x=0;
			y+=64;
		}

	}
	//draw.drawRect(0,0,w,h,Color(0,255,0,255));
}

void TilesFrame::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	//printf("x=%d, y=%d\n", event->p.x, event->p.y);
	int newtile=(event->p.y/64)*4+(event->p.x/64);
	if (selectedTile!=newtile && newtile<tiles->numSprites()) {
		selectedTile=newtile;
		needsRedraw();
	}
}

void TilesFrame::mouseMoveEvent(ppl7::tk::MouseEvent *event)
{
	//printf("x=%d, y=%d\n", event->p.x, event->p.y);
	if (event->buttonMask&MouseState::Left) {
		int newtile=(event->p.y/64)*4+(event->p.x/64);
		if (selectedTile!=newtile && newtile<tiles->numSprites()) {
			selectedTile=newtile;
			needsRedraw();
		}
	}
}




TilesSelection::TilesSelection(int x, int y, int width, int height, Game *game, ppl7::grafix::Sprite *tiles)
: ppl7::tk::Frame(x,y,width,height)
{
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();
	tilesframe=new TilesFrame(5,50,client.width()-10, client.height()-60, game, tiles);

	this->addChild(tilesframe);

}


} //EOF namespace Decker::ui

