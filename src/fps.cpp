#include "decker.h"

FPS::FPS()
{
	fps_start_time=ppl7::GetTime();
	fps_frame_count=0;
	fps=0;
}

int FPS::getFPS() const
{
	return fps;
}

void FPS::update()
{
	fps_frame_count++;
	ppl7::ppl_time_t now=ppl7::GetTime();
	if (now>fps_start_time) {
		fps_start_time=now;
		fps=fps_frame_count;
		fps_frame_count=0;
	}
}

