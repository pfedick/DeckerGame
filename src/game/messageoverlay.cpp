#include <stdio.h>
#include <ppl7.h>
#include "decker.h"


MessageOverlay::MessageOverlay(SDL& sdl)
	:sdl(sdl)
{
	overlay=NULL;
	timeout=0.0f;
	nextBlink=0.0f;
	nextPhonetic=0.0f;
	character=Character::George;
	mouth=8;
	eyes=0;
	phonetics_map.insert(std::pair<int, int>(' ', 8));
	phonetics_map.insert(std::pair<int, int>(',', 8));
	phonetics_map.insert(std::pair<int, int>('a', 0));
	phonetics_map.insert(std::pair<int, int>('b', 6));
	phonetics_map.insert(std::pair<int, int>('c', 3));
	phonetics_map.insert(std::pair<int, int>('d', 3));
	phonetics_map.insert(std::pair<int, int>('e', 2));
	phonetics_map.insert(std::pair<int, int>('f', 4));
	phonetics_map.insert(std::pair<int, int>('g', 5));
	phonetics_map.insert(std::pair<int, int>('h', -1));
	phonetics_map.insert(std::pair<int, int>('i', 0));
	phonetics_map.insert(std::pair<int, int>('j', 3));
	phonetics_map.insert(std::pair<int, int>('k', 3));
	phonetics_map.insert(std::pair<int, int>('l', 5));
	phonetics_map.insert(std::pair<int, int>('m', 6));
	phonetics_map.insert(std::pair<int, int>('n', 3));
	phonetics_map.insert(std::pair<int, int>('o', 7));
	phonetics_map.insert(std::pair<int, int>('p', 6));
	phonetics_map.insert(std::pair<int, int>('q', 10));
	phonetics_map.insert(std::pair<int, int>('r', 3));
	phonetics_map.insert(std::pair<int, int>('s', 3));
	phonetics_map.insert(std::pair<int, int>('t', 5));
	phonetics_map.insert(std::pair<int, int>('u', 9));
	phonetics_map.insert(std::pair<int, int>('v', 4));
	phonetics_map.insert(std::pair<int, int>('w', 10));
	phonetics_map.insert(std::pair<int, int>('x', -1));
	phonetics_map.insert(std::pair<int, int>('y', 3));
	phonetics_map.insert(std::pair<int, int>('z', 3));
	phonetics_map.insert(std::pair<int, int>('S', 18));

}


MessageOverlay::~MessageOverlay()
{
	if (overlay) sdl.destroyTexture(overlay);
	overlay=NULL;
}

void MessageOverlay::loadSprites()
{
	spriteset.load(sdl, "res/phonetics.tex");
}

void MessageOverlay::resize(const ppl7::grafix::Size& size)
{
	if (overlay) sdl.destroyTexture(overlay);
	overlay=sdl.createStreamingTexture(size.width - 100, 200);

	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	font=style.buttonFont;

	font.setName("NotoSansBlack");
	font.setBold(false);
	font.setSize(50);
	font.setOrientation(ppl7::grafix::Font::TOP);
	font.setColor(ppl7::grafix::Color(5, 1, 1, 255));
	//font.setBorderColor(ppl7::grafix::Color(0, 0, 0, 255));
	//font.setDrawBorder(true);

	/*
	font.setSize(30);

	*/
	//ppl7::PrintDebugTime("resize: %d,%d\n", size.width, size.height);


	lastSize=size;

}




void MessageOverlay::updatePhonetics()
{
	if (phonetics.isEmpty()) {
		mouth=8;
		return;
	}
	double time=ppl7::GetMicrotime();
	if (time < nextPhonetic) return;
	ppl7::String p=phonetics.left(1);
	phonetics.chopLeft();
	if (phonetics.isEmpty() && p != "-") phonetics="-";
	if (p == "-") {
		mouth=8;
	} else {
		std::map<int, int>::const_iterator it=phonetics_map.find(p[0]);
		if (it != phonetics_map.end() && (*it).second >= 0) mouth=(*it).second;

	}
	nextPhonetic=time + 0.1f;
}

void MessageOverlay::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport)
{
	if (!overlay) return;
	if (timeout < ppl7::GetMicrotime()) {
		timeout=0.0f;
		return;
	}
	updatePhonetics();

	if (viewport.size() != lastSize) {
		resize(viewport.size());
		render();
	}

	SDL_Rect target;
	target.x=viewport.x1 + 50;
	target.y=viewport.y1 + 10;
	target.w=lastSize.width - 100;
	target.h=200;
	//ppl7::PrintDebugTime("draw: %d:%d, %d:%d\n", target.x, target.y, target.w, target.h);
	//SDL_SetTextureColorMod(overlay, 255, 255, 255);
	//SDL_SetTextureAlphaMod(overlay, 255);

	SDL_RenderCopy(renderer, overlay, NULL, &target);
	// void SpriteTexture::draw(SDL_Renderer* renderer, int id, const SDL_Rect& source, const SDL_Rect& target) const
	SDL_Rect tr;
	SDL_Rect source;

	//ppl7::PrintDebugTime("eyes=%d, mouth=%d\n", eyes, mouth);
	// eyes
	source=spriteset.getSpriteSource(eyes);

	tr.x=viewport.x1 + 70;
	tr.y=viewport.y1 + 210 - 5 - source.h;
	source.h-=57;
	tr.w=source.w;
	tr.h=source.h;
	spriteset.draw(renderer, eyes, source, tr);

	// mouth
	source=spriteset.getSpriteSource(mouth);
	source.y+=source.h - 57;
	source.h=57;
	tr.y=viewport.y1 + 210 - 5 - 57;
	tr.w=source.w;
	tr.h=57;
	spriteset.draw(renderer, mouth, source, tr);
}

void MessageOverlay::render()
{
	if (!overlay) return;
	ppl7::grafix::Drawable draw=sdl.lockTexture(overlay);
	draw.cls(0);
	ppl7::grafix::Color border(255, 240, 0, 255);
	ppl7::grafix::Color background(192, 192, 192, 128);
	for (int i=0;i < 5;i++) {
		draw.drawRect(0 + i, 0 + i, draw.width() - i, draw.height() - i, border);
	}
	draw.fillRect(5, 5, draw.width() - 5, draw.height() - 5, background);
	ppl7::Array a(text, " ");
	int x=140;
	int y=10;
	for (size_t i=0;i < a.size();i++) {
		ppl7::String word=a[i];
		word+=" ";
		ppl7::grafix::Size size=font.measure(word);
		if (x + size.width > draw.width() - 20) {
			x=140;
			y+=font.size();
		}
		draw.print(font, x, y, word);
		x+=size.width;
	}
	sdl.unlockTexture(overlay);
}

void MessageOverlay::setText(Character c, const ppl7::String& text, const ppl7::String& phonetics, float timeout)
{
	if (timeout == 0.0f) {
		timeout=((float)text.size()) * 0.08f;
		if (timeout < 3.0f) timeout=3.0f;
	}
	this->timeout=ppl7::GetMicrotime() + timeout;
	this->text=text;
	this->phonetics=phonetics;
	if (phonetics.isEmpty()) this->phonetics=text;
	character=c;
	nextBlink=0.0f;
	nextPhonetic=0.0f;
	eyes=0;
	mouth=8;
	render();
}

bool MessageOverlay::hasMessage() const
{
	if (timeout == 0.0f) return false;
	return true;
}


void MessageOverlay::clear()
{
	timeout=0.0f;
	nextBlink=0.0f;
	nextPhonetic=0.0f;
}
