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




TilesSelection::TilesSelection(int x, int y, int width, int height, Game *game)
: ppl7::tk::Frame(x,y,width,height)
{
	for (int i=0;i<=MAX_TILESETS;i++)
		tilesets[i]=NULL;
	tileset=1;
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();

	this->addChild(new ppl7::tk::Label(5,10,100,20,"Tiletype: "));
	tiletypesframe=new TilesFrame(5,30,client.width()-10, 200, game);
	this->addChild(tiletypesframe);

	this->addChild(new ppl7::tk::Label(5,240,80,30,"Tileset: "));
	tileset_combobox=new ComboBox(85,240,client.width()-85,25);
	tileset_combobox->setEventHandler(this);
	this->addChild(tileset_combobox);

	this->addChild(new ppl7::tk::Label(5,270,70,20,"Layer: "));
	layer0=new RadioButton(60,270,50,20,"0", true);
	this->addChild(layer0);

	layer1=new RadioButton(110,270,50,20,"1");
	this->addChild(layer1);

	layer2=new RadioButton(160,270,50,20,"2");
	this->addChild(layer2);

	tilesframe=new TilesFrame(5,300,client.width()-10, client.height()-300, game);
	this->addChild(tilesframe);

}

void TilesSelection::setSelectedTile(int nr)
{
	tilesframe->setSelectedTile(nr);
}

int TilesSelection::selectedTile() const
{
	return tilesframe->selectedTile();
}

int TilesSelection::currentLayer() const
{
	if (layer0->checked()) return 0;
	if (layer1->checked()) return 1;
	if (layer2->checked()) return 2;

	return 0;
}

void TilesSelection::setCurrentTileSet(int id)
{
	if (id<1 || id>MAX_TILESETS) return;
	if (tilesets[id]==NULL) return;
	tileset=id;
	tilesframe->setSprites(tilesets[id]);
	tileset_combobox->setCurrentIndex(id-1);
}

int TilesSelection::currentTileSet() const
{
	return tileset;
}

void TilesSelection::setTileSet(int id, const ppl7::String &name, ppl7::grafix::Sprite *sprites)
{
	if (id<1 || id>MAX_TILESETS) return;
	tilesets[id]=sprites;
	tilesetName[id]=name;
	tileset_combobox->add(name,ppl7::ToString("%d",id));
	if (id==1) setCurrentTileSet(1);

}

void TilesSelection::setTileTypesSprites(ppl7::grafix::Sprite *tiles)
{
	tiletypesframe->setSprites(tiles);
}

void TilesSelection::valueChangedEvent(ppl7::tk::Event *event, int value)
{
	if (event->widget()==tileset_combobox) {
		//printf("value=%d\n",value);
		setCurrentTileSet(value+1);
	}
}



} //EOF namespace Decker::ui

