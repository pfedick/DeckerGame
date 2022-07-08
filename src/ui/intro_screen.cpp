#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"
#include "videoplayer.h"


static void FadeToBlack(SDL_Renderer* renderer, int fade_to_black)
{
	if (fade_to_black > 0) {
		SDL_BlendMode currentBlendMode;
		SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
		//SDL_BlendMode newBlendMode=SDL_BLENDMODE_BLEND;
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, fade_to_black);
		SDL_RenderFillRect(renderer, NULL);
		SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
	}
}

static void getVideoDestination(VideoPlayer& video, ppl7::tk::Window& window, SDL_Rect& dest)
{
	float aspect=(float)video.width() / (float)video.height();
	dest.x=0;
	dest.y=0;
	dest.w=video.width();
	dest.h=video.height();
	if (dest.w > window.width()) dest.w=window.width();
	dest.h=dest.w / aspect;
	if (dest.h > window.height()) {
		dest.h=window.height();
		dest.w=dest.h * aspect;
	}
	dest.y=(window.height() - dest.h) / 2;
	dest.x=(window.width() - dest.w) / 2;
}

void Game::playIntroVideo()
{
	controlsEnabled=false;
	world_widget->setVisible(false);
	world_widget->setEnabled(false);
	ppl7::grafix::Color black(0, 0, 0, 255);
	SDL_Renderer* renderer=sdl.getRenderer();
	IntroScreen* intro_widget=new IntroScreen(0, 0, this->width(), this->height());
	VideoPlayer video(renderer);

	ppl7::String filename="res/video/george_decker_game-vp9.webm";
	AudioStream IntroSequence("res/audio/IntroSequence.mp3");
	IntroSequence.setVolume(1.0f);

	if (video.load(filename)) {
		this->addChild(intro_widget);
		wm->setKeyboardFocus(intro_widget);
		showUi(false);
		audiosystem.play(&IntroSequence);
		int frame=0;
		int fade_to_black=255;
		int fade_state=0;

		SDL_Rect dest;

		while (!intro_widget->stopSignal()) {
			wm->handleEvents();
			sdl.startFrame(black);
			ppl7::tk::MouseState mouse=wm->getMouseState();
			//drawWidgets();
			if (frame % 2 == 0) {
				if (!video.nextFrame()) break;
				if (fade_state == 0) {
					fade_to_black-=5;
					if (fade_to_black == 0) fade_state=1;
				} else if (fade_state == 1 && frame > 1573 * 2 && fade_to_black < 255) {
					fade_to_black+=5;
				}

			}
			getVideoDestination(video, *this, dest);
			video.renderFrame(&dest);
			resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
			FadeToBlack(renderer, fade_to_black);
			presentScreen();
			frame++;
		}
		this->removeChild(intro_widget);
		audiosystem.stop(&IntroSequence);
	}
	delete intro_widget;
}


IntroScreen::IntroScreen(int x, int y, int width, int height)
{
	stop_playback=false;
	create(x, y, width, height);
}

IntroScreen::~IntroScreen()
{

}




bool IntroScreen::stopSignal() const
{
	return stop_playback;
}

void IntroScreen::keyDownEvent(ppl7::tk::KeyEvent* event)
{
	if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE) {
		stop_playback=true;

	}
}
