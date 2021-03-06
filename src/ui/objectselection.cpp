#include "decker.h"
#include "ui.h"
#include "objects.h"

namespace Decker::ui {

ObjectsFrame::Item::Item(int id, const ppl7::String& name, int sprite_no)
{
	this->id=id;
	this->name=name;
	this->sprite_no=sprite_no;
}

ObjectsFrame::ObjectsFrame(int x, int y, int width, int height)
	: ppl7::tk::Frame(x, y, width, height)
{
	ppl7::grafix::Rect client=this->clientRect();

	scrollbar=new ppl7::tk::Scrollbar(client.width() - 28, client.y1, 28, client.height());
	scrollbar->setName("objects-scrollbar");
	scrollbar->setEventHandler(this);
	this->addChild(scrollbar);

	addObject(Decker::Objects::Type::PlayerStartpoint, "Player startpoint", 19);
	addObject(Decker::Objects::Type::Savepoint, "Savepoint", 10);
	addObject(Decker::Objects::Type::Medikit, "Medikit", 11);
	addObject(Decker::Objects::Type::Crystal, "Crystal", 0);
	addObject(Decker::Objects::Type::Diamond, "Diamond", 1);
	addObject(Decker::Objects::Type::Coin, "Coin", 12);
	addObject(Decker::Objects::Type::ExtraLife, "Extra Life", 40);
	addObject(Decker::Objects::Type::Apple, "Apple", 42);
	addObject(Decker::Objects::Type::Cherry, "Cherry", 45);
	addObject(Decker::Objects::Type::Key, "Key", 8);
	addObject(Decker::Objects::Type::Door, "Door", 23);
	addObject(Decker::Objects::Type::LevelEnd, "Level End", 43);
	addObject(Decker::Objects::Type::Arrow, "Arrow", 2);
	addObject(Decker::Objects::Type::ThreeSpeers, "3 Speers", 18);
	addObject(Decker::Objects::Type::Rat, "Rat", 5);
	addObject(Decker::Objects::Type::Bird, "Bird", 44);
	addObject(Decker::Objects::Type::Bat, "Bat", 39);
	addObject(Decker::Objects::Type::Scorpion, "Scorpion", 39);
	addObject(Decker::Objects::Type::HangingSpider, "hanging Spider", 6);
	addObject(Decker::Objects::Type::FloaterHorizontal, "Floater horizontal", 13);
	addObject(Decker::Objects::Type::FloaterVertical, "Floater vertical", 14);
	addObject(Decker::Objects::Type::Mummy, "Mummy", 21);
	addObject(Decker::Objects::Type::Skeleton, "Skeleton", 20);
	addObject(Decker::Objects::Type::BreakingGround, "breaking Ground", 24);
	addObject(Decker::Objects::Type::LaserBeamHorizontal, "Laserbeam horizontal", 27);
	addObject(Decker::Objects::Type::LaserBeamVertical, "Laserbeam vertical", 26);
	addObject(Decker::Objects::Type::Fire, "Fire", 25);
	addObject(Decker::Objects::Type::Vent, "Vent", 31);
	addObject(Decker::Objects::Type::Speaker, "Speaker", 29);
	addObject(Decker::Objects::Type::WindEmitter, "Wind emiter", 28);
	addObject(Decker::Objects::Type::RainEmitter, "Rain emiter", 41);
	addObject(Decker::Objects::Type::ParticleEmitter, "Particle emiter", 46);
	addObject(Decker::Objects::Type::TouchEmitter, "Object emiter", 30);
	addObject(Decker::Objects::Type::Mushroom, "Mushroom", 32);
	addObject(Decker::Objects::Type::TreasureChest, "Treasure Chest", 33);
	addObject(Decker::Objects::Type::Scarabeus, "Scarabeus", 34);
	addObject(Decker::Objects::Type::Switch, "Switch", 35);
	addObject(Decker::Objects::Type::StamperVertical, "Stamper vertical", 36);
	addObject(Decker::Objects::Type::Wallenstein, "Wallenstein", 37);
	addObject(Decker::Objects::Type::Helena, "Helena", 38);

	scrollbar->setSize(object_map.size() / 2);
	scrollbar->setVisibleItems((height - 44) / 160 / 2);
}


int ObjectsFrame::selectedObjectType() const
{
	return selected_object;
}

void ObjectsFrame::setObjectType(int type)
{
	if (type != selected_object) {
		selected_object=type;
		size_t pos=0;
		std::map<size_t, Item>::const_iterator it;
		for (it=object_map.begin();it != object_map.end();++it) {
			if (it->second.id == type) {
				pos=it->first;
				break;
			}
		}
		scrollbar->setPosition(pos / 2);
		needsRedraw();
	}
}

void ObjectsFrame::addObject(int id, const ppl7::String& name, int sprite_no)
{
	object_map.insert(std::pair<size_t, Item>(object_map.size(), Item(id, name, sprite_no)));
}

void ObjectsFrame::setSpriteSet(SpriteTexture* texture)
{
	spriteset=texture;
}

ppl7::String ObjectsFrame::widgetType() const
{
	return "ObjectsFrame";
}

void ObjectsFrame::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == scrollbar) {
		needsRedraw();
	}
}

