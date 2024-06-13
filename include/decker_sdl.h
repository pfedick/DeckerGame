#ifndef INCLUDE_DECKER_SDL_H_
#define INCLUDE_DECKER_SDL_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppltk.h>

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

	typedef ppltk::Window::DisplayMode DisplayMode;

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

	static SDL_Texture* createTexture(SDL_Renderer* renderer, const ppl7::grafix::Drawable& d);

};


class SDLFont
{
private:
	enum class FontFlags {
		bold=1,
		drawBorder=2,
		monospace=4
	};

	class Font6Glyph
	{
	public:
		Font6Glyph();
		int width;
		int height;
		int bearingX;
		int bearingY;
		int advance;
		//const char* bitmap;
		int sprite_no;
		std::map<wchar_t, int> Hints;
		int getHint(wchar_t nextGlyph) const;
	};

	class Font6Face
	{
	public:
		int	Flags;
		int Pixelformat;
		int Size;
		int MaxBearingY;
		int MaxHeight;
		int Underscore;
		std::map<wchar_t, Font6Glyph> Glyphs;
		const Font6Glyph* getGlyph(wchar_t code) const;
	};

	class SpriteIndexItem
	{
	public:
		int id;
		int textureId;
		SDL_Texture* tex;
		SDL_Rect r;
		ppl7::grafix::Point Pivot;
		ppl7::grafix::Point Offset;

		SpriteIndexItem()
		{
			id=0;
			textureId=0;
			tex=NULL;
		}
		SpriteIndexItem(const SpriteIndexItem& other)
			:r(other.r), Pivot(other.Pivot), Offset(other.Offset)
		{
			id=other.id;
			textureId=other.textureId;
			tex=other.tex;
		}
	};


	std::map<int, SDL_Texture*> TextureMap;
	std::map<int, SpriteIndexItem> SpriteMap;


	uint8_t				flags;
	uint16_t			fontSize;
	ppl7::grafix::Color	cForeground;
	ppl7::grafix::Color	cBorder;
	float rotation;

public:
	SDLFont();
	~SDLFont();
	void clear();
	void load(SDL_Renderer* renderer, const ppl7::String& filename);
	void load(SDL_Renderer* renderer, ppl7::FileObject& file);


	void setBold(bool enable);
	void setSize(int size);
	void setDrawBorder(bool enable);
	void setMonospace(bool enable);
	void setColor(const ppl7::grafix::Color& color);
	void setBorderColor(const ppl7::grafix::Color& color);
	void setRotation(float degrees);

	ppl7::grafix::Size measure(const ppl7::WideString& text) const;
	ppl7::grafix::Rect boundary(const ppl7::WideString& text, int x=0, int y=0) const;

	void print(SDL_Renderer* renderer, int x, int y, const ppl7::String& text) const;
	void printf(SDL_Renderer* renderer, int x, int y, const char* fmt, ...) const;

};

#endif
