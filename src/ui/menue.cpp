#include "decker.h"
#include "ui.h"

namespace Decker::ui {


MainMenue::MainMenue(int x, int y, int width, int height, Game* game)
	: ppl7::tk::Frame(x, y, width, height)
{
	this->game=game;
	visibility=NULL;
	visibility_plane_player=true;
	visibility_plane_front=true;
	visibility_plane_back=true;
	visibility_plane_middle=true;
	visibility_plane_far=true;
	visibility_plane_horizon=true;
	visibility_plane_near=true;
	visibility_sprites=true;
	visibility_objects=true;
	visibility_particles=true;
	visibility_grid=false;
	visibility_tiletypes=false;
	visibility_collision=false;
	level_dialog=NULL;
	metrics=NULL;
	controlsEnabled=game->getControlsEnabled();

	setupUi();
}

void MainMenue::resize(int x, int y, int width, int height)
{
	this->setPos(x, y);
	this->setSize(width, height);
	this->destroyChilds();
	setupUi();
	needsRedraw();
}

void MainMenue::setupUi()
{
	ppl7::grafix::Size s=this->clientSize();
	ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
	exit_button=new ppl7::tk::Button(s.width - 100, 0, 100, s.height, "Exit");
	exit_button->setIcon(gfx->Toolbar.getDrawable(68));
	exit_button->setEventHandler(this);
	this->addChild(exit_button);

	save_button=new ppl7::tk::Button(0, 0, 64, s.height, "Save");
	save_button->setIcon(gfx->Toolbar.getDrawable(33));
	save_button->setEventHandler(this);
	this->addChild(save_button);

	save_as_button=new ppl7::tk::Button(65, 0, 100, s.height, "Save as...");
	save_as_button->setIcon(gfx->Toolbar.getDrawable(67));
	save_as_button->setEventHandler(this);
	this->addChild(save_as_button);

	load_button=new ppl7::tk::Button(166, 0, 64, s.height, "Load");
	load_button->setIcon(gfx->Toolbar.getDrawable(32));
	load_button->setEventHandler(this);
	this->addChild(load_button);

	new_button=new ppl7::tk::Button(231, 0, 64, s.height, "New");
	new_button->setIcon(gfx->Toolbar.getDrawable(31));
	new_button->setEventHandler(this);
	this->addChild(new_button);

	edit_level_button=new ppl7::tk::Button(320, 0, 70, s.height, "Level");
	edit_level_button->setIcon(gfx->Toolbar.getDrawable(73));
	edit_level_button->setEventHandler(this);
	this->addChild(edit_level_button);

	edit_tiles_button=new ppl7::tk::Button(391, 0, 50, s.height, "Tiles");
	edit_tiles_button->setEventHandler(this);
	this->addChild(edit_tiles_button);

	edit_tiletypes_button=new ppl7::tk::Button(442, 0, 80, s.height, "TileTypes");
	edit_tiletypes_button->setEventHandler(this);
	this->addChild(edit_tiletypes_button);

	edit_sprites_button=new ppl7::tk::Button(523, 0, 70, s.height, "Sprites");
	edit_sprites_button->setEventHandler(this);
	this->addChild(edit_sprites_button);

	edit_objects_button=new ppl7::tk::Button(594, 0, 70, s.height, "Objects");
	edit_objects_button->setEventHandler(this);
	this->addChild(edit_objects_button);

	edit_waynet_button=new ppl7::tk::Button(665, 0, 70, s.height, "WayNet");
	edit_waynet_button->setEventHandler(this);
	this->addChild(edit_waynet_button);


	show_visibility_submenu_button=new ppl7::tk::Button(756, 0, 80, s.height, "Visibility");
	show_visibility_submenu_button->setEventHandler(this);
	this->addChild(show_visibility_submenu_button);


	ppl7::tk::Label* label=new ppl7::tk::Label(837, 0, 100, s.height, "active Plane: ");
	this->addChild(label);

	active_plane_combobox=new ppl7::tk::ComboBox(938, 0, 150, s.height);
	active_plane_combobox->add("PlayerPlane", "0");
	active_plane_combobox->add("FrontPlane", "1");
	active_plane_combobox->add("FarPlane", "2");
	active_plane_combobox->add("BackPlane", "3");
	active_plane_combobox->add("MiddlePlane", "4");
	active_plane_combobox->add("HorizonPlane", "5");
	active_plane_combobox->add("NearPlane", "6");

	this->addChild(active_plane_combobox);

	show_metrics_submenu_button=new ppl7::tk::Button(1098, 0, 70, s.height, "Metrics");
	show_metrics_submenu_button->setEventHandler(this);
	this->addChild(show_metrics_submenu_button);

	pause_button=new ppl7::tk::Button(1168, 0, 70, s.height, "Pause");
	pause_button->setEventHandler(this);
	pause_button->setCheckable(true);
	this->addChild(pause_button);
	step_button=new ppl7::tk::Button(1238, 0, 70, s.height, "Step");
	step_button->setEventHandler(this);
	this->addChild(step_button);



	godmode_checkbox=new ppl7::tk::CheckBox(width() - 520, 0, 100, s.height, "god mode", false);
	this->addChild(godmode_checkbox);

	soundtrack_checkbox=new ppl7::tk::CheckBox(width() - 420, 0, 150, s.height, "play soundtrack", true);
	this->addChild(soundtrack_checkbox);

	world_follows_player_checkbox=new ppl7::tk::CheckBox(width() - 280, 0, 180, s.height, "World follows player", true);
	this->addChild(world_follows_player_checkbox);

	update();
}

void MainMenue::update()
{
	if (game->getLevelFilename().isEmpty()) save_button->setEnabled(false);
	else save_button->setEnabled(true);
}

void MainMenue::openLevelDialog(bool new_flag)
{
	controlsEnabled=game->getControlsEnabled();
	if (level_dialog) delete level_dialog;
	game->enableControls(false);
	int w=800, h=640;
	if (w >= game->window().width()) w=game->window().width() - 100;
	if (h >= game->window().height()) h=game->window().height() - 100;
	level_dialog=new LevelDialog(w, h);
	level_dialog->setNewLevelFlag(new_flag);
	level_dialog->setGame(game);
	level_dialog->setEventHandler(this);
	if (!new_flag) level_dialog->loadValues(game->getLevel().params);
	game->window().addChild(level_dialog);
}

void MainMenue::mouseClickEvent(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == exit_button) {
		game->quitEvent(NULL);
	} else if (event->widget() == edit_tiles_button) {
		game->showTilesSelection();
	} else if (event->widget() == edit_tiletypes_button) {
		game->showTileTypeSelection();
	} else if (event->widget() == edit_sprites_button) {
		game->showSpriteSelection();
	} else if (event->widget() == edit_objects_button) {
		game->showObjectsSelection();
	} else if (event->widget() == edit_waynet_button) {
		game->showWayNetEdit();
	} else if (event->widget() == edit_level_button) {
		openLevelDialog(false);
	} else if (event->widget() == save_button) {
		game->save(game->getLevelFilename());
	} else if (event->widget() == save_as_button) {
		game->openSaveAsDialog();
	} else if (event->widget() == new_button) {
		openLevelDialog(true);
	} else if (event->widget() == load_button) {
		game->openLoadDialog();
	} else if (event->widget() == show_visibility_submenu_button) {
		if (visibility) {
			delete visibility;
			visibility=NULL;
		} else {
			ppl7::tk::Widget* top=show_visibility_submenu_button->getTopmostParent();
			ppl7::grafix::Point p=show_visibility_submenu_button->absolutePosition();
			visibility=new VisibilitySubMenu(p.x, height(), this);
			top->addChild(visibility);
		}
	} else if (event->widget() == show_metrics_submenu_button) {
		showMetrics();
	} else if (event->widget() == pause_button) {
		game->pauseGame(pause_button->isChecked());
	} else if (event->widget() == step_button) {
		pause_button->setChecked(true);
		game->stepFrame();

	}
}

