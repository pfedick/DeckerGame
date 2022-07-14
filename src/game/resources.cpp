#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>


Resources::BrickResource::BrickResource()
{
	ldraw_material=-1;
}

static Resources* resources=NULL;

Resources& getResources()
{
	if (!resources) throw ResourcesInitialized();
	return *resources;
}

Resources::Resources()
{
	if (!resources) resources=this;
	max_tileset_id=0;
	background_images.push_back(ppl7::String("res/sky2.png"));
	background_images.push_back(ppl7::String("res/Cloudy_sky1.jpg"));
	background_images.push_back(ppl7::String("res/sunset-sky-1455125487HWs.jpg"));

}

int Resources::getMaxTilesetId() const
{
	return max_tileset_id;
}

void Resources::loadBricks(SDL& sdl, int tileset, const ppl7::String& name, int ldraw_material, const ppl7::grafix::Color& tint)
{
	if (tileset >= MAX_TILESETS) return;
	bricks[tileset].name=name;
	bricks[tileset].ldraw_material=ldraw_material;
	bricks[tileset].world.enableOutlines(false);
	bricks[tileset].world.enableMemoryBuffer(false);
	bricks[tileset].world.load(sdl, "res/bricks_white.tex", tint);
	bricks[tileset].ui.enableSDLBuffer(false);
	bricks[tileset].ui.enableMemoryBuffer(true);
	bricks[tileset].ui.load(sdl, "res/bricks_white_ui.tex", tint);
	if (tileset > max_tileset_id) max_tileset_id=tileset;
}

void Resources::loadBricks(SDL& sdl)
{
	bricks[1].name="Solid Colors";
	bricks[1].world.load(sdl, "res/bricks_solid.tex");
	bricks[1].ui.enableMemoryBuffer(true);
	bricks[1].ui.load(sdl, "res/bricks_solid_ui.tex");

	bricks[2].name="Default Bricks";
	bricks[2].ldraw_material=15;
	bricks[2].world.enableOutlines(true);
	bricks[2].world.enableMemoryBuffer(true);
	bricks[2].world.load(sdl, "res/bricks_white.tex");
	bricks[2].ui.enableSDLBuffer(false);
	bricks[2].ui.enableMemoryBuffer(true);
	bricks[2].ui.load(sdl, "res/bricks_white_ui.tex");
	max_tileset_id=2;


	/*
	loadBricks(sdl, 3, "Medium Stone Grey", 71, ppl7::grafix::Color(0xa0, 0xa5, 0xa9, 255));
	loadBricks(sdl, 4, "Dark Stone Grey", 72, ppl7::grafix::Color(0x6c, 0x6e, 0x68, 255));
	loadBricks(sdl, 5, "Green", 2, ppl7::grafix::Color(0x23, 0x78, 0x41, 255));
	loadBricks(sdl, 6, "Red", 4, ppl7::grafix::Color(161, 21, 7, 255));
	loadBricks(sdl, 7, "Yellow", 3, ppl7::grafix::Color(0xf2, 0xcd, 0x37, 0xff));
	loadBricks(sdl, 8, "Blue", 1, ppl7::grafix::Color(0x00, 0x55, 0xbf, 0xff));
	loadBricks(sdl, 9, "Black", 1, ppl7::grafix::Color(14, 25, 33, 0xff));
	loadBricks(sdl, 10, "Dark Blue", 63, ppl7::grafix::Color(0x0a, 0x34, 0x63, 0xff));
	loadBricks(sdl, 11, "Medium Blue", 43, ppl7::grafix::Color(0x5a, 0x93, 0xdb, 0xff));
	loadBricks(sdl, 12, "Brown", 6, ppl7::grafix::Color(0x58, 0x39, 0x27, 0xff));
	loadBricks(sdl, 13, "Reddish Brown", 70, ppl7::grafix::Color(0x58, 0x2a, 0x12, 0xff));
	loadBricks(sdl, 14, "Orange", 25, ppl7::grafix::Color(0xfe, 0x8a, 0x18, 0xff));
	loadBricks(sdl, 15, "light Orange", 125, ppl7::grafix::Color(0xfa, 0x9c, 0x1c, 0xff));
	loadBricks(sdl, 16, "Dark Tan", 28, ppl7::grafix::Color(0x95, 0x8a, 0x73, 0xff));
	loadBricks(sdl, 17, "Tan", 19, ppl7::grafix::Color(0xe4, 0xcd, 0x9e, 0xff));
	loadBricks(sdl, 18, "Bright Green", 10, ppl7::grafix::Color(0x4b, 0x9f, 0x4a, 0xff));
	loadBricks(sdl, 19, "Bright Yellow Green (Lime)", 27, ppl7::grafix::Color(0xbb, 0xe9, 0x0b, 0xff));
	loadBricks(sdl, 20, "Rust", 216, ppl7::grafix::Color(0xb3, 0x10, 0x04, 0xff));
	loadBricks(sdl, 21, "Dark Red", 320, ppl7::grafix::Color(0x72, 0x0e, 0x0f, 0xff));
	loadBricks(sdl, 22, "Dark Green", 320, ppl7::grafix::Color(0x18, 0x46, 0x32, 0xff));
	loadBricks(sdl, 23, "Brick White", 15, ppl7::grafix::Color(204, 204, 204, 255));
	loadBricks(sdl, 24, "Darker Grey", 0, ppl7::grafix::Color(0x4c, 0x4e, 0x48, 255));
	*/



}
