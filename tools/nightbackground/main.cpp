#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <math.h>


int main(int argc, char** argv)
{
	ppl7::grafix::Grafix gfx;
	int width=1920 * 2;
	int height=1080 * 2;
	ppl7::grafix::Color c;

	ppl7::grafix::Image img(width, height, ppl7::grafix::RGBFormat::A8R8G8B8);

	for (int y=0;y < height;y++) {
		c.setColor(0, 0, y * 64 / height, 255);
		img.line(0, y, width, y, c);
	}


	c.setColor(220, 220, 255, 255);
	for (int i=0;i < 1000;i++) {
		int x=ppl7::rand(0, width);
		int y=ppl7::rand(0, height);
		img.putPixel(x, y, c);
	}

	c.setColor(64, 92, 64, 255);
	for (int i=0;i < 10000;i++) {
		int x=ppl7::rand(0, width);
		int y=ppl7::rand(0, height);
		img.putPixel(x, y, c);
	}

	c.setColor(140, 128, 128, 255);
	for (int i=0;i < 10000;i++) {
		int x=ppl7::rand(0, width);
		int y=ppl7::rand(0, height);
		img.putPixel(x, y, c);
	}


	c.setColor(255, 255, 255, 255);
	for (int i=0;i < 10000;i++) {
		int x=ppl7::rand(0, width);
		int y=ppl7::rand(0, height);
		img.putPixel(x, y, c);
	}

	for (int i=0;i < 1000;i++) {
		int x=ppl7::rand(0, width);
		int y=ppl7::rand(0, height);
		img.putPixel(x, y, c);
		img.putPixel(x - 1, y, c);
		img.putPixel(x + 1, y, c);
		img.putPixel(x, y - 1, c);
		img.putPixel(x, y + 1, c);
	}

	ppl7::grafix::ImageFilter_JPEG jpg;
	ppl7::AssocArray param;
	param.set("quality", "90");
	jpg.saveFile("res/backgrounds/night1.jpg", img, param);

	return 0;
}

//57,2958
