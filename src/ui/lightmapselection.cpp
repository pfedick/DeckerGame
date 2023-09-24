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

	/*

	this->addChild(new ppl7::tk::Label(0, yy, 80, 30, "Layer:"));
	layer_selection=new ppl7::tk::ComboBox(80, yy, width - 90, 30);
	layer_selection->add("Before Player", ppl7::ToString("%d", static_cast<int>(Decker::Objects::Object::Layer::BeforePlayer)));
	layer_selection->add("Behind Player", ppl7::ToString("%d", static_cast<int>(Decker::Objects::Object::Layer::BehindPlayer)));
	layer_selection->add("Behind Bricks", ppl7::ToString("%d", static_cast<int>(Decker::Objects::Object::Layer::BehindBricks)));
	layer_selection->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(Decker::Objects::Object::Layer::BehindPlayer)));
	layer_selection->setEventHandler(this);
	this->addChild(layer_selection);
	yy+=30;
	this->addChild(new ppl7::tk::Label(0, yy, 80, 30, "difficulty:"));
	difficulty_easy=new ppl7::tk::CheckBox(80, yy, 80, 30, "easy", true);
	difficulty_easy->setEventHandler(this);
	this->addChild(difficulty_easy);
	difficulty_normal=new ppl7::tk::CheckBox(145, yy, 100, 30, "normal", true);
	difficulty_normal->setEventHandler(this);
	this->addChild(difficulty_normal);
	difficulty_hard=new ppl7::tk::CheckBox(225, yy, 90, 30, "hard", true);
	difficulty_hard->setEventHandler(this);
	this->addChild(difficulty_hard);
	yy+=30;
	this->addChild(new ppl7::tk::Label(0, yy, width, 30, "Object selection:"));
	yy+=30;
	*/

	tilesframe=new TilesFrame(client.left(), yy, client.width(), client.height() - yy, game);
	tilesframe->setEventHandler(this);
	this->addChild(tilesframe);

}


void LightSelection::setSpriteSet(SpriteTexture* texture)
{
	tilesframe->setSprites(texture);
}

ppl7::String LightSelection::widgetType() const
{
	return "LightSelection";
}

void LightSelection::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == tilesframe) {
		//game->setSpriteModeToDraw();
	} else if (event->widget() == global_lighting) {
		game->updateLayerForSelectedObject(value);
		game->getLevel().params.GlobalLighting=global_lighting->color();
	}

}


} //EOF namespace Decker::ui
