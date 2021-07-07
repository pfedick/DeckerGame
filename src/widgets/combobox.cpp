#include "decker.h"
#include "ui.h"

namespace Decker::ui {

ComboBox::ComboBox()
: ppl7::tk::Widget()
{

}

ComboBox::ComboBox(int x, int y, int width, int height)
: ppl7::tk::Widget()
{
	selection=NULL;
	create(x,y,width,height);
	WindowManager *wm=ppl7::tk::GetWindowManager();
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

String ComboBox::widgetType() const
{
	return "ComboBox";
}

void ComboBox::paint(Drawable &draw)
{
	const WidgetStyle &style=GetWidgetStyle();
	int w=width()-1;
	int h=height()-1;
	draw.cls(style.buttonBackgroundColor);
	draw.drawRect(0,0,w,h,style.frameBorderColorLight);
	Font myFont=style.buttonFont;
	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(Font::TOP);
	Size s=myFont.measure(myCurrentText);
	draw.print(myFont,4,(draw.height()-s.height)>>1,myCurrentText);


}

void ComboBox::mouseWheelEvent(MouseEvent *event)
{
	//printf ("Wheel: %d\n", event->wheel.y);
	if (event->wheel.y<0 && myCurrentIndex<items.size()-1) {
		setCurrentIndex(myCurrentIndex+1);
	} else if (event->wheel.y>0 && myCurrentIndex>0) {
		setCurrentIndex(myCurrentIndex-1);
	}
}

void ComboBox::mouseDownEvent(MouseEvent *event)
{
	if (event->widget()==dropdown_button) {
		if (selection) {
			delete(selection);
			selection=NULL;
		} else {
			// we need the absolute coordinates of this widget on window
			// and create a new Frame with window as parent and topmost

		}
	}
}

ComboBox::SelectionFrame::SelectionFrame(int x, int y, int width, int height)
: ppl7::tk::Frame(x,y,width,height)
{

}

} //EOF namespace Decker::ui

