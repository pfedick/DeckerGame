#include "decker.h"
#include "ui.h"

namespace Decker::ui {



LightSelection::LightSelection(int x, int y, int width, int height, Game* game)
	: ppl7::tk::Frame(x, y, width, height)
{
	setClientOffset(4, 4, 4, 4);
	spriteset=NULL;
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


	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "LightId:"));
	light_id=new ppl7::tk::Label(70, yy, client.width(), 30, "-");
	light_id->setBorderStyle(ppl7::tk::Frame::BorderStyle::Inset);
	this->addChild(light_id);
	yy+=30;
	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "States:"));
	initial_state=new ppl7::tk::CheckBox(70, yy, 100, 30, "initial");
	initial_state->setEventHandler(this);
	this->addChild(initial_state);
	current_state=new ppl7::tk::CheckBox(70 + 90, yy, 100, 30, "current");
	current_state->setEventHandler(this);
	this->addChild(current_state);

	yy+=30;

	this->addChild(new ppl7::tk::Label(0, yy, client.width(), 30, "LightObject:"));
	yy+=30;
	tilesframe=new TilesFrame(client.left(), yy, client.width(), client.height() - yy - 530, game);
	tilesframe->setEventHandler(this);
	this->addChild(tilesframe);

	yy=client.height() - 520;
	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "Planes:"));
	front_plane_checkbox=new ppl7::tk::CheckBox(70, yy, 90, 30, "front");
	front_plane_checkbox->setEventHandler(this);
	this->addChild(front_plane_checkbox);
	player_plane_checkbox=new ppl7::tk::CheckBox(70 + 70, yy, 90, 30, "player", true);
	player_plane_checkbox->setEventHandler(this);
	this->addChild(player_plane_checkbox);
	back_plane_checkbox=new ppl7::tk::CheckBox(70 + 140, yy, 90, 30, "back", true);
	back_plane_checkbox->setEventHandler(this);
	this->addChild(back_plane_checkbox);
	yy+=30;

	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "Intensity:"));
	intensity=new ppl7::tk::HorizontalSlider(70, yy, client.width() - 70, 30);
	intensity->setEventHandler(this);
	intensity->setLimits(0, 255);
	intensity->enableSpinBox(true, 1, 80);
	intensity->setValue(255);
	this->addChild(intensity);
	yy+=30;

	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "Scale x:"));
	scale_x=new ppl7::tk::DoubleHorizontalSlider(70, yy, client.width() - 70, 30);
	scale_x->setEventHandler(this);
	scale_x->setLimits(0.1f, 5.0f);
	scale_x->enableSpinBox(true, 0.05, 1, 80);
	scale_x->setValue(1.0f);
	this->addChild(scale_x);
	yy+=30;

	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "Scale y:"));
	scale_y=new ppl7::tk::DoubleHorizontalSlider(70, yy, client.width() - 70, 30);
	scale_y->setEventHandler(this);
	scale_y->setLimits(0.1f, 5.0f);
	scale_y->enableSpinBox(true, 0.05, 1, 80);
	scale_y->setValue(1.0f);
	this->addChild(scale_y);
	yy+=30;

	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "Angle:"));
	angle=new ppl7::tk::DoubleHorizontalSlider(70, yy, client.width() - 70, 30);
	angle->setEventHandler(this);
	angle->setLimits(0.0f, 355.0f);
	angle->enableSpinBox(true, 5, 0, 80);
	angle->setValue(0.0f);
	this->addChild(angle);
	yy+=30;
	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "Lensflare:"));
	lensflare=new ppl7::tk::CheckBox(70, yy, 80, 30, "yes", false);
	lensflare->setEventHandler(this);
	this->addChild(lensflare);
	yy+=30;
	this->addChild(new ppl7::tk::Label(0, yy, 70, 30, "Intensity:"));
	lensflareIntensity=new ppl7::tk::HorizontalSlider(70, yy, client.width() - 70, 30);
	lensflareIntensity->setEventHandler(this);
	lensflareIntensity->setLimits(0, 255);
	lensflareIntensity->enableSpinBox(true, 1, 80);
	lensflareIntensity->setValue(255);
	this->addChild(lensflareIntensity);
	yy+=40;


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

void LightSelection::setLightScaleX(float factor)
{
	if (factor >= 0.01f && factor <= 5.0f) scale_x->setValue(factor);
}

float LightSelection::lightScaleX() const
{
	return scale_x->value();
}

void LightSelection::setLightScaleY(float factor)
{
	if (factor >= 0.01f && factor <= 5.0f) scale_y->setValue(factor);
}

float LightSelection::lightScaleY() const
{
	return scale_y->value();
}

int LightSelection::colorIndex() const
{
	return colorframe->colorIndex();
}

void LightSelection::setColorIndex(int index)
{
	colorframe->setColorIndex(index);
}


int LightSelection::colorIntensity() const
{
	return intensity->value();
}

void LightSelection::setColorIntensity(int index)
{
	intensity->setValue(index);
}


float LightSelection::lightAngle() const
{
	return angle->value();
}

void LightSelection::setLightAngle(float angle)
{
	this->angle->setValue(angle);
}

void LightSelection::setPlayerPlaneMatrix(LightPlayerPlaneMatrix matrix)
{
	player_plane_checkbox->setChecked(static_cast<int>(matrix) & static_cast<int>(LightPlayerPlaneMatrix::Player));
	front_plane_checkbox->setChecked(static_cast<int>(matrix) & static_cast<int>(LightPlayerPlaneMatrix::Front));
	back_plane_checkbox->setChecked(static_cast<int>(matrix) & static_cast<int>(LightPlayerPlaneMatrix::Back));
}

LightPlayerPlaneMatrix LightSelection::getPlayerPlaneMatrix() const
{
	int v=0;
	if (player_plane_checkbox->checked()) v|=static_cast<int>(LightPlayerPlaneMatrix::Player);
	if (front_plane_checkbox->checked()) v|=static_cast<int>(LightPlayerPlaneMatrix::Front);
	if (back_plane_checkbox->checked()) v|=static_cast<int>(LightPlayerPlaneMatrix::Back);
	return static_cast<LightPlayerPlaneMatrix>(v);
}



void LightSelection::setCurrentState(bool enabled)
{
	current_state->setChecked(enabled);
}

void LightSelection::setInitialState(bool enabled)
{
	initial_state->setChecked(enabled);
}

void LightSelection::setLensflare(bool enabled)
{
	lensflare->setChecked(enabled);
}

void LightSelection::setLensflareIntensity(uint8_t intensity)
{
	lensflareIntensity->setValue(intensity);
}

void LightSelection::setLightId(uint32_t id)
{
	light_id->setText(ppl7::ToString("%d", id));
}

bool LightSelection::getCurrentState() const
{
	return current_state->checked();
}

bool LightSelection::getInitialState() const
{
	return initial_state->checked();
}

bool LightSelection::getLensflare() const
{
	return lensflare->checked();
}

uint8_t LightSelection::getLensflareIntensity() const
{
	return (uint8_t)lensflareIntensity->value();
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
		//game->updateLightFromUi();
	}

}


void LightSelection::valueChangedEvent(ppl7::tk::Event* event, double value)
{
	//game->updateLightFromUi();
}

void LightSelection::valueChangedEvent(ppl7::tk::Event* event, int64_t value)
{
	//game->updateLightFromUi();
}

} //EOF namespace Decker::ui
