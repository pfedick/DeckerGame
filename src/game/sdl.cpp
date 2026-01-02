#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <ppl7-grafix.h>

// Map SDL pixel format to ppl7::grafix format
static ppl7::grafix::RGBFormat SDL2RGBFormat(const SDL_PixelFormat  f)
{
	switch (f) {
	case SDL_PIXELFORMAT_INDEX8: return ppl7::grafix::RGBFormat::Palette;
	case SDL_PIXELFORMAT_RGB332: return ppl7::grafix::RGBFormat::R3G3B2;
	case SDL_PIXELFORMAT_XRGB4444: return ppl7::grafix::RGBFormat::X4R4G4B4;
	case SDL_PIXELFORMAT_XRGB1555: return ppl7::grafix::RGBFormat::X1R5G5B5;
	case SDL_PIXELFORMAT_XBGR1555: return ppl7::grafix::RGBFormat::X1B5G5R5;
	case SDL_PIXELFORMAT_ARGB4444: return ppl7::grafix::RGBFormat::A4R4G4B4;
	case SDL_PIXELFORMAT_ABGR4444: return ppl7::grafix::RGBFormat::A4B4G4R4;
	case SDL_PIXELFORMAT_ARGB1555: return ppl7::grafix::RGBFormat::A1R5G5B5;
	case SDL_PIXELFORMAT_ABGR1555: return ppl7::grafix::RGBFormat::A1B5G5R5;
	case SDL_PIXELFORMAT_RGB565: return ppl7::grafix::RGBFormat::R5G6B5;
	case SDL_PIXELFORMAT_BGR565: return ppl7::grafix::RGBFormat::B5G6R5;
	case SDL_PIXELFORMAT_RGB24: return ppl7::grafix::RGBFormat::R8G8B8;
	case SDL_PIXELFORMAT_BGR24: return ppl7::grafix::RGBFormat::B8G8R8;
	case SDL_PIXELFORMAT_XRGB8888: return ppl7::grafix::RGBFormat::X8R8G8B8;
	case SDL_PIXELFORMAT_XBGR8888: return ppl7::grafix::RGBFormat::X8B8G8R8;
	case SDL_PIXELFORMAT_ARGB8888: return ppl7::grafix::RGBFormat::A8R8G8B8;
	case SDL_PIXELFORMAT_ABGR8888: return ppl7::grafix::RGBFormat::A8B8G8R8;
	default: break;
	}
	throw ppl7::grafix::UnsupportedColorFormatException("format=%d", (int)f);
}

// Map ppl7::grafix format back to SDL pixel format
static SDL_PixelFormat RGBFormat2SDLFormat(const ppl7::grafix::RGBFormat& format)
{
	switch (format) {
	case ppl7::grafix::RGBFormat::Palette:
		return SDL_PIXELFORMAT_INDEX8;
	case ppl7::grafix::RGBFormat::A8R8G8B8:
		return SDL_PIXELFORMAT_ARGB8888;
	case ppl7::grafix::RGBFormat::X8R8G8B8:
		return SDL_PIXELFORMAT_XRGB8888;
	case ppl7::grafix::RGBFormat::A8B8G8R8:
		return SDL_PIXELFORMAT_ABGR8888;
	case ppl7::grafix::RGBFormat::X8B8G8R8:
		return SDL_PIXELFORMAT_XBGR8888;
	case ppl7::grafix::RGBFormat::R8G8B8:
		return SDL_PIXELFORMAT_XRGB8888;
	case ppl7::grafix::RGBFormat::B8G8R8:
		return SDL_PIXELFORMAT_XBGR8888;
	default:
		throw ppl7::grafix::UnsupportedColorFormatException();
	}
	throw ppl7::grafix::UnsupportedColorFormatException();
}


SDL::VideoDisplay::VideoDisplay(int id, const ppl7::String& name)
{
	this->id = id;
	this->name = name;
}



SDL::SDL()
{
	renderer = NULL;
	gpu = NULL;
	screensaver_enabled = SDL_ScreenSaverEnabled();
	if (screensaver_enabled) {
		SDL_DisableScreenSaver();
	}
}


SDL::~SDL()
{
	if (screensaver_enabled) {
		SDL_EnableScreenSaver();
	}
}

