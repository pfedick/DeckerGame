#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation Switch::representation()
{
	return Representation(Spriteset::GenericObjects, 1);
}

Switch::TargetObject::TargetObject()
{
	object_id=0;
	enable=true;
}

Switch::Switch()
	:Object(Type::ObjectType::Switch)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=1;
	collisionDetection=true;
	visibleAtPlaytime=true;
	sprite_no_representation=215;
	initial_state=false;
	one_time_switch=false;
	auto_toggle_on_collision=false;
	color_scheme=0;
	save_size+=1 + 10 * 3;
	cooldown=0.0f;
	state=0;
	current_state=initial_state;
	init();
}

void Switch::init()
{
	state=0;
	sprite_no=1;
	if (current_state == true) {
		sprite_no+=2;
	}
	this->updateBoundary();

}

void Switch::reset()
{
	current_state=initial_state;
	init();
}

void Switch::notify_targets()
{
	ObjectSystem* objs=GetObjectSystem();
	for (int i=0;i < 10;i++) {
		if (targets[i].object_id > 0) {
			Object* target=objs->getObject(targets[i].object_id);
			if (target) {
				bool target_state=targets[i].enable;
				if (!current_state) {
					target_state=!target_state;
				}
				target->toggle(target_state, this);
			}
		}
	}
}


void Switch::update(double time, TileTypePlane& ttplane, Player& player)
{

}

void Switch::handleCollision(Player* player, const Collision& collision)
{
	if (auto_toggle_on_collision == true && state == 0) {
		state=1;
		return;
	}
	int keyboard=player->getKeyboardMatrix();
	double now=ppl7::GetMicrotime();
	if (cooldown < now && (keyboard & KeyboardKeys::Action)) {
		//printf("switch\n");
		if (current_state == true) current_state=false;
		else current_state=true;
		init();
		notify_targets();
		cooldown=now + 0.2;
	}
}

size_t Switch::save(unsigned char* buffer, size_t size)
{
	if (size < save_size) return 0;
	size_t bytes=Object::save(buffer, size);
	int flags=(color_scheme << 4) & 0xf0;
	if (initial_state) flags|=1;
	if (one_time_switch) flags|=2;
	if (auto_toggle_on_collision) flags|=4;
	if (visibleAtPlaytime) flags|=8;

	ppl7::Poke8(buffer + bytes, flags);
	bytes+=1;
	for (int i=0;i < 10;i++) {
		ppl7::Poke16(buffer + bytes, targets[i].object_id);
		ppl7::Poke8(buffer + bytes + 2, (int)targets[i].enable);
		bytes+=3;
	}
	return bytes;
}

size_t Switch::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < save_size) return 0;
	int flags=ppl7::Peek8(buffer + bytes);
	bytes+=1;
	color_scheme=flags >> 4;
	initial_state=(bool)(flags & 1);
	current_state=initial_state;
	one_time_switch=(bool)(flags & 2);
	auto_toggle_on_collision=(bool)(flags & 4);
	visibleAtPlaytime=(bool)(flags & 8);
	for (int i=0;i < 10;i++) {
		targets[i].object_id=ppl7::Peek16(buffer + bytes);
		targets[i].enable=ppl7::Peek8(buffer + bytes + 2);
		bytes+=3;
	}
	init();
	return size;
}


class SwitchDialog : public Decker::ui::Dialog
{
private:
	Decker::ui::ComboBox* color_scheme;
	Decker::ui::CheckBox* visible_at_playtime;
	Decker::ui::CheckBox* initial_state;
	Decker::ui::CheckBox* one_time_switch;
	Decker::ui::CheckBox* auto_toggle_on_collision;
	ppl7::tk::LineInput* target_id[10];
	Decker::ui::CheckBox* target_state[10];
	ppl7::tk::Button* reset;

	Switch* object;

public:
	SwitchDialog(Switch* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent* event);
};

void Switch::openUi()
{
	SwitchDialog* dialog=new SwitchDialog(this);
	GetGameWindow()->addChild(dialog);
}

SwitchDialog::SwitchDialog(Switch* object)
	: Decker::ui::Dialog(640, 660)
{
	this->object=object;
	setWindowTitle("Switch");
	addChild(new ppl7::tk::Label(0, 0, 120, 30, "Switch-Color: "));
	addChild(new ppl7::tk::Label(0, 40, 120, 30, "Flags: "));
	addChild(new ppl7::tk::Label(0, 170, 120, 30, "Targets: "));

	color_scheme=new Decker::ui::ComboBox(120, 0, 400, 30);
	color_scheme->add("yellow", "0");
	color_scheme->setCurrentIdentifier(ppl7::ToString("%d", object->color_scheme));
	color_scheme->setEventHandler(this);
	addChild(color_scheme);

	visible_at_playtime=new Decker::ui::CheckBox(120, 40, 400, 30, "switch is visible", object->visibleAtPlaytime);
	visible_at_playtime->setEventHandler(this);
	addChild(visible_at_playtime);

	initial_state=new Decker::ui::CheckBox(120, 70, 400, 30, "Initial state: on", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);

	one_time_switch=new Decker::ui::CheckBox(120, 100, 400, 30, "one time switch", object->one_time_switch);
	one_time_switch->setEventHandler(this);
	addChild(one_time_switch);

	auto_toggle_on_collision=new Decker::ui::CheckBox(120, 130, 400, 30, "auto toggle on collision", object->auto_toggle_on_collision);
	auto_toggle_on_collision->setEventHandler(this);
	addChild(auto_toggle_on_collision);

	for (int i=0;i < 10;i++) {
		addChild(new ppl7::tk::Label(120, 200 + i * 35, 80, 30, ppl7::ToString("Object %d: ", i * 1)));
		target_id[i]=new ppl7::tk::LineInput(220, 200 + i * 35, 100, 30, ppl7::ToString("%d", object->targets[i].object_id));
		target_id[i]->setEventHandler(this);
		addChild(target_id[i]);
		target_state[i]=new Decker::ui::CheckBox(325, 200 + i * 35, 100, 30, "enable", object->targets[i].enable);
		target_state[i]->setEventHandler(this);
		addChild(target_state[i]);
	}
	reset=new ppl7::tk::Button(0, 560, 80, 30, "Reset");
	reset->setEventHandler(this);
	addChild(reset);
}

void SwitchDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == color_scheme) {
		object->color_scheme=color_scheme->currentIdentifier().toInt();
		object->init();

	}
}

void SwitchDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	for (int i=0;i < 10;i++) {
		if (event->widget() == target_id[i]) {
			//handled=true;
			object->targets[i].object_id=(text.toInt() & 0xffff);
		}
	}
}

void SwitchDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	for (int i=0;i < 10;i++) {
		if (event->widget() == target_state[i]) {
			object->targets[i].enable=checked;
		}
	}
	if (event->widget() == visible_at_playtime) object->visibleAtPlaytime=checked;
	else if (event->widget() == initial_state) object->initial_state=checked;
	else if (event->widget() == one_time_switch) object->one_time_switch=checked;
	else if (event->widget() == auto_toggle_on_collision) object->auto_toggle_on_collision=checked;

}

void SwitchDialog::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == reset) object->reset();
	else Decker::ui::Dialog::mouseDownEvent(event);
}



}	// EOF namespace Decker::Objects
