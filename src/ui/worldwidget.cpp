#include "decker.h"
#include "ui.h"
#include "player.h"

namespace Decker::ui {

StatsFrame::StatsFrame(int x, int y, int width, int height, const ppl7::String& label)
	: ppltk::Widget()
{
	create(x, y, width, height);
	setClientOffset(8, 8, 8, 8);
	const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
	font=style.buttonFont;
	font.setName("NotoSansBlack");
	font.setBold(false);
	font.setSize(50);
	font.setOrientation(ppl7::grafix::Font::TOP);
}

void StatsFrame::setLabel(const ppl7::String& label)
{
	this->labeltext=label;
	this->needsRedraw();
}

const ppl7::String& StatsFrame::label() const
{
	return labeltext;
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
	ppl7::grafix::Size s=font.measure(labeltext);
	draw.print(font, 10, 0, labeltext);
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


OxygenFrame::OxygenFrame(int x, int y, int width, int height, const ppl7::String& label)
	: ppltk::Widget()
{
	create(x, y, width, height);
	setClientOffset(8, 8, 8, 8);
	const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
	font=style.buttonFont;
	font.setName("NotoSansBlack");
	font.setBold(false);
	font.setSize(50);
	font.setOrientation(ppl7::grafix::Font::TOP);
	this->label=label;
	seconds_total=0.0f;
	seconds_left=0.0f;
}

ppl7::String OxygenFrame::widgetType() const
{
	return "OxygenFrame";
}

void OxygenFrame::setLabel(const ppl7::String& label)
{
	this->label=label;
	needsRedraw();
}

void OxygenFrame::paint(ppl7::grafix::Drawable& draw)
{
	//ppl7::PrintDebugTime("OxygenFrame::paint width=%d, height=%d\n", draw.width(), draw.height());
	ppl7::grafix::Color black(0, 0, 0, 255);
	ppl7::grafix::Color white(255, 255, 255, 255);
	ppl7::grafix::Color oxygen(64, 64, 255, 192);
	if (seconds_left < 15 && seconds_left >= 8) {
		int blend=(seconds_left - 8.0f) * 255 / 7;
		oxygen=ppl7::grafix::Color::getBlended(ppl7::grafix::Color(255, 192, 0, 192),
			oxygen,
			blend);
	} else if (seconds_left < 8 && seconds_left >= 3) {
		int blend=(seconds_left - 3.0f) * 255 / 5;
		oxygen=ppl7::grafix::Color::getBlended(ppl7::grafix::Color(255, 0, 0, 192),
			ppl7::grafix::Color(255, 192, 0, 192),
			blend);
	} else if (seconds_left < 3) {
		oxygen.setColor(255, 0, 0, 192);
	}
	oxygen.setAlpha(192);

	draw.cls(ppl7::grafix::Color(1, 1, 1, 128));
	draw.drawRect(0, 0, draw.width(), draw.height(), black);
	draw.drawRect(1, 1, draw.width() - 1, draw.height() - 1, black);
	draw.drawRect(2, 2, draw.width() - 2, draw.height() - 2, white);
	draw.drawRect(3, 3, draw.width() - 3, draw.height() - 3, white);
	font.setColor(ppl7::grafix::Color(255, 255, 255, 255));
	ppl7::grafix::Size s=font.measure(label);
	draw.print(font, 10, 5, label);
	font.setColor(ppl7::grafix::Color(255, 220, 0, 255));
	//draw.printf(font, 20 + s.width, 0, "%0.0f s", seconds_left);
	int w=draw.width() - s.width - 30;
	int p=seconds_left / seconds_total * w;
	draw.fillRect(20 + s.width, 10, 20 + s.width + p, height() - 10, oxygen);
	ppl7::WideString value;
	value.setf("%0.0f s", seconds_left);
	s=font.measure(value);
	draw.print(font, draw.width() - s.width - 20, 5, value);
}

void OxygenFrame::setFontSize(int size)
{
	font.setSize(size);
}

void OxygenFrame::setValue(float seconds_total, float seconds_left)
{
	if (seconds_left != this->seconds_left) {
		this->seconds_total=seconds_total;
		this->seconds_left=seconds_left;
		this->needsRedraw();
	}
}


WorldWidget::WorldWidget()
	: Widget::Widget()
{
	setClientOffset(0, 0, 0, 0);
	stats_health=new StatsFrame(0, 0, 400, 70, translate("Health:"));
	stats_lifes=new StatsFrame(0, 0, 300, 70, translate("Lifes:"));
	stats_points=new StatsFrame(0, 0, 400, 70, translate("Points:"));
	stats_oxygen=new OxygenFrame(0, 0, 400, 70, translate("Oxygen:"));
	stats_oxygen->setVisible(false);
	this->addChild(stats_health);
	this->addChild(stats_lifes);
	this->addChild(stats_points);
	this->addChild(stats_oxygen);
	value_health=0;
	value_lifes=0;
	value_points=0;
	oxygen_cooldown=0.0f;
	showui=false;
	retranslateUi();
	ppltk::Label l;

}

void WorldWidget::retranslateUi()
{
	stats_health->setLabel(translate("Health:"));
	stats_lifes->setLabel(translate("Lifes:"));
	stats_points->setLabel(translate("Points:"));
	stats_oxygen->setLabel(translate("Oxygen:"));
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
		stats_oxygen->setSize(viewport.width() / 2 - 20, 40);
		y=viewport.height() - 50;

	} else {
		stats_health->setSize(520, 70);
		stats_points->setSize(400, 70);
		stats_lifes->setSize(300, 70);
		stats_oxygen->setSize(viewport.width() / 2 - 20, 40);
	}
	stats_oxygen->setPos(20, 10);
	stats_oxygen->setFontSize(20);


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

static int calculatePointDiff(int display, int player)
{

	if (display < player) {
		int pdiff=(player - display) / 30;
		if (pdiff < 1) pdiff=1;
		return pdiff;
	} else if (display > player) {
		int pdiff=(display - player) / 30;
		if (pdiff < 1) pdiff=1;
		return -pdiff;
	}
	return 0;
}


void WorldWidget::updatePlayerStats(const Player* player)
{
	double now=ppl7::GetMicrotime();
	if (value_health != player->health) value_health+=calculatePointDiff(value_health, player->health);
	if (value_points != player->points) value_points+=calculatePointDiff(value_points, player->points);
	if (value_lifes != player->lifes) value_lifes+=calculatePointDiff(value_lifes, player->lifes);

	stats_health->setValue(ppl7::ToString("%d %%", value_health));
	stats_lifes->setValue(ppl7::ToString("%d", value_lifes));
	stats_points->setValue(ppl7::ToString("%d", (int)value_points));
	stats_oxygen->setValue(player->maxair, player->air);
	if (player->air >= player->maxair) {
		if (oxygen_cooldown < now && stats_oxygen->isVisible()) {
			//ppl7::PrintDebugTime("invisible\n");
			stats_oxygen->setVisible(false);
			parentMustRedraw();
		}
	} else {
		oxygen_cooldown=now + 3.0f;
		if (!stats_oxygen->isVisible()) {
			stats_oxygen->setVisible(true);
			parentMustRedraw();
		}
	}
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

void WorldWidget::setShowUi(bool enable)
{
	showui=enable;
}

void WorldWidget::paint(ppl7::grafix::Drawable& draw)
{
	//printf("WorldWidget::paint %d x %d\n", draw.width(), draw.height());
	//draw.cls();
	if (!showui) return;
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
