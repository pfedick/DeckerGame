#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "decker.h"


Background::Background(SDL& s)
    :sdl(s)
{
    tex_sky=NULL;
}

Background::~Background()
{
    if (tex_sky) {
        sdl.destroyTexture(tex_sky);
        tex_sky=NULL;
    }
}

void Background::setImage(const ppl7::String& filename)
{
    if (filename == last_image) return;
    if (tex_sky) {
        sdl.destroyTexture(tex_sky);
        tex_sky=NULL;
    }
    if (filename.notEmpty() && ppl7::File::exists(filename)) {
        tex_sky=sdl.createStreamingTexture(filename);
    }
    last_image=filename;
}

void Background::setColor(const ppl7::grafix::Color& color)
{
    this->color=color;
}

void Background::setBackgroundType(Type t)
{
    this->t=t;
}

void Background::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& WorldCoords)
{
    if (t == Type::Color) {
        SDL_SetRenderDrawColor(renderer, color.red(), color.green(), color.blue(), 255);
        SDL_RenderClear(renderer);
    } else if (tex_sky) {
        SDL_Rect target;
        target.x=viewport.x1;
        target.y=viewport.y1;
        target.w=viewport.width();
        target.h=viewport.height();
        SDL_Rect source;
        ppl7::grafix::Point c=WorldCoords * 0.1f;
        source.x=c.x;
        source.y=c.y;
        source.w=viewport.width();
        source.h=viewport.height();
        SDL_RenderCopy(renderer, tex_sky, &source, &target);
    } else {
        SDL_SetRenderDrawColor(renderer, 47, 47, 47, 255);
        SDL_RenderClear(renderer);
    }
}
