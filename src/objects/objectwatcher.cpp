#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"

namespace Decker::Objects {




Representation ObjectWatcher::representation()
{
	return Representation(Spriteset::GenericObjects, 39);
}


ObjectWatcher::ObjectWatcher()
	:Object(Type::ObjectType::ObjectWatcher)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=39;
	collisionDetection=false;
	visibleAtPlaytime=false;
	sprite_no_representation=39;
	for (int i=0;i < 5;i++) {
		triggerObjects[i].object_id=0;
	}
	for (int i=0;i < 10;i++) {
		watchObjects[i].object_id=0;
		watchObjects[i].expectedState=false;
	}
}

ObjectWatcher::~ObjectWatcher()
{

}


void ObjectWatcher::notifyTargets() const
{
	ObjectSystem* objs=GetObjectSystem();
	for (int i=0;i < 5;i++) {
		if (triggerObjects[i].object_id > 0) {
			Object* target=objs->getObject(triggerObjects[i].object_id);
			if (target) {
				target->trigger();
			}
		}
	}

}

void ObjectWatcher::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	if (!enabled) return;
	ObjectSystem* objs=GetObjectSystem();
	int maxcount=0;
	int state_achived=0;
	for (int i=0;i < 10;i++) {
		if (watchObjects[i].object_id > 0) {
			Object* obj=objs->getObject(watchObjects[i].object_id);
			if (obj) {
				//ppl7::PrintDebugTime("   Object %d has state: %d\n", obj->id, obj->enabled);
				maxcount++;
				if (obj->isEnabled() == watchObjects[i].expectedState) {
					state_achived++;
				}
			}

		}
	}
	//ppl7::PrintDebugTime("ObjectWatcher::update: max: %d, current: %d\n", maxcount, state_achived);
	if (maxcount > 0 && state_achived == maxcount) {
		enabled=false;
		notifyTargets();
		//ppl7::PrintDebugTime("notify targets\n");
	}

}

void ObjectWatcher::reset()
{
	enabled=true;
}

size_t ObjectWatcher::saveSize() const
{
	size_t s=1 + 5 * 2 + 10 * 3;
	return Object::saveSize() + s;

}

size_t ObjectWatcher::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	int p=1;
	for (int i=0;i < 5;i++) {
		ppl7::Poke16(buffer + bytes + p, triggerObjects[i].object_id);
		p+=2;
	}
	for (int i=0;i < 10;i++) {
		ppl7::Poke16(buffer + bytes + p, watchObjects[i].object_id);
		ppl7::Poke8(buffer + bytes + p + 2, watchObjects[i].expectedState);
		p+=3;
	}
	return bytes + p;
}

size_t ObjectWatcher::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	//ppl7::HexDump(buffer, size);
	//ppl7::PrintDebugTime("load\n");

	int p=1;
	for (int i=0;i < 5;i++) {
		triggerObjects[i].object_id=ppl7::Peek16(buffer + bytes + p);
		p+=2;
	}
	for (int i=0;i < 10;i++) {
		watchObjects[i].object_id=ppl7::Peek16(buffer + bytes + p);
		watchObjects[i].expectedState=ppl7::Peek8(buffer + bytes + p + 2);
		p+=3;
	}
	return size;
}

class ObjectWatcherDialog : public Decker::ui::Dialog
{
private:
	ppltk::SpinBox* target_id[5];
	ppltk::CheckBox* target_state[5];

	ppltk::SpinBox* watch_id[10];
	ppltk::CheckBox* watch_state[10];

	ObjectWatcher* object;

public:
	ObjectWatcherDialog(ObjectWatcher* object);
	~ObjectWatcherDialog();

	//virtual void valueChangedEvent(ppltk::Event* event, int value);
	//virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
	virtual void toggledEvent(ppltk::Event* event, bool checked) override;
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
};


void ObjectWatcher::openUi()
{
	ObjectWatcherDialog* dialog=new ObjectWatcherDialog(this);
	GetGameWindow()->addChild(dialog);
}

ObjectWatcherDialog::ObjectWatcherDialog(ObjectWatcher* object)
	: Decker::ui::Dialog(700, 650, Buttons::OK | Buttons::Reset | Buttons::Test)
{
	//ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle(ppl7::ToString("Object Watcher, Object ID: %u", object->id));
	int y=0;

	//int sw=width() / 2;

	addChild(new ppltk::Label(0, y, 400, 30, "Objects to watch for:"));
	y+=35;

	for (int i=0;i < 5;i++) {
		int x=0;
		addChild(new ppltk::Label(x + 30, y, 80, 30, ppl7::ToString("Object %d: ", i + 1)));
		watch_id[i]=new ppltk::SpinBox(x + 110, y, 100, 30, object->watchObjects[i].object_id);
		watch_id[i]->setLimits(0, 65535);
		watch_id[i]->setEventHandler(this);
		addChild(watch_id[i]);

		watch_state[i]=new ppltk::CheckBox(x + 215, y, 100, 30, "enable", object->watchObjects[i].expectedState);
		watch_state[i]->setEventHandler(this);
		addChild(watch_state[i]);

		x=330;
		addChild(new ppltk::Label(x + 30, y, 80, 30, ppl7::ToString("Object %d: ", 5 + i + 1)));
		watch_id[5 + i]=new ppltk::SpinBox(x + 110, y, 100, 30, object->watchObjects[5 + i].object_id);
		watch_id[5 + i]->setLimits(0, 65535);
		watch_id[5 + i]->setEventHandler(this);
		addChild(watch_id[5 + i]);

		watch_state[5 + i]=new ppltk::CheckBox(x + 215, y, 100, 30, "enable", object->watchObjects[5 + i].expectedState);
		watch_state[5 + i]->setEventHandler(this);
		addChild(watch_state[5 + i]);

		y+=35;
	}
	y+=35;


	addChild(new ppltk::Label(0, y, 400, 30, "Objects to trigger:"));
	y+=35;

	for (int i=0;i < 5;i++) {
		addChild(new ppltk::Label(30, y, 80, 30, ppl7::ToString("Object %d: ", i + 1)));
		target_id[i]=new ppltk::SpinBox(110, y, 100, 30, object->triggerObjects[i].object_id);
		target_id[i]->setLimits(0, 65535);
		target_id[i]->setEventHandler(this);
		addChild(target_id[i]);

		y+=35;
	}


}

ObjectWatcherDialog::~ObjectWatcherDialog()
{

}

void ObjectWatcherDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	for (int i=0;i < 10;i++) {
		if (event->widget() == watch_state[i]) object->watchObjects[i].expectedState=checked;
	}
}


void ObjectWatcherDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{

	for (int i=0;i < 5;i++) {
		if (event->widget() == target_id[i]) object->triggerObjects[i].object_id=(uint16_t)value;
	}
	for (int i=0;i < 10;i++) {
		if (event->widget() == watch_id[i]) object->watchObjects[i].object_id=(uint16_t)value;
	}
}

void ObjectWatcherDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button == Buttons::Reset) object->reset();
	if (button == Buttons::Reset) object->notifyTargets();
}


}	// EOF namespace Decker::Objects
