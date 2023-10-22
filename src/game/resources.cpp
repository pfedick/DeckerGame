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
	background_images.push_back(ppl7::String("res/backgrounds/sky2.png"));
	background_images.push_back(ppl7::String("res/backgrounds/Cloudy_sky1.jpg"));
	background_images.push_back(ppl7::String("res/backgrounds/sunset-sky-1455125487HWs.jpg"));
	background_images.push_back(ppl7::String("res/backgrounds/IMG_20220726_125250.jpg"));
	background_images.push_back(ppl7::String("res/backgrounds/IMG_20220726_125308.jpg"));
	background_images.push_back(ppl7::String("res/backgrounds/night1.jpg"));

}

int Resources::getMaxTilesetId() const
{
	return max_tileset_id;
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


}
