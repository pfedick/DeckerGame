#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

Level::Level()
{
	renderer=NULL;

}

Level::~Level()
{
	clear();
}

void Level::clear()
{

}

void Level::create(int width, int height, int layers)
{
	clear();

}

void Level::load(const ppl7::String &Filename)
{

}

void Level::save(const ppl7::String &Filename)
{

}

void Level::setRenderer(SDL_Renderer *renderer)
{
	this->renderer=renderer;
}


