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
    void gameControllerButtonDownEvent(ppl7::tk::GameControllerButtonEvent* event);
    void gameControllerDeviceAdded(ppl7::tk::GameControllerEvent* event);
    void gameControllerDeviceRemoved(ppl7::tk::GameControllerEvent* event);

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
        Controller=2,
        Misc=3,
        Menue=4,
        Exit=5,
        Back=6,
    };
    enum class SettingsAudio {
        Total,
        Music,
        Effects,
        Ambience,
        Speech
    };
    enum class SettingsMisc {

    };
    enum class SettingsController {
        Deadzone=0,
        AxisWalk,
        AxisJump,
        ButtonUp,
        ButtonDown,
        ButtonLeft,
        ButtonRight,
        ButtonMenu,
        ButtonAction,
        ButtonBack,
        ButtonJump
    };
    enum class KeyFocusArea {
        Menu=0,
        Audio,
        Controller,
        Misc
    };
    KeyFocusArea keyfocus;

    Game& game;
    bool ingame;
    //SDL& sdl;
    SettingsMenue currentMenueSelection;
    SettingsAudio currentAudioSelection;
    SettingsController currentControllerSelection;
    StyleElement style_heading, style_label, style_menue;
    ppl7::grafix::Point settings_page;
    int input_widget_x;
    ppl7::grafix::Size input_widget;


    ppl7::tk::Frame* menue;
    ppl7::tk::Frame* page_audio;
    ppl7::tk::Frame* page_video;
    ppl7::tk::Frame* page_controller;
    ppl7::tk::Frame* page_misc;


    Decker::ui::GameMenuArea* select_audio;
    Decker::ui::GameMenuArea* select_video;
    Decker::ui::GameMenuArea* select_misc;
    Decker::ui::GameMenuArea* select_controller;
    Decker::ui::GameMenuArea* select_back;

    Decker::ui::GameMenuArea* select_menue;
    Decker::ui::GameMenuArea* select_game_exit;


    // Audio
    //ppl7::tk::ComboBox* audio_device_combobox;
    ppl7::tk::HorizontalSlider* audio_total_slider;
    ppl7::tk::HorizontalSlider* audio_music_slider;
    ppl7::tk::HorizontalSlider* audio_effects_slider;
    ppl7::tk::HorizontalSlider* audio_ambience_slider;
    ppl7::tk::HorizontalSlider* audio_speech_slider;

    // Video
    ppl7::tk::ComboBox* video_device_combobox;
    ppl7::tk::ComboBox* screen_resolution_combobox;
    ppl7::tk::ComboBox* windowmode_combobox;
    ppl7::tk::Button* save_video_settings_button;

    // Controller
    ppl7::tk::Label* controller_name;
    ppl7::tk::HorizontalSlider* controller_deadzone;
    Decker::ui::ControllerButtonSelector* controller_axis_walk;
    Decker::ui::ControllerButtonSelector* controller_axis_jump;
    Decker::ui::ControllerButtonSelector* controller_button_up;
    Decker::ui::ControllerButtonSelector* controller_button_down;
    Decker::ui::ControllerButtonSelector* controller_button_left;
    Decker::ui::ControllerButtonSelector* controller_button_right;
    Decker::ui::ControllerButtonSelector* controller_button_menu;
    Decker::ui::ControllerButtonSelector* controller_button_action;
    Decker::ui::ControllerButtonSelector* controller_button_back;
    Decker::ui::ControllerButtonSelector* controller_button_jump;

    // Misc
    ppl7::tk::ComboBox* text_language_combobox;
    ppl7::tk::ComboBox* speech_language_combobox;
    ppl7::tk::CheckBox* skipIntro_checkbox;
    ppl7::tk::HorizontalSlider* difficulty_slider;
    ppl7::tk::Label* difficulty_name;
    ppl7::tk::Label* difficulty_description;



    void selectSettingsPage(SettingsMenue page);
    void setupUi();
    void updateStyles();
    void initPageAudio();
    void initPageVideo();
    void initPageMisc();
    void initPageController();

    void updateDifficultyDescription();
    void updateVideoModes();
    void retranslateUi();

    void handleKeyDownEvent(int key);
    void handleMenuKeyDownEvent(int key);
    void handleAudioKeyDownEvent(int key);
    void handleControllerKeyDownEvent(int key);
    void setFocusToControllerWidget();

    ppl7::tk::HorizontalSlider* getCurrentAudioSlider();
    Decker::ui::ControllerButtonSelector* getCurrentControllerInput();


