#include "decker.h"
#include "ui.h"

namespace Decker::ui {


SpriteSelection::SpriteSelection(int x, int y, int width, int height, Game* game)
	: ppltk::Frame(x, y, width, height)
{
	for (int i=0;i <= MAX_SPRITESETS;i++)
		tilesets[i]=NULL;
	tileset=1;
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();
	int y1=5;

	this->addChild(new ppltk::Label(5, y1, 80, 30, "Spriteset: "));
	tileset_combobox=new ppltk::ComboBox(85, y1, client.width() - 85, 25);
	tileset_combobox->setEventHandler(this);
	this->addChild(tileset_combobox);
	y1+=30;

	this->addChild(new ppltk::Label(5, y1, 70, 20, "Layer: "));
	layer0=new ppltk::RadioButton(60, y1, 110, 20, "before Tiles", true);
	layer0->setEventHandler(this);
	this->addChild(layer0);

	layer1=new ppltk::RadioButton(170, y1, 110, 20, "behind Tiles");
	layer1->setEventHandler(this);
	this->addChild(layer1);
	y1+=30;
	this->addChild(new ppltk::Label(5, y1, 80, 30, "Z-Axis: "));
	z_axis=new ppltk::HorizontalSlider(85, y1, client.width() - 85, 30);
	z_axis->setLimits(0, 15);
	z_axis->enableSpinBox(true, 1, 70);
	z_axis->setEventHandler(this);
	this->addChild(z_axis);

	y1+=35;

	tilesframe=new TilesFrame(5, y1, client.width() - 8, client.height() - 300 - y1, game);
	this->addChild(tilesframe);
	scale=1.0f;
	rotation=0.0f;

	colorframe=new ColorSelectionFrame(5, client.height() - 300, client.width() - 8, 300, game->getLevel().palette);
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

void SpriteSelection::setZAxis(int z)
{
	z_axis->setValue(z);
}

int SpriteSelection::zAxis() const
{
	return ((int)z_axis->value());
}

void SpriteSelection::valueChangedEvent(ppltk::Event* event, int value)
{
	if (event->widget() == tileset_combobox) {
		int v=tileset_combobox->currentIdentifier().toInt();
		//printf("value=%d\n", v);
		setCurrentSpriteSet(v);
	} else if (event->widget() == colorframe) {
		tilesframe->setColor(colorframe->color());
		game->updateSpriteFromUi();
	}
}


void SpriteSelection::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	if (event->widget() == z_axis) {
		game->updateSpriteFromUi();
	}
}

void SpriteSelection::toggledEvent(ppltk::Event* event, bool checked)
{
	//ppl7::PrintDebug("SpriteSelection::toggledEvent\n");
	if ((event->widget() == layer0 || event->widget() == layer1) && checked == true) {
		//ppl7::PrintDebug("   SpriteSelection::toggledEvent => yes!\n");
		game->updateSpriteFromUi();
	}
}


} //EOF namespace Decker::ui
