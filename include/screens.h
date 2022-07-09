#ifndef INCLUDE_SCREENS_H_
#define INCLUDE_SCREENS_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "decker.h"
#include "widgets.h"

class IntroScreen : public ppl7::tk::Widget
{
private:
    bool stop_playback;

public:
    IntroScreen(int x, int y, int width, int height);
    ~IntroScreen();

    bool stopSignal() const;
    // EventHandler
    void keyDownEvent(ppl7::tk::KeyEvent* event);

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
    //virtual void mouseEnterEvent(ppl7::tk::MouseEvent* event);


};

class StyleElement
{
public:
    ppl7::grafix::Font font;
    ppl7::grafix::Size size;
    ppl7::grafix::Size total;
    int border_width;

};


class SettingsScreen : public ppl7::tk::Widget
{
private:
    enum class SettingsMenue {
        Audio=0,
        Video=1,
        Misc=2,
        Back=3
    };
    Game& game;
    SDL& sdl;
    SettingsMenue currentMenueSelection;
    StyleElement style_heading, style_label, style_menue;
    ppl7::grafix::Point settings_page;
    int input_widget_x;
    ppl7::grafix::Size input_widget;


    ppl7::tk::Frame* menue;
    ppl7::tk::Frame* page_audio;
    ppl7::tk::Frame* page_video;
    ppl7::tk::Frame* page_misc;

    GameMenuArea* select_audio;
    GameMenuArea* select_video;
    GameMenuArea* select_misc;
    GameMenuArea* select_back;

    // Audio
    Decker::ui::ComboBox* audio_device_combobox;
    Decker::ui::HorizontalSlider* audio_total_slider;
    Decker::ui::HorizontalSlider* audio_music_slider;
    Decker::ui::HorizontalSlider* audio_effects_slider;

    // Video
    Decker::ui::ComboBox* video_device_combobox;
    Decker::ui::ComboBox* screen_resolution_combobox;
    Decker::ui::ComboBox* windowmode_combobox;
    ppl7::tk::Button* save_video_settings_button;

    // Misc
    Decker::ui::ComboBox* language_combobox;

    void selectSettingsPage(SettingsMenue page);
    void setupUi();
    void updateStyles();
    void initPageAudio();
    void initPageVideo();
    void initPageMisc();

    void updateVideoModes();


public:
    SettingsScreen(Game& game, SDL& s, int x, int y, int width, int height);
    ~SettingsScreen();
    virtual void paint(ppl7::grafix::Drawable& draw);

    virtual void keyDownEvent(ppl7::tk::KeyEvent* event);
    virtual void mouseEnterEvent(ppl7::tk::MouseEvent* event);
    virtual void mouseClickEvent(ppl7::tk::MouseEvent* event);

    virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
    virtual void resizeEvent(ppl7::tk::ResizeEvent* event);

};



class StartScreen : public ppl7::tk::Widget
{
public:

    enum class State {
        None=0,
        QuitGame,
        StartGame,
        ShowSettings,
        StartEditor
    };

private:
    Game& game;
    SDL& sdl;

    GameMenuArea* start_game;
    GameMenuArea* settings;
    GameMenuArea* editor;
    GameMenuArea* end;
    SettingsScreen* settings_screen;

    ppl7::tk::Frame* menue;
    ppl7::tk::Label* version;

    //Decker::ui::FileDialog *filedialog;

    State state;

    void resizeSettingsScreen();

public:


    StartScreen(Game& g, SDL& s, int x, int y, int width, int height);
    ~StartScreen();

    State getState() const;
    void setState(State state);

    void showSettings();

    virtual void paint(ppl7::grafix::Drawable& draw);
    virtual void mouseEnterEvent(ppl7::tk::MouseEvent* event);
    virtual void mouseClickEvent(ppl7::tk::MouseEvent* event);
    virtual void keyDownEvent(ppl7::tk::KeyEvent* event);

    virtual void closeEvent(ppl7::tk::Event* event);
    virtual void resizeEvent(ppl7::tk::ResizeEvent* event);


    // Event handler

};


#endif // INCLUDE_SCREENS_H_
