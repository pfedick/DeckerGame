#include "decker.h"
#include "ui.h"

namespace Decker::ui {

ListWidget::ListWidget(int x, int y, int width, int height)
: ppl7::tk::Frame(x,y,width, height)
{
	setClientOffset(2,2,2,2);
	scrollbar=NULL;
	myCurrentIndex=0;
	scrollbar=new Scrollbar(width-30,0,30,height-4);
	scrollbar->setEventHandler(this);
	this->addChild(scrollbar);
	mouseOverIndex=-1;
	visibleItems=height/30;
	scrollbar->setVisibleItems(visibleItems);
	//printf ("visibleItems=%d\n",visibleItems);
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
		std::list<ListWidgetItem>::iterator it;
		for (it=items.begin();it!=items.end();++it) {
			if ((*it).text==text) {
				myCurrentText=(*it).text;
				myCurrentIdentifier=(*it).identifier;
				myCurrentIndex=(*it).index;
				size_t start=scrollbar->position();
				if (start+visibleItems<=myCurrentIndex) {
					scrollbar->setPosition(myCurrentIndex);
				}
				needsRedraw();
				return;
			}
		}
	}
}

void ListWidget::setCurrentIndex(size_t index)
{
	if (index>=items.size()) return;
	std::list<ListWidgetItem>::iterator it;
	for (it=items.begin();it!=items.end();++it) {
		if ((*it).index==index) {
			myCurrentIndex=index;
			size_t start=scrollbar->position();
			if (start+visibleItems<=index) {
				scrollbar->setPosition(index);
			}
			myCurrentText=(*it).text;
			myCurrentIdentifier=(*it).identifier;
			needsRedraw();
			return;
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
	scrollbar->setSize((int)items.size());
	needsRedraw();
}


void ListWidget::clear()
{
	myCurrentText.clear();
	myCurrentIdentifier.clear();
	items.clear();
	scrollbar->setPosition(0);
	scrollbar->setSize(0);
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
	//int h=height()-1;
	int y=0;
	ppl7::grafix::Font myFont=style.buttonFont;
	ppl7::grafix::Color selectionColor=style.frameBackgroundColor*1.4f;
	ppl7::grafix::Color mouseoverColor=style.frameBackgroundColor*1.7f;
	std::list<ListWidgetItem>::iterator it;
	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(ppl7::grafix::Font::TOP);
	int start=scrollbar->position();
	int c=0;
	for (it=items.begin();it!=items.end();++it) {
		if (start<=c) {
			if (c==mouseOverIndex)
				draw.fillRect(0, y,w,y+29,mouseoverColor);
			else if ((*it).index==myCurrentIndex)
				draw.fillRect(0, y,w,y+29,selectionColor);
			ppl7::grafix::Size s=myFont.measure((*it).text);
			draw.print(myFont,4,y+((30-s.height)>>1),(*it).text);
			draw.line(0,y+30,w,y+30,style.frameBorderColorLight);
			y+=30;
		}
		c++;
	}
}

void ListWidget::valueChangedEvent(ppl7::tk::Event *event, int value)
{
	if (event->widget()==scrollbar) {
		this->needsRedraw();
	} else {
		EventHandler::valueChangedEvent(event, value);
	}
}

void ListWidget::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	if (event->p.x<width()-30 && event->widget()==this) {
		size_t index=scrollbar->position()+event->p.y/30;
		if (index>items.size()) return;
		setCurrentIndex((size_t)index);
		ppl7::tk::Event ev(ppl7::tk::Event::ValueChanged);
		ev.setWidget(this);
		valueChangedEvent(&ev, myCurrentIndex);
		needsRedraw();
	}
	EventHandler::mouseDownEvent(event);
}

void ListWidget::mouseWheelEvent(ppl7::tk::MouseEvent *event)
{
	if (event->wheel.y<0) {
		scrollbar->setPosition(scrollbar->position()+1);
		this->needsRedraw();
	} else if (event->wheel.y>0) {
		scrollbar->setPosition(scrollbar->position()-1);
		this->needsRedraw();
	}
}

void ListWidget::mouseMoveEvent(ppl7::tk::MouseEvent *event)
{
	if (event->p.x<width()-30 && event->widget()==this) {
		//printf ("x=%d\n",event->p.x);
		mouseOverIndex=scrollbar->position()+event->p.y/30;
		this->needsRedraw();
	}
}


void ListWidget::lostFocusEvent(ppl7::tk::FocusEvent *event)
{
	//printf ("ListWidget::lostFocusEvent\n");
}

} //EOF namespace Decker::ui

