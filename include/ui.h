/*
 * decker.h
 *
 *  Created on: 03.10.2017
 *      Author: patrick
 */

#ifndef INCLUDE_DECKER_UI_H_
#define INCLUDE_DECKER_UI_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>

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
	ppl7::tk::Label *mouse_buttons;
	ppl7::tk::Label *time_label;


public:
	StatusBar(int x, int y, int width, int height);

	void setFps(int fps);
	void setMouse(const MouseState &mouse);
	void setWorldCoords(const Point &p);

	// Events
	void timerEvent(Event *event);

};

class MainMenue : public ppl7::tk::Frame
{
private:
	ppl7::tk::Button *exit_button;
	ppl7::tk::Button *edit_tiles_button;
	Game *game;


public:
	MainMenue(int x, int y, int width, int height, Game *game);

	void mouseClickEvent(ppl7::tk::MouseEvent *event);


};

class Scrollbar : public ppl7::tk::Widget
{
private:
	ppl7::tk::Button *up_button;
	ppl7::tk::Button *down_button;
public:
	Scrollbar(int x, int y, int width, int height);

	void setSize(int size);
	void setPosition(int position);
	int position() const;
	virtual String widgetType() const;
	virtual void paint(Drawable &draw);


	void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

class TilesSelection : public ppl7::tk::Frame
{
private:
	Game *game;
	::Sprite *tiles;

	Scrollbar *scrollbar;


public:
	TilesSelection(int x, int y, int width, int height, Game *game, ::Sprite *tiles);

	virtual void paint(Drawable &draw);
	void mouseClickEvent(ppl7::tk::MouseEvent *event);


};

}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_UI_H_ */
