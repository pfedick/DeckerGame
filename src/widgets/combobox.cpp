#include "decker.h"
#include "ui.h"

namespace Decker::ui {

ComboBox::ComboBox(int x, int y, int width, int height)
: ppl7::tk::Widget()
{
	selection=NULL;
	create(x,y,width,height);
	ppl7::tk::WindowManager *wm=ppl7::tk::GetWindowManager();
	myCurrentIndex=0;

	dropdown_button=new ppl7::tk::Label(width-24,0,20,height);
	dropdown_button->setIcon(wm->ButtonSymbols.getDrawable(4));
	dropdown_button->setEventHandler(this);
	this->addChild(dropdown_button);


}


void ComboBox::setCurrentText(const ppl7::String &text)
{
	if (text!=myCurrentText) {
		myCurrentText=text;
		needsRedraw();
	}
}

ppl7::String ComboBox::currentText() const
{
	return myCurrentText;
}

ppl7::String ComboBox::currentIdentifier() const
{
	return myCurrentIdentifier;
}

void ComboBox::setCurrentIndex(size_t index)
{
	if (index>=items.size()) return;
	std::list<ComboBoxItem>::iterator it;
	for (it=items.begin();it!=items.end();++it) {
		if ((*it).index==index) {
			myCurrentIndex=index;
			myCurrentText=(*it).text;
			myCurrentIdentifier=(*it).identifier;
			needsRedraw();
		}
	}
}

size_t ComboBox::currentIndex() const
{
	return myCurrentIndex;
}

void ComboBox::add(const ppl7::String &text, const ppl7::String &identifier)
{
	ComboBoxItem item;
	item.text=text;
	item.identifier=identifier;
	item.index=items.size();
	items.push_back(item);
	if (items.size()==1) setCurrentIndex(0);
	needsRedraw();
}


void ComboBox::clear()
{
	myCurrentText.clear();
	myCurrentIdentifier.clear();
	needsRedraw();
}

ppl7::String ComboBox::widgetType() const
{
	return "ComboBox";
}

void ComboBox::paint(ppl7::grafix::Drawable &draw)
{
	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	int w=width()-1;
	int h=height()-1;
	draw.cls(style.buttonBackgroundColor);
	draw.drawRect(0,0,w,h,style.frameBorderColorLight);
	ppl7::grafix::Font myFont=style.buttonFont;
	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(ppl7::grafix::Font::TOP);
	ppl7::grafix::Size s=myFont.measure(myCurrentText);
	draw.print(myFont,4,(draw.height()-s.height)>>1,myCurrentText);


}

void ComboBox::mouseWheelEvent(ppl7::tk::MouseEvent *event)
{
	//printf ("Wheel: %d\n", event->wheel.y);
	if (event->wheel.y<0 && myCurrentIndex<items.size()-1) {
		setCurrentIndex(myCurrentIndex+1);
		ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
		ev.setWidget(this);
		valueChangedEvent(&ev, myCurrentIndex);

	} else if (event->wheel.y>0 && myCurrentIndex>0) {
		setCurrentIndex(myCurrentIndex-1);
		ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
		ev.setWidget(this);
		valueChangedEvent(&ev, myCurrentIndex);
	}
}

void ComboBox::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	if (selection) {
		delete(selection);
		selection=NULL;
	} else {
		// we need the absolute coordinates of this widget on window
		// and create a new Frame with window as parent and topmost
		ppl7::grafix::Point p=absolutePosition();
		ppl7::tk::Widget *window=getTopmostParent();
		size_t maxsize=items.size();
		if (maxsize>10) maxsize=10;
		selection=new ListWidget(p.x, p.y+this->height(),this->width(),maxsize*30);
		selection->setTopmost(true);
		std::list<ComboBoxItem>::const_iterator it;
		for (it=items.begin();it!=items.end();++it) {
			selection->add((*it).text, (*it).identifier);
		}
		selection->setCurrentText(myCurrentText);
		window->addChild(selection);
	}
}





} //EOF namespace Decker::ui

