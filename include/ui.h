#ifndef INCLUDE_DECKER_UI_H_
#define INCLUDE_DECKER_UI_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "widgets.h"
#include "metrics.h"

class Game;
class SpriteTexture;
class LevelParameter;
class ColorPalette;


#ifndef MAX_TILESETS
#define MAX_TILESETS 30
#endif


#ifndef MAX_SPRITESETS
#define MAX_SPRITESETS 15
#endif

class Player;
class Metrics;

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
	ppl7::tk::Label* version_label;
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
	ppl7::tk::CheckBox* show_grid_checkbox;
	ppl7::tk::CheckBox* show_tiletypes_checkbox;
	ppl7::tk::CheckBox* show_collision_checkbox;
	ppl7::tk::CheckBox* show_sprites_checkbox;
	ppl7::tk::CheckBox* show_objects_checkbox;
	ppl7::tk::CheckBox* show_particles_checkbox;
	ppl7::tk::CheckBox* visible_plane_player_checkbox;
	ppl7::tk::CheckBox* visible_plane_front_checkbox;
	ppl7::tk::CheckBox* visible_plane_far_checkbox;
	ppl7::tk::CheckBox* visible_plane_back_checkbox;
	ppl7::tk::CheckBox* visible_plane_middle_checkbox;
	ppl7::tk::CheckBox* visible_plane_horizon_checkbox;
	ppl7::tk::CheckBox* visible_plane_near_checkbox;

public:
	VisibilitySubMenu(int x, int y, MainMenue* menue);
	void setShowTileTypes(bool show);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
};

class LevelDialog;

class MetricsSubMenu : public ppl7::tk::Frame
{
private:
	MainMenue* menue;
	Metrics metrics;
	ppl7::grafix::Font font;
	void drawMillisecondMetric(ppl7::grafix::Drawable& draw, int c1, int  c2, int y, const ppl7::String& text, double value);
	void drawDoubleMetric(ppl7::grafix::Drawable& draw, int c1, int  c2, int y, const ppl7::String& text, double value);
	void drawIntMetric(ppl7::grafix::Drawable& draw, int c1, int  c2, int y, const ppl7::String& text, uint64_t value);
	void drawCountMetric(ppl7::grafix::Drawable& draw, int c1, int  c2, int c3, int y, const ppl7::String& text, uint64_t value1, uint64_t value2);
public:
	MetricsSubMenu(int x, int y, MainMenue* menue);
	void update(const Metrics& metrics);

	virtual void paint(ppl7::grafix::Drawable& draw);

};

class MainMenue : public ppl7::tk::Frame
{
private:
	ppl7::tk::Button* exit_button;
	ppl7::tk::Button* edit_tiles_button;

	ppl7::tk::Button* save_button;
	ppl7::tk::Button* save_as_button;
	ppl7::tk::Button* load_button;
	ppl7::tk::Button* new_button;

	ppl7::tk::Button* edit_tiletypes_button;
	ppl7::tk::Button* edit_sprites_button;
	ppl7::tk::Button* edit_objects_button;
	ppl7::tk::Button* edit_waynet_button;
	ppl7::tk::Button* edit_level_button;
	ppl7::tk::Button* show_visibility_submenu_button;
	ppl7::tk::Button* show_metrics_submenu_button;

	ppl7::tk::ComboBox* active_plane_combobox;

	ppl7::tk::CheckBox* world_follows_player_checkbox;
	ppl7::tk::CheckBox* soundtrack_checkbox;
	ppl7::tk::CheckBox* godmode_checkbox;
	VisibilitySubMenu* visibility;
	MetricsSubMenu* metrics;

	LevelDialog* level_dialog;
	Game* game;
	bool controlsEnabled;

	void setupUi();




public:
	MainMenue(int x, int y, int width, int height, Game* game);

	void update();

	void resize(int x, int y, int width, int height);

	int currentPlane() const;
	void setShowTileTypes(bool show);
	void setCurrentPlane(int index);
	void setWorldFollowsPlayer(bool enable);
	void openLevelDialog(bool new_flag);
	void updateMetrics(const Metrics& last_metrics);
	void fitMetrics(const ppl7::grafix::Rect& viewport);
	void showMetrics();

