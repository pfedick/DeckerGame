#ifndef INCLUDE_DECKER_UI_H_
#define INCLUDE_DECKER_UI_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "widgets.h"

class Game;
class Sprite;

namespace Decker {
namespace ui {

using namespace ppl7;
using namespace ppl7::grafix;
using namespace ppl7::tk;



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
	void setMouse(const MouseState &mouse);
	void setWorldCoords(const Point &p);
	void setPlayerCoords(const Point &p);

	// Events
	void timerEvent(Event *event);

};

class MainMenue : public ppl7::tk::Frame
{
private:
	ppl7::tk::Button *exit_button;
	ppl7::tk::Button *edit_tiles_button;
	ppl7::tk::Button *save_button;
	ppl7::tk::Button *load_button;
	CheckBox *show_grid_checkbox;
	Game *game;


public:
	MainMenue(int x, int y, int width, int height, Game *game);

	void mouseClickEvent(ppl7::tk::MouseEvent *event);
	bool showGrid() const;

};

class TilesFrame : public ppl7::tk::Frame
{
private:
	Game *game;
	ppl7::grafix::Sprite *tiles;
	Scrollbar *scrollbar;
	int selected_tile;
public:
	TilesFrame(int x, int y, int width, int height, Game *game, ppl7::grafix::Sprite *tiles);
	virtual void paint(Drawable &draw);
	void mouseDownEvent(ppl7::tk::MouseEvent *event);
	void mouseMoveEvent(ppl7::tk::MouseEvent *event);
	void mouseWheelEvent(ppl7::tk::MouseEvent *event);
	void valueChangedEvent(ppl7::tk::Event *event, int value);

	void setSelectedTile(int nr);
	int selectedTile() const;

};

class TilesSelection : public ppl7::tk::Frame
{
private:
	Game *game;
	TilesFrame *tilesframe;


public:
	TilesSelection(int x, int y, int width, int height, Game *game, ppl7::grafix::Sprite *tiles);

	//virtual void paint(Drawable &draw);

	void setSelectedTile(int nr);
	int selectedTile() const;



};

}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_UI_H_ */
