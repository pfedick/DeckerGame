#include "decker.h"
#include "ui.h"

namespace Decker::ui {


SpriteSelection::SpriteSelection(int x, int y, int width, int height, Game* game)
	: ppl7::tk::Frame(x, y, width, height)
{
	for (int i=0;i <= MAX_SPRITESETS;i++)
		tilesets[i]=NULL;
	tileset=1;
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();

	this->addChild(new ppl7::tk::Label(5, 5, 80, 30, "Spriteset: "));
	tileset_combobox=new ppl7::tk::ComboBox(85, 5, client.width() - 85, 25);
	tileset_combobox->setEventHandler(this);
	this->addChild(tileset_combobox);

	this->addChild(new ppl7::tk::Label(5, 35, 70, 20, "Layer: "));
	layer0=new ppl7::tk::RadioButton(60, 35, 110, 20, "before Tiles", true);
	this->addChild(layer0);

	layer1=new ppl7::tk::RadioButton(170, 35, 110, 20, "behind Tiles");
	this->addChild(layer1);

	tilesframe=new TilesFrame(5, 60, client.width() - 10, client.height() - 360, game);
	this->addChild(tilesframe);
	scale=1.0f;
	rotation=0.0f;

	colorframe=new ColorSelectionFrame(5, client.height() - 300, client.width() - 10, 300, game->getLevel().palette);
	colorframe->setEventHandler(this);
	this->addChild(colorframe);
	tilesframe->setColor(colorframe->color());
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

int SpriteSelection::spriteSetDimensions() const
{
	return SpriteDimensions[tileset];
}

void SpriteSelection::setCurrentLayer(int layer)
{
	if (layer == 1) layer0->setChecked(true);
	if (layer == 0) layer1->setChecked(true);
}

void SpriteSelection::setCurrentSpriteSet(int id)
{
	if (id<1 || id>MAX_SPRITESETS) return;
	if (tilesets[id] == NULL) return;
	tileset=id;
	tilesframe->setSprites(tilesets[id]);
	tileset_combobox->setCurrentIndex(id - 1);
}

int SpriteSelection::currentSpriteSet() const
{
	return tileset;
}

void SpriteSelection::setSpriteSet(int id, const ppl7::String& name, SpriteTexture* sprites, int dimensions)
{
	if (id<1 || id>MAX_SPRITESETS) return;
	tilesets[id]=sprites;
	tilesetName[id]=name;
	SpriteDimensions[id]=dimensions;
	tileset_combobox->add(name, ppl7::ToString("%d", id));
	if (id == 1) setCurrentSpriteSet(1);

}

void SpriteSelection::setSpriteScale(float factor)
{
	if (factor >= 0.1f && factor <= 2.0f) scale=factor;
}

float SpriteSelection::spriteScale() const
{
	return scale;
}

void SpriteSelection::setSpriteRotation(float rotation)
{
	this->rotation=rotation;
}

float SpriteSelection::spriteRotation() const
{
	return rotation;
}

int SpriteSelection::colorIndex() const
{
	return colorframe->colorIndex();
}

void SpriteSelection::setColorIndex(int index)
{
	colorframe->setColorIndex(index);
}


void SpriteSelection::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == tileset_combobox) {
		int v=tileset_combobox->currentIdentifier().toInt();
		//printf("value=%d\n", v);
		setCurrentSpriteSet(v);
	} else if (event->widget() == colorframe) {
		tilesframe->setColor(colorframe->color());
	}
}



} //EOF namespace Decker::ui
