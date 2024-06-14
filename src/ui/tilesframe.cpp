#include "decker.h"
#include "ui.h"

namespace Decker::ui {

TilesFrame::TilesFrame(int x, int y, int width, int height, Game* game)
	: ppltk::Frame(x, y, width, height)
{
	selected_tile=-1;
	this->game=game;
	this->tiles=NULL;
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	setBorderStyle(ppltk::Frame::Inset);
	setBackgroundColor(ppl7::grafix::Color(32, 32, 32, 255));
	ppl7::grafix::Rect client=this->clientRect();
	scrollbar=new ppltk::Scrollbar(width - 30, 0, 29, client.height());
	scrollbar->setName("tiles-scrollbar");
	//scrollbar->setSize(tiles->numSprites()/4+1);
	scrollbar->setSize(0);
	scrollbar->setEventHandler(this);
	color.set(255, 255, 255, 255);

	this->addChild(scrollbar);

}

void TilesFrame::setSprites(SpriteTexture* tiles)
{
	this->tiles=tiles;
	scrollbar->setSize(tiles->numSprites() / 4 + 1);
	scrollbar->setVisibleItems(height() / 64);
	setSelectedTile(-1);

	needsRedraw();
}

void TilesFrame::paint(ppl7::grafix::Drawable& draw)
{
	//printf ("selected_tile=%d\n",selected_tile);
	Frame::paint(draw);
	//int w=width()-1;
	//int h=height()-1;
	if (tiles == NULL) return;
	ppl7::grafix::Color white(245, 245, 242, 255);
	int x=0, y=0;
	try {
		for (int i=scrollbar->position() * 4;i < tiles->numSprites();i++) {
			try {
				tiles->draw(draw, 2 + x, 2 + y, i, color);
			} catch (...) {
			}
			if (i == selected_tile) {
				draw.drawRect(2 + x, 2 + y, 66 + x, 66 + y, white);
				draw.drawRect(3 + x, 3 + y, 65 + x, 65 + y, white);
			}
			x+=64;
			if (x > 255) {
				x=0;
				y+=64;
			}

		}
	} catch (...) {

	}
}

void TilesFrame::mouseDownEvent(ppltk::MouseEvent* event)
{
	if (tiles == NULL) return;
	if (event->widget() == this && event->buttonMask & ppltk::MouseState::Left) {
		int newtile=(event->p.y / 64) * 4 + (event->p.x / 64) + scrollbar->position() * 4;
		if (selected_tile != newtile && newtile < tiles->numSprites()) {
			selected_tile=newtile;
			needsRedraw();
		}
	}
}

void TilesFrame::mouseMoveEvent(ppltk::MouseEvent* event)
{
	if (tiles == NULL) return;
	if (event->widget() == this && event->buttonMask & ppltk::MouseState::Left) {
		int newtile=(event->p.y / 64) * 4 + (event->p.x / 64) + scrollbar->position() * 4;
		if (selected_tile != newtile && newtile < tiles->numSprites()) {
			selected_tile=newtile;
			needsRedraw();
		}
	}
}

void TilesFrame::setSelectedTile(int nr)
{
	if (tiles == NULL) return;
	if (nr != selected_tile && nr < tiles->numSprites()) {
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

void TilesFrame::mouseWheelEvent(ppltk::MouseEvent* event)
{
	if (tiles == NULL) return;
	scrollbar->mouseWheelEvent(event);
	/*
	if (event->wheel.y != 0) {
		scrollbar->setPosition(scrollbar->position() + event->wheel.y * -1);
		needsRedraw();
	}
	*/
}

void TilesFrame::valueChangedEvent(ppltk::Event* event, int value)
{
	if (event->widget() == scrollbar) {
		needsRedraw();
	}
}

void TilesFrame::setColor(const ppl7::grafix::Color& color)
{
	this->color=color;
	needsRedraw();
}



} //EOF namespace Decker::ui
