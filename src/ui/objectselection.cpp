#include "decker.h"
#include "ui.h"

namespace Decker::ui {

ObjectSelection::Item::Item(int id, const ppl7::String &name, int sprite_no)
{
	this->id=id;
	this->name=name;
	this->sprite_no=sprite_no;
}

ObjectSelection::ObjectSelection(int x, int y, int width, int height, Game *game)
: ppl7::tk::Frame(x,y,width,height)
{
	spriteset=NULL;
	this->game=game;
	selected_object=-1;
	scale=1.0f;
	ppl7::grafix::Rect client=this->clientRect();

	scrollbar=new Scrollbar(width-34,40,29,client.height()-40);
	scrollbar->setName("objects-scrollbar");
	scrollbar->setEventHandler(this);

	this->addChild(scrollbar);
	addObject(1,"Player startpoint",19);
	addObject(2,"Savepoint",10);
	addObject(3,"Medikit",11);
	addObject(100,"Crystal",0);
	addObject(101,"Diamond",1);
	addObject(102,"Coin",12);
	addObject(103,"Key",8);
	addObject(200,"Arrow",4);
	addObject(201,"3 Speets",18);
	addObject(300,"Rat",5);
	addObject(301,"hanging Spider",6);
	addObject(500,"Floater horizontal",13);
	addObject(501,"Floater vertical",14);

	scrollbar->setSize(object_map.size()/2);
	scrollbar->setVisibleItems((height-40)/160/2);
}

void ObjectSelection::addObject(int id, const ppl7::String &name, int sprite_no)
{
	object_map.insert(std::pair<size_t,Item>(object_map.size(),Item(id,name,sprite_no)));
}

void ObjectSelection::setSpriteSet(SpriteTexture *texture)
{
	spriteset=texture;
}

void ObjectSelection::setSpriteScale(float factor)
{
	if (factor>=0.1f && factor<=2.0f) scale=factor;
}

float ObjectSelection::spriteScale() const
{
	return scale;
}

ppl7::String ObjectSelection::widgetType() const
{
	return "ObjectSelection";
}

void ObjectSelection::valueChangedEvent(ppl7::tk::Event *event, int value)
{
	if (event->widget()==scrollbar) {
		needsRedraw();
	}
}

void ObjectSelection::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	if (!spriteset) return;
	if (event->widget()==this && event->buttonMask&ppl7::tk::MouseState::Left && event->p.y>40) {
		size_t object_pos=((event->p.y-40)/160)*2+((event->p.x-8)/130)+scrollbar->position()*2;
		std::map<size_t,Item>::const_iterator it;
		it=object_map.find(object_pos);
		if (it!=object_map.end()) {
			selected_object=it->second.id;
			needsRedraw();
		}
	}
}



void ObjectSelection::mouseWheelEvent(ppl7::tk::MouseEvent *event)
{
	if (!spriteset) return;

	if (event->wheel.y!=0) {
		scrollbar->setPosition(scrollbar->position()+event->wheel.y*-1);
		needsRedraw();
	}
}

void ObjectSelection::paint(ppl7::grafix::Drawable &draw)
{

	const ppl7::tk::WidgetStyle &style=ppl7::tk::GetWidgetStyle();
	ppl7::grafix::Color myBorderColorLight=style.frameBorderColorLight;
	ppl7::grafix::Color myBorderColorShadow=style.frameBorderColorShadow;
	ppl7::grafix::Font myFont=style.labelFont;
	ppl7::grafix::Color shade1=style.frameBackgroundColor*0.90f;
	ppl7::grafix::Color shade2=style.frameBackgroundColor*0.70f;

	ppl7::grafix::Color shade3=style.frameBackgroundColor*1.5f;
	ppl7::grafix::Color shade4=style.frameBackgroundColor*1.2f;

	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(ppl7::grafix::Font::TOP);

	ppl7::tk::Frame::paint(draw);
	if (!spriteset) return;
	ppl7::grafix::Color white(245,245,242,255);
	int x=8, y=40, c=0;
	std::map<size_t,Item>::const_iterator it;
	for(it=object_map.begin();it!=object_map.end();++it) {
		const Item &item=it->second;
		if (c>=scrollbar->position()*2) {
			ppl7::grafix::Drawable frame=draw.getDrawable(x, y, x+128, y+128+30);
			int w=frame.width()-1;
			int h=frame.height()-1;
			if (item.id==selected_object) {
				frame.colorGradient(frame.rect(),shade3,shade4,1);
			} else {
				frame.colorGradient(frame.rect(),shade1,shade2,1);
			}
			frame.line(0,0,w,0,myBorderColorShadow);
			frame.line(0,0,0,h,myBorderColorShadow);
			frame.line(0,h,w,h,myBorderColorLight);
			frame.line(w,0,w,h,myBorderColorLight);

			spriteset->draw(frame, 0, 0,item.sprite_no);
			ppl7::grafix::Size s=myFont.measure(item.name);
			frame.print(myFont,(128-s.width)>>1,128,item.name);


			x+=130;
			if (x>255) {
				x=8;
				y+=160;
			}
			if (y>draw.height()) return;
		}
		c++;
	}
}






} //EOF namespace Decker::ui

