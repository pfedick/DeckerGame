#include "decker.h"
#include "ui.h"

namespace Decker::ui {


ObjectSelection::ObjectSelection(int x, int y, int width, int height, Game *game)
: ppl7::tk::Frame(x,y,width,height)
{
	spriteset=NULL;
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();

	tilesframe=new TilesFrame(5,60,client.width()-10, client.height()-60, game);
	this->addChild(tilesframe);
	scale=1.0f;

}

void ObjectSelection::setSpriteSet(SpriteTexture *texture)
{
	spriteset=texture;
}

void ObjectSelection::setSpriteScale(float factor)
{
	if (factor>=0.1f && factor<=2.0f) scale=factor;
}

float ObjectSelection::spriteScale() const
{
	return scale;
}





} //EOF namespace Decker::ui

