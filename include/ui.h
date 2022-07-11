#ifndef INCLUDE_DECKER_UI_H_
#define INCLUDE_DECKER_UI_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "widgets.h"

class Game;
class SpriteTexture;

#ifndef MAX_TILESETS
#define MAX_TILESETS 30
#endif


#ifndef MAX_SPRITESETS
#define MAX_SPRITESETS 10
#endif

class Player;

namespace Decker {
namespace ui {

class StatusBar : public ppl7::tk::Frame
{
private:
	ppl7::tk::Label* fps_label;
	ppl7::tk::Label* mouse_coords;
	ppl7::tk::Label* world_coords;
	ppl7::tk::Label* player_coords;
	ppl7::tk::Label* mouse_buttons;
	ppl7::tk::Label* time_label;
	ppl7::tk::Label* sprite_count;
	ppl7::tk::Label* object_count;
	ppl7::tk::Label* player_state;
	ppl7::tk::Label* object_id;

	int timer_id;

	void setupUi();

public:
	StatusBar(int x, int y, int width, int height);
	~StatusBar();

	void resize(int x, int y, int width, int height);

	void setFps(int fps);
	void setMouse(const ppl7::tk::MouseState& mouse);
	void setWorldCoords(const ppl7::grafix::Point& p);
	void setPlayerCoords(const ppl7::grafix::Point& p);
	void setSpriteCount(size_t total, size_t visible);
	void setObjectCount(size_t total, size_t visible);
	void setPlayerState(const ppl7::String& state);
	void setSelectedObject(int id);

	// Events
	void timerEvent(ppl7::tk::Event* event);

};

class MainMenue;

class VisibilitySubMenu : public ppl7::tk::Frame
{
private:
	MainMenue* menue;
	CheckBox* show_grid_checkbox;
	CheckBox* show_tiletypes_checkbox;
	CheckBox* show_collision_checkbox;
	CheckBox* show_sprites_checkbox;
	CheckBox* show_objects_checkbox;
	CheckBox* visible_plane_player_checkbox;
	CheckBox* visible_plane_front_checkbox;
	CheckBox* visible_plane_far_checkbox;
	CheckBox* visible_plane_back_checkbox;
	CheckBox* visible_plane_middle_checkbox;
	CheckBox* visible_plane_horizon_checkbox;

public:
	VisibilitySubMenu(int x, int y, MainMenue* menue);
	void setShowTileTypes(bool show);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
};

class MainMenue : public ppl7::tk::Frame
{
private:
	ppl7::tk::Button* exit_button;
	ppl7::tk::Button* edit_tiles_button;

	ppl7::tk::Button* save_button;
	ppl7::tk::Button* load_button;
	ppl7::tk::Button* new_button;

	ppl7::tk::Button* edit_tiletypes_button;
	ppl7::tk::Button* edit_sprites_button;
	ppl7::tk::Button* edit_objects_button;
	ppl7::tk::Button* edit_waynet_button;
	ppl7::tk::Button* show_visibility_submenu_button;

	ComboBox* active_plane_combobox;

	CheckBox* world_follows_player_checkbox;
	CheckBox* soundtrack_checkbox;
	CheckBox* godmode_checkbox;
	VisibilitySubMenu* visibility;
	Game* game;

	void setupUi();

public:
	MainMenue(int x, int y, int width, int height, Game* game);

	void resize(int x, int y, int width, int height);

	void mouseClickEvent(ppl7::tk::MouseEvent* event);
	int currentPlane() const;
	void setShowTileTypes(bool show);
	void setCurrentPlane(int index);
	void setWorldFollowsPlayer(bool enable);

	bool worldFollowsPlayer() const;
	bool soundTrackEnabled() const;
	bool godModeEnabled() const;
	bool visibility_plane_player;
	bool visibility_plane_front;
	bool visibility_plane_back;
	bool visibility_plane_middle;
	bool visibility_plane_far;
	bool visibility_plane_horizon;
	bool visibility_sprites;
	bool visibility_objects;
	bool visibility_grid;
	bool visibility_tiletypes;
	bool visibility_collision;


	void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text) override;
};


class TilesFrame : public ppl7::tk::Frame
{
private:
	Game* game;
	SpriteTexture* tiles;
	Scrollbar* scrollbar;
	int selected_tile;
public:
	TilesFrame(int x, int y, int width, int height, Game* game);
	void paint(ppl7::grafix::Drawable& draw)  override;
	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;
	void mouseMoveEvent(ppl7::tk::MouseEvent* event) override;
	void mouseWheelEvent(ppl7::tk::MouseEvent* event) override;
	void valueChangedEvent(ppl7::tk::Event* event, int value) override;

	void setSelectedTile(int nr);
	int selectedTile() const;
	void setSprites(SpriteTexture* tiles);

};

class TilesSelection : public ppl7::tk::Frame
{
private:
	Game* game;
	TilesFrame* tilesframe;
	RadioButton* layer0;
	RadioButton* layer1;
	RadioButton* layer2;
	RadioButton* layer3;
	ComboBox* tileset_combobox;

