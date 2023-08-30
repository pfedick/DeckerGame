#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"
#include "videoplayer.h"



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


static void getDestinationRect(ppl7::grafix::Size img_size, ppl7::tk::Window& window, SDL_Rect& dest)
{
	float aspect=(float)img_size.width / (float)img_size.height;
	dest.x=0;
	dest.y=0;
	dest.w=img_size.width;
	dest.h=img_size.height;
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


	ppl7::String filename="res/video/george_decker_game.ivf";
	AudioStream IntroSequence("res/audio/IntroSequence.mp3", AudioClass::Music);
	SDL_Texture* title_tex=sdl.createStreamingTexture("res/game_title.png");


	ppl7::grafix::Size title_size=sdl.getTextureSize(title_tex);
	SDL_Rect title_rect;
	getDestinationRect(title_size, *this, title_rect);
	float title_blend=0.0f;

	if (video.load(filename)) {
		this->addChild(intro_widget);
		wm->setKeyboardFocus(intro_widget);
		showUi(false);
		audiosystem.play(&IntroSequence);

		audiosystem.setVolume(AudioClass::Music, 0.8f);
		int frame=0;
		float fade_to_black=255;
		int fade_state=0;

		SDL_Rect dest;
		double next_video_frame=0.0f;
		double video_frametime=1.0f / (double)video.framerate();

		while (1) {
			if ((quitGame || intro_widget->stopSignal()) && fade_state != 3) {
				fade_state=3;
				IntroSequence.fadeout(2.0f);
				if (quitGame) gameState = GameState::QuitGame;
			}
			double now=ppl7::GetMicrotime();
			frame_rate_compensation=1.0f;
			if (last_frame_time > 0.0f) {
				float frametime=now - last_frame_time;
				frame_rate_compensation=frametime / (1.0f / 60.0f);
				if (frame_rate_compensation > 2.0f) frame_rate_compensation=2.0f;
			}
			last_frame_time=now;


			if (next_video_frame == 0.0f) next_video_frame=now;
			wm->handleEvents();
			sdl.startFrame(black);
			ppl7::tk::MouseState mouse=wm->getMouseState();
			//drawWidgets();
			if (next_video_frame <= now) {
				frame++;
				next_video_frame+=video_frametime;
				if (!video.nextFrame()) break;
			}

			if (fade_state == 0) {
				fade_to_black-=5 * frame_rate_compensation;
				if (fade_to_black <= 0) {
					fade_state=1;
					fade_to_black=0.0f;
				}
			}

			getVideoDestination(video, *this, dest);
			video.renderFrame(&dest);
			FadeToBlack(renderer, (int)fade_to_black);

			if (frame > 1700) {
				if (title_blend < 255.0f) title_blend+=8 * frame_rate_compensation;
				if (title_blend > 255.0f) title_blend=255.0f;
				SDL_SetTextureAlphaMod(title_tex, title_blend);
				getDestinationRect(title_size, *this, title_rect);
				title_rect.y=0;
				SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);
			}
			if (frame > 1800 && fade_to_black < 255.0f) {
				fade_to_black+=4 * frame_rate_compensation;
				if (fade_to_black > 255.0f) fade_to_black=255.0f;
			} else if (fade_state == 3) {
				fade_to_black+=4 * frame_rate_compensation;
				if (fade_to_black > 255.0f) {
					fade_to_black=255.0f;
					break;
				}
			}

			resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);

			presentScreen();

		}
	}
	this->removeChild(intro_widget);
	audiosystem.stop(&IntroSequence);
	sdl.destroyTexture(title_tex);
	delete intro_widget;
	audiosystem.setVolume(AudioClass::Music, config.volumeMusic);
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
	if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE || event->key == ppl7::tk::KeyEvent::KEY_SPACE) {
		stop_playback=true;

	}
}
