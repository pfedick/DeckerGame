#include "decker.h"
#include "ui.h"

namespace Decker::ui {


TilesSelection::TilesSelection(int x, int y, int width, int height, Game* game)
	: ppl7::tk::Frame(x, y, width, height)
{
	for (int i=0;i <= MAX_TILESETS;i++)
		tilesets[i]=NULL;
	tileset=1;
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();

	this->addChild(new ppl7::tk::Label(5, 5, 80, 30, "Tileset: "));
	tileset_combobox=new ppl7::tk::ComboBox(85, 5, client.width() - 85, 25);
	tileset_combobox->setEventHandler(this);
	this->addChild(tileset_combobox);

	this->addChild(new ppl7::tk::Label(5, 35, 70, 20, "Layer: "));
	layer0=new ppl7::tk::RadioButton(60, 35, 50, 20, "0", true);
	this->addChild(layer0);

	layer1=new ppl7::tk::RadioButton(110, 35, 50, 20, "1");
	this->addChild(layer1);

	layer2=new ppl7::tk::RadioButton(160, 35, 50, 20, "2");
	this->addChild(layer2);

	layer3=new ppl7::tk::RadioButton(210, 35, 50, 20, "3");
	this->addChild(layer3);

	tilesframe=new TilesFrame(5, 60, client.width() - 10, client.height() - 60, game);
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
	if (layer3->checked()) return 3;

	return 0;
}

void TilesSelection::setCurrentTileSet(int id)
{
	if (id<1 || id>MAX_TILESETS) return;
	if (tilesets[id] == NULL) return;
	tileset=id;
	tilesframe->setSprites(tilesets[id]);
	tileset_combobox->setCurrentIndex(id - 1);
}

int TilesSelection::currentTileSet() const
{
	return tileset;
}

void TilesSelection::setTileSet(int id, const ppl7::String& name, SpriteTexture* sprites)
{
	if (id<1 || id>MAX_TILESETS) return;
	tilesets[id]=sprites;
	tilesetName[id]=name;
	tileset_combobox->add(name, ppl7::ToString("%d", id));
	if (id == 1) setCurrentTileSet(1);

}

void TilesSelection::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == tileset_combobox) {
		//printf("value=%d\n",value);
		setCurrentTileSet(value + 1);
	}
}



} //EOF namespace Decker::ui
