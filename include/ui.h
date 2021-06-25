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

namespace Decker {

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


}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_UI_H_ */
