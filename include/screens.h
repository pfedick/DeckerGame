#ifndef INCLUDE_SCREENS_H_
#define INCLUDE_SCREENS_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppltk.h>
#include "decker.h"
#include "widgets.h"

class IntroScreen : public ppltk::Widget
{
private:
    bool stop_playback;

public:
    IntroScreen(int x, int y, int width, int height);
    ~IntroScreen();

    bool stopSignal() const;
    // EventHandler
    void keyDownEvent(ppltk::KeyEvent* event);
    void gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event);
    void gameControllerDeviceAdded(ppltk::GameControllerEvent* event);
    void gameControllerDeviceRemoved(ppltk::GameControllerEvent* event);

};


class StyleElement
{
public:
    ppl7::grafix::Font font;
    ppl7::grafix::Size size;
    ppl7::grafix::Size total;
    int border_width;

};


class SettingsScreen : public ppltk::Widget
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
        ButtonJump,
        ButtonCrouch,
        ButtonFlashlight,
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


    ppltk::Frame* menue;
    ppltk::Frame* page_audio;
    ppltk::Frame* page_video;
    ppltk::Frame* page_controller;
    ppltk::Frame* page_misc;


    Decker::ui::GameMenuArea* select_audio;
    Decker::ui::GameMenuArea* select_video;
    Decker::ui::GameMenuArea* select_misc;
    Decker::ui::GameMenuArea* select_controller;
    Decker::ui::GameMenuArea* select_back;

    Decker::ui::GameMenuArea* select_menue;
    Decker::ui::GameMenuArea* select_game_exit;


    // Audio
    //ppltk::ComboBox* audio_device_combobox;
    ppltk::HorizontalSlider* audio_total_slider;
    ppltk::HorizontalSlider* audio_music_slider;
    ppltk::HorizontalSlider* audio_effects_slider;
    ppltk::HorizontalSlider* audio_ambience_slider;
    ppltk::HorizontalSlider* audio_speech_slider;

    // Video
    ppltk::ComboBox* video_device_combobox;
    ppltk::ComboBox* screen_resolution_combobox;
    ppltk::ComboBox* windowmode_combobox;
    ppltk::Button* save_video_settings_button;

    // Controller
    ppltk::Label* controller_name;
    ppltk::HorizontalSlider* controller_deadzone;
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
    Decker::ui::ControllerButtonSelector* controller_button_flashlight;
    Decker::ui::ControllerButtonSelector* controller_button_crouch;
    ppltk::CheckBox* controller_use_rumble;

    // Misc
    ppltk::ComboBox* text_language_combobox;
    ppltk::ComboBox* speech_language_combobox;
    ppltk::CheckBox* skipIntro_checkbox;
    ppltk::HorizontalSlider* difficulty_slider;
    ppltk::Label* difficulty_name;
    ppltk::Label* difficulty_description;
    ppltk::CheckBox* noBlood_checkbox;



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

    ppltk::HorizontalSlider* getCurrentAudioSlider();
    Decker::ui::ControllerButtonSelector* getCurrentControllerInput();


public:
    SettingsScreen(Game& game, int x, int y, int width, int height, bool ingame=false);
    ~SettingsScreen();
    virtual void paint(ppl7::grafix::Drawable& draw);

    virtual void keyDownEvent(ppltk::KeyEvent* event);
    virtual void mouseEnterEvent(ppltk::MouseEvent* event);
    virtual void mouseClickEvent(ppltk::MouseEvent* event);

    virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
    virtual void valueChangedEvent(ppltk::Event* event, int value);
    virtual void resizeEvent(ppltk::ResizeEvent* event);
    virtual void toggledEvent(ppltk::Event* event, bool checked) override;
    void gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event);
    void gameControllerAxisMotionEvent(ppltk::GameControllerAxisEvent* event);
    void gameControllerDeviceAdded(ppltk::GameControllerEvent* event);
    void gameControllerDeviceRemoved(ppltk::GameControllerEvent* event);

};

class LevelSelectScreen : public ppltk::Widget
{
private:
    Game& game;
    Decker::ui::LevelSelection* levelselection;
    Decker::ui::GameMenuArea* back_button;
    ppl7::String selectedLevelFilename;

public:
    LevelSelectScreen(Game& game, int x, int y, int width, int height);
    ~LevelSelectScreen();

    ppl7::String selectedLevel() const;

    void paint(ppl7::grafix::Drawable& draw) override;

    void keyDownEvent(ppltk::KeyEvent* event) override;
    void mouseEnterEvent(ppltk::MouseEvent* event) override;
    void mouseLeaveEvent(ppltk::MouseEvent* event) override;
    void mouseClickEvent(ppltk::MouseEvent* event) override;

    void textChangedEvent(ppltk::Event* event, const ppl7::String& text) override;

    void gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event) override;
    void gameControllerAxisMotionEvent(ppltk::GameControllerAxisEvent* event) override;
    void gameControllerDeviceAdded(ppltk::GameControllerEvent* event) override;
    void gameControllerDeviceRemoved(ppltk::GameControllerEvent* event) override;

};



class StartScreen : public ppltk::Widget
{
public:

    enum class State {
        None=0,
        QuitGame,
        StartGame,
        StartTutorial,
        ShowSettings,
        StartEditor,
        SelectLevel,
        StartLevel
    };

private:
    Game& game;
    ppl7::String selectedLevelFilename;
    Decker::ui::GameMenuArea* start_tutorial;
    Decker::ui::GameMenuArea* start_game;
    Decker::ui::GameMenuArea* select_level;
    Decker::ui::GameMenuArea* settings;
    Decker::ui::GameMenuArea* editor;
    Decker::ui::GameMenuArea* end;
    SettingsScreen* settings_screen;
    LevelSelectScreen* level_select_screen;

    ppltk::Frame* menue;
    ppltk::Label* version;

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
    void showLevelSection();
    ppl7::String selectedLevel() const;

    virtual void paint(ppl7::grafix::Drawable& draw);
    virtual void mouseEnterEvent(ppltk::MouseEvent* event);
    virtual void mouseClickEvent(ppltk::MouseEvent* event);
    virtual void keyDownEvent(ppltk::KeyEvent* event);
    void gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event);
    void gameControllerDeviceAdded(ppltk::GameControllerEvent* event);
    void gameControllerDeviceRemoved(ppltk::GameControllerEvent* event);

    virtual void closeEvent(ppltk::Event* event);
    virtual void resizeEvent(ppltk::ResizeEvent* event);


    // Event handler

};

class IngameMenuScreen : public ppltk::Widget
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
    virtual void closeEvent(ppltk::Event* event);
    virtual void resizeEvent(ppltk::ResizeEvent* event);
};


class GameStatsScreen : public ppltk::Widget
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

    void mouseClickEvent(ppltk::MouseEvent* event) override;
    void keyDownEvent(ppltk::KeyEvent* event) override;
    void gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event) override;
    void gameControllerDeviceAdded(ppltk::GameControllerEvent* event) override;
    void gameControllerDeviceRemoved(ppltk::GameControllerEvent* event) override;

};


#endif // INCLUDE_SCREENS_H_
