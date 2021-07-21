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

	save_button=new ppl7::tk::Button(0,0,64,s.height,"Save");
	save_button->setIcon(gfx->Toolbar.getDrawable(33));
	save_button->setEventHandler(this);
	this->addChild(save_button);

	load_button=new ppl7::tk::Button(66,0,64,s.height,"Load");
	load_button->setIcon(gfx->Toolbar.getDrawable(32));
	load_button->setEventHandler(this);
	this->addChild(load_button);

	new_button=new ppl7::tk::Button(130,0,64,s.height,"New");
	new_button->setIcon(gfx->Toolbar.getDrawable(31));
	new_button->setEventHandler(this);
	this->addChild(new_button);


	edit_tiles_button=new ppl7::tk::Button(140+60,0,60,s.height,"Tiles");
	edit_tiles_button->setEventHandler(this);
	this->addChild(edit_tiles_button);

	edit_tiletypes_button=new ppl7::tk::Button(202+60,0,80,s.height,"TileTypes");
	edit_tiletypes_button->setEventHandler(this);
	this->addChild(edit_tiletypes_button);

	edit_sprites_button=new ppl7::tk::Button(284+60,0,70,s.height,"Sprites");
	edit_sprites_button->setEventHandler(this);
	this->addChild(edit_sprites_button);

	edit_objects_button=new ppl7::tk::Button(356+60,0,70,s.height,"Objects");
	edit_objects_button->setEventHandler(this);
	this->addChild(edit_objects_button);



	show_grid_checkbox=new CheckBox(500,0,62,s.height,"Grid");
	show_grid_checkbox->setEventHandler(this);
	this->addChild(show_grid_checkbox);
	show_tiletypes_checkbox=new CheckBox(562,0,90,s.height,"Tiletypes");
	show_tiletypes_checkbox->setEventHandler(this);
	this->addChild(show_tiletypes_checkbox);
	show_collision_checkbox=new CheckBox(650,0,90,s.height,"Collision");
	show_collision_checkbox->setEventHandler(this);
	this->addChild(show_collision_checkbox);


	ppl7::tk::Label *label=new ppl7::tk::Label(740,0,100,s.height,"active Plane: ");
	this->addChild(label);

	active_plane_combobox=new ComboBox(840,0,150,s.height);
	active_plane_combobox->add("PlayerPlane");
	active_plane_combobox->add("FrontPlane");
	active_plane_combobox->add("FarPlane");
	active_plane_combobox->add("BackPlane");
	active_plane_combobox->add("MiddlePlane");
	active_plane_combobox->add("HorizonPlane");

	this->addChild(active_plane_combobox);

	label=new ppl7::tk::Label(1000,0,100,s.height,"visible Planes:");
	this->addChild(label);

	visible_plane_player_checkbox=new CheckBox(1110,0,80,s.height,"Player", true);
	this->addChild(visible_plane_player_checkbox);

	visible_plane_front_checkbox=new CheckBox(1190,0,70,s.height,"Front", true);
	this->addChild(visible_plane_front_checkbox);

	visible_plane_back_checkbox=new CheckBox(1260,0,70,s.height,"Back", true);
	this->addChild(visible_plane_back_checkbox);
	visible_plane_middle_checkbox=new CheckBox(1330,0,60,s.height,"Middle", true);
	this->addChild(visible_plane_middle_checkbox);
	visible_plane_far_checkbox=new CheckBox(1400,0,60,s.height,"Far", true);
	this->addChild(visible_plane_far_checkbox);


	world_follows_player_checkbox=new CheckBox(width-280,0,180,s.height,"World follows player", true);
	this->addChild(world_follows_player_checkbox);


}

void MainMenue::mouseClickEvent(ppl7::tk::MouseEvent *event)
{
	if (event->widget()==exit_button) {
		game->quitEvent(NULL);
	} else if (event->widget()==edit_tiles_button) {
		game->showTilesSelection();
	} else if (event->widget()==edit_tiletypes_button) {
		game->showTileTypeSelection();
	} else if (event->widget()==edit_sprites_button) {
		game->showSpriteSelection();
	} else if (event->widget()==edit_objects_button) {
		game->showObjectsSelection();
	} else if (event->widget()==save_button) {
		game->save();
	} else if (event->widget()==new_button) {
		game->clearLevel();
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

bool MainMenue::showCollision() const
{
	if (show_collision_checkbox) return show_collision_checkbox->checked();
	return false;
}

void MainMenue::setShowTileTypes(bool flag)
{
	if (show_tiletypes_checkbox) show_tiletypes_checkbox->setChecked(flag);
}

void MainMenue::setCurrentPlane(int index)
{
	if (active_plane_combobox) active_plane_combobox->setCurrentIndex(index);
}


int MainMenue::currentPlane() const
{
	if (active_plane_combobox) return active_plane_combobox->currentIndex();
	return 0;
}

bool MainMenue::playerPlaneVisible() const
{
	if (visible_plane_player_checkbox) return visible_plane_player_checkbox->checked();
	return true;
}

bool MainMenue::frontPlaneVisible() const
{
	if (visible_plane_front_checkbox) return visible_plane_front_checkbox->checked();
	return true;
}

bool MainMenue::farPlaneVisible() const
{
	if (visible_plane_far_checkbox) return visible_plane_far_checkbox->checked();
	return true;
}

bool MainMenue::backPlaneVisible() const
{
	if (visible_plane_back_checkbox) return visible_plane_back_checkbox->checked();
	return true;
}

bool MainMenue::middlePlaneVisible() const
{
	if (visible_plane_middle_checkbox) return visible_plane_middle_checkbox->checked();
	return true;
}


bool MainMenue::worldFollowsPlayer() const
{
	if (world_follows_player_checkbox) return world_follows_player_checkbox->checked();
	return true;
}


} //EOF namespace Decker