void MainMenue::showMetrics()
{
	if (metrics) {
		delete metrics;
		metrics=NULL;
	} else {
		ppl7::tk::Widget* top=show_metrics_submenu_button->getTopmostParent();
		//ppl7::grafix::Point p=show_metrics_submenu_button->absolutePosition();
		metrics=new MetricsSubMenu(top->width() - 450, height(), this);
		top->addChild(metrics);
	}
}

void MainMenue::closeEvent(ppl7::tk::Event* event)
{
	if (event->widget() == level_dialog) {
		if (level_dialog->state() == LevelDialog::DialogState::OK) {
			if (level_dialog->isNewLevel()) {
				LevelParameter new_params;
				level_dialog->saveValues(new_params);
				delete(level_dialog);
				level_dialog=NULL;
				game->createNewLevel(new_params);
				return;
			} else {
				level_dialog->saveValues(game->getLevel().params);
				delete(level_dialog);
				level_dialog=NULL;
				game->enableControls(controlsEnabled);
			}
		} else if (level_dialog->state() == LevelDialog::DialogState::Aborted) {
			delete(level_dialog);
			level_dialog=NULL;
			game->enableControls(controlsEnabled);
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
	if (active_plane_combobox) active_plane_combobox->setCurrentIdentifier(ppl7::ToString("%d", index));
}


void MainMenue::setWorldFollowsPlayer(bool enable)
{
	if (world_follows_player_checkbox) world_follows_player_checkbox->setChecked(enable);
}



int MainMenue::currentPlane() const
{
	if (active_plane_combobox) return active_plane_combobox->currentIdentifier().toInt();
	return 0;
}

bool MainMenue::worldFollowsPlayer() const
{
	if (world_follows_player_checkbox) return world_follows_player_checkbox->checked();
	return true;
}

bool MainMenue::godModeEnabled() const
{
	if (godmode_checkbox) return godmode_checkbox->checked();
	return false;
}


bool MainMenue::soundTrackEnabled() const
{
	if (soundtrack_checkbox) return soundtrack_checkbox->checked();
	return true;
}

void MainMenue::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	//printf ("MainMenue::textChangedEvent => %s\n",(const char*)text);
}

void MainMenue::updateMetrics(const Metrics& last_metrics)
{
	if (metrics) metrics->update(last_metrics);
}

void MainMenue::fitMetrics(const ppl7::grafix::Rect& viewport)
{
	if (metrics) metrics->setPos(viewport.right() - metrics->width(), viewport.top());
}

VisibilitySubMenu::VisibilitySubMenu(int x, int y, MainMenue* menue)
	: ppl7::tk::Frame(x, y, 140, 330)
{
	this->menue=menue;
	int y1=0;
	this->addChild(new ppl7::tk::Label(0, y1, 100, 20, "Misc:"));
	y1+=20;

	show_grid_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Grid", menue->visibility_grid);
	show_grid_checkbox->setEventHandler(this);
	this->addChild(show_grid_checkbox);
	y1+=20;

	show_tiletypes_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Tiletypes", menue->visibility_tiletypes);
	show_tiletypes_checkbox->setEventHandler(this);
	this->addChild(show_tiletypes_checkbox);
	y1+=20;

	show_collision_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Collision", menue->visibility_collision);
	show_collision_checkbox->setEventHandler(this);
	this->addChild(show_collision_checkbox);
	y1+=20;

	show_sprites_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Sprites", menue->visibility_sprites);
	show_sprites_checkbox->setEventHandler(this);
	this->addChild(show_sprites_checkbox);
	y1+=20;

	show_objects_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Objects", menue->visibility_objects);
	show_objects_checkbox->setEventHandler(this);
	this->addChild(show_objects_checkbox);
	y1+=20;

	show_particles_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Particles", menue->visibility_particles);
	show_particles_checkbox->setEventHandler(this);
	this->addChild(show_particles_checkbox);
	y1+=20;

	y1+=20;
	this->addChild(new ppl7::tk::Label(0, y1, 100, 20, "visible Planes:"));
	y1+=20;

	visible_plane_near_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Near", menue->visibility_plane_near);
	visible_plane_near_checkbox->setEventHandler(this);
	this->addChild(visible_plane_near_checkbox);
	y1+=20;

	visible_plane_front_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Front", menue->visibility_plane_front);
	visible_plane_front_checkbox->setEventHandler(this);
	this->addChild(visible_plane_front_checkbox);
	y1+=20;

	visible_plane_player_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Player", menue->visibility_plane_player);
	visible_plane_player_checkbox->setEventHandler(this);
	this->addChild(visible_plane_player_checkbox);
	y1+=20;

	visible_plane_back_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Back", menue->visibility_plane_back);
	visible_plane_back_checkbox->setEventHandler(this);
	this->addChild(visible_plane_back_checkbox);
	y1+=20;

	visible_plane_middle_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Middle", menue->visibility_plane_middle);
	visible_plane_middle_checkbox->setEventHandler(this);
	this->addChild(visible_plane_middle_checkbox);
	y1+=20;

	visible_plane_far_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Far", menue->visibility_plane_far);
	visible_plane_far_checkbox->setEventHandler(this);
	this->addChild(visible_plane_far_checkbox);
	y1+=20;

	visible_plane_horizon_checkbox=new ppl7::tk::CheckBox(20, y1, 100, 20, "Horizon", menue->visibility_plane_horizon);
	visible_plane_horizon_checkbox->setEventHandler(this);
	this->addChild(visible_plane_horizon_checkbox);
	y1+=20;

}

void VisibilitySubMenu::setShowTileTypes(bool show)
{
	show_tiletypes_checkbox->setChecked(show);
}

void VisibilitySubMenu::toggledEvent(ppl7::tk::Event* event, bool checked)
{

	ppl7::tk::Widget* widget=event->widget();
	if (widget == visible_plane_player_checkbox) {
		menue->visibility_plane_player=checked;
	} else if (widget == visible_plane_front_checkbox) {
		menue->visibility_plane_front=checked;
	} else if (widget == visible_plane_far_checkbox) {
		menue->visibility_plane_far=checked;
	} else if (widget == visible_plane_back_checkbox) {
		menue->visibility_plane_back=checked;
	} else if (widget == visible_plane_middle_checkbox) {
		menue->visibility_plane_middle=checked;
	} else if (widget == visible_plane_horizon_checkbox) {
		menue->visibility_plane_horizon=checked;
	} else if (widget == visible_plane_near_checkbox) {
		menue->visibility_plane_near=checked;
	} else if (widget == show_grid_checkbox) {
		menue->visibility_grid=checked;
	} else if (widget == show_tiletypes_checkbox) {
		menue->visibility_tiletypes=checked;
	} else if (widget == show_collision_checkbox) {
		menue->visibility_collision=checked;
	} else if (widget == show_sprites_checkbox) {
		menue->visibility_sprites=checked;
	} else if (widget == show_objects_checkbox) {
		menue->visibility_objects=checked;
	} else if (widget == show_particles_checkbox) {
		menue->visibility_particles=checked;
	};
}



} //EOF namespace Decker