void ObjectsFrame::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	if (!spriteset) return;
	if (event->widget() == this && event->buttonMask & ppl7::tk::MouseState::Left) {
		size_t object_pos=((event->p.y) / 160) * 2 + ((event->p.x) / 130) + scrollbar->position() * 2;
		std::map<size_t, Item>::const_iterator it;
		it=object_map.find(object_pos);
		if (it != object_map.end()) {
			selected_object=it->second.id;
			// TODO
			//game->setSpriteModeToDraw();
			ppl7::tk::Event event(ppl7::tk::Event::ValueChanged);
			event.setWidget(this);
			getParent()->valueChangedEvent(&event, selected_object);
			needsRedraw();
		}
	}
}

void ObjectsFrame::mouseWheelEvent(ppl7::tk::MouseEvent* event)
{
	if (!spriteset) return;

	if (event->wheel.y != 0) {
		scrollbar->setPosition(scrollbar->position() + event->wheel.y * -1);
		needsRedraw();
	}
}

void ObjectsFrame::paint(ppl7::grafix::Drawable& draw)
{
	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	ppl7::grafix::Color myBorderColorLight=style.frameBorderColorLight;
	ppl7::grafix::Color myBorderColorShadow=style.frameBorderColorShadow;
	ppl7::grafix::Font myFont=style.labelFont;
	ppl7::grafix::Color shade1=style.frameBackgroundColor * 0.70f;
	ppl7::grafix::Color shade2=style.frameBackgroundColor * 0.50f;

	ppl7::grafix::Color shade3=style.frameBackgroundColor * 1.5f;
	ppl7::grafix::Color shade4=style.frameBackgroundColor * 1.2f;

	myFont.setColor(style.labelFontColor);
	myFont.setOrientation(ppl7::grafix::Font::TOP);

	if (!spriteset) return;
	ppl7::tk::Frame::paint(draw);
	draw=clientDrawable(draw);

	ppl7::grafix::Color white(245, 245, 242, 255);
	int x=0, y=0, c=0;
	std::map<size_t, Item>::const_iterator it;
	for (it=object_map.begin();it != object_map.end();++it) {
		const Item& item=it->second;
		if (c >= scrollbar->position() * 2) {
			ppl7::grafix::Drawable frame=draw.getDrawable(x, y, x + 128, y + 128 + 30);
			int w=frame.width() - 1;
			int h=frame.height() - 1;
			if (item.id == selected_object) {
				frame.colorGradient(frame.rect(), shade3, shade4, 1);
			} else {
				frame.colorGradient(frame.rect(), shade1, shade2, 1);
			}
			frame.line(0, 0, w, 0, myBorderColorShadow);
			frame.line(0, 0, 0, h, myBorderColorShadow);
			frame.line(0, h, w, h, myBorderColorLight);
			frame.line(w, 0, w, h, myBorderColorLight);

			spriteset->draw(frame, 0, 0, item.sprite_no);
			ppl7::grafix::Size s=myFont.measure(item.name);
			frame.print(myFont, (128 - s.width) >> 1, 128, item.name);
			x+=130;
			if (x > 255) {
				x=0;
				y+=160;
			}
			if (y > draw.height()) return;
		}
		c++;
	}
}



// ***************************************************************

ObjectSelection::ObjectSelection(int x, int y, int width, int height, Game* game)
	: ppl7::tk::Frame(x, y, width, height)
{
	setClientOffset(4, 4, 4, 4);
	spriteset=NULL;
	this->game=game;
	selected_object=-1;
	ppl7::grafix::Rect client=this->clientRect();

	this->addChild(new ppl7::tk::Label(0, 0, 60, 30, "Layer:"));
	layer_selection=new ppl7::tk::ComboBox(60, 0, width - 70, 30);
	layer_selection->add("Before Player", ppl7::ToString("%d", static_cast<int>(Decker::Objects::Object::Layer::BeforePlayer)));
	layer_selection->add("Behind Player", ppl7::ToString("%d", static_cast<int>(Decker::Objects::Object::Layer::BehindPlayer)));
	layer_selection->add("Behind Bricks", ppl7::ToString("%d", static_cast<int>(Decker::Objects::Object::Layer::BehindBricks)));
	layer_selection->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(Decker::Objects::Object::Layer::BehindPlayer)));
	this->addChild(layer_selection);

	this->addChild(new ppl7::tk::Label(0, 30, width, 60, "Object selection:"));

	objects_frame=new ObjectsFrame(client.left(), 70, client.width(), client.height() - 70);
	objects_frame->setEventHandler(this);
	this->addChild(objects_frame);

}

int ObjectSelection::selectedObjectType() const
{
	return objects_frame->selectedObjectType();
}

void ObjectSelection::setObjectType(int type)
{
	objects_frame->setObjectType(type);
}

int ObjectSelection::currentLayer() const
{
	return layer_selection->currentIdentifier().toInt();
}

void ObjectSelection::setLayer(int layer)
{
	layer_selection->setCurrentIdentifier(ppl7::ToString("%d", layer));
}

void ObjectSelection::setSpriteSet(SpriteTexture* texture)
{
	objects_frame->setSpriteSet(texture);
}

ppl7::String ObjectSelection::widgetType() const
{
	return "ObjectSelection";
}

void ObjectSelection::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	//printf("ObjectSelection::valueChangedEvent\n");
	game->setSpriteModeToDraw();
}
void ObjectSelection::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	printf("ObjectSelection::mouseDownEvent\n");
}








} //EOF namespace Decker::ui
