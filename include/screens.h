#ifndef INCLUDE_SCREENS_H_
#define INCLUDE_SCREENS_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "decker_sdl.h"

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
    virtual void paint(ppl7::grafix::Drawable& draw);
    //virtual void mouseEnterEvent(ppl7::tk::MouseEvent* event);


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
    SDL& sdl;
    ppl7::grafix::Image TitleImage;
    GameMenuArea* start_game;
    GameMenuArea* settings;
    GameMenuArea* editor;
    GameMenuArea* end;

    ppl7::tk::Frame* menue;
    ppl7::tk::Label* version;

    State state;

public:


    StartScreen(SDL& s, int x, int y, int width, int height);
    ~StartScreen();

    State getState() const;
    void setState(State state);



    virtual void paint(ppl7::grafix::Drawable& draw);
    virtual void mouseEnterEvent(ppl7::tk::MouseEvent* event);
    virtual void mouseClickEvent(ppl7::tk::MouseEvent* event);
    virtual void keyDownEvent(ppl7::tk::KeyEvent* event);


    // Event handler

};


class SettingsScreen : public ppl7::tk::Widget
{
private:
    SDL& sdl;
public:
    SettingsScreen(SDL& s, int x, int y, int width, int height);
    ~SettingsScreen();
    virtual void paint(ppl7::grafix::Drawable& draw);

};

#endif // INCLUDE_SCREENS_H_