	ppl7::String tilesetName[MAX_TILESETS + 1];
	SpriteTexture* tilesets[MAX_TILESETS + 1];

	int tileset;



public:
	TilesSelection(int x, int y, int width, int height, Game* game);

	//virtual void paint(Drawable &draw);

	void setSelectedTile(int nr);
	int selectedTile() const;
	void setCurrentTileSet(int id);
	int currentTileSet() const;
	void setTileSet(int id, const ppl7::String& name, SpriteTexture* tiles);
	int currentLayer() const;


	void valueChangedEvent(ppl7::tk::Event* event, int value) override;


};

class TileTypeSelection : public ppl7::tk::Frame
{
private:
	Game* game;
	TilesFrame* tiletypesframe;

public:
	TileTypeSelection(int x, int y, int width, int height, Game* game, SpriteTexture* tiletypes);

	void setTileType(int nr);
	int tileType() const;
};

class SpriteSelection : public ppl7::tk::Frame
{
private:
	Game* game;
	TilesFrame* tilesframe;
	RadioButton* layer0;
	RadioButton* layer1;
	ComboBox* tileset_combobox;

	ppl7::String tilesetName[MAX_SPRITESETS + 1];
	SpriteTexture* tilesets[MAX_SPRITESETS + 1];

	int tileset;
	float scale;



public:
	SpriteSelection(int x, int y, int width, int height, Game* game);

	//virtual void paint(Drawable &draw);

	void setSelectedSprite(int nr);
	int selectedSprite() const;
	void setCurrentSpriteSet(int id);
	int currentSpriteSet() const;
	void setSpriteScale(float factor);
	float spriteScale() const;
	void setSpriteSet(int id, const ppl7::String& name, SpriteTexture* sprites);
	int currentLayer() const;
	void setCurrentLayer(int layer);
	void valueChangedEvent(ppl7::tk::Event* event, int value)  override;
};

class ObjectsFrame : public ppl7::tk::Frame
{
private:
	SpriteTexture* spriteset;
	Scrollbar* scrollbar;
	int selected_object;
	class Item
	{
	public:
		int id;
		ppl7::String name;
		int sprite_no;
		Item(int id, const ppl7::String& name, int sprite_no);
	};
	std::map<size_t, Item> object_map;
	void addObject(int id, const ppl7::String& name, int sprite_no);

public:
	ObjectsFrame(int x, int y, int width, int height);



	void setSpriteSet(SpriteTexture* texture);
	int selectedObjectType() const;

	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void valueChangedEvent(ppl7::tk::Event* event, int value) override;
	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;
	void mouseWheelEvent(ppl7::tk::MouseEvent* event) override;

};

class ObjectSelection : public ppl7::tk::Frame
{
private:
	Game* game;
	//TilesFrame *tilesframe;
	SpriteTexture* spriteset;
	Scrollbar* scrollbar;
	ComboBox* layer_selection;
	ObjectsFrame* objects_frame;
	int selected_object;

	class Item
	{
	public:
		int id;
		ppl7::String name;
		int sprite_no;
		Item(int id, const ppl7::String& name, int sprite_no);
	};
	std::map<size_t, Item> object_map;
	void addObject(int id, const ppl7::String& name, int sprite_no);

public:
	ObjectSelection(int x, int y, int width, int height, Game* game);
	void setSpriteSet(SpriteTexture* texture);
	int selectedObjectType() const;
	int currentLayer() const;
	void setLayer(int layer);
	ppl7::String widgetType() const override;
	void valueChangedEvent(ppl7::tk::Event* event, int value) override;
	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;


};

class WayNetEdit : public ppl7::tk::Frame
{
private:
	Game* game;
	RadioButton* type_clear;
	RadioButton* type_walk;
	RadioButton* type_jump_up;
	RadioButton* type_jump_left;
	RadioButton* type_jump_right;
	RadioButton* type_climb;
	ppl7::tk::LineInput* cost;


public:
	WayNetEdit(int x, int y, int width, int height, Game* game);

	int getSelectedWayType() const;
	int getCost() const;
};


class StatsFrame : public ppl7::tk::Widget
{
private:
	ppl7::String label;
	ppl7::String value;
	ppl7::grafix::Font font;


public:
	StatsFrame(int x, int y, int width, int height, const ppl7::String& label);
	void setValue(const ppl7::String& value);
	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void setFontSize(int size);


};

class WorldWidget : public ppl7::tk::Widget
{
private:
	StatsFrame* stats_health, * stats_lifes, * stats_points;
	int value_health, value_lifes, value_points;
public:
	WorldWidget();
	void setViewport(const ppl7::grafix::Rect& viewport);
	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;

	void updatePlayerStats(const Player* player);
	void resetPlayerStats(const Player* player);


};



}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_UI_H_ */
