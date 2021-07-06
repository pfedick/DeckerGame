#include "decker.h"
#include "ui.h"
#include <typeinfo>

namespace Decker::ui {

	RadioButton::RadioButton()
	: ppl7::tk::Label()
	{
		ischecked=false;
	}

	RadioButton::RadioButton(int x, int y, int width, int height, const String &text, bool checked) // @suppress("Class members should be properly initialized")
	: ppl7::tk::Label(x,y,width,height,text)
	{
		ischecked=checked;
	}

	RadioButton::~RadioButton()
	{

	}

	ppl7::String RadioButton::widgetType() const
	{
		return ppl7::String("RadioButton");
	}

	bool RadioButton::checked() const
	{
		return ischecked;
	}

	void RadioButton::setChecked(bool checked)
	{
		bool laststate=ischecked;
		ischecked=checked;
		needsRedraw();
		// uncheck all other RadioButtons in Parent-Widget
		if (checked==true && this->getParent()) {
			Widget *parent=this->getParent();
			std::list<Widget*>::iterator it;
			for (it=parent->childsBegin(); it!=parent->childsEnd();++it) {
				if (typeid(**it) == typeid(RadioButton) && *it!=this) {
					((RadioButton*)(*it))->setChecked(false);
				}
			}
		}
		ppl7::tk::Event ev(ppl7::tk::Event::Toggled);
		ev.setWidget(this);
		if (checked!=laststate)	{
			toggledEvent(&ev, checked);
		}
	}


	void RadioButton::paint(Drawable &draw)
	{
		const WidgetStyle &style=GetWidgetStyle();
		Drawable d=draw.getDrawable(16,0, draw.width()-16, draw.height());
		Label::paint(d);
		int y1=draw.height()/2;
		draw.circle(9,y1,7,style.frameBorderColorLight);
		draw.circle(9,y1,6,style.frameBorderColorLight);
		if (ischecked) draw.floodFill(9,y1,this->color(), style.frameBorderColorLight);
	}

	void RadioButton::mouseDownEvent(MouseEvent *event)
	{
		setChecked(true);
	}


} //EOF namespace Decker::ui

