#ifndef INCLUDE_DECKER_WIDGETS_H_
#define INCLUDE_DECKER_WIDGETS_H_

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



class Scrollbar : public ppl7::tk::Widget
{
private:
	ppl7::tk::Button *up_button;
	ppl7::tk::Button *down_button;
	int size;
	int pos;

public:
	Scrollbar(int x, int y, int width, int height);

	void setSize(int size);
	void setPosition(int position);
	int position() const;
	virtual String widgetType() const;
	virtual void paint(Drawable &draw);


	void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

class CheckBox : public ppl7::tk::Label
{
private:
	bool	ischecked;

public:
	CheckBox();
	CheckBox(int x, int y, int width, int height, const String &text=String(), bool checked=false);
	~CheckBox();
	bool checked() const;
	void setChecked(bool checked);

	virtual String widgetType() const;
	virtual void paint(Drawable &draw);
	virtual void mouseDownEvent(MouseEvent *event);
};

class RadioButton : public ppl7::tk::Label
{
private:
	bool	ischecked;

public:
	RadioButton();
	RadioButton(int x, int y, int width, int height, const String &text=String(), bool checked=false);
	~RadioButton();
	bool checked() const;
	void setChecked(bool checked);

	virtual String widgetType() const;
	virtual void paint(Drawable &draw);
	virtual void mouseDownEvent(MouseEvent *event);
};


}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_WIDGETS_H_ */
