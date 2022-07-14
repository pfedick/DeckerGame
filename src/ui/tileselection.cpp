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

	tilesframe=new TilesFrame(5, 60, client.width() - 10, client.height() - 60 - 300, game);
	this->addChild(tilesframe);


	colorframe=new ColorSelectionFrame(5, client.height() - 300, client.width() - 10, 300, game->getLevel().palette);
	colorframe->setEventHandler(this);
	this->addChild(colorframe);
	tilesframe->setColor(colorframe->color());
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

int TilesSelection::colorIndex() const
{
	return colorframe->colorIndex();
}

void TilesSelection::setColorIndex(int index)
{
	colorframe->setColorIndex(index);
}

void TilesSelection::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == tileset_combobox) {
		//printf("value=%d\n",value);
		setCurrentTileSet(value + 1);
	} else if (event->widget() == colorframe) {
		tilesframe->setColor(colorframe->color());
	}
}


ColorSelectionFrame::ColorSelectionFrame(int x, int y, int width, int height, ColorPalette& palette)
	: ppl7::tk::Frame(x, y, width, height, ppl7::tk::Frame::BorderStyle::Upset), palette(palette)
{
	color_index=2;
	spinbox=new ppl7::tk::SpinBox(0, 0, 70, 30, color_index);
	spinbox->setLimits(0, 255);
	spinbox->setEventHandler(this);
	this->addChild(spinbox);
}


int ColorSelectionFrame::colorIndex() const
{
	return color_index;
}

ppl7::grafix::Color ColorSelectionFrame::color() const
{
	return palette.getColor(color_index);
}

void ColorSelectionFrame::setColorIndex(int index)
{
	color_index=index;
}

void ColorSelectionFrame::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	ppl7::tk::Widget* w=event->widget();
	if (w == spinbox) {
		color_index=spinbox->value();
		ppl7::tk::Event new_event(ppl7::tk::Event::ValueChanged);
		new_event.setWidget(this);
		EventHandler::valueChangedEvent(&new_event, color_index);
	}
}



} //EOF namespace Decker::ui
