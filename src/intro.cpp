#include <stdio.h>
#include <stdlib.h>

#include "decker_sdl.h"
#include "intro.h"


IntroVideo::IntroVideo(SDL &s)
    :sdl(s)
{
    wm=ppl7::tk::GetWindowManager();
    stop_playback=false;
}

IntroVideo::~IntroVideo()
{

}

void IntroVideo::run()
{
	SDL_Renderer* renderer=sdl.getRenderer();
	/*
	AudioStream* playing_song=&audiopool.song[1];
	playing_song->setVolume(0.1f);
	audiosystem.play(playing_song);
	*/
    ppl7::grafix::Color black(0,0,0,255);
    stop_playback=false;
	while (!stop_playback) {
		wm->handleEvents();
		double now=ppl7::GetMicrotime();
		sdl.startFrame(black);
		ppl7::tk::MouseState mouse=wm->getMouseState();
		
		presentScreen();
	}
}


void IntroVideo::quitEvent(ppl7::tk::Event *event)
{
    stop_playback=true;
}

void IntroVideo::closeEvent(ppl7::tk::Event *event)
{
    stop_playback=true;
}

void IntroVideo::keyDownEvent(ppl7::tk::KeyEvent *event)
{
    if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE) {
        stop_playback=true;

    }
}