public:
    SettingsScreen(Game& game, int x, int y, int width, int height, bool ingame=false);
    ~SettingsScreen();
    virtual void paint(ppl7::grafix::Drawable& draw);

    virtual void keyDownEvent(ppl7::tk::KeyEvent* event);
    virtual void mouseEnterEvent(ppl7::tk::MouseEvent* event);
    virtual void mouseClickEvent(ppl7::tk::MouseEvent* event);

    virtual void valueChangedEvent(ppl7::tk::Event* event, int64_t value);
    virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
    virtual void resizeEvent(ppl7::tk::ResizeEvent* event);
    virtual void toggledEvent(ppl7::tk::Event* event, bool checked) override;
    void gameControllerButtonDownEvent(ppl7::tk::GameControllerButtonEvent* event);
    void gameControllerAxisMotionEvent(ppl7::tk::GameControllerAxisEvent* event);
    void gameControllerDeviceAdded(ppl7::tk::GameControllerEvent* event);
    void gameControllerDeviceRemoved(ppl7::tk::GameControllerEvent* event);

};



class StartScreen : public ppl7::tk::Widget
{
public:

    enum class State {
        None=0,
        QuitGame,
        StartGame,
        StartTutorial,
        ShowSettings,
        StartEditor
    };

private:
    Game& game;
    Decker::ui::GameMenuArea* start_tutorial;
    Decker::ui::GameMenuArea* start_game;
    Decker::ui::GameMenuArea* settings;
    Decker::ui::GameMenuArea* editor;
    Decker::ui::GameMenuArea* end;
    SettingsScreen* settings_screen;

    ppl7::tk::Frame* menue;
    ppl7::tk::Label* version;

    //Decker::ui::FileDialog *filedialog;

    State state;

    void resizeSettingsScreen();
    void retranslateUi();
    void handleKeyDownEvent(int key);

public:


    StartScreen(Game& g, int x, int y, int width, int height);
    ~StartScreen();

    State getState() const;
    void setState(State state);

    void showSettings();

    virtual void paint(ppl7::grafix::Drawable& draw);
    virtual void mouseEnterEvent(ppl7::tk::MouseEvent* event);
    virtual void mouseClickEvent(ppl7::tk::MouseEvent* event);
    virtual void keyDownEvent(ppl7::tk::KeyEvent* event);
    void gameControllerButtonDownEvent(ppl7::tk::GameControllerButtonEvent* event);
    void gameControllerDeviceAdded(ppl7::tk::GameControllerEvent* event);
    void gameControllerDeviceRemoved(ppl7::tk::GameControllerEvent* event);

    virtual void closeEvent(ppl7::tk::Event* event);
    virtual void resizeEvent(ppl7::tk::ResizeEvent* event);


    // Event handler

};

class IngameMenuScreen : public ppl7::tk::Widget
{
public:
    enum class State {
        None=0,
        EndGame,
        ToDesktop,
        ShowSettings,
        Continue
    };
private:
    Decker::ui::GameMenuArea* end_game;
    Decker::ui::GameMenuArea* back_to_desktop;
    Decker::ui::GameMenuArea* show_settings;
    Decker::ui::GameMenuArea* continue_game;

    State state;
    void setupUi();

public:
    IngameMenuScreen(int x, int y, int width, int height);
    ~IngameMenuScreen();

    State getState() const;
    void setState(State state);

    virtual void paint(ppl7::grafix::Drawable& draw);
    virtual void closeEvent(ppl7::tk::Event* event);
    virtual void resizeEvent(ppl7::tk::ResizeEvent* event);
};


class GameStatsScreen : public ppl7::tk::Widget
{
private:
    Game& game;
    StatsScreenReason reason;
    bool closeWindow;

    StyleElement style_heading, style_label, style_anykey;
    int y, x1, x2, x3;

    void updateStyles();

public:
    GameStatsScreen(Game& g, int x, int y, int width, int height, StatsScreenReason reason);
    ~GameStatsScreen();
    virtual void paint(ppl7::grafix::Drawable& draw);
    void setPlayer(const Player& player);
    void setLevelStats(const LevelStats& level);
    void signalContinue();
    bool continueSignaled() const;

    void addResult(const ppl7::String& caption, int object_type, size_t max, size_t achived, size_t bonus =0);
    void addTotal(const ppl7::String& caption, int points);
    void addLine(const ppl7::String& caption);

    void gameControllerButtonDownEvent(ppl7::tk::GameControllerButtonEvent* event);
    void gameControllerDeviceAdded(ppl7::tk::GameControllerEvent* event);
    void gameControllerDeviceRemoved(ppl7::tk::GameControllerEvent* event);

};


#endif // INCLUDE_SCREENS_H_
