#ifndef INCLUDE_DECKER_WIDGETS_H_
#define INCLUDE_DECKER_WIDGETS_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppltk.h>
#include <list>

class Game;
class SpriteTexture;


namespace Decker {
namespace ui {

class Dialog : public ppltk::Widget
{
public:
	enum Buttons {
		None=0,
		OK=1,
		Copy=2,
		Paste=4,
		CopyAndPaste=6,
		Test=8,
		Reset=16
	};
private:
	ppl7::String WindowTitle;
	ppl7::grafix::Image WindowIcon;
	ppl7::grafix::Image widget_drawable;
	ppl7::grafix::Color	myBackground;
	ppltk::Button* ok_button;
	ppltk::Button* copy_button;
	ppltk::Button* paste_button;
	ppltk::Button* test_button;
	ppltk::Button* reset_button;
	ppltk::Button* close_button;
	ppl7::grafix::Point move_start;
	ppltk::Frame* client_frame;

	bool drag_started;
	int drag_offset;
	ppl7::grafix::Point drag_start_pos;

public:
	Dialog(int width, int height, int buttons=Buttons::OK);
	~Dialog();

	void addChild(Widget* w);
	void removeChild(Widget* w);
	void destroyChilds();
	ppl7::grafix::Rect clientRect() const;
	ppl7::grafix::Size clientSize() const;



	const ppl7::String& windowTitle() const;
	void setWindowTitle(const ppl7::String& title);
	const ppl7::grafix::Drawable& windowIcon() const;
	void setWindowIcon(const ppl7::grafix::Drawable& icon);
	const ppl7::grafix::Color& backgroundColor() const;
	void setBackgroundColor(const ppl7::grafix::Color& c);
	virtual void paint(ppl7::grafix::Drawable& draw);
	virtual void mouseDownEvent(ppltk::MouseEvent* event);
	virtual void dialogButtonEvent(Dialog::Buttons button);
	void mouseClickEvent(ppltk::MouseEvent* event) override;

	virtual void mouseUpEvent(ppltk::MouseEvent* event);
	virtual void lostFocusEvent(ppltk::FocusEvent* event);
	virtual void mouseMoveEvent(ppltk::MouseEvent* event);
};


class FileDialog : public Dialog
{
public:
	enum class FileMode {
		AnyFile=0,
		ExistingFile,
		Directory
	};

	enum class DialogState {
		Open=0,
		OK,
		Aborted
	};

private:
	ppltk::Button* ok_button;
	ppltk::Button* cancel_button;
	ppltk::LineInput* path_lineinput;
	ppltk::LineInput* filename_lineinput;
	ppltk::ListWidget* dir_list;
	ppltk::ListWidget* file_list;
#ifdef WIN32
	ppltk::ComboBox* drives_combobox;
#endif
	bool matchFilter(const ppl7::String& filename) const;

	DialogState my_state;
	std::list<ppl7::String> filter;



public:
	FileDialog(int width, int height, FileMode mode=FileMode::AnyFile);
	~FileDialog();

	int custom_id;

	ppl7::String directory() const;
	ppl7::String filename() const;
	DialogState state() const;

	void setFilename(const ppl7::String& filename);
	void setDirectory(const ppl7::String& path);
	void setFilter(const ppl7::String& patternlist);

	void mouseDblClickEvent(ppltk::MouseEvent* event) override;
	void mouseDownEvent(ppltk::MouseEvent* event) override;
	void mouseClickEvent(ppltk::MouseEvent* event) override;
	void valueChangedEvent(ppltk::Event* event, int value) override;
	void keyDownEvent(ppltk::KeyEvent* event) override;
	void closeEvent(ppltk::Event* event) override;

};

class ColorSliderWidget : public ppltk::Widget
{
private:
	ppltk::SpinBox* color_red;
	ppltk::SpinBox* color_green;
	ppltk::SpinBox* color_blue;
	ppltk::SpinBox* color_alpha;
	ppltk::HorizontalSlider* slider_red;
	ppltk::HorizontalSlider* slider_green;
	ppltk::HorizontalSlider* slider_blue;
	ppltk::HorizontalSlider* slider_alpha;
	ppltk::Frame* color_preview;
	bool with_alphachannel;



