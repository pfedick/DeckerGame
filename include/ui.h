#ifndef INCLUDE_DECKER_UI_H_
#define INCLUDE_DECKER_UI_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppltk.h>
#include "widgets.h"
#include "metrics.h"
#include "waynet.h"
#include "light.h"

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
class Screenshot;

namespace Decker {
namespace ui {

class StatusBar : public ppltk::Frame
{
private:
	ppltk::Label* fps_label;
	ppltk::Label* mouse_coords;
	ppltk::Label* world_coords;
	ppltk::Label* player_coords;
	ppltk::Label* mouse_buttons;
	ppltk::Label* time_label;
	ppltk::Label* version_label;
	ppltk::Label* player_state;
	ppltk::Label* object_id;

	int timer_id;

	void setupUi();

public:
	StatusBar(int x, int y, int width, int height);
	~StatusBar();

	void resize(int x, int y, int width, int height);

	void setFps(int fps);
	void setMouse(const ppltk::MouseState& mouse);
	void setWorldCoords(const ppl7::grafix::Point& p);
	void setPlayerCoords(const ppl7::grafix::Point& p);
	void setPlayerState(const ppl7::String& state);
	void setSelectedObject(int id);

	// Events
	void timerEvent(ppltk::Event* event);

};

class MainMenue;

class VisibilitySubMenu : public ppltk::Frame
{
private:
	MainMenue* menue;
	ppltk::CheckBox* lighting_checkbox;
	ppltk::CheckBox* show_grid_checkbox;
	ppltk::CheckBox* show_tiletypes_checkbox;
	ppltk::CheckBox* show_collision_checkbox;
	ppltk::CheckBox* show_sprites_checkbox;
	ppltk::CheckBox* show_objects_checkbox;
	ppltk::CheckBox* show_particles_checkbox;
	ppltk::CheckBox* visible_plane_player_checkbox;
	ppltk::CheckBox* visible_plane_front_checkbox;
	ppltk::CheckBox* visible_plane_far_checkbox;
	ppltk::CheckBox* visible_plane_back_checkbox;
	ppltk::CheckBox* visible_plane_middle_checkbox;
	ppltk::CheckBox* visible_plane_horizon_checkbox;
	ppltk::CheckBox* visible_plane_near_checkbox;

public:
	VisibilitySubMenu(int x, int y, MainMenue* menue);
	void setShowTileTypes(bool show);
	virtual void toggledEvent(ppltk::Event* event, bool checked);
};

class LevelDialog;

class MetricsSubMenu : public ppltk::Frame
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

class MainMenue : public ppltk::Frame
{
private:
	ppltk::Button* exit_button;
	ppltk::Button* edit_tiles_button;

	ppltk::Button* save_button;
	ppltk::Button* save_as_button;
	ppltk::Button* load_button;
	ppltk::Button* new_button;

	ppltk::Button* edit_tiletypes_button;
	ppltk::Button* edit_sprites_button;
	ppltk::Button* edit_objects_button;
	ppltk::Button* edit_waynet_button;
	ppltk::Button* edit_lights_button;
	ppltk::Button* edit_level_button;
	ppltk::Button* show_visibility_submenu_button;
	ppltk::Button* show_metrics_submenu_button;

	ppltk::Button* pause_button;
	ppltk::Button* step_button;


	ppltk::ComboBox* active_plane_combobox;

	ppltk::CheckBox* world_follows_player_checkbox;
	ppltk::CheckBox* soundtrack_checkbox;
	ppltk::CheckBox* godmode_checkbox;
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
	bool visibility_lighting;

	void mouseClickEvent(ppltk::MouseEvent* event) override;
	void textChangedEvent(ppltk::Event* event, const ppl7::String& text) override;
	void closeEvent(ppltk::Event* event) override;
};


class TilesFrame : public ppltk::Frame
{
private:
	Game* game;
	SpriteTexture* tiles;
	ppltk::Scrollbar* scrollbar;
	ppl7::grafix::Color color;
	int selected_tile;
public:
	TilesFrame(int x, int y, int width, int height, Game* game);
	void paint(ppl7::grafix::Drawable& draw)  override;
	void mouseDownEvent(ppltk::MouseEvent* event) override;
	void mouseMoveEvent(ppltk::MouseEvent* event) override;
	void mouseWheelEvent(ppltk::MouseEvent* event) override;
	void valueChangedEvent(ppltk::Event* event, int value) override;

	void setSelectedTile(int nr);
	int selectedTile() const;
	void setColor(const ppl7::grafix::Color& color);
	void setSprites(SpriteTexture* tiles);

};

class ColorPaletteFrame : public ppltk::Frame
{
private:
	ColorPalette& palette;
	int color_index;
	ppltk::Scrollbar* scrollbar;
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

