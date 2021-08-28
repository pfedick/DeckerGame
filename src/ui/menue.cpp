#include "decker.h"
#include "ui.h"

namespace Decker::ui {


MainMenue::MainMenue(int x, int y, int width, int height, Game *game)
: ppl7::tk::Frame(x,y,width,height)
{
	this->game=game;
	visibility=NULL;
	visibility_plane_player=true;
	visibility_plane_front=true;
	visibility_plane_back=true;
	visibility_plane_middle=true;
	visibility_plane_far=true;
	visibility_plane_horizon=true;
	visibility_sprites=true;
	visibility_objects=true;
	visibility_grid=false;
	visibility_tiletypes=false;
	visibility_collision=false;



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

	edit_objects_button=new ppl7::tk::Button(416,0,70,s.height,"Objects");
	edit_objects_button->setEventHandler(this);
	this->addChild(edit_objects_button);

	edit_waynet_button=new ppl7::tk::Button(488,0,70,s.height,"WayNet");
	edit_waynet_button->setEventHandler(this);
	this->addChild(edit_waynet_button);


	show_visibility_submenu_button=new ppl7::tk::Button(600,0,80,s.height,"Visibility");
	show_visibility_submenu_button->setEventHandler(this);
	this->addChild(show_visibility_submenu_button);


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

	soundtrack_checkbox=new CheckBox(width-420,0,140,s.height,"play soundtrack", true);
	this->addChild(soundtrack_checkbox);

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
	} else if (event->widget()==edit_waynet_button) {
		game->showWayNetEdit();
	} else if (event->widget()==save_button) {
		game->save();
	} else if (event->widget()==new_button) {
		game->clearLevel();
	} else if (event->widget()==load_button) {
		game->load();
	} else if (event->widget()==show_visibility_submenu_button) {
		if (visibility) {
			delete visibility;
			visibility=NULL;
		} else {
			ppl7::tk::Widget *top=show_visibility_submenu_button->getTopmostParent();
			ppl7::grafix::Point p=show_visibility_submenu_button->absolutePosition();
			visibility=new VisibilitySubMenu(p.x,height(),this);
			top->addChild(visibility);
		}
	}
}

void MainMenue::setShowTileTypes(bool show)
{
	visibility_tiletypes=show;
	if (visibility) visibility->setShowTileTypes(show);
}

void MainMenue::setCurrentPlane(int index)
{
	if (active_plane_combobox) active_plane_combobox->setCurrentIndex(index);
}


void MainMenue::setWorldFollowsPlayer(bool enable)
{
	if (world_follows_player_checkbox) world_follows_player_checkbox->setChecked(enable);
}



int MainMenue::currentPlane() const
{
	if (active_plane_combobox) return active_plane_combobox->currentIndex();
	return 0;
}

bool MainMenue::worldFollowsPlayer() const
{
	if (world_follows_player_checkbox) return world_follows_player_checkbox->checked();
	return true;
}


bool MainMenue::soundTrackEnabled() const
{
	if (soundtrack_checkbox) return soundtrack_checkbox->checked();
	return true;
}

void MainMenue::textChangedEvent(ppl7::tk::Event *event, const ppl7::String &text)
{
	//printf ("MainMenue::textChangedEvent => %s\n",(const char*)text);
}

VisibilitySubMenu::VisibilitySubMenu(int x, int y, MainMenue *menue)
: ppl7::tk::Frame(x,y,140,290)
{
	this->menue=menue;
	this->addChild(new ppl7::tk::Label(0,0,100,20,"Misc:"));


	show_grid_checkbox=new CheckBox(20,20,100,20,"Grid", menue->visibility_grid);
	show_grid_checkbox->setEventHandler(this);
	this->addChild(show_grid_checkbox);

	show_tiletypes_checkbox=new CheckBox(20,40,100,20,"Tiletypes", menue->visibility_tiletypes);
	show_tiletypes_checkbox->setEventHandler(this);
	this->addChild(show_tiletypes_checkbox);

	show_collision_checkbox=new CheckBox(20,60,100,20,"Collision", menue->visibility_collision);
	show_collision_checkbox->setEventHandler(this);
	this->addChild(show_collision_checkbox);

	show_sprites_checkbox=new CheckBox(20,80,100,20,"Sprites", menue->visibility_sprites);
	show_sprites_checkbox->setEventHandler(this);
	this->addChild(show_sprites_checkbox);

	show_objects_checkbox=new CheckBox(20,100,100,20,"Objects", menue->visibility_objects);
	show_objects_checkbox->setEventHandler(this);
	this->addChild(show_objects_checkbox);


	this->addChild(new ppl7::tk::Label(0,140,100,20,"visible Planes:"));

	visible_plane_front_checkbox=new CheckBox(20,160,100,20,"Front", menue->visibility_plane_front);
	visible_plane_front_checkbox->setEventHandler(this);
	this->addChild(visible_plane_front_checkbox);

	visible_plane_player_checkbox=new CheckBox(20,180,100,20,"Player", menue->visibility_plane_player);
	visible_plane_player_checkbox->setEventHandler(this);
	this->addChild(visible_plane_player_checkbox);

	visible_plane_back_checkbox=new CheckBox(20,200,100,20,"Back", menue->visibility_plane_back);
	visible_plane_back_checkbox->setEventHandler(this);
	this->addChild(visible_plane_back_checkbox);

	visible_plane_middle_checkbox=new CheckBox(20,220,100,20,"Middle", menue->visibility_plane_middle);
	visible_plane_middle_checkbox->setEventHandler(this);
	this->addChild(visible_plane_middle_checkbox);

	visible_plane_far_checkbox=new CheckBox(20,240,100,20,"Far", menue->visibility_plane_far);
	visible_plane_far_checkbox->setEventHandler(this);
	this->addChild(visible_plane_far_checkbox);

	visible_plane_horizon_checkbox=new CheckBox(20,260,100,20,"Horizon", menue->visibility_plane_horizon);
	visible_plane_horizon_checkbox->setEventHandler(this);
	this->addChild(visible_plane_horizon_checkbox);

}

void VisibilitySubMenu::setShowTileTypes(bool show)
{
	show_tiletypes_checkbox->setChecked(show);
}

void VisibilitySubMenu::toggledEvent(ppl7::tk::Event *event, bool checked)
{

	ppl7::tk::Widget *widget=event->widget();
	if (widget==visible_plane_player_checkbox) {
		menue->visibility_plane_player=checked;
	} else if (widget==visible_plane_front_checkbox) {
		menue->visibility_plane_front=checked;
	} else if (widget==visible_plane_far_checkbox) {
		menue->visibility_plane_far=checked;
	} else if (widget==visible_plane_back_checkbox) {
		menue->visibility_plane_back=checked;
	} else if (widget==visible_plane_middle_checkbox) {
		menue->visibility_plane_middle=checked;
	} else if (widget==visible_plane_horizon_checkbox) {
		menue->visibility_plane_horizon=checked;
	} else if (widget==show_grid_checkbox) {
		menue->visibility_grid=checked;
	} else if (widget==show_tiletypes_checkbox) {
		menue->visibility_tiletypes=checked;
	} else if (widget==show_collision_checkbox) {
		menue->visibility_collision=checked;
	} else if (widget==show_sprites_checkbox) {
		menue->visibility_sprites=checked;
	} else if (widget==show_objects_checkbox) {
		menue->visibility_objects=checked;
	};
}

} //EOF namespace Decker

