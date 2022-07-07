#ifndef INCLUDE_DECKER_SDL_H_
#define INCLUDE_DECKER_SDL_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>

class SDL
{
private:
	SDL_Renderer *renderer;
	bool screensaver_enabled;



public:
	class VideoDisplay {
		public:
			VideoDisplay();

			int id;
			ppl7::String name;
	};

	class DisplayMode {
		public:
			DisplayMode();
			ppl7::grafix::RGBFormat	format;
			int			width;
			int			height;
			int			refresh_rate;
	};


	SDL();
	~SDL();
	void setRenderer(SDL_Renderer *r);
	ppl7::grafix::Drawable lockTexture(SDL_Texture *texture);
	void unlockTexture(SDL_Texture *texture);
	SDL_Texture *createTexture(const ppl7::grafix::Drawable &d);
	SDL_Texture *createStreamingTexture(int width, int height);
	SDL_Texture *createRenderTargetTexture(int width, int height);
	void destroyTexture(SDL_Texture *texture);
	void startFrame(const ppl7::grafix::Color &background);
	SDL_Renderer *getRenderer();
	void present();

	ppl7::grafix::Size getWindowSize() const;
	ppl7::grafix::Rect getClientWindow() const;

	static ppl7::grafix::Size desktopResolution();
	static ppl7::grafix::RGBFormat desktopRGBFormat();
	static void getVideoDisplays(std::list<VideoDisplay> &display_list);
	static void getDisplayModes(std::list<DisplayMode> &mode_list, int display_id);

};

#endif