#include <stdio.h>
#include <ppl7.h>
#include "decker.h"

GameViewport::GameViewport()
{
	menu_offset_x=0;
	scaling_enabled=true;
	allow_upscale=true;
	render_size.setSize(1920, 1080);
	render_rect.x=0;
	render_rect.y=0;
	render_rect.w=1920;
	render_rect.h=1080;
	setWidth(1920);
	setHeight(1080);
}


void GameViewport::update()
{
	float aspect=(float)render_size.width / (float)render_size.height;
	int w=real_viewport.width;
	int h=real_viewport.height;
	if (!allow_upscale) {
		if (w > render_size.width) w=render_size.width;
		if (h > render_size.height) h=render_size.height;
	}

	render_rect.w=w;
	render_rect.h=(float)w / aspect;
	if (render_rect.h > h) {
		render_rect.h= h;
		render_rect.w=(float)render_rect.h * aspect;
	}


	if (render_rect.w < real_viewport.width) {
		render_rect.x=(real_viewport.width - render_rect.w) / 2;
	} else {
		render_rect.x=0;
	}
	if (render_rect.h < real_viewport.height) {
		render_rect.y=(real_viewport.height - render_rect.h) / 2;
	} else {
		render_rect.y=0;
	}
	if (menu_offset_x) render_rect.x+=(menu_offset_x / 2);
	//ppl7::PrintDebugTime("vp width=%d, height=%d\n", render_rect.w, render_rect.h);

}

void GameViewport::setRealViewport(const ppl7::grafix::Size& size)
{
	real_viewport=size;
	update();
}

void GameViewport::setRenderSize(const ppl7::grafix::Size& size)
{
	render_size=size;
	update();
}

void GameViewport::setMenuOffset(int x)
{
	menu_offset_x=x;
	update();
}

void GameViewport::setScalingEnabled(bool enable)
{
	scaling_enabled=enable;
	update();
}

void GameViewport::setAllowUpscale(bool allow)
{
	allow_upscale=allow;
	update();
}



void GameViewport::translateMouseEvent(ppl7::tk::MouseEvent* event)
{
	ppl7::tk::MouseState mouse=ppl7::tk::GetWindowManager()->getMouseState();
	float factor=(float)render_size.width / (float)render_rect.w;
	mouse.p.x=((float)mouse.p.x * factor) - render_rect.x;
	mouse.p.y=((float)mouse.p.y * factor) - render_rect.y;
	//ppl7::PrintDebugTime("%d:%d\n", mouse.p.x, mouse.p.y);
	event->p=mouse.p;
}

ppl7::grafix::Point GameViewport::translate(const ppl7::grafix::Point& coords) const
{
	float factor=(float)render_size.width / (float)render_rect.w;
	ppl7::grafix::Point p;
	p.x=((float)coords.x * factor) - render_rect.x;
	p.y=((float)coords.y * factor) - render_rect.y;
	return p;
}


void GameViewport::getRenderRect(SDL_Rect& rect) const
{
	rect.x=render_rect.x;
	rect.y=render_rect.y;
	rect.w=render_rect.w;
	rect.h=render_rect.h;
}

const SDL_Rect& GameViewport::getRenderRect() const
{
	return render_rect;
}
