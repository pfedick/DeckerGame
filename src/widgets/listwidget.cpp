#include "decker.h"
#include "ui.h"

namespace Decker::ui {

ListWidget::ListWidget(int x, int y, int width, int height)
: ppl7::tk::Frame(x,y,width, height)
{
	setClientOffset(0,0,0,0);
	scrollbar=NULL;
	myCurrentIndex=0;
	scrollbar=new Scrollbar(width-30,0,30,height);
	scrollbar->setEventHandler(this);
	this->addChild(scrollbar);
}

ppl7::String ListWidget::currentText() const
{
	return myCurrentText;
}

ppl7::String ListWidget::currentIdentifier() const
{
	return myCurrentIdentifier;
}

void ListWidget::setCurrentText(const ppl7::String &text)
{
	if (text!=myCurrentText) {
		myCurrentText=text;
		needsRedraw();
	}
}

void ListWidget::setCurrentIndex(size_t index)
{
	if (index>=items.size()) return;
	std::list<ListWidgetItem>::iterator it;
	for (it=items.begin();it!=items.end();++it) {
		if ((*it).index==index) {
			myCurrentIndex=index;
			myCurrentText=(*it).text;
			myCurrentIdentifier=(*it).identifier;
			needsRedraw();
		}
	}
}

size_t ListWidget::currentIndex() const
{
	return myCurrentIndex;
}

void ListWidget::add(const ppl7::String &text, const ppl7::String &identifier)
{
	ListWidgetItem item;
	item.text=text;
	item.identifier=identifier;
	item.index=items.size();
	items.push_back(item);
	if (items.size()==1) setCurrentIndex(0);
	needsRedraw();
}


void ListWidget::clear()
{
	myCurrentText.clear();
	myCurrentIdentifier.clear();
	needsRedraw();
}

ppl7::String ListWidget::widgetType() const
{
	return "ListWidget";
}

void ListWidget::paint(ppl7::grafix::Drawable &draw)
{
	ppl7::tk::Frame::paint(draw);
	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	int w=width()-30;
	int h=height()-1;
	int y=0;
	ppl7::grafix::Font myFont=style.buttonFont;
	ppl7::grafix::Color selectionColor=style.frameBackgroundColor*1.4f;
	std::list<ListWidgetItem>::iterator it;
	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(ppl7::grafix::Font::TOP);

	for (it=items.begin();it!=items.end();++it) {
		if ((*it).index==myCurrentIndex)
			draw.fillRect(0, y,w,y+29,selectionColor);
		ppl7::grafix::Size s=myFont.measure((*it).text);
		draw.print(myFont,4,y+(30-s.height>>1),(*it).text);
		draw.line(0,y+30,w,y+30,style.frameBorderColorLight);
		y+=30;
	}
	/*
	draw.cls(style.buttonBackgroundColor);
	draw.drawRect(0,0,w,h,style.frameBorderColorLight);
	ppl7::grafix::Font myFont=style.buttonFont;
	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(ppl7::grafix::Font::TOP);
	ppl7::grafix::Size s=myFont.measure(myCurrentText);
	draw.print(myFont,4,(draw.height()-s.height)>>1,myCurrentText);
	*/

}

void ListWidget::valueChangedEvent(ppl7::tk::Event *event, int value)
{

}

} //EOF namespace Decker::ui

