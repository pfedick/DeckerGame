#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

static ppl7::grafix::RGBFormat SDL2RGBFormat(const Uint32 f)
{
	switch (f) {
	case SDL_PIXELFORMAT_INDEX8: return ppl7::grafix::RGBFormat::Palette;
	case SDL_PIXELFORMAT_RGB332: return ppl7::grafix::RGBFormat::R3G3B2;
	case SDL_PIXELFORMAT_RGB444: return ppl7::grafix::RGBFormat::X4R4G4B4;
	case SDL_PIXELFORMAT_RGB555: return ppl7::grafix::RGBFormat::X1R5G5B5;
	case SDL_PIXELFORMAT_BGR555: return ppl7::grafix::RGBFormat::X1B5G5R5;
	case SDL_PIXELFORMAT_ARGB4444: return ppl7::grafix::RGBFormat::A4R4G4B4;
	case SDL_PIXELFORMAT_ABGR4444: return ppl7::grafix::RGBFormat::A4B4G4R4;
	case SDL_PIXELFORMAT_ARGB1555: return ppl7::grafix::RGBFormat::A1R5G5B5;
	case SDL_PIXELFORMAT_ABGR1555: return ppl7::grafix::RGBFormat::A1B5G5R5;
	case SDL_PIXELFORMAT_RGB565: return ppl7::grafix::RGBFormat::R5G6B5;
	case SDL_PIXELFORMAT_BGR565: return ppl7::grafix::RGBFormat::B5G6R5;
	case SDL_PIXELFORMAT_RGB24: return ppl7::grafix::RGBFormat::R8G8B8;
	case SDL_PIXELFORMAT_BGR24: return ppl7::grafix::RGBFormat::B8G8R8;
	case SDL_PIXELFORMAT_RGB888: return ppl7::grafix::RGBFormat::X8R8G8B8;
	case SDL_PIXELFORMAT_BGR888: return ppl7::grafix::RGBFormat::X8B8G8R8;
	case SDL_PIXELFORMAT_ARGB8888: return ppl7::grafix::RGBFormat::A8R8G8B8;
	case SDL_PIXELFORMAT_ABGR8888: return ppl7::grafix::RGBFormat::A8B8G8R8;
	}
	throw ppl7::grafix::UnsupportedColorFormatException();

	switch (f) {
	case SDL_PIXELFORMAT_RGB888: printf("SDL_PIXELFORMAT_RGB888\n"); break;
	case SDL_PIXELFORMAT_RGBX8888: printf("SDL_PIXELFORMAT_RGBX8888\n"); break;
	case SDL_PIXELFORMAT_BGR888: printf("SDL_PIXELFORMAT_BGR888\n"); break;
	case SDL_PIXELFORMAT_BGRX8888: printf("SDL_PIXELFORMAT_BGRX8888\n"); break;
	case SDL_PIXELFORMAT_ARGB8888: printf("SDL_PIXELFORMAT_ARGB8888\n"); break;
	case SDL_PIXELFORMAT_RGBA8888: printf("SDL_PIXELFORMAT_RGBA8888\n"); break;
	case SDL_PIXELFORMAT_ABGR8888: printf("SDL_PIXELFORMAT_ABGR8888\n"); break;
	case SDL_PIXELFORMAT_BGRA8888: printf("SDL_PIXELFORMAT_BGRA8888\n"); break;
	case SDL_PIXELFORMAT_ARGB2101010: printf("SDL_PIXELFORMAT_ARGB2101010\n"); break;
	default: printf("Verdammt!\n"); break;

	}

	throw ppl7::grafix::UnsupportedColorFormatException("format=%d", f);
}

static Uint32 RGBFormat2SDLFormat(const ppl7::grafix::RGBFormat& format)
{
	switch (format) {
	case ppl7::grafix::RGBFormat::Palette:
		return SDL_PIXELFORMAT_INDEX8;
	case ppl7::grafix::RGBFormat::A8R8G8B8:
		return SDL_PIXELFORMAT_ARGB8888;
	case ppl7::grafix::RGBFormat::X8R8G8B8:
		return SDL_PIXELFORMAT_ARGB8888;
	case ppl7::grafix::RGBFormat::A8B8G8R8:
		return SDL_PIXELFORMAT_ABGR8888;
	case ppl7::grafix::RGBFormat::X8B8G8R8:
		return SDL_PIXELFORMAT_ABGR8888;

	case ppl7::grafix::RGBFormat::R8G8B8:
		return SDL_PIXELFORMAT_RGB888;
	case ppl7::grafix::RGBFormat::B8G8R8:
		return SDL_PIXELFORMAT_BGR888;
	default:
		throw ppl7::grafix::UnsupportedColorFormatException();
	}
	throw ppl7::grafix::UnsupportedColorFormatException();
}


SDL::VideoDisplay::VideoDisplay(int id, const ppl7::String& name)
{
	this->id=id;
	this->name=name;
}


SDL::DisplayMode::DisplayMode()
{
	width=0;
	height=0;
	refresh_rate=0;
}

SDL::DisplayMode::DisplayMode(const ppl7::grafix::RGBFormat& format, int width, int height, int refresh_rate)
{
	this->format=format;
	this->width=width;
	this->height=height;
	this->refresh_rate=refresh_rate;
}


SDL::SDL()
{
	/*
	if (0!=SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)) {
		throw InitializationFailed("SDL");
	}
	*/
	//window=NULL;
	renderer=NULL;
	screensaver_enabled=SDL_IsScreenSaverEnabled();
	if (screensaver_enabled) {
		SDL_DisableScreenSaver();
	}
}


SDL::~SDL()
{
	//destroyWindow();
	if (screensaver_enabled) {
		SDL_EnableScreenSaver();
	}
	//SDL_Quit();
}

