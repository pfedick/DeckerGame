#include "decker.h"
#include "ui.h"
#include "player.h"

namespace Decker::ui {

StatsFrame::StatsFrame(int x, int y, int width, int height, const ppl7::String& label)
	: ppl7::tk::Widget()
{
	create(x, y, width, height);
	setClientOffset(8, 8, 8, 8);
	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	font=style.buttonFont;
	font.setName("NotoSansBlack");
	font.setBold(false);
	font.setSize(50);
	font.setOrientation(ppl7::grafix::Font::TOP);
	this->label=label;
}

ppl7::String StatsFrame::widgetType() const
{
	return "StatsFrame";
}


void StatsFrame::paint(ppl7::grafix::Drawable& draw)
{
	ppl7::grafix::Color black(0, 0, 0, 255);
	ppl7::grafix::Color white(255, 255, 255, 255);
	draw.cls(ppl7::grafix::Color(1, 1, 1, 128));
	draw.drawRect(0, 0, draw.width(), draw.height(), black);
	draw.drawRect(1, 1, draw.width() - 1, draw.height() - 1, black);
	draw.drawRect(2, 2, draw.width() - 2, draw.height() - 2, white);
	draw.drawRect(3, 3, draw.width() - 3, draw.height() - 3, white);
	font.setColor(ppl7::grafix::Color(255, 255, 255, 255));
	ppl7::grafix::Size s=font.measure(label);
	draw.print(font, 10, 0, label);
	font.setColor(ppl7::grafix::Color(255, 220, 0, 255));
	draw.print(font, 20 + s.width, 0, value);
}

void StatsFrame::setFontSize(int size)
{
	font.setSize(size);
}

void StatsFrame::setValue(const ppl7::String& value)
{
	if (value != this->value) {
		this->value=value;
		this->needsRedraw();
	}
}

WorldWidget::WorldWidget()
	: Widget::Widget()
{
	setClientOffset(0, 0, 0, 0);
	stats_health=new StatsFrame(0, 0, 400, 70, "Health:");
	stats_lifes=new StatsFrame(0, 0, 300, 70, "Lifes:");
	stats_points=new StatsFrame(0, 0, 400, 70, "Points:");
	this->addChild(stats_health);
	this->addChild(stats_lifes);
	this->addChild(stats_points);
	value_health=0;
	value_lifes=0;
	value_points=0;
}


void WorldWidget::setViewport(const ppl7::grafix::Rect& viewport)
{
	//printf("WorldWidget::setViewport: %d x %d\n", viewport.width(), viewport.height());
	this->setPos(viewport.x1, viewport.y1);
	this->setSize(viewport.width(), viewport.height());

	int fs=50;
	int y=viewport.height() - 80;
	if (viewport.width() < 1280) {
		fs=30;
		stats_health->setSize(300, 40);
		stats_points->setSize(300, 40);
		stats_lifes->setSize(300, 40);
		y=viewport.height() - 50;

	} else {
		stats_health->setSize(400, 70);
		stats_points->setSize(400, 70);
		stats_lifes->setSize(300, 70);
	}
	stats_health->setPos(20, y);
	stats_lifes->setPos((viewport.width() - stats_lifes->width()) / 2, y);
	stats_points->setPos(viewport.width() - stats_points->width() - 20, y);

	stats_health->setFontSize(fs);
	stats_lifes->setFontSize(fs);
	stats_points->setFontSize(fs);


	this->needsRedraw();
}

ppl7::String WorldWidget::widgetType() const
{
	return "WorldWidget";
}


void WorldWidget::updatePlayerStats(const Player* player)
{
	if (value_health > player->health) value_health--;
	else if (value_health < player->health) value_health++;
	if (value_lifes > player->lifes) value_lifes--;
	else if (value_lifes < player->lifes) value_lifes++;
	if (value_points > player->points) value_points--;
	else if (value_points < player->points) value_points++;

	stats_health->setValue(ppl7::ToString("%d %%", value_health));
	stats_lifes->setValue(ppl7::ToString("%d", value_lifes));
	stats_points->setValue(ppl7::ToString("%d", value_points));

}

void WorldWidget::resetPlayerStats(const Player* player)
{
	value_health=player->health;
	value_lifes=player->lifes;
	value_points=player->points;
	updatePlayerStats(player);
}


static void drawFrame(ppl7::grafix::Drawable& draw, const ppl7::grafix::Point& p1, const ppl7::grafix::Point& p2, const ppl7::grafix::Color& light, const ppl7::grafix::Color& dark)
{
	draw.line(p1.x, p1.y, p2.x, p1.y, dark);
	draw.line(p1.x, p1.y, p1.x, p2.y, dark);
	draw.line(p1.x, p2.y, p2.x, p2.y, light);
	draw.line(p2.x, p1.y, p2.x, p2.y, light);
}

void WorldWidget::paint(ppl7::grafix::Drawable& draw)
{
	//printf("WorldWidget::paint %d x %d\n", draw.width(), draw.height());
	ppl7::grafix::Point p0(1, 1);
	ppl7::grafix::Point p1(0, 0);
	ppl7::grafix::Point p2(draw.width() - 1, draw.height() - 1);
	drawFrame(draw, p1, p2, ppl7::grafix::Color(192, 192, 196, 255), ppl7::grafix::Color(32, 32, 25, 255));
	p1+=p0;
	p2-=p0;
	drawFrame(draw, p1, p2, ppl7::grafix::Color(192, 192, 196, 200), ppl7::grafix::Color(32, 32, 25, 200));
	p1+=p0;
	p2-=p0;
	drawFrame(draw, p1, p2, ppl7::grafix::Color(192, 192, 196, 150), ppl7::grafix::Color(32, 32, 25, 150));
	p1+=p0;
	p2-=p0;
	drawFrame(draw, p1, p2, ppl7::grafix::Color(192, 192, 196, 100), ppl7::grafix::Color(32, 32, 25, 100));
	p1+=p0;
	p2-=p0;
	drawFrame(draw, p1, p2, ppl7::grafix::Color(192, 192, 196, 60), ppl7::grafix::Color(32, 32, 25, 60));

	//Widget::paint(draw);
}



}
