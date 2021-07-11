#include "decker.h"
#include "ui.h"

namespace Decker::ui {


SpriteSelection::SpriteSelection(int x, int y, int width, int height, Game *game)
: ppl7::tk::Frame(x,y,width,height)
{
	for (int i=0;i<=MAX_TILESETS;i++)
		tilesets[i]=NULL;
	tileset=1;
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();

	this->addChild(new ppl7::tk::Label(5,5,80,30,"Spriteset: "));
	tileset_combobox=new ComboBox(85,5,client.width()-85,25);
	tileset_combobox->setEventHandler(this);
	this->addChild(tileset_combobox);

	this->addChild(new ppl7::tk::Label(5,35,70,20,"Layer: "));
	layer0=new RadioButton(60,35,110,20,"before Tiles", true);
	this->addChild(layer0);

	layer1=new RadioButton(170,35,110,20,"behind Tiles");
	this->addChild(layer1);

	tilesframe=new TilesFrame(5,60,client.width()-10, client.height()-60, game);
	this->addChild(tilesframe);
	scale=1.0f;

}

void SpriteSelection::setSelectedSprite(int nr)
{
	tilesframe->setSelectedTile(nr);
}

int SpriteSelection::selectedSprite() const
{
	return tilesframe->selectedTile();
}

int SpriteSelection::currentLayer() const
{
	if (layer0->checked()) return 1;
	if (layer1->checked()) return 0;
	return 1;
}

void SpriteSelection::setCurrentSpriteSet(int id)
{
	if (id<1 || id>MAX_TILESETS) return;
	if (tilesets[id]==NULL) return;
	tileset=id;
	tilesframe->setSprites(tilesets[id]);
	tileset_combobox->setCurrentIndex(id-1);
}

int SpriteSelection::currentSpriteSet() const
{
	return tileset;
}

void SpriteSelection::setSpriteSet(int id, const ppl7::String &name, Sprite *sprites)
{
	if (id<1 || id>MAX_TILESETS) return;
	tilesets[id]=sprites;
	tilesetName[id]=name;
	tileset_combobox->add(name,ppl7::ToString("%d",id));
	if (id==1) setCurrentSpriteSet(1);

}

void SpriteSelection::setSpriteScale(float factor)
{
	if (factor>=0.1f && factor<=2.0f) scale=factor;
}

float SpriteSelection::spriteScale() const
{
	return scale;
}


void SpriteSelection::valueChangedEvent(ppl7::tk::Event *event, int value)
{
	if (event->widget()==tileset_combobox) {
		//printf("value=%d\n",value);
		setCurrentSpriteSet(value+1);
	}
}



} //EOF namespace Decker::ui

