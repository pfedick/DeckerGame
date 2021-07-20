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
	ppl7::tk::Label *fps_label;
	ppl7::tk::Label *mouse_coords;
	ppl7::tk::Label *world_coords;
	ppl7::tk::Label *player_coords;
	ppl7::tk::Label *mouse_buttons;
	ppl7::tk::Label *time_label;
	ppl7::tk::Label *sprite_count;
	ppl7::tk::Label *player_state;


public:
	StatusBar(int x, int y, int width, int height);

	void setFps(int fps);
	void setMouse(const ppl7::tk::MouseState &mouse);
	void setWorldCoords(const ppl7::grafix::Point &p);
	void setPlayerCoords(const ppl7::grafix::Point &p);
	void setSpriteCount(size_t total, size_t visible);
	void setPlayerState(const ppl7::String &state);

	// Events
	void timerEvent(ppl7::tk::Event *event);

};

class MainMenue : public ppl7::tk::Frame
{
private:
	ppl7::tk::Button *exit_button;
	ppl7::tk::Button *edit_tiles_button;

	ppl7::tk::Button *save_button;
	ppl7::tk::Button *load_button;
	ppl7::tk::Button *new_button;

	ppl7::tk::Button *edit_tiletypes_button;
	ppl7::tk::Button *edit_sprites_button;
	ppl7::tk::Button *edit_objects_button;
	CheckBox *show_grid_checkbox;
	CheckBox *show_tiletypes_checkbox;
	CheckBox *show_collision_checkbox;
	CheckBox *visible_plane_player_checkbox;
	CheckBox *visible_plane_front_checkbox;
	CheckBox *visible_plane_far_checkbox;
	CheckBox *visible_plane_back_checkbox;
	CheckBox *visible_plane_middle_checkbox;

	ComboBox *active_plane_combobox;

	CheckBox *world_follows_player_checkbox;

	Game *game;


public:
	MainMenue(int x, int y, int width, int height, Game *game);

	void mouseClickEvent(ppl7::tk::MouseEvent *event);
	bool showGrid() const;
	bool showTileTypes() const;
	bool showCollision() const;
	int currentPlane() const;
	void setShowTileTypes(bool show);
	void setCurrentPlane(int index);

	bool playerPlaneVisible() const;
	bool frontPlaneVisible() const;
	bool farPlaneVisible() const;
	bool backPlaneVisible() const;
	bool middlePlaneVisible() const;
	bool worldFollowsPlayer() const;

};

class TilesFrame : public ppl7::tk::Frame
{
private:
	Game *game;
	SpriteTexture *tiles;
	Scrollbar *scrollbar;
	int selected_tile;
public:
	TilesFrame(int x, int y, int width, int height, Game *game);
	virtual void paint(ppl7::grafix::Drawable &draw);
	void mouseDownEvent(ppl7::tk::MouseEvent *event);
	void mouseMoveEvent(ppl7::tk::MouseEvent *event);
	void mouseWheelEvent(ppl7::tk::MouseEvent *event);
	void valueChangedEvent(ppl7::tk::Event *event, int value);

	void setSelectedTile(int nr);
	int selectedTile() const;
	void setSprites(SpriteTexture *tiles);

};

class TilesSelection : public ppl7::tk::Frame
{
private:
	Game *game;
	TilesFrame *tilesframe;
	RadioButton *layer0;
	RadioButton *layer1;
	RadioButton *layer2;
	RadioButton *layer3;
	ComboBox *tileset_combobox;

	ppl7::String tilesetName[MAX_TILESETS+1];
	SpriteTexture *tilesets[MAX_TILESETS+1];

	int tileset;



public:
	TilesSelection(int x, int y, int width, int height, Game *game);

	//virtual void paint(Drawable &draw);

	void setSelectedTile(int nr);
	int selectedTile() const;
	void setCurrentTileSet(int id);
	int currentTileSet() const;
	void setTileSet(int id, const ppl7::String &name, SpriteTexture *tiles);
	int currentLayer() const;


	void valueChangedEvent(ppl7::tk::Event *event, int value);


};

class TileTypeSelection : public ppl7::tk::Frame
{
private:
	Game *game;
	TilesFrame *tiletypesframe;

public:
	TileTypeSelection(int x, int y, int width, int height, Game *game, SpriteTexture *tiletypes);

	void setTileType(int nr);
	int tileType() const;
};

class SpriteSelection : public ppl7::tk::Frame
{
private:
	Game *game;
	TilesFrame *tilesframe;
	RadioButton *layer0;
	RadioButton *layer1;
	ComboBox *tileset_combobox;

	ppl7::String tilesetName[MAX_SPRITESETS+1];
	SpriteTexture *tilesets[MAX_SPRITESETS+1];

	int tileset;
	float scale;



public:
	SpriteSelection(int x, int y, int width, int height, Game *game);

	//virtual void paint(Drawable &draw);

	void setSelectedSprite(int nr);
	int selectedSprite() const;
	void setCurrentSpriteSet(int id);
	int currentSpriteSet() const;
	void setSpriteScale(float factor);
	float spriteScale() const;
	void setSpriteSet(int id, const ppl7::String &name, SpriteTexture *sprites);
	int currentLayer() const;
	void setCurrentLayer(int layer);
	void valueChangedEvent(ppl7::tk::Event *event, int value);
};

class ObjectSelection : public ppl7::tk::Frame
{
private:
	Game *game;
	//TilesFrame *tilesframe;
	SpriteTexture *spriteset;
	Scrollbar *scrollbar;
	int selected_object;

	class Item
	{
	public:
		int id;
		ppl7::String name;
		int sprite_no;
		Item(int id, const ppl7::String &name, int sprite_no);
	};
	std::map<size_t,Item> object_map;
	void addObject(int id, const ppl7::String &name, int sprite_no);

public:
	ObjectSelection(int x, int y, int width, int height, Game *game);
	void setSpriteSet(SpriteTexture *texture);
	int selectedObjectType() const;
	virtual ppl7::String widgetType() const;
	virtual void paint(ppl7::grafix::Drawable &draw);
	void valueChangedEvent(ppl7::tk::Event *event, int value);
	void mouseDownEvent(ppl7::tk::MouseEvent *event);
	void mouseWheelEvent(ppl7::tk::MouseEvent *event);

};


class StatsFrame : public ppl7::tk::Widget
{
private:
	ppl7::String label;
	ppl7::String value;
	ppl7::grafix::Font font;

public:
	StatsFrame(int x, int y, int width, int height, const ppl7::String &label);
	void setValue(const ppl7::String &value);
	virtual ppl7::String widgetType() const;
	virtual void paint(ppl7::grafix::Drawable &draw);


};

class WorldWidget : public ppl7::tk::Widget
{
private:
	StatsFrame *stats_health, *stats_lifes, *stats_points;
public:
	WorldWidget();
	void setViewport(const ppl7::grafix::Rect &viewport);
	virtual ppl7::String widgetType() const;
	virtual void paint(ppl7::grafix::Drawable &draw);

	void updatePlayerStats(const Player *player);


};



}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_UI_H_ */
