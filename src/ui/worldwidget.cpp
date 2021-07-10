#include "decker.h"
#include "ui.h"

namespace Decker::ui {
void WorldWidget::setViewport(const ppl7::grafix::Rect &viewport)
{
	this->setPos(viewport.x1, viewport.y1);
	this->setSize(viewport.width(), viewport.height());
}


}
