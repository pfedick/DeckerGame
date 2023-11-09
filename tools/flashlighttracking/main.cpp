#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <math.h>

const ppl7::grafix::Point pivot(256, 195);
const float pi=3.141592653;

class Rect
{
public:
	int x1, y1, x2, y2;
	Rect();
	void update(int x, int y);
	ppl7::grafix::Point middle() const;
};

Rect::Rect()
{
	x1=-1;
	y1=0;
	x2=0;
	y2=0;
}

void Rect::update(int x, int y)
{
	if (x1 == -1) {
		x1=x;
		y1=y;
		x2=x;
		y2=y;
		return;
	}
	if (x < x1) x1=x;
	if (x > x2) x2=x;
	if (y < y1) y1=y;
	if (y > y2) y2=y;
}

ppl7::grafix::Point Rect::middle() const
{
	if (x1 == -1) return ppl7::grafix::Point(0, 0);
	return ppl7::grafix::Point(x1 + (x2 - x1) / 2, y1 + (y2 - y1) / 2);
}

bool finddots(const ppl7::grafix::Drawable& img, ppl7::grafix::Point& red, ppl7::grafix::Point& green)
{
	Rect r_red, r_green;
	for (int y=0;y < img.height();y++) {
		for (int x=0;x < img.width();x++) {
			ppl7::grafix::Color c=img.getPixel(x, y);
			if (c.red() > 63) r_red.update(x, y);
			if (c.green() > 63) r_green.update(x, y);
		}
	}
	red=r_red.middle();
	green=r_green.middle();
	if (red.x > 0 && green.x > 0) return true;
	return false;
}

int main(int argc, char** argv)
{
	ppl7::grafix::Grafix gfx;
	ppl7::File ff;
	/*
	float a=3.0f;
	float b=6.5f;
	float angle= atanf(a / b) * 360 / (2 * pi);
	printf("angle=%0.3f\n", angle);
	return 0;
	*/
	ff.open("tracking.tmp", ppl7::File::WRITE);
	for (int i=314;i <= 401;i++) {
		ppl7::grafix::Image img;
		ppl7::String filename;
		filename.setf("lightwave/Render/george_flashlight_tracking/frame_%04d.png", i);
		printf("loading: %s\n", (const char*)filename);
		fflush(stdout);
		img.load(filename);
		ppl7::grafix::Point red;
		ppl7::grafix::Point green;
		if (finddots(img, red, green)) {

			ppl7::grafix::Point p=red - pivot;
			float a=abs(red.x - green.x);
			float b=abs(red.y - green.y);
			float angle= atanf(a / b) * 360 / (2 * pi);
			if (red.x > green.x) {
				if (red.y > green.y) angle=90 + (90 - angle);
				else angle=90 - (90 - angle);
			} else {
				if (red.y > green.y) angle=270 - (90 - angle);
				else angle=270 + (90 - angle);
			}
			if (i == 341) angle=-1.0f; //stand front

			printf("   red: %d:%d, green: %d:%d, a=%0.0f, b=%0.0f, angle=%0.3f\n", red.x, red.y, green.x, green.y, a, b, angle);

			ff.putsf("flashlight_pivots.insert(std::pair<int, FlashLightPivot>(%d, FlashLightPivot(%d, %d, %0.1ff)));\n",
				i, p.x, p.y, angle);
		}
	}
	return 0;
}

//57,2958
