#ifndef INCLUDE_INTRO_H_
#define INCLUDE_INTRO_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "decker_sdl.h"

#include "audio.h"

class IntroVideo : private ppl7::tk::Window
{
    private:
        SDL &sdl;
        ppl7::tk::WindowManager *wm;
        bool stop_playback;

    public:
        IntroVideo(SDL &s);
        ~IntroVideo();

        // EventHandler
        void quitEvent(ppl7::tk::Event *event);
	    void closeEvent(ppl7::tk::Event *event);
        void keyDownEvent(ppl7::tk::KeyEvent *event);

        void run();
};


#endif // INCLUDE_INTRO_H_