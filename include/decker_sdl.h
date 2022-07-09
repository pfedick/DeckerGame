#ifndef INCLUDE_DECKER_SDL_H_
#define INCLUDE_DECKER_SDL_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>

class SDL
{
private:
	SDL_Renderer* renderer;
	bool screensaver_enabled;



public:
	class VideoDisplay {
	public:
		VideoDisplay(int id, const ppl7::String& name);

		int id;
		ppl7::String name;
	};

	typedef ppl7::tk::Window::DisplayMode DisplayMode;

	SDL();
	~SDL();
	void setRenderer(SDL_Renderer* r);
	ppl7::grafix::Drawable lockTexture(SDL_Texture* texture);
	void unlockTexture(SDL_Texture* texture);
	SDL_Texture* createTexture(const ppl7::grafix::Drawable& d);
	SDL_Texture* createStreamingTexture(int width, int height);
	SDL_Texture* createStreamingTexture(const ppl7::String& filename);
	SDL_Texture* createRenderTargetTexture(int width, int height);
	void destroyTexture(SDL_Texture* texture);
	ppl7::grafix::Size getTextureSize(SDL_Texture* texture);
	void startFrame(const ppl7::grafix::Color& background);
	SDL_Renderer* getRenderer();
	void present();

	ppl7::grafix::Size getDisplaySize(int display_no=0) const;
	ppl7::grafix::Rect getDisplayWindow(int display_no=0) const;

	static DisplayMode desktopDisplayMode(int display_id=0);
	static void getVideoDisplays(std::list<VideoDisplay>& display_list);
	static void getDisplayModes(int display_id, std::list<DisplayMode>& mode_list);

};

#endif
