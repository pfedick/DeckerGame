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
        tex_size=sdl.getTextureSize(tex_sky);
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

void Background::setLevelDimension(const ppl7::grafix::Rect& tiles)
{
    level_dimension.x1=tiles.x1 * TILE_WIDTH;
    level_dimension.x2=tiles.x2 * TILE_WIDTH;
    level_dimension.y1=tiles.y1 * TILE_HEIGHT;
    level_dimension.y2=tiles.y2 * TILE_HEIGHT;
    /*
    printf("Level dimension: (%d:%d)-(%d:%d)\n", level_dimension.x1, level_dimension.y1,
        level_dimension.x2, level_dimension.y2);
        */

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
        ppl7::grafix::Point origin=WorldCoords - level_dimension.topLeft();
        ppl7::grafix::Size level_size=level_dimension.size();

        //printf("Level-size: %d:%d, Position=%d:%d, ", level_size.width, level_size.height, origin.x, origin.y);
        //printf("Image: %d,%d", tex_size.width, tex_size.height);

        ppl7::grafix::Point c;
        if (tex_size.width > viewport.width()) c.x=origin.x * (tex_size.width - viewport.width()) / level_size.width;
        if (tex_size.height > viewport.height())c.y=origin.y * (tex_size.height - viewport.height()) / level_size.height;
        //printf("ImagePos: %d:%d\n", c.x, c.y);
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