/*
void SDL::destroyWindow()
{
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
	renderer=NULL;
	window=NULL;
}

void SDL::createWindow()
{
	SDL_Rect desktop;
	SDL_GetDisplayBounds(0,&desktop);
	window=SDL_CreateWindow("Decker", 0, 0,
			desktop.w,
			desktop.h,
			SDL_WINDOW_FULLSCREEN);
	if (!window) {
		throw SDLException("Couldn't create window and renderer: %s", SDL_GetError());
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


}
*/

SDL_Texture* SDL::createStreamingTexture(int width, int height)
{
	SDL_Texture* texture;
	texture=SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width,
		height);
	if (!texture) {
		ppl7::String err(SDL_GetError());
		throw SDLException("Couldn't create texture: " + err);
	}
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	return texture;
}

SDL_Texture* SDL::createRenderTargetTexture(int width, int height)
{
	SDL_Texture* texture;
	texture=SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_TARGET,
		width,
		height);
	if (!texture) {
		ppl7::String err(SDL_GetError());
		throw SDLException("Couldn't create texture: " + err);
	}
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	return texture;
}



SDL_Texture* SDL::createTexture(const ppl7::grafix::Drawable& d)
{
	SDL_Surface* surface=SDL_CreateRGBSurfaceWithFormat(0,
		d.width(),
		d.height(),
		d.bitdepth(),
		RGBFormat2SDLFormat(d.rgbformat()));
	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
	SDL_LockSurface(surface);
	ppl7::grafix::Drawable s(surface->pixels, surface->pitch,
		surface->w, surface->h, SDL2RGBFormat(surface->format->format));
	s.blt(d);
	//s.line(0,0,1024,1024,0xffffffff);
	//s.drawRect(0,0,1024,1024,0xffffffff);
	SDL_UnlockSurface(surface);
	SDL_Texture* tex=SDL_CreateTextureFromSurface(renderer, surface);
	if (!tex) {
		ppl7::String err(SDL_GetError());
		SDL_FreeSurface(surface);
		throw SDLException("Couldn't create texture: " + err);
	}
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(surface);
	return tex;
}

void SDL::destroyTexture(SDL_Texture* texture)
{
	if (texture) SDL_DestroyTexture(texture);
}

ppl7::grafix::Size SDL::getWindowSize() const
{
	SDL_Rect desktop;
	SDL_GetDisplayBounds(0, &desktop);
	return ppl7::grafix::Size(desktop.w, desktop.h);
}

ppl7::grafix::Rect SDL::getClientWindow() const
{
	SDL_Rect desktop;
	SDL_GetDisplayBounds(0, &desktop);
	return ppl7::grafix::Rect(desktop.x, desktop.y, desktop.w, desktop.h);
}

void SDL::startFrame(const ppl7::grafix::Color& background)
{
	SDL_SetRenderDrawColor(renderer, background.red(), background.green(), background.blue(), 255);
	SDL_RenderClear(renderer);
}

void SDL::setRenderer(SDL_Renderer* r)
{
	renderer=r;
}

SDL_Renderer* SDL::getRenderer()
{
	return renderer;
}

void SDL::present()
{
	SDL_RenderPresent(renderer);
}

/*
void SDL::loop()
{
	SDL_Event event;
	while (1) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			break;
		}
		startFrame();
		present();
	}
}
*/


ppl7::grafix::Drawable SDL::lockTexture(SDL_Texture* texture)
{
	void* pixels;
	int pitch;
	Uint32 format;
	int access;
	int width, height;

	if (SDL_QueryTexture(texture, &format, &access, &width, &height)) {
		throw SDLException("Couldn't query GUI texture: %s", SDL_GetError());
	}
	if (SDL_LockTexture(texture, NULL, &pixels, &pitch)) {
		throw SDLException("Couldn't lock GUI texture: %s", SDL_GetError());
	}
	return ppl7::grafix::Drawable(pixels, pitch, width, height,
		SDL2RGBFormat(format));
}

void SDL::unlockTexture(SDL_Texture* texture)
{
	SDL_UnlockTexture(texture);
}


ppl7::grafix::Size SDL::desktopResolution(int display_id)
{
	SDL_DisplayMode mode;
	mode.driverdata=NULL;
	if (SDL_GetDesktopDisplayMode(display_id, &mode) == 0) {
		return ppl7::grafix::Size(mode.w, mode.h);
	}
	return ppl7::grafix::Size();
}

ppl7::grafix::RGBFormat SDL::desktopRGBFormat(int display_id)
{
	SDL_DisplayMode mode;
	mode.driverdata=NULL;
	if (SDL_GetDesktopDisplayMode(display_id, &mode) == 0) {
		return SDL2RGBFormat(mode.format);
	}
	return ppl7::grafix::RGBFormat();
}

void SDL::getVideoDisplays(std::list<VideoDisplay>& display_list)
{
	int num_displays=SDL_GetNumVideoDisplays();
	display_list.clear();
	for (int i=0;i < num_displays;i++) {
		display_list.push_back(SDL::VideoDisplay(i, ppl7::String(SDL_GetDisplayName(i))));
	}
}

void SDL::getDisplayModes(int display_id, std::list<DisplayMode>& mode_list)
{
	int numDispModes = SDL_GetNumDisplayModes(display_id);
	mode_list.clear();
	SDL_DisplayMode mode;
	for (int i=0;i < numDispModes;i++) {
		mode.driverdata=NULL;
		if (SDL_GetDisplayMode(display_id, i, &mode) == 0) {
			mode_list.push_back(SDL::DisplayMode(SDL2RGBFormat(mode.format),
				mode.w, mode.h, mode.refresh_rate));
		}
	}
}