	bool worldFollowsPlayer() const;
	bool soundTrackEnabled() const;
	bool godModeEnabled() const;
	bool visibility_plane_player;
	bool visibility_plane_front;
	bool visibility_plane_back;
	bool visibility_plane_middle;
	bool visibility_plane_far;
	bool visibility_plane_horizon;
	bool visibility_plane_near;
	bool visibility_sprites;
	bool visibility_objects;
	bool visibility_particles;
	bool visibility_grid;
	bool visibility_tiletypes;
	bool visibility_collision;

	void mouseClickEvent(ppl7::tk::MouseEvent* event) override;
	void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text) override;
	void closeEvent(ppl7::tk::Event* event) override;
};


class TilesFrame : public ppl7::tk::Frame
{
private:
	Game* game;
	SpriteTexture* tiles;
	ppl7::tk::Scrollbar* scrollbar;
	ppl7::grafix::Color color;
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
	void setColor(const ppl7::grafix::Color& color);
	void setSprites(SpriteTexture* tiles);

};

class ColorPaletteFrame : public ppl7::tk::Frame
{
private:
	ColorPalette& palette;
	int color_index;
	ppl7::tk::Scrollbar* scrollbar;
	int tsize;
	int items_per_row;
	int rows;
	ppl7::grafix::Color color_clipboard;

public:
	ColorPaletteFrame(int x, int y, int width, int height, ColorPalette& palette);
	~ColorPaletteFrame();
	int colorIndex() const;
	void setColorIndex(int index);
	ppl7::grafix::Color color() const;

	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;
	void mouseWheelEvent(ppl7::tk::MouseEvent* event) override;
	void valueChangedEvent(ppl7::tk::Event* event, int value) override;
	void paint(ppl7::grafix::Drawable& draw) override;


};

class ColorSelectionFrame : public ppl7::tk::Widget
{
private:
	ColorPalette& palette;
	ColorPaletteFrame* color_palette;
	ppl7::tk::LineInput* color_name;
	ppl7::tk::HorizontalSlider* slider_red;
	ppl7::tk::HorizontalSlider* slider_green;
	ppl7::tk::HorizontalSlider* slider_blue;

	ppl7::tk::SpinBox* color_red;
	ppl7::tk::SpinBox* color_green;
	ppl7::tk::SpinBox* color_blue;

	ppl7::grafix::Color color_clipboard;

	void sendEventValueChanged();


public:
	ColorSelectionFrame(int x, int y, int width, int height, ColorPalette& palette);
	int colorIndex() const;
	void setColorIndex(int index);
	ppl7::grafix::Color color() const;

	void paint(ppl7::grafix::Drawable& draw) override;
	void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text) override;
	void valueChangedEvent(ppl7::tk::Event* event, int value) override;
	void valueChangedEvent(ppl7::tk::Event* event, int64_t value) override;
	void keyDownEvent(ppl7::tk::KeyEvent* event) override;
};

class TilesSelection : public ppl7::tk::Frame
{
private:
	Game* game;
	TilesFrame* tilesframe;
	ColorSelectionFrame* colorframe;
	ppl7::tk::RadioButton* layer0;
	ppl7::tk::RadioButton* layer1;
	ppl7::tk::RadioButton* layer2;
	ppl7::tk::RadioButton* layer3;
	ppl7::tk::ComboBox* tileset_combobox;


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
	void setLayer(int layer);
	int colorIndex() const;
	void setColorIndex(int index);


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
	ColorSelectionFrame* colorframe;
	ppl7::tk::RadioButton* layer0;
	ppl7::tk::RadioButton* layer1;
	ppl7::tk::ComboBox* tileset_combobox;

	ppl7::String tilesetName[MAX_SPRITESETS + 1];
	SpriteTexture* tilesets[MAX_SPRITESETS + 1];
	int SpriteDimensions[MAX_SPRITESETS + 1];