	void mouseDownEvent(ppltk::MouseEvent* event) override;
	void mouseWheelEvent(ppltk::MouseEvent* event) override;
	void valueChangedEvent(ppltk::Event* event, int value) override;
	void paint(ppl7::grafix::Drawable& draw) override;


};

class ColorSelectionFrame : public ppltk::Widget
{
private:
	ColorPalette& palette;
	ColorPaletteFrame* color_palette;
	ppltk::LineInput* color_name;
	ppltk::HorizontalSlider* slider_red;
	ppltk::HorizontalSlider* slider_green;
	ppltk::HorizontalSlider* slider_blue;

	ppltk::SpinBox* color_red;
	ppltk::SpinBox* color_green;
	ppltk::SpinBox* color_blue;

	ppl7::grafix::Color color_clipboard;

	void sendEventValueChanged();


public:
	ColorSelectionFrame(int x, int y, int width, int height, ColorPalette& palette);
	int colorIndex() const;
	void setColorIndex(int index);
	ppl7::grafix::Color color() const;

	void paint(ppl7::grafix::Drawable& draw) override;
	void textChangedEvent(ppltk::Event* event, const ppl7::String& text) override;
	void valueChangedEvent(ppltk::Event* event, int value) override;
	void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	void keyDownEvent(ppltk::KeyEvent* event) override;
};

class TilesSelection : public ppltk::Frame
{
private:
	Game* game;
	TilesFrame* tilesframe;
	ColorSelectionFrame* colorframe;
	ppltk::RadioButton* layer0;
	ppltk::RadioButton* layer1;
	ppltk::RadioButton* layer2;
	ppltk::RadioButton* layer3;
	ppltk::ComboBox* tileset_combobox;


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


	void valueChangedEvent(ppltk::Event* event, int value) override;


};

class TileTypeSelection : public ppltk::Frame
{
private:
	Game* game;
	TilesFrame* tiletypesframe;

public:
	TileTypeSelection(int x, int y, int width, int height, Game* game, SpriteTexture* tiletypes);

	void setTileType(int nr);
	int tileType() const;
};

class SpriteSelection : public ppltk::Frame
{
private:
	Game* game;
	TilesFrame* tilesframe;
	ColorSelectionFrame* colorframe;
	ppltk::RadioButton* layer0;
	ppltk::RadioButton* layer1;
	ppltk::RadioButton* layer2;
	ppltk::HorizontalSlider* z_axis;
	ppltk::ComboBox* tileset_combobox;
	ppltk::ComboBox* plane_combobox;
	ppltk::DoubleHorizontalSlider* scale_slider;
	ppltk::DoubleHorizontalSlider* rotation_slider;

	ppl7::String tilesetName[MAX_SPRITESETS + 1];
	SpriteTexture* tilesets[MAX_SPRITESETS + 1];
	int SpriteDimensions[MAX_SPRITESETS + 1];

	int tileset;

	bool notifies_enabled;



public:
	SpriteSelection(int x, int y, int width, int height, Game* game);

	//virtual void paint(Drawable &draw);
	void enableNotfies(bool enable);

	void setSelectedSprite(int nr);
	int selectedSprite() const;
	void setCurrentSpriteSet(int id);
	int currentSpriteSet() const;
	int spriteSetDimensions() const;
	void setSpriteScale(float factor);
	float spriteScale() const;
	void setSpriteRotation(float rotation);
	float spriteRotation() const;
	void setSpriteSet(int id, const ppl7::String& name, SpriteTexture* sprites, int dimensions=1);
	int currentLayer() const;
	void setCurrentLayer(int layer);
	int colorIndex() const;
	void setColorIndex(int index);
	void setZAxis(int z);
	int zAxis() const;
	void setPlane(int plane);
	int plane() const;
	void valueChangedEvent(ppltk::Event* event, int value)  override;
	void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	void valueChangedEvent(ppltk::Event* event, double value) override;
	void toggledEvent(ppltk::Event* event, bool checked) override;
};

class ObjectsFrame : public ppltk::Frame
{
private:
	SpriteTexture* spriteset;
	ppltk::Scrollbar* scrollbar;
	int selected_object;
	bool playerPlaneObjectsVisible;
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

	void showPlayerPlaneObjects();
	void showNonPlayerPlaneObjects();


