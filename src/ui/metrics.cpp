#include "decker.h"
#include "ui.h"

namespace Decker::ui {


MetricsSubMenu::MetricsSubMenu(int x, int y, MainMenue* menue)
	: ppl7::tk::Frame(x, y, 450, 255)
{
	this->menue=menue;
	this->setTransparent(false);
	setBackgroundColor(ppl7::grafix::Color(0, 0, 0, 196));
	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	font=style.labelFont;
	font.setColor(style.labelFontColor);
	font.setSize(10);
	font.setOrientation(ppl7::grafix::Font::Orientation::TOP);
}

void MetricsSubMenu::update(const Metrics& metrics)
{
	this->metrics=metrics;
	needsRedraw();
}

void MetricsSubMenu::drawMillisecondMetric(ppl7::grafix::Drawable& draw, int c1, int  c2, int y, const ppl7::String& text, double value)
{
	draw.print(font, c1, y, text);
	ppl7::WideString v;
	v.setf("%0.3f ms", value * 1000.0f);
	ppl7::grafix::Size s=font.measure(v);
	draw.print(font, c2 - s.width, y, v);

}

void MetricsSubMenu::drawDoubleMetric(ppl7::grafix::Drawable& draw, int c1, int  c2, int y, const ppl7::String& text, double value)
{
	draw.print(font, c1, y, text);
	ppl7::WideString v;
	v.setf("%0.3f", value);
	ppl7::grafix::Size s=font.measure(v);
	draw.print(font, c2 - s.width, y, v);

}

void MetricsSubMenu::drawIntMetric(ppl7::grafix::Drawable& draw, int c1, int  c2, int y, const ppl7::String& text, uint64_t value)
{
	draw.print(font, c1, y, text);
	ppl7::WideString v;
	v.setf("%ld", value);
	ppl7::grafix::Size s=font.measure(v);
	draw.print(font, c2 - s.width, y, v);
}

void MetricsSubMenu::drawCountMetric(ppl7::grafix::Drawable& draw, int c1, int  c2, int c3, int y, const ppl7::String& text, uint64_t value1, uint64_t value2)
{
	draw.print(font, c1, y, text);
	ppl7::WideString v;
	v.setf("%ld", value1);
	ppl7::grafix::Size s=font.measure(v);
	draw.print(font, c2 - s.width, y, v);

	v.setf("%ld", value2);
	s=font.measure(v);
	draw.print(font, c3 - s.width, y, v);
}

void MetricsSubMenu::paint(ppl7::grafix::Drawable& draw)
{
	Frame::paint(draw);
	ppl7::grafix::Color line_color(192, 192, 192, 255);
	int y=5;
	int line=15;
	int c1=5;
	int c2=190;
	double max=metrics.time_total.get();
	if (metrics.time_particle_thread.get() > max) max=metrics.time_particle_thread.get();
	if (metrics.time_audioengine.get() > max) max=metrics.time_audioengine.get();

	drawMillisecondMetric(draw, c1, c2, y, "total:", max);
	drawMillisecondMetric(draw, c2 + 5, c2 + 80, y, "of", metrics.time_frame.get());
	ppl7::WideString v;
	v.setf("= %0.1f%%", max * 100.0f / metrics.time_frame.get());
	draw.print(font, c2 + 85, y, v);
	y+=line * 2;

	drawMillisecondMetric(draw, c1, c2, y, "main thread:", metrics.time_total.get());
	draw.line(194, y + 8, 210, y + 8, line_color);
	draw.line(210, y + 8, 214, y + 4, line_color);
	draw.line(210, y + 8, 214, y + 12, line_color);

	draw.line(214, y - 2, 214, y + 4, line_color);
	draw.line(214, y + 12, 214, draw.height() - 11, line_color);
	draw.line(214, y - 2, 224, y - 2, line_color);
	draw.line(214, draw.height() - 10, 224, draw.height() - 10, line_color);

	y+=line;
	drawMillisecondMetric(draw, c1, c2, y, "particle update thread:", metrics.time_particle_thread.get());
	y+=line;
	drawMillisecondMetric(draw, c1, c2, y, "audio engine thread:", metrics.time_audioengine.get());


	y+=(line * 1.5f);
	drawIntMetric(draw, c1, c2, y, "FPS:", metrics.fps);
	y+=line;
	drawDoubleMetric(draw, c1, c2, y, "FPS comp:", metrics.frame_rate_compensation);
	y+=line;
	drawMillisecondMetric(draw, c1, c2, y, "Frametime:", metrics.frametime);
	y+=(line * 1.5f);
	draw.print(font, c1, y, "Counter:");
	y+=line;
	drawCountMetric(draw, c1, 120, 190, y, "Sprites:", metrics.total_sprites, metrics.visible_sprites);
	y+=line;
	drawCountMetric(draw, c1, 120, 190, y, "Objects:", metrics.total_objects, metrics.visible_objects);
	y+=line;
	drawCountMetric(draw, c1, 120, 190, y, "Particles:", metrics.total_particles, metrics.visible_particles);
	y+=line;
	drawCountMetric(draw, c1, 120, 190, y, "Lights:", metrics.total_lights, metrics.visible_lights);
	y+=line;
	drawCountMetric(draw, c1, 120, 190, y, "Audio Tracks:", metrics.total_audiotracks, metrics.hearable_audiotracks);

	y=5 + 2 * line;
	c1=220;
	c2=440;
	drawMillisecondMetric(draw, c1, c2, y, "draw userinterface:", metrics.time_draw_ui.get());
	y+=line;
	drawMillisecondMetric(draw, c1, c2, y, "handle events:", metrics.time_events.get());
	y+=line;
	drawMillisecondMetric(draw, c1, c2, y, "misc:", metrics.time_misc.get());
	y+=line;
	drawMillisecondMetric(draw, c1, c2, y, "draw the world:", metrics.time_draw_world.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "update sprites:", metrics.time_update_sprites.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "update objects:", metrics.time_update_objects.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "update particles:", metrics.time_update_particles.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "update lights:", metrics.time_update_lights.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "draw background:", metrics.time_draw_background.get());
	y+=line;
	//drawDoubleMetric(draw, c1 + 20, c2, y, "time_draw_tsop", metrics.time_draw_tsop.get());
	//y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "draw tiles:", metrics.time_plane.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "draw sprites:", metrics.time_sprites.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "draw objects:", metrics.time_objects.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "draw particles:", metrics.time_draw_particles.get());
	y+=line;
	drawMillisecondMetric(draw, c1 + 20, c2, y, "draw lights:", metrics.time_lights.get());
	y+=line;

}

} //EOF namespace Decker
