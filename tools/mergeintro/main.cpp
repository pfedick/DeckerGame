#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>


int main(int argc, char** argv)
{
	ppl7::grafix::Grafix gfx;
	ppl7::String BasePath="m:/Decker2/render";
	ppl7::String src, dst;
	int df=0;
	//int start_world_sequence=104;	// 730 => 104
	int start_world_sequence=82;
	int end_logo_sequence=716;
	int blend_frames=30;
	ppl7::Dir::mkDir(BasePath + "/george_decker_game");

	// step 1 copy logo sequence
	for (int i=0;i < end_logo_sequence;i++, df++) {	// 730

		printf("copy frame %04d\n", df);
		src.setf("%s/decker_game_intro/frame_%04d.png", (const char*)BasePath, i);
		dst.setf("%s/george_decker_game/frame_%04d.png", (const char*)BasePath, df);
		if (!ppl7::File::exists(dst))
			ppl7::File::copy(src, dst);


	}

	// step 2 merge logo sequence into world
	ppl7::grafix::ImageFilter_PNG png;
	ppl7::grafix::ImageFilter_BMP bmp;

	for (int i=0;i < blend_frames;i++, df++, start_world_sequence++) {

		printf("merge frames %04d and %04d\n", end_logo_sequence + i, start_world_sequence);
		ppl7::grafix::Image img1, img2;
		src.setf("%s/decker_game_intro/frame_%04d.png", (const char*)BasePath, end_logo_sequence + i);
		img1.load(src);
		src.setf("%s/intro/frame_%04d.png", (const char*)BasePath, start_world_sequence);
		img2.load(src);
		img1.bltBlend(img2, (float)i / (float)blend_frames);
		dst.setf("%s/george_decker_game/frame_%04d.png", (const char*)BasePath, df);
		png.saveFile(dst, img1);

	}
	// step 3 copy rest of world sequence
	for (;start_world_sequence <= 1200;start_world_sequence++, df++) {
		printf("copy frame %04d\n", df);
		src.setf("%s/intro/frame_%04d.png", (const char*)BasePath, start_world_sequence);
		dst.setf("%s/george_decker_game/frame_%04d.png", (const char*)BasePath, df);
		ppl7::File::copy(src, dst);
	}


	return 0;
}
