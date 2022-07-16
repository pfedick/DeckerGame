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
	};
private:
	ppl7::String WindowTitle;
	ppl7::grafix::Image WindowIcon;
	ppl7::grafix::Color	myBackground;
	ppl7::tk::Button* ok_button;
	ppl7::grafix::Point move_start;

public:
	Dialog(int width, int height, int buttons=Buttons::OK);
	~Dialog();

	const ppl7::String& windowTitle() const;
	void setWindowTitle(const ppl7::String& title);
	const ppl7::grafix::Drawable& windowIcon() const;
	void setWindowIcon(const ppl7::grafix::Drawable& icon);
	const ppl7::grafix::Color& backgroundColor() const;
	void setBackgroundColor(const ppl7::grafix::Color& c);
	virtual void paint(ppl7::grafix::Drawable& draw);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent* event);
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


	DialogState my_state;



public:
	FileDialog(int width, int height, FileMode mode=FileMode::AnyFile);
	~FileDialog();

	int custom_id;

	ppl7::String directory() const;
	ppl7::String filename() const;
	DialogState state() const;

	void setFilename(const ppl7::String& filename);
	void setDirectory(const ppl7::String& path);
	void setFilter(const ppl7::String& filter);

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
	ppl7::tk::HorizontalSlider* slider_red;
	ppl7::tk::HorizontalSlider* slider_green;
	ppl7::tk::HorizontalSlider* slider_blue;
	ppl7::tk::Frame* color_preview;


	void setupUi();
	void updateColorPreview();

public:
	ColorSliderWidget(int x, int y, int width, int height, const ppl7::grafix::Color& color=ppl7::grafix::Color());
	void setColor(const ppl7::grafix::Color& color);
	ppl7::grafix::Color color() const;

	ppl7::String widgetType() const override;
	void paint(ppl7::grafix::Drawable& draw) override;
	void valueChangedEvent(ppl7::tk::Event* event, int value) override;
	void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text) override;
	void keyDownEvent(ppl7::tk::KeyEvent* event) override;

};



}	// EOF namespace ui
}	// EOF namespace Decker

#endif /* INCLUDE_DECKER_WIDGETS_H_ */