	void setSpriteSet(SpriteTexture* texture);
	int selectedObjectType() const;
	void setObjectType(int type);

	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void valueChangedEvent(ppltk::Event* event, int value) override;
	void mouseDownEvent(ppltk::MouseEvent* event) override;
	void mouseWheelEvent(ppltk::MouseEvent* event) override;

};

class ObjectSelection : public ppltk::Frame
{
private:
	Game* game;
	//TilesFrame *tilesframe;
	SpriteTexture* spriteset;
	ppltk::Scrollbar* scrollbar;
	ppltk::ComboBox* plane_combobox;
	ppltk::ComboBox* layer_selection;
	ppltk::CheckBox* difficulty_easy;
	ppltk::CheckBox* difficulty_normal;
	ppltk::CheckBox* difficulty_hard;
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

public:
	ObjectSelection(int x, int y, int width, int height, Game* game);
	void setSpriteSet(SpriteTexture* texture);
	int selectedObjectType() const;
	int currentLayer() const;
	void setLayer(int layer);
	int currentPlane() const;
	void setPlane(int plane);
	void setObjectType(int type);
	void setObjectDifficulty(uint8_t matrix);
	uint8_t getDifficulty() const;
	ppl7::String widgetType() const override;
	void valueChangedEvent(ppltk::Event* event, int value) override;
	void mouseDownEvent(ppltk::MouseEvent* event) override;
	void toggledEvent(ppltk::Event* event, bool checked);


};

class WayNetEdit : public ppltk::Frame
{
private:
	Game* game;
	ppltk::RadioButton* type_clear;
	ppltk::RadioButton* type_walk;
	ppltk::RadioButton* type_jump_up;
	ppltk::RadioButton* type_jump_left;
	ppltk::RadioButton* type_jump_right;
	ppltk::RadioButton* type_climb;
	ppltk::RadioButton* type_go;
	ppltk::Button* debug_mode;
	ppltk::DoubleHorizontalSlider* cost;
	WayPoint wp_start, wp_end;

	void debugWaynet();


public:
	WayNetEdit(int x, int y, int width, int height, Game* game);

	int getSelectedWayType() const;
	bool debugMode() const;
	float getCost() const;
	void setDebugStart(const WayPoint& wp);
	void setDebugEnd(const WayPoint& wp);

	virtual void toggledEvent(ppltk::Event* event, bool checked);


};


class StatsFrame : public ppltk::Widget
{
private:
	ppl7::String labeltext;
	ppl7::String value;
	ppl7::grafix::Font font;

public:
	StatsFrame(int x, int y, int width, int height, const ppl7::String& label);
	void setLabel(const ppl7::String& label);
	void setValue(const ppl7::String& value);
	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void setFontSize(int size);
	const ppl7::String& label() const;
};

class OxygenFrame : public ppltk::Widget
{
	ppl7::String label;
	float seconds_total, seconds_left;
	ppl7::grafix::Font font;
public:
	OxygenFrame(int x, int y, int width, int height, const ppl7::String& label);
	void setValue(float seconds_total, float seconds_left);
	void setLabel(const ppl7::String& label);
	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void setFontSize(int size);
};

class WorldWidget : public ppltk::Widget
{
private:
	StatsFrame* stats_health, * stats_lifes, * stats_points;
	OxygenFrame* stats_oxygen;
	int value_health, value_lifes, value_points;
	double oxygen_cooldown;
	bool showui;
public:
	WorldWidget();
	void retranslateUi();
	void setViewport(const ppl7::grafix::Rect& viewport);
	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;

	void updatePlayerStats(const Player* player);
	void resetPlayerStats(const Player* player);
	void setShowUi(bool enable);


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
	ppltk::TabWidget* tabwidget;

	//ppltk::LineInput* level_name;
	ppltk::SpinBox* level_width;
	ppltk::SpinBox* level_height;
	ppltk::Label* level_pixel_size;
	ppltk::TabWidget* tstrings;
	std::map<ppl7::String, ppltk::LineInput*> LevelName;
	std::map<ppl7::String, ppltk::TextEdit*> Description;
	ppltk::SpinBox* LevelSort;
	ppltk::CheckBox* part_of_story;
	ppltk::CheckBox* level_is_listed;
	ppltk::Label* thumbnail;
	ppltk::Button* thumb_take_screenshot;
	ppltk::Button* thumb_to_clipboard;
	ppltk::Button* thumb_from_clipboard;
	ppltk::Button* thumb_load;
	ppltk::Button* thumb_save;
	Screenshot* screenshot;
	ppltk::TextEdit* author;
	ppl7::ByteArray compressed_screenshot;
	int screenshot_timer_id;



	ppltk::RadioButton* radio_image;
	ppltk::RadioButton* radio_color;
	ppltk::ComboBox* background_image;

	//ppltk::Button* image_fileselect;

	ppltk::SpinBox* color_red;
	ppltk::SpinBox* color_green;
	ppltk::SpinBox* color_blue;
	ppltk::HorizontalSlider* slider_red;
	ppltk::HorizontalSlider* slider_green;
	ppltk::HorizontalSlider* slider_blue;
	ppltk::Frame* color_preview;
	//ppltk::Button* color_picker;

	ppltk::ComboBox* base_soundtrack;