SDL_Texture* SDL::createStreamingTexture(int width, int height)
{
	SDL_Texture* texture;
	texture = SDL_CreateTexture(renderer,
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

SDL_Texture* SDL::createStreamingTexture(const ppl7::String& filename)
{
	ppl7::grafix::Image img;
	img.load(filename);
	SDL_Texture* tex = createStreamingTexture(img.width(), img.height());
	ppl7::grafix::Drawable draw = lockTexture(tex);
	draw.blt(img);
	unlockTexture(tex);
	return tex;
}

SDL_Texture* SDL::createRenderTargetTexture(int width, int height)
{
	SDL_Texture* texture;
	texture = SDL_CreateTexture(renderer,
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

SDL_Texture* SDL::createTexture(SDL_Renderer* renderer, const ppl7::grafix::Drawable& d)
{
	SDL_Surface* surface = SDL_CreateSurface(
		d.width(),
		d.height(),
		RGBFormat2SDLFormat(d.rgbformat()));
	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
	SDL_LockSurface(surface);
	ppl7::grafix::Drawable s(surface->pixels, surface->pitch,
		surface->w, surface->h, SDL2RGBFormat(surface->format));
	s.blt(d);
	//s.line(0,0,1024,1024,0xffffffff);
	//s.drawRect(0,0,1024,1024,0xffffffff);
	SDL_UnlockSurface(surface);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
	if (!tex) {
		ppl7::String err(SDL_GetError());
		SDL_DestroySurface(surface);
		throw SDLException("Couldn't create texture: " + err);
	}
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	SDL_DestroySurface(surface);
	return tex;
}

SDL_Texture* SDL::createTexture(const ppl7::grafix::Drawable& d)
{
	return SDL::createTexture(renderer, d);
}

void SDL::destroyTexture(SDL_Texture* texture)
{
	if (texture) SDL_DestroyTexture(texture);
}

// Query display size using SDL3 display list API
ppl7::grafix::Size SDL::getDisplaySize(int display_no) const
{
	int count = 0;
	SDL_DisplayID* displays = SDL_GetDisplays(&count);
	if (!displays || display_no < 0 || display_no >= count) {
		if (displays) SDL_free(displays);
		throw SDLException("Couldn't query displays (index %d)", display_no);
	}
	SDL_DisplayID display = displays[display_no];
	SDL_free(displays);

	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
	if (mode) {
		return ppl7::grafix::Size(mode->w, mode->h);
	}
	ppl7::String err(SDL_GetError());
	throw SDLException("Couldn't determine display size for display %d: %s", display_no, (const char*)err);
}

// Query display bounds using SDL3 display ID
ppl7::grafix::Rect SDL::getDisplayWindow(int display_no) const
{
	int count = 0;
	SDL_DisplayID* displays = SDL_GetDisplays(&count);
	if (!displays || display_no < 0 || display_no >= count) {
		if (displays) SDL_free(displays);
		throw SDLException("Couldn't query displays (index %d)", display_no);
	}
	SDL_DisplayID display = displays[display_no];
	SDL_free(displays);

	SDL_Rect desktop;
	if (SDL_GetDisplayBounds(display, &desktop) == 0) {
		return ppl7::grafix::Rect(desktop.x, desktop.y, desktop.w, desktop.h);
	}
	ppl7::String err(SDL_GetError());
	throw SDLException("Couldn't determine display bounds for display %d: %s", display_no, (const char*)err);
}

void SDL::startFrame(const ppl7::grafix::Color& background)
{
	SDL_SetRenderDrawColor(renderer, background.red(), background.green(), background.blue(), 255);
	SDL_RenderClear(renderer);
}

void SDL::setRenderer(SDL_Renderer* r)
{
	renderer = r;
}

SDL_Renderer* SDL::getRenderer()
{
	return renderer;
}

void SDL::setGPUDevice(SDL_GPUDevice* device)
{
	gpu = device;
}
SDL_GPUDevice* SDL::getGPUDevice()
{
	return gpu;
}

void SDL::present()
{
	SDL_RenderPresent(renderer);
}

ppl7::grafix::Drawable SDL::lockTexture(SDL_Texture* texture)
{
	void* pixels;
	int pitch;
	if (!SDL_LockTexture(texture, NULL, &pixels, &pitch)) {
		//ppl7::PrintDebug("SDL_LockTexture failed: %s\n", SDL_GetError());
		throw SDLException("Couldn't lock texture: %s", SDL_GetError());
	}
	return ppl7::grafix::Drawable(pixels, pitch, texture->w, texture->h, SDL2RGBFormat(texture->format));
}

void SDL::unlockTexture(SDL_Texture* texture)
{
	SDL_UnlockTexture(texture);
}

ppl7::grafix::Size SDL::getTextureSize(SDL_Texture* texture)
{
	if (!texture) {
		throw SDLException("Couldn't get texture size as texture points to NULL");
	}
	return ppl7::grafix::Size(texture->w, texture->h);
}


// Current desktop display mode via SDL3 display ID
SDL::DisplayMode SDL::desktopDisplayMode(int display_id)
{
	int count = 0;
	SDL_DisplayID* displays = SDL_GetDisplays(&count);
	if (!displays || display_id < 0 || display_id >= count) {
		if (displays) SDL_free(displays);
		throw SDLException("Couldn't query displays (index %d)", display_id);
	}
	SDL_DisplayID display = displays[display_id];
	SDL_free(displays);

	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
	if (mode) {
		return SDL::DisplayMode(SDL2RGBFormat(mode->format),
			mode->w, mode->h, static_cast<int>(mode->refresh_rate));
	}
	ppl7::String err(SDL_GetError());
	throw SDLException("Couldn't determine display mode for display %d: %s", display_id, (const char*)err);
}

void SDL::getVideoDisplays(std::list<VideoDisplay>& display_list)
{
	int count = 0;
	SDL_DisplayID* displays = SDL_GetDisplays(&count);
	display_list.clear();
	if (!displays) return;
	for (int i = 0;i < count;i++) {
		const char* name = SDL_GetDisplayName(displays[i]);
		display_list.push_back(SDL::VideoDisplay(i, ppl7::String(name ? name : "")));
	}
	SDL_free(displays);
}

void SDL::getDisplayModes(int display_id, std::list<DisplayMode>& mode_list)
{
	int count = 0;
	SDL_DisplayID* displays = SDL_GetDisplays(&count);
	mode_list.clear();
	if (!displays || display_id < 0 || display_id >= count) {
		if (displays) SDL_free(displays);
		return;
	}
	SDL_DisplayID display = displays[display_id];
	SDL_free(displays);

	int modesCount = 0;
	SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(display, &modesCount);
	for (int i = 0; modes && i < modesCount; i++) {
		const SDL_DisplayMode* m = modes[i];
		mode_list.push_back(SDL::DisplayMode(SDL2RGBFormat(m->format),
			m->w, m->h, static_cast<int>(m->refresh_rate)));
	}
	if (modes) SDL_free(modes);
}



SDL_GPUTexture* SDL::createGPUTexture(const ppl7::grafix::Drawable& surface)
{
	// Textur-Beschreibung
	SDL_GPUTextureCreateInfo texture_info = {
	.type = SDL_GPU_TEXTURETYPE_2D,
	.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,  // Filterbar, Sampling-fähig
	.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,  // Für Shader-Sampling
	.width = (Uint32)surface.width(),
	.height = (Uint32)surface.height(),
	};
	// Textur erstellen
	SDL_GPUTexture* texture = SDL_CreateGPUTexture(gpu, &texture_info);
	if (!texture) {
		throw SDLException("SDL_CreateGPUTexture failed: %s", SDL_GetError());
	}
	// Daten in GPU hochladen
	SDL_GPUTransferBufferCreateInfo  transfer_info = {
		.size = (Uint32)surface.width() * (Uint32)surface.height() * 4,  // RGBA8 = 4 Bytes/Pixel
	};
	SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(gpu, &transfer_info);
	if (!transfer_buffer) {
		SDL_ReleaseGPUTexture(gpu, texture);
		throw SDLException("SDL_CreateGPUTransferBuffer failed: %s", SDL_GetError());
	}

	// Pixel-Daten kopieren
	void* mapped = SDL_MapGPUTransferBuffer(gpu, transfer_buffer, false);
	memcpy(mapped, surface.adr(), surface.width() * surface.height() * 4);
	SDL_UnmapGPUTransferBuffer(gpu, transfer_buffer);

	// Mit Command Buffer zur GPU transferieren
	SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpu);
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);
	SDL_GPUTextureTransferInfo transfer_region = {
		.transfer_buffer = transfer_buffer,
		.offset = 0
	};
	SDL_GPUTextureRegion texture_region = {
		.texture = texture,
		.mip_level = 0,
		.layer = 0,
		.x = 0,
		.y = 0,
		.z = 0,
		.w = (Uint32)surface.width(),
		.h = (Uint32)surface.height(),
		.d = 1
	};
	SDL_UploadToGPUTexture(copy_pass, &transfer_region, &texture_region, false);
	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(cmd);

	SDL_ReleaseGPUTransferBuffer(gpu, transfer_buffer);
	return texture;
}

void SDL::destroyGPUTexture(SDL_GPUTexture* texture)
{
	if (texture) {
		SDL_ReleaseGPUTexture(gpu, texture);
	}
}


