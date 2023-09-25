#include "decker.h"
#include "ui.h"

namespace Decker::ui {



LightSelection::LightSelection(int x, int y, int width, int height, Game* game)
	: ppl7::tk::Frame(x, y, width, height)
{
	setClientOffset(4, 4, 4, 4);
	spriteset=NULL;
	scale=1.0f;
	angle=0.0f;
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();
	int yy=0;
	/*
	enable_lighting=new ppl7::tk::CheckBox(80, yy, 80, 30, "enable light", game->getM);
	enable_lighting->setEventHandler(this);
	this->addChild(enable_lighting);
	yy+=40;
	*/

	this->addChild(new ppl7::tk::Label(0, yy, client.width(), 30, "Global Lighting:"));
	yy+=30;
	global_lighting=new ColorSliderWidget(0, yy, client.width(), 100);
	global_lighting->setEventHandler(this);
	global_lighting->setColor(game->getLevel().params.GlobalLighting);
	global_lighting->setColorPreviewSize(64, 90);
	this->addChild(global_lighting);
	yy+=110;



	tilesframe=new TilesFrame(client.left(), yy, client.width(), client.height() - yy - 300, game);
	tilesframe->setEventHandler(this);
	this->addChild(tilesframe);

	colorframe=new ColorSelectionFrame(client.left(), client.height() - 300, client.width() - 10, 300, game->getLevel().palette);
	colorframe->setEventHandler(this);
	this->addChild(colorframe);
	tilesframe->setColor(colorframe->color());
}


void LightSelection::setSpriteSet(SpriteTexture* texture)
{
	tilesframe->setSprites(texture);
}

ppl7::String LightSelection::widgetType() const
{
	return "LightSelection";
}

void LightSelection::setSelectedLight(int nr)
{
	tilesframe->setSelectedTile(nr);
}

int LightSelection::selectedLight() const
{
	return tilesframe->selectedTile();
}

void LightSelection::setLightScale(float factor)
{
	if (factor >= 0.1f && factor <= 4.0f) scale=factor;
}

float LightSelection::lightScale() const
{
	return scale;
}

int LightSelection::colorIndex() const
{
	return colorframe->colorIndex();
}

void LightSelection::setColorIndex(int index)
{
	colorframe->setColorIndex(index);
}

float LightSelection::lightAngle() const
{
	return angle;
}

void LightSelection::setLightAngle(float angle)
{
	this->angle=angle;
}


void LightSelection::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == tilesframe) {
		//game->setSpriteModeToDraw();
	} else if (event->widget() == global_lighting) {
		game->updateLayerForSelectedObject(value);
		game->getLevel().params.GlobalLighting=global_lighting->color();
	} else if (event->widget() == colorframe) {
		tilesframe->setColor(colorframe->color());
	}

}


} //EOF namespace Decker::ui
