#include "decker.h"
#include "ui.h"

namespace Decker::ui {

CheckBox::CheckBox()
: ppl7::tk::Label()
{
	ischecked=false;
}

CheckBox::CheckBox(int x, int y, int width, int height, const String &text, bool checked) // @suppress("Class members should be properly initialized")
: ppl7::tk::Label(x,y,width,height,text)
{
	ischecked=checked;
}

CheckBox::~CheckBox()
{

}

ppl7::String CheckBox::widgetType() const
{
	return ppl7::String("CheckBox");
}

bool CheckBox::checked() const
{
	return ischecked;
}

void CheckBox::setChecked(bool checked)
{
	ischecked=checked;
	needsRedraw();
}


void CheckBox::paint(Drawable &draw)
{
	Label::paint(draw);
}


} //EOF namespace Decker::ui

