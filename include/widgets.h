#ifndef INCLUDE_DECKER_WIDGETS_H_
#define INCLUDE_DECKER_WIDGETS_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include <list>

class Game;
class SpriteTexture;

namespace Decker {
namespace ui {

/*
using namespace ppl7;
using namespace ppl7::grafix;
using namespace ppl7::tk;
*/


class Scrollbar : public ppl7::tk::Widget
{
private:
	ppl7::tk::Button *up_button;
	ppl7::tk::Button *down_button;
	int size;
	int pos;
	int visibleItems;

public:
	Scrollbar(int x, int y, int width, int height);

	void setSize(int size);
	void setPosition(int position);
	void setVisibleItems(int items);
	int position() const;
	virtual ppl7::String widgetType() const;
	virtual void paint(ppl7::grafix::Drawable &draw);


	void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

class CheckBox : public ppl7::tk::Label
{
private:
	bool	ischecked;

public:
	CheckBox();
	CheckBox(int x, int y, int width, int height, const ppl7::String &text=ppl7::String(), bool checked=false);
	~CheckBox();
	bool checked() const;
	void setChecked(bool checked);

	virtual ppl7::String widgetType() const;
	virtual void paint(ppl7::grafix::Drawable &draw);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

class RadioButton : public ppl7::tk::Label
{
private:
	bool	ischecked;

public:
	RadioButton();
	RadioButton(int x, int y, int width, int height, const ppl7::String &text=ppl7::String(), bool checked=false);
	~RadioButton();
	bool checked() const;
	void setChecked(bool checked);

	virtual ppl7::String widgetType() const;
	virtual void paint(ppl7::grafix::Drawable &draw);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

class ListWidget : public ppl7::tk::Frame
{
private:
	Scrollbar *scrollbar;
	ppl7::String myCurrentText;
	ppl7::String myCurrentIdentifier;
	size_t myCurrentIndex;
	size_t visibleItems;
	int mouseOverIndex;
	class ListWidgetItem
	{
	public:
		ppl7::String text;
		ppl7::String identifier;
		size_t index;
	};
	std::list<ListWidgetItem> items;
public:
	ListWidget(int x, int y, int width, int height);
	void setCurrentText(const ppl7::String &text);
	ppl7::String currentText() const;
	ppl7::String currentIdentifier() const;

	void setCurrentIndex(size_t index);
	size_t currentIndex() const;

	void clear();

	void add(const ppl7::String &text, const ppl7::String &identifier=ppl7::String());

	virtual ppl7::String widgetType() const;
	virtual void paint(ppl7::grafix::Drawable &draw);
	virtual void valueChangedEvent(ppl7::tk::Event *event, int value);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
	virtual void mouseWheelEvent(ppl7::tk::MouseEvent *event);
	virtual void lostFocusEvent(ppl7::tk::FocusEvent *event);
	virtual void mouseMoveEvent(ppl7::tk::MouseEvent *event);
};

class ComboBox : public ppl7::tk::Widget
{
private:
	ppl7::tk::Label *dropdown_button;
	ppl7::String myCurrentText;
	ppl7::String myCurrentIdentifier;
	size_t myCurrentIndex;

	class ComboBoxItem
	{
	public:
		ppl7::String text;
		ppl7::String identifier;
		size_t index;
	};
	std::list<ComboBoxItem> items;

	ListWidget *selection;

public:
	ComboBox(int x, int y, int width, int height);

	void setCurrentText(const ppl7::String &text);
	ppl7::String currentText() const;
	ppl7::String currentIdentifier() const;

	void setCurrentIndex(size_t index);
	size_t currentIndex() const;

	void clear();

	void add(const ppl7::String &text, const ppl7::String &identifier=ppl7::String());

	virtual ppl7::String widgetType() const;
	virtual void paint(ppl7::grafix::Drawable &draw);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
	virtual void mouseWheelEvent(ppl7::tk::MouseEvent *event);
	virtual void valueChangedEvent(ppl7::tk::Event *event, int value);
};


}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_WIDGETS_H_ */
