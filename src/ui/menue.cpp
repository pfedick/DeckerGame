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

	edit_tiles_button=new ppl7::tk::Button(0,0,60,s.height,"Tiles");
	edit_tiles_button->setEventHandler(this);
	this->addChild(edit_tiles_button);

	show_grid_checkbox=new CheckBox(70,0,80,s.height,"show grid");
	show_grid_checkbox->setEventHandler(this);
	this->addChild(show_grid_checkbox);
}

void MainMenue::mouseClickEvent(ppl7::tk::MouseEvent *event)
{
	if (event->widget()==exit_button) {
		game->quitEvent(NULL);
	} else if (event->widget()==edit_tiles_button) {
		game->showTilesSelection();

	}
}

} //EOF namespace Decker

