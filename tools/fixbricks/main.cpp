#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>

#define TILE_WIDTH 32
#define TILE_HEIGHT 38


void fix(const ppl7::String &filename, int width, int height)
{
	ppl7::grafix::Image source;
	source.load(filename);
	int h=source.height();
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
			ppl7::grafix::Color c=source.getPixel(x,h-y);
			if (c.alpha()<255) {
				c.setAlpha(255);
				source.putPixel(x,h-y,c);
			}
		}
	}
	ppl7::grafix::ImageFilter_PNG png;
	png.saveFile(filename, source);
}

int main(int argc, char **argv)
{
	ppl7::grafix::Grafix Grafix;
	fix("lightwave/Render/bricks_white/frame_0017.png",TILE_WIDTH*1, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0018.png",TILE_WIDTH*2, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0019.png",TILE_WIDTH*3, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0020.png",TILE_WIDTH*4, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0021.png",TILE_WIDTH*6, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0022.png",TILE_WIDTH*8, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0023.png",TILE_WIDTH*2, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0024.png",TILE_WIDTH*2, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0025.png",TILE_WIDTH*1, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0026.png",TILE_WIDTH*1, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0027.png",TILE_WIDTH*2, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0028.png",TILE_WIDTH*2, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0029.png",TILE_WIDTH*2, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0030.png",TILE_WIDTH*2, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0031.png",TILE_WIDTH*2, TILE_HEIGHT);
	fix("lightwave/Render/bricks_white/frame_0032.png",TILE_WIDTH*2, TILE_HEIGHT);

	return 0;
}