	ppltk::ComboBox* additional_soundtrack;
	ppltk::Button* add_soundtrack_button;
	ppltk::Button* delete_soundtrack_button;
	ppltk::ListWidget* soundtrack_list;
	ppltk::CheckBox* soundtrack_random;

	// Items & Options
	ppltk::CheckBox* option_drain_battery;
	ppltk::CheckBox* option_flashlite_on_on_level_start;
	ppltk::DoubleHorizontalSlider* battery_drain_rate;
	ppltk::ComboBox* available_items;
	ppltk::Button* add_item_button;
	ppltk::Button* delete_item_button;
	ppltk::ListWidget* initial_items_list;
	ppltk::Label* battery_empty_time;



	ppltk::Button* ok_button;
	ppltk::Button* cancel_button;

	bool newlevel;
	DialogState my_state;

	std::map<ppl7::String, ppl7::String> song_map_identifier;
	Game* game;
	ppl7::grafix::Color previous_background;

	void updateColorPreview();
	void setupUi();
	void setupLevelTab();
	void setupBackgroundTab();
	void setupSoundtrackTab();
	void setupItemsAndOptionsTab();

public:
	int custom_id;
	LevelDialog(int width, int height);
	~LevelDialog();

	void setGame(Game* game);
	void setNewLevelFlag(bool newlevel);
	bool isNewLevel() const;
	DialogState state() const;

	void loadValues(const LevelParameter& params);
	void saveValues(LevelParameter& params) const;

	ppl7::String widgetType() const override;
	void mouseClickEvent(ppltk::MouseEvent* event) override;
	void mouseDownEvent(ppltk::MouseEvent* event) override;
	void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	void valueChangedEvent(ppltk::Event* event, double value) override;
	void keyDownEvent(ppltk::KeyEvent* event) override;
	void textChangedEvent(ppltk::Event* event, const ppl7::String& text) override;
	void closeEvent(ppltk::Event* event);
	void timerEvent(ppltk::Event* event);

};


class LightSelection : public ppltk::Frame
{
private:
	Game* game;
	TilesFrame* tilesframe;
	ColorSliderWidget* global_lighting;
	ppltk::CheckBox* enable_lighting;
	SpriteTexture* spriteset;
	//ColorSelectionFrame* colorframe;
	ColorSliderWidget* colorframe;

	ppltk::DoubleHorizontalSlider* scale_x;
	ppltk::DoubleHorizontalSlider* scale_y;
	ppltk::DoubleHorizontalSlider* angle;
	ppltk::HorizontalSlider* intensity;
	ppltk::CheckBox* player_plane_checkbox;
	ppltk::CheckBox* front_plane_checkbox;
	ppltk::CheckBox* back_plane_checkbox;

	ppltk::CheckBox* initial_state;
	ppltk::CheckBox* current_state;
	ppltk::CheckBox* lensflare;
	ppltk::CheckBox* flare_useLightColor;
	ppltk::HorizontalSlider* lensflareIntensity;
	ppltk::ComboBox* flarePlane;
	ppltk::ComboBox* lightType;
	ppltk::DoubleHorizontalSlider* lightParameter;


	ppltk::Label* light_id;


	//float scale;
	//float angle;

public:
	LightSelection(int x, int y, int width, int height, Game* game);
	ppl7::String widgetType() const override;
	void setSpriteSet(SpriteTexture* sprites);
	void setSelectedLight(int nr);
	int selectedLight() const;
	void setLightScaleX(float factor_x);
	float lightScaleX() const;
	void setLightScaleY(float factor_x);
	float lightScaleY() const;
	void setLightAngle(float angle);
	float lightAngle() const;
	ppl7::grafix::Color color() const;
	void setColor(const ppl7::grafix::Color& color);
	int colorIntensity() const;
	void setColorIntensity(int index);
	void setPlayerPlaneMatrix(LightPlayerPlaneMatrix matrix);
	LightPlayerPlaneMatrix getPlayerPlaneMatrix() const;

	void setLensFlarePlane(uint8_t matrix);
	uint8_t getLensFlarePlane() const;

	void setCurrentState(bool enabled);
	void setInitialState(bool enabled);
	void setLensflare(bool enabled);
	void setLensflareIntensity(uint8_t intensity);

	void setLightId(uint32_t id);

	bool getCurrentState() const;
	bool getInitialState() const;
	bool getLensflare() const;
	uint8_t getLensflareIntensity() const;

	bool getFlareUseLightColor() const;
	void setFlareUseLightColor(bool flag);

	LightType getLightType() const;
	void setLightType(LightType type);
	float lightTypeParameter() const;
	void setLightTypeParameter(float value);


	void valueChangedEvent(ppltk::Event* event, int value)  override;
	void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	void valueChangedEvent(ppltk::Event* event, double value)  override;
};


}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_UI_H_ */
