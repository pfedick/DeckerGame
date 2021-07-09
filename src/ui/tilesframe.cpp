#include "decker.h"
#include "ui.h"

namespace Decker::ui {

TilesFrame::TilesFrame(int x, int y, int width, int height, Game *game)
: ppl7::tk::Frame(x,y,width,height)
{
	selected_tile=0;
	this->game=game;
	this->tiles=NULL;
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	setBorderStyle(ppl7::tk::Frame::Inset);
	setBackgroundColor(ppl7::grafix::Color(32,32,32,255));
	ppl7::grafix::Rect client=this->clientRect();
	scrollbar=new Scrollbar(width-30,0,29,client.height());
	scrollbar->setName("tiles-scrollbar");
	//scrollbar->setSize(tiles->numSprites()/4+1);
	scrollbar->setSize(0);
	scrollbar->setEventHandler(this);

	this->addChild(scrollbar);

}

void TilesFrame::setSprites(ppl7::grafix::Sprite *tiles)
{
	this->tiles=tiles;
	scrollbar->setSize(tiles->numSprites()/4+1);
	setSelectedTile(0);
	needsRedraw();
}

void TilesFrame::paint(Drawable &draw)
{
	Frame::paint(draw);
	//int w=width()-1;
	//int h=height()-1;
	if (tiles==NULL) return;
	ppl7::grafix::Color white(245,245,242,255);
	int x=0, y=0;
	try {
		for (int i=scrollbar->position()*4;i<tiles->numSprites();i++) {
			try {
				tiles->draw(draw, 2+x, 2+y,i);
			} catch (...) {
			}
			if (i==selected_tile) {
				draw.drawRect(2+x,2+y,66+x,66+y,white);
				draw.drawRect(3+x,3+y,65+x,65+y,white);
			}
			x+=64;
			if (x>255) {
				x=0;
				y+=64;
			}

		}
	} catch (...) {

	}
}

void TilesFrame::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	if (tiles==NULL) return;
	if (event->widget()==this && event->buttonMask&MouseState::Left) {
		int newtile=(event->p.y/64)*4+(event->p.x/64)+scrollbar->position()*4;
		if (selected_tile!=newtile && newtile<tiles->numSprites()) {
			selected_tile=newtile;
			needsRedraw();
		}
	}
}

void TilesFrame::mouseMoveEvent(ppl7::tk::MouseEvent *event)
{
	if (tiles==NULL) return;
	if (event->widget()==this && event->buttonMask&MouseState::Left) {
		int newtile=(event->p.y/64)*4+(event->p.x/64)+scrollbar->position()*4;
		if (selected_tile!=newtile && newtile<tiles->numSprites()) {
			selected_tile=newtile;
			needsRedraw();
		}
	}
}

void TilesFrame::setSelectedTile(int nr)
{
	if (tiles==NULL) return;
	if (nr!=selected_tile && nr<tiles->numSprites()) {
		selected_tile=nr;
		//ppl7::grafix::Rect client=this->clientRect();
		//int max_visible=client.height()/64;

		needsRedraw();
	}
}

int TilesFrame::selectedTile() const
{
	return selected_tile;
}

void TilesFrame::mouseWheelEvent(ppl7::tk::MouseEvent *event)
{
	if (tiles==NULL) return;
	if (event->wheel.y!=0) {
		scrollbar->setPosition(scrollbar->position()+event->wheel.y*-1);
		needsRedraw();
	}
}

void TilesFrame::valueChangedEvent(ppl7::tk::Event *event, int value)
{
	if (event->widget()==scrollbar) {
		needsRedraw();
	}
}




} //EOF namespace Decker::ui