	void setupUi();
	void updateColorPreview();

public:
	ColorSliderWidget(int x, int y, int width, int height, bool withAlphaChannel=false);
	void setColor(const ppl7::grafix::Color& color);
	ppl7::grafix::Color color() const;
	void setColorPreviewSize(int w, int h);

	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	void keyDownEvent(ppltk::KeyEvent* event) override;

};

class GradientWidget : public ppltk::Widget
{
private:
	class Item
	{
	public:
		Item();
		float age;
		float value;
		ppl7::grafix::Color color;
	};
	ppl7::grafix::Image gradient_bg;
	ppl7::grafix::Image gradient_vg;
	size_t max_items;
	size_t max_id;
	size_t selected_id;
	std::map<size_t, Item> items;
	std::map<size_t, int> handler_pos;
	int gradient_y1;
	int gradient_y2;

	bool drag_started;
	int drag_offset;
	ppl7::grafix::Point drag_start_pos;

	ppltk::Button* add_item_button;
	ppltk::Button* delete_item_button;

	std::list<Item> getSortedList() const;

	void drawCheckboard(ppl7::grafix::Drawable& draw);
	void drawGradient(ppl7::grafix::Drawable& draw);
	void drawHandler(ppl7::grafix::Drawable& draw, int y1, int y2);
	void drawHandlerItem(ppl7::grafix::Drawable& draw, int y, const ppl7::grafix::Color handler_color);

public:
	GradientWidget(int x, int y, int width, int height);
	~GradientWidget();

	void clear();
	void addItem(float age, const ppl7::grafix::Color& color, float value=0.0f);
	std::map<float, ppl7::grafix::Color> getColorItems() const;
	std::map<float, float> getValueItems() const;

	float currentAge() const;
	float currentValue() const;
	ppl7::grafix::Color currentColor() const;
	void setCurrentAge(float age);
	void setCurrentValue(float age);
	void setCurrentColor(const ppl7::grafix::Color& color);
	void deleteCurrentItem();

	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void mouseDownEvent(ppltk::MouseEvent* event) override;
	void mouseUpEvent(ppltk::MouseEvent* event) override;
	void lostFocusEvent(ppltk::FocusEvent* event) override;
	void mouseMoveEvent(ppltk::MouseEvent* event) override;

};


class GameMenuArea : public ppltk::Widget
{
private:
	ppl7::String text;
	ppl7::grafix::Font font;
	int border_width;
	bool selected;
public:
	GameMenuArea(int x, int y, int width, int height, const ppl7::String& text=ppl7::String());
	void setText(const ppl7::String& text);
	void setSelected(bool selected);
	bool isSelected() const;
	void setFontSize(int size);
	void setBorderWidth(int width);
	virtual void paint(ppl7::grafix::Drawable& draw);
};

class ControllerButtonSelector : public ppltk::Widget
{
public:
	enum class ControllerType {
		Axis,
		Trigger,
		Button
	};
private:
	ppl7::String text;
	ppl7::grafix::Font font;
	int border_width;
	int button_id;
	ControllerType controllertype;
	bool inputmode;

	void emmitValueChangedEvent();
public:
	ControllerButtonSelector(int x, int y, int width, int height, const ppl7::String& text=ppl7::String());
	void setFontSize(int size);
	void setControllerType(ControllerType type);
	void setId(int id);
	int getId() const;
	void setName(const ppl7::String& name);
	void setInputmode();
	virtual void paint(ppl7::grafix::Drawable& draw);
	virtual void gameControllerAxisMotionEvent(ppltk::GameControllerAxisEvent* event);
	virtual void gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event);
	virtual void mouseDownEvent(ppltk::MouseEvent* event);
};




}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_WIDGETS_H_ */
