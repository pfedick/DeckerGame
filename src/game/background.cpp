#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "decker.h"


Background::Background(SDL& s)
    :sdl(s)
{
    tex_sky=NULL;
    fade_target_tex=NULL;
    fade_progress=0.0f;
}

void Background::clear()
{
    tex_sky=NULL;
    fade_target_tex=NULL;
    fade_progress=0.0f;
    last_image.clear();
    fade_target_image_filename.clear();

}

void Background::setImage(const ppl7::String& filename)
{
    if (filename == last_image && tex_sky != NULL) return;
    tex_sky=GetGame().texture_cache.get(filename);
    if (tex_sky) tex_size=sdl.getTextureSize(tex_sky);
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

void Background::setFadeTargetColor(const ppl7::grafix::Color& color)
{
    fade_target_color=color;
    fade_target_type=Type::Color;
    fade_progress=0.0f;
    //ppl7::PrintDebugTime("Background::setFadeTargetColor\n");
}

void Background::setFadeTargetImage(const ppl7::String& filename)
{
    if (filename == fade_target_image_filename && fade_target_tex != NULL) return;
    fade_target_tex=GetGame().texture_cache.get(filename);
    if (fade_target_tex) fade_tex_size=sdl.getTextureSize(fade_target_tex);
    fade_target_image_filename=filename;
    fade_target_type=Type::Image;
    fade_progress=0.0f;
}

void Background::setFadeProgress(float progress)
{
    if (progress >= 1.0f) {
        fade_progress=0.0f;
        tex_sky=fade_target_tex;
        tex_size=fade_tex_size;
        fade_target_tex=NULL;
        color=fade_target_color;
        t=fade_target_type;
        last_image=fade_target_image_filename;
        fade_target_image_filename.clear();
        return;
    }
    fade_progress=progress;
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
        SDL_SetTextureAlphaMod(tex_sky, 255);
        SDL_RenderCopy(renderer, tex_sky, &source, &target);
    } else {
        SDL_SetRenderDrawColor(renderer, 47, 47, 47, 255);
        SDL_RenderClear(renderer);
    }
    if (fade_progress > 0.0f) drawFade(renderer, viewport, WorldCoords);
}

void Background::drawFade(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& WorldCoords)
{

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    //ppl7::PrintDebugTime("progress: %0.3f, type=%d\n", fade_progress, static_cast<int>(fade_target_type));
    if (fade_target_type == Type::Color) {
        SDL_SetRenderDrawColor(renderer, fade_target_color.red(), fade_target_color.green(), fade_target_color.blue(), (Uint8)(255.0f * fade_progress));
        SDL_RenderFillRect(renderer, NULL);
        //SDL_RenderClear(renderer);
    } else if (fade_target_tex) {
        //ppl7::PrintDebugTime("progress: %0.3f, type=%d\n", fade_progress, static_cast<int>(fade_target_type));
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
        if (fade_tex_size.width > viewport.width()) c.x=origin.x * (fade_tex_size.width - viewport.width()) / level_size.width;
        if (fade_tex_size.height > viewport.height())c.y=origin.y * (fade_tex_size.height - viewport.height()) / level_size.height;
        //printf("ImagePos: %d:%d\n", c.x, c.y);
        source.x=c.x;
        source.y=c.y;
        source.w=viewport.width();
        source.h=viewport.height();
        //SDL_SetTextureBlendMode(fade_target_tex, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(fade_target_tex, (Uint8)(255.0f * fade_progress));
        SDL_RenderCopy(renderer, fade_target_tex, &source, &target);
    } else {
        //SDL_SetRenderDrawColor(renderer, 47, 47, 47, (Uint8)(255.0f * fade_progress));
        //SDL_RenderClear(renderer);
    }

}
