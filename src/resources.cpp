#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>

/*
 *
	// https://rebrickable.com/colors/
	// Black: 4,15,23 (Mat 0)
	// Blue: 0,68,153 (Mat 1)
	// Green: 30,98,50 (Mat 2)
	// Turkise: 0,105,114 (Mat 3)
	// Red: 161,21,7 (Mat 4)
	// Pink: 160,90,128 (Mat 5)
	// Brown: 70,46,31 (Mat 6)
	// Grey: 124,129,126 (Mat 7)
	// Tan: 87,88,74 (Mat 8)
	// Bright Light Blue: 144,168,182 (Mat 9)
	// Bright Green: 60,127,59 (Mat 10)
	// Light Red: 194, 90, 75 (Mat 12)
	// Light Pink: 202,121,138 (Mat 13)
	// Yellow: 194,164,44 (Mat 14)
	// White: 204,204,204 (Mat 15)
	// Light Yellow: 201,18,120 (Mat 18)
	// Light Tan: 182,164,126 (Mat 19)
	// Dark Blue: 26,40,141 (Mat 23)
	// Orange: 203, 110,19 (Mat 25)
	// Bright Yello Green: 150,186,9 (Mat 27)
	// Dark Tan: 119,110,92 (Mat 28)
	// Pearl_White: f2f3f2 (Mat 183)



	// Dark Green: 0x18,0x46,0x32
	// Dark Blue: 0a3463

	// Medium Blue: 5a93db
	// Bright Green: 4b9f4a
	// Sand Green: a0bcac
	// Light Yellow: fbe696
	// Yellow: f2cd37
	// Bright Light Orange: f8bb3d
	// Medium Orange: ffa70b
	// Tan: e4cd9e
	// Earth Orange: fa9c1c
	// Light Orange: f9ba61
	// Orange: fe8a18
	// Dark Tan: 958a73
	// Brown: 583927
	// Reddish Brown: 582a12
	// Dark Red: 720e0f
	//
 */

Resources::BrickResource::BrickResource()
{
	ldraw_material=-1;
}

Resources::Resources()
{
	max_tileset_id=0;
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

	whitebricks_world.enableOutlines(true);
	whitebricks_world.enableMemoryBuffer(true);
	whitebricks_world.load(sdl, "res/bricks_white.tex");
	whitebricks_ui.enableSDLBuffer(false);
	whitebricks_ui.enableMemoryBuffer(true);
	whitebricks_ui.load(sdl, "res/bricks_white_ui.tex");


	bricks[2].name="White";
	bricks[2].ldraw_material=15;
	bricks[2].world.enableOutlines(true);
	bricks[2].world.enableMemoryBuffer(true);
	bricks[2].world.load(sdl, "res/bricks_white.tex");
	bricks[2].ui.enableSDLBuffer(false);
	bricks[2].ui.enableMemoryBuffer(true);
	bricks[2].ui.load(sdl, "res/bricks_white_ui.tex");
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






}