	int tileset;
	float scale;



public:
	SpriteSelection(int x, int y, int width, int height, Game* game);

	//virtual void paint(Drawable &draw);

	void setSelectedSprite(int nr);
	int selectedSprite() const;
	void setCurrentSpriteSet(int id);
	int currentSpriteSet() const;
	int spriteSetDimensions() const;
	void setSpriteScale(float factor);
	float spriteScale() const;
	void setSpriteSet(int id, const ppl7::String& name, SpriteTexture* sprites, int dimensions=1);
	int currentLayer() const;
	void setCurrentLayer(int layer);
	int colorIndex() const;
	void setColorIndex(int index);
	void valueChangedEvent(ppl7::tk::Event* event, int value)  override;
};

class ObjectsFrame : public ppl7::tk::Frame
{
private:
	SpriteTexture* spriteset;
	ppl7::tk::Scrollbar* scrollbar;
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
	void setObjectType(int type);

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
	ppl7::tk::Scrollbar* scrollbar;
	ppl7::tk::ComboBox* layer_selection;
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
	void setObjectType(int type);
	ppl7::String widgetType() const override;
	void valueChangedEvent(ppl7::tk::Event* event, int value) override;
	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;


};

class WayNetEdit : public ppl7::tk::Frame
{
private:
	Game* game;
	ppl7::tk::RadioButton* type_clear;
	ppl7::tk::RadioButton* type_walk;
	ppl7::tk::RadioButton* type_jump_up;
	ppl7::tk::RadioButton* type_jump_left;
	ppl7::tk::RadioButton* type_jump_right;
	ppl7::tk::RadioButton* type_climb;
	ppl7::tk::RadioButton* type_go;
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


class LevelDialog : public Dialog
{
public:
	enum class DialogState {
		Open=0,
		OK,
		Aborted
	};
private:
	ppl7::tk::LineInput* level_name;
	ppl7::tk::SpinBox* level_width;
	ppl7::tk::SpinBox* level_height;
	ppl7::tk::Label* level_pixel_size;

	ppl7::tk::Frame* level_background_frame;

	ppl7::tk::RadioButton* radio_image;
	ppl7::tk::RadioButton* radio_color;
	ppl7::tk::ComboBox* background_image;

	//ppl7::tk::Button* image_fileselect;

	ppl7::tk::SpinBox* color_red;
	ppl7::tk::SpinBox* color_green;
	ppl7::tk::SpinBox* color_blue;
	ppl7::tk::HorizontalSlider* slider_red;
	ppl7::tk::HorizontalSlider* slider_green;
	ppl7::tk::HorizontalSlider* slider_blue;
	ppl7::tk::Frame* color_preview;
	//ppl7::tk::Button* color_picker;

	ppl7::tk::Frame* level_soundtrack_frame;
	ppl7::tk::ComboBox* base_soundtrack;

	ppl7::tk::ComboBox* additional_soundtrack;
	ppl7::tk::Button* add_soundtrack_button;
	ppl7::tk::Button* delete_soundtrack_button;
	ppl7::tk::ListWidget* soundtrack_list;
	ppl7::tk::CheckBox* soundtrack_random;


	ppl7::tk::Button* ok_button;
	ppl7::tk::Button* cancel_button;

	bool newlevel;
	DialogState my_state;

	std::map<ppl7::String, ppl7::String> song_map_identifier;
	Game* game;
	ppl7::grafix::Color previous_background;

	void updateColorPreview();
	void setupUi();

public:
	int custom_id;
	LevelDialog(int width, int height);

	void setGame(Game* game);
	void setNewLevelFlag(bool newlevel);
	bool isNewLevel() const;
	DialogState state() const;

	void loadValues(const LevelParameter& params);
	void saveValues(LevelParameter& params) const;

	ppl7::String widgetType() const override;
	void mouseClickEvent(ppl7::tk::MouseEvent* event) override;
	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;
	void valueChangedEvent(ppl7::tk::Event* event, int64_t value) override;
	void keyDownEvent(ppl7::tk::KeyEvent* event) override;
	void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text) override;
};


}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_UI_H_ */
