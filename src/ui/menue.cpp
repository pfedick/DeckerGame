#include "decker.h"
#include "ui.h"

namespace Decker::ui {


MainMenue::MainMenue(int x, int y, int width, int height, Game *game)
: ppl7::tk::Frame(x,y,width,height)
{
	this->game=game;
	ppl7::grafix::Size s=this->clientSize();
	ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	exit_button=new ppl7::tk::Button(s.width-100,0,100,s.height,"Exit");
	exit_button->setIcon(gfx->Toolbar.getDrawable(68));
	exit_button->setEventHandler(this);
	this->addChild(exit_button);

	save_button=new ppl7::tk::Button(0,0,60,s.height,"Save");
	save_button->setEventHandler(this);
	this->addChild(save_button);

	load_button=new ppl7::tk::Button(62,0,60,s.height,"Load");
	load_button->setEventHandler(this);
	this->addChild(load_button);


	edit_tiles_button=new ppl7::tk::Button(127,0,60,s.height,"Tiles");
	edit_tiles_button->setEventHandler(this);
	this->addChild(edit_tiles_button);

	show_grid_checkbox=new CheckBox(200,0,100,s.height,"show grid");
	show_grid_checkbox->setEventHandler(this);
	this->addChild(show_grid_checkbox);
	show_tiletypes_checkbox=new CheckBox(300,0,130,s.height,"show tile types");
	show_tiletypes_checkbox->setEventHandler(this);
	this->addChild(show_tiletypes_checkbox);

}

void MainMenue::mouseClickEvent(ppl7::tk::MouseEvent *event)
{
	if (event->widget()==exit_button) {
		game->quitEvent(NULL);
	} else if (event->widget()==edit_tiles_button) {
		game->showTilesSelection();
	} else if (event->widget()==save_button) {
		game->save();

	} else if (event->widget()==load_button) {
		game->load();
	}
}

bool MainMenue::showGrid() const
{
	if (show_grid_checkbox) return show_grid_checkbox->checked();
	return false;
}

bool MainMenue::showTileTypes() const
{
	if (show_tiletypes_checkbox) return show_tiletypes_checkbox->checked();
	return false;
}


} //EOF namespace Decker

