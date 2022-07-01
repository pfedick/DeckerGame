#ifndef INCLUDE_SCREENS_H_
#define INCLUDE_SCREENS_H_

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "decker_sdl.h"

class IntroScreen: public ppl7::tk::Widget
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




class StartScreen : public ppl7::tk::Widget
{
private:
    SDL &sdl;
    ppl7::grafix::Image TitleImage;
    ppl7::tk::Frame *frame_start_game;
    ppl7::tk::Frame *frame_settings;
    ppl7::tk::Frame *frame_end;

public:

    StartScreen(SDL &s, int x, int y, int width, int height);
    ~StartScreen();

    virtual void paint(ppl7::grafix::Drawable &draw);



    // Event handler



};


#endif // INCLUDE_SCREENS_H_
