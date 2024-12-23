#include "decker.h"
#include "ui.h"

namespace Decker::ui {


TileTypeSelection::TileTypeSelection(int x, int y, int width, int height, Game* game, SpriteTexture* tiletypes)
	: ppltk::Frame(x, y, width, height)
{
	this->game=game;
	ppl7::grafix::Rect client=this->clientRect();

	this->addChild(new ppltk::Label(5, 10, 100, 20, "Tiletype: "));
	tiletypesframe=new TilesFrame(5, 30, client.width() - 8, 400, game);
	tiletypesframe->setSprites(tiletypes);
	this->addChild(tiletypesframe);
}

void TileTypeSelection::setTileType(int nr)
{
	tiletypesframe->setSelectedTile(nr);
}

int TileTypeSelection::tileType() const
{
	return tiletypesframe->selectedTile();
}


} //EOF namespace Decker::ui
