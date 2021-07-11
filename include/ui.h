#ifndef INCLUDE_DECKER_UI_H_
#define INCLUDE_DECKER_UI_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "widgets.h"

class Game;
class Sprite;

#define MAX_TILESETS 10
#define MAX_SPRITESETS 10

namespace Decker {
namespace ui {

/*
using namespace ppl7;
using namespace ppl7::grafix;
using namespace ppl7::tk;
*/


class StatusBar : public ppl7::tk::Frame
{
private:
	ppl7::tk::Label *fps_label;
	ppl7::tk::Label *mouse_coords;
	ppl7::tk::Label *world_coords;
	ppl7::tk::Label *player_coords;
	ppl7::tk::Label *mouse_buttons;
	ppl7::tk::Label *time_label;


public:
	StatusBar(int x, int y, int width, int height);

	void setFps(int fps);
	void setMouse(const ppl7::tk::MouseState &mouse);
	void setWorldCoords(const ppl7::grafix::Point &p);
	void setPlayerCoords(const ppl7::grafix::Point &p);

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

	ppl7::tk::Button *edit_tiletypes_button;
	ppl7::tk::Button *edit_sprites_button;
	ppl7::tk::Button *edit_enemies_button;
	CheckBox *show_grid_checkbox;
	CheckBox *show_tiletypes_checkbox;
	CheckBox *visible_plane_player_checkbox;
	CheckBox *visible_plane_front_checkbox;
	CheckBox *visible_plane_far_checkbox;

	ComboBox *active_plane_combobox;

	Game *game;


public:
	MainMenue(int x, int y, int width, int height, Game *game);

	void mouseClickEvent(ppl7::tk::MouseEvent *event);
	bool showGrid() const;
	bool showTileTypes() const;
	int currentPlane() const;
	void setShowTileTypes(bool show);
	void setCurrentPlane(int index);

	bool playerPlaneVisible() const;
	bool frontPlaneVisible() const;
	bool farPlaneVisible() const;

};

class TilesFrame : public ppl7::tk::Frame
{
private:
	Game *game;
	Sprite *tiles;
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
	void setSprites(Sprite *tiles);

};

class TilesSelection : public ppl7::tk::Frame
{
private:
	Game *game;
	TilesFrame *tilesframe;
	RadioButton *layer0;
	RadioButton *layer1;
	RadioButton *layer2;
	ComboBox *tileset_combobox;

	ppl7::String tilesetName[MAX_TILESETS+1];
	Sprite *tilesets[MAX_TILESETS+1];

	int tileset;



public:
	TilesSelection(int x, int y, int width, int height, Game *game);

	//virtual void paint(Drawable &draw);

	void setSelectedTile(int nr);
	int selectedTile() const;
	void setCurrentTileSet(int id);
	int currentTileSet() const;
	void setTileSet(int id, const ppl7::String &name, Sprite *tiles);
	int currentLayer() const;


	void valueChangedEvent(ppl7::tk::Event *event, int value);


};

class TileTypeSelection : public ppl7::tk::Frame
{
private:
	Game *game;
	TilesFrame *tiletypesframe;

public:
	TileTypeSelection(int x, int y, int width, int height, Game *game, Sprite *tiletypes);

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
	Sprite *tilesets[MAX_SPRITESETS+1];

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
	void setSpriteSet(int id, const ppl7::String &name, Sprite *sprites);
	int currentLayer() const;
	void valueChangedEvent(ppl7::tk::Event *event, int value);
};

class WorldWidget : public ppl7::tk::Widget
{
private:
public:
	void setViewport(const ppl7::grafix::Rect &viewport);


};



}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_UI_H_ */
