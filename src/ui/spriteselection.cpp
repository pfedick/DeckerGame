#include "decker.h"
#include "ui.h"

namespace Decker::ui {


SpriteSelection::SpriteSelection(int x, int y, int width, int height, Game* game)
	: ppltk::Frame(x, y, width, height)
{
	notifies_enabled=false;
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

	this->addChild(new ppltk::Label(5, y1, 80, 30, "Plane: "));
	plane_combobox=new ppltk::ComboBox(85, y1, client.width() - 85, 25);
	plane_combobox->add("PlayerPlane", "0");
	plane_combobox->add("FrontPlane", "1");
	plane_combobox->add("FarPlane", "2");
	plane_combobox->add("BackPlane", "3");
	plane_combobox->add("MiddlePlane", "4");
	plane_combobox->add("HorizonPlane", "5");
	plane_combobox->add("NearPlane", "6");
	plane_combobox->setEventHandler(this);
	this->addChild(plane_combobox);
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
	y1+=30;
	this->addChild(new ppltk::Label(5, y1, 80, 30, "Scale: "));
	scale_slider=new ppltk::DoubleHorizontalSlider(85, y1, client.width() - 85, 30);
	scale_slider->setValue(1.0f);
	scale_slider->setLimits(0.1f, 2.0f);
	scale_slider->enableSpinBox(true, 0.05, 2, 70);
	scale_slider->setEventHandler(this);
	this->addChild(scale_slider);
	y1+=30;

	this->addChild(new ppltk::Label(5, y1, 80, 30, "Rotation: "));
	rotation_slider=new ppltk::DoubleHorizontalSlider(85, y1, client.width() - 85, 30);
	rotation_slider->setValue(0.0f);
	rotation_slider->setLimits(0.0f, 360.0f);
	rotation_slider->enableSpinBox(true, 5.0f, 0, 70);
	rotation_slider->setEventHandler(this);
	this->addChild(rotation_slider);

	y1+=35;

	tilesframe=new TilesFrame(5, y1, client.width() - 8, client.height() - 300 - y1, game);
	this->addChild(tilesframe);

	colorframe=new ColorSelectionFrame(5, client.height() - 300, client.width() - 8, 300, game->getLevel().palette);
	colorframe->setEventHandler(this);
	this->addChild(colorframe);
	tilesframe->setColor(colorframe->color());
	notifies_enabled=true;
}

void SpriteSelection::enableNotfies(bool enable)
{
	notifies_enabled=enable;
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
	tilesframe->setSelectedTile(-1);
	//tileset_combobox->setCurrentIndex(id - 1);
	tileset_combobox->setCurrentIdentifier(ppl7::ToString("%d", id));
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

void SpriteSelection::setPlane(int plane)
{
	plane_combobox->setCurrentIdentifier(ppl7::ToString("%d", plane));
}

int SpriteSelection::plane() const
{
	return plane_combobox->currentIdentifier().toInt();
}

void SpriteSelection::setSpriteScale(float factor)
{
	if (factor >= 0.1f && factor <= 2.0f) scale_slider->setValue(factor);
}

float SpriteSelection::spriteScale() const
{
	return scale_slider->value();
}

void SpriteSelection::setSpriteRotation(float rotation)
{
	rotation_slider->setValue(rotation);
}

float SpriteSelection::spriteRotation() const
{
	return rotation_slider->value();
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
		//ppl7::PrintDebug("value=%d, text=%s\n", v, (const char*)tileset_combobox->currentText());
		setCurrentSpriteSet(v);
	} else if (event->widget() == colorframe) {
		tilesframe->setColor(colorframe->color());
		if (notifies_enabled) game->updateSpriteFromUi();
	} else if (event->widget() == plane_combobox) {
		if (notifies_enabled) game->updateSpriteFromUi();
	}
}


void SpriteSelection::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	if (event->widget() == z_axis) {
		if (notifies_enabled) game->updateSpriteFromUi();
	}
}

void SpriteSelection::valueChangedEvent(ppltk::Event* event, double value)
{
	if (event->widget() == scale_slider || event->widget() == rotation_slider) {
		if (notifies_enabled) game->updateSpriteFromUi();
	}
}



void SpriteSelection::toggledEvent(ppltk::Event* event, bool checked)
{
	//ppl7::PrintDebug("SpriteSelection::toggledEvent\n");
	if ((event->widget() == layer0 || event->widget() == layer1) && checked == true) {
		//ppl7::PrintDebug("   SpriteSelection::toggledEvent => yes!\n");
		if (notifies_enabled) game->updateSpriteFromUi();
	}
}


} //EOF namespace Decker::ui
