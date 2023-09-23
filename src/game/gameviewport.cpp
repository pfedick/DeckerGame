#include <stdio.h>
#include <ppl7.h>
#include "decker.h"

GameViewport::GameViewport()
{
	menu_offset_x=0;
	scaling_enabled=true;
	render_size.setSize(1920, 1080);
	render_rect.x=0;
	render_rect.y=0;
	render_rect.w=1920;
	render_rect.h=1080;
}


void GameViewport::update()
{
	render_rect.w=real_viewport.width;
	render_rect.h=real_viewport.height;

}

void GameViewport::setRealViewport(ppl7::grafix::Size& size)
{
	real_viewport=size;
	update();
}

void GameViewport::setRenderSize(ppl7::grafix::Size& size)
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


ppl7::grafix::Point GameViewport::translate(const ppl7::grafix::Point& coords) const
{
	return coords;
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
