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

class Dialog : public ppl7::tk::Widget
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
	ppl7::tk::Button* ok_button;
	ppl7::tk::Button* copy_button;
	ppl7::tk::Button* paste_button;
	ppl7::tk::Button* test_button;
	ppl7::tk::Button* reset_button;
	ppl7::grafix::Point move_start;
	ppl7::tk::Frame* client_frame;

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
	virtual void mouseDownEvent(ppl7::tk::MouseEvent* event);
	virtual void dialogButtonEvent(Dialog::Buttons button);

	virtual void mouseUpEvent(ppl7::tk::MouseEvent* event);
	virtual void lostFocusEvent(ppl7::tk::FocusEvent* event);
	virtual void mouseMoveEvent(ppl7::tk::MouseEvent* event);
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
	ppl7::tk::Button* ok_button;
	ppl7::tk::Button* cancel_button;
	ppl7::tk::LineInput* path_lineinput;
	ppl7::tk::LineInput* filename_lineinput;
	ppl7::tk::ListWidget* dir_list;
	ppl7::tk::ListWidget* file_list;
#ifdef WIN32
	ppl7::tk::ComboBox* drives_combobox;
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

	void mouseDblClickEvent(ppl7::tk::MouseEvent* event) override;
	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;
	void mouseClickEvent(ppl7::tk::MouseEvent* event) override;
	void valueChangedEvent(ppl7::tk::Event* event, int value) override;
	void keyDownEvent(ppl7::tk::KeyEvent* event) override;

};

class ColorSliderWidget : public ppl7::tk::Widget
{
private:
	ppl7::tk::SpinBox* color_red;
	ppl7::tk::SpinBox* color_green;
	ppl7::tk::SpinBox* color_blue;
	ppl7::tk::SpinBox* color_alpha;
	ppl7::tk::HorizontalSlider* slider_red;
	ppl7::tk::HorizontalSlider* slider_green;
	ppl7::tk::HorizontalSlider* slider_blue;
	ppl7::tk::HorizontalSlider* slider_alpha;
	ppl7::tk::Frame* color_preview;
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
	void valueChangedEvent(ppl7::tk::Event* event, int64_t value) override;
	void keyDownEvent(ppl7::tk::KeyEvent* event) override;

};

class TabWidget : public ppl7::tk::Widget
{
private:
	int current_tab;
	Widget* current_child;
	class Tab {
	public:
		Tab();
		Tab(ppl7::tk::Widget* widget, const ppl7::String& title, const ppl7::grafix::Drawable& icon=ppl7::grafix::Drawable());
		Tab(const Tab& other);
		ppl7::tk::Widget* widget;
		ppl7::String title;
		ppl7::grafix::Image icon;
		int x;
		int width;
	};
	std::map<int, Tab> tabs;
public:
	TabWidget(int x, int y, int width, int height);
	~TabWidget();

	ppl7::tk::Widget* addTab(int id, const ppl7::String& title, const ppl7::grafix::Drawable& icon=ppl7::grafix::Drawable());
	void addWidget(int id, const ppl7::String& title, ppl7::tk::Widget* widget, const ppl7::grafix::Drawable& icon=ppl7::grafix::Drawable());
	void removeTab(int id);
	void setVisible(int id, bool visible);
	void setEnabled(int id, bool enabled);
	void setTitle(int id, const ppl7::String& title);
	void setIcon(int id, const ppl7::grafix::Drawable& icon);
	void setWidget(int id, ppl7::tk::Widget* widget);
	ppl7::tk::Widget* getWidget(int id) const;
	int currentTab() const;
	void setCurrentTab(int id);
	size_t tabCount() const;
	void clear();

	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;

};

class GradientWidget : public ppl7::tk::Widget
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

	ppl7::tk::Button* add_item_button;
	ppl7::tk::Button* delete_item_button;

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
	void mouseDownEvent(ppl7::tk::MouseEvent* event) override;
	void mouseUpEvent(ppl7::tk::MouseEvent* event) override;
	void lostFocusEvent(ppl7::tk::FocusEvent* event) override;
	void mouseMoveEvent(ppl7::tk::MouseEvent* event) override;

};


class GameMenuArea : public ppl7::tk::Widget
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

class ControllerButtonSelector : public ppl7::tk::Widget
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
	ControllerType controllertype;
public:
	ControllerButtonSelector(int x, int y, int width, int height, const ppl7::String& text=ppl7::String());
	void setFontSize(int size);
	void setControllertype(ControllerType type);
	void setId(int id);
	int getId() const;
	void setName(const ppl7::String& name);
	virtual void paint(ppl7::grafix::Drawable& draw);
	virtual void gameControllerAxisMotionEvent(ppl7::tk::GameControllerAxisEvent* event);
	virtual void gameControllerButtonDownEvent(ppl7::tk::GameControllerButtonEvent* event);
};




}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_WIDGETS_H_ */
