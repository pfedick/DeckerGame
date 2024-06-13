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
	return Representation(Spriteset::Switches, 0);
}

Switch::TargetObject::TargetObject()
{
	object_id=0;
	state=TargetState::enable;
}

Switch::Switch()
	:Object(Type::ObjectType::Switch)
{
	sprite_set=Spriteset::Switches;
	sprite_no=3;
	collisionDetection=true;
	pixelExactCollision=false;
	visibleAtPlaytime=true;
	sprite_no_representation=0;
	initial_state=false;
	one_time_switch=false;
	auto_toggle_on_collision=false;
	switch_style=SwitchStyle::SwitchWithLever;
	color_base=7;
	color_lever=2;
	color_button=20;
	cooldown=0.0f;
	current_state=initial_state;
	color_mod.set(226, 192, 52, 255);
	init();
}

void Switch::init()
{
	switch (switch_style) {
		case SwitchStyle::SwitchWithLever:
			sprite_no=3;
			break;
		case SwitchStyle::SwitchWithLeverAndTop:
			sprite_no=9;
			break;
		case SwitchStyle::SwitchWithSmallLeverAndTop:
			sprite_no=18;
			break;
		case SwitchStyle::LightSwitch:
			sprite_no=27;
			break;
		case SwitchStyle::Lever:
			sprite_no=31;
			break;
	}
	sprite_no_representation=sprite_no;


	this->updateBoundary();

}

void Switch::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	const ColorPalette& palette=GetColorPalette();
	int sprite_base=0;
	int sprite_lever=0;
	int sprite_button=0;

	switch (switch_style) {
		case SwitchStyle::SwitchWithLever:
			sprite_base=3; sprite_lever=6;
			if (current_state == true) { sprite_base=1; sprite_lever=4; }
			break;
		case SwitchStyle::SwitchWithLeverAndTop:
			sprite_base=9; sprite_lever=12; sprite_button=15;
			if (current_state == true) { sprite_base=7; sprite_lever=10; sprite_button=13; }
			break;
		case SwitchStyle::SwitchWithSmallLeverAndTop:
			sprite_base=18; sprite_lever=21; sprite_button=24;
			if (current_state == true) { sprite_base=16; sprite_lever=19; sprite_button=22; }
			break;
		case SwitchStyle::LightSwitch:
			sprite_base=27; sprite_lever=30;
			if (current_state == true) { sprite_base=25; sprite_lever=28; }
			break;
		case SwitchStyle::Lever:
			sprite_base=31;
			if (current_state == true) { sprite_base=33; }
			break;
		default:
			return;
	}
	if (sprite_base) texture->draw(renderer,
		p.x + coords.x,
		p.y + coords.y,
		sprite_base, palette.getColor(color_base));
	if (sprite_lever) texture->draw(renderer,
		p.x + coords.x,
		p.y + coords.y,
		sprite_lever, palette.getColor(color_lever));
	if (sprite_button) texture->draw(renderer,
		p.x + coords.x,
		p.y + coords.y,
		sprite_button, palette.getColor(color_button));

}

void Switch::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	draw(renderer, coords);
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
				TargetState target_state=targets[i].state;
				if (target_state == TargetState::trigger) target->trigger(this);
				else {
					bool s=false;
					if (target_state == TargetState::enable) s=true;
					if (!current_state) {
						s=!s;
					}
					target->toggle(s, this);
				}
			}
		}
	}
}

void Switch::toggle(bool enable, Object* source)
{
	if (source == this) return;
	current_state=enable;
	init();
	notify_targets();
}

void Switch::trigger(Object* source)
{
	toggle(!current_state, source);
}


void Switch::update(double, TileTypePlane&, Player&, float)
{

}

void Switch::handleCollision(Player* player, const Collision& collision)
{
	if (auto_toggle_on_collision == true && current_state == false) {
		current_state=true;
		return;
	}
	Player::Keys keyboard=player->getKeyboardMatrix();
	double now=ppl7::GetMicrotime();
	if (cooldown < now && (keyboard.matrix & KeyboardKeys::Action)) {
		//printf("switch\n");
		if (current_state == true) current_state=false;
		else current_state=true;
		getAudioPool().playOnce(AudioClip::light_switch1, 0.7f);
		init();
		notify_targets();
		cooldown=now + 0.2;
	}
}

size_t Switch::saveSize() const
{
	return Object::saveSize() + 6 + 15 * 3;
}

size_t Switch::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 2);		// Object Version

	int flags=0;
	if (initial_state) flags|=1;
	if (one_time_switch) flags|=2;
	if (auto_toggle_on_collision) flags|=4;
	if (visibleAtPlaytime) flags|=8;
	ppl7::Poke8(buffer + bytes + 1, flags);
	ppl7::Poke8(buffer + bytes + 2, static_cast<int>(switch_style));
	ppl7::Poke8(buffer + bytes + 3, color_base);
	ppl7::Poke8(buffer + bytes + 4, color_lever);
	ppl7::Poke8(buffer + bytes + 5, color_button);
	bytes+=6;
	for (int i=0;i < 15;i++) {
		ppl7::Poke16(buffer + bytes, targets[i].object_id);
		ppl7::Poke8(buffer + bytes + 2, static_cast<int>(targets[i].state));
		bytes+=3;
	}
	return bytes;
}

size_t Switch::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version < 1 || version>2) return 0;

	int flags=ppl7::Peek8(buffer + bytes + 1);
	initial_state=(bool)(flags & 1);
	current_state=initial_state;
	one_time_switch=(bool)(flags & 2);
	auto_toggle_on_collision=(bool)(flags & 4);
	visibleAtPlaytime=(bool)(flags & 8);
	if (version == 1) {
		bytes+=2;
		switch_style=static_cast<SwitchStyle>(flags >> 4);
		for (int i=0;i < 10;i++) {
			targets[i].object_id=ppl7::Peek16(buffer + bytes);
			targets[i].state=static_cast<TargetState>(ppl7::Peek8(buffer + bytes + 2));
			bytes+=3;
		}
	} else if (version == 2) {
		switch_style=static_cast<SwitchStyle>(ppl7::Peek8(buffer + bytes + 2));
		color_base=ppl7::Peek8(buffer + bytes + 3);
		color_lever=ppl7::Peek8(buffer + bytes + 4);
		color_button=ppl7::Peek8(buffer + bytes + 5);
		bytes+=6;
		for (int i=0;i < 15;i++) {
			targets[i].object_id=ppl7::Peek16(buffer + bytes);
			targets[i].state=static_cast<TargetState>(ppl7::Peek8(buffer + bytes + 2));
			bytes+=3;
		}
	}
	init();
	return size;
}


class SwitchDialog : public Decker::ui::Dialog
{
private:
	Switch* object;
	ppltk::ComboBox* switch_style;
	ppltk::CheckBox* visible_at_playtime;
	ppltk::CheckBox* initial_state, * current_state;
	ppltk::CheckBox* one_time_switch;
	ppltk::CheckBox* auto_toggle_on_collision;
	ppltk::SpinBox* target_id[15];
	ppltk::RadioButton* target_state_on[15];
	ppltk::RadioButton* target_state_off[15];
	ppltk::RadioButton* target_state_trigger[15];
	//ppltk::Button* reset;

	Decker::ui::ColorSelectionFrame* colorframe;
	ppltk::Button* button_color_base;
	ppltk::Button* button_color_lever;
	ppltk::Button* button_color_button;
	ppltk::Frame* frame_color_base;
	ppltk::Frame* frame_color_lever;
	ppltk::Frame* frame_color_button;
	ppltk::Label* current_element_label;
	ppltk::Frame* current_element_color_frame;
	int* color_target;

	enum class Element {
		Base,
		Lever,
		Button
	};

	Element current_element;
	void setCurrentElement(Element element);



public:
	SwitchDialog(Switch* object);
	virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
	virtual void toggledEvent(ppltk::Event* event, bool checked);
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
	void mouseDownEvent(ppltk::MouseEvent* event) override;

};

void Switch::openUi()
{
	SwitchDialog* dialog=new SwitchDialog(this);
	GetGameWindow()->addChild(dialog);
}

SwitchDialog::SwitchDialog(Switch* object)
	: Decker::ui::Dialog(900, 600, Dialog::Buttons::OK | Dialog::Buttons::Reset)
{
	this->object=object;
	current_element=Element::Base;
	current_element_color_frame=NULL;
	color_target=NULL;

	setWindowTitle(ppl7::ToString("Switch, ID: %d", object->id));
	//ppl7::grafix::Rect client=clientRect();
	int y=0;
	//int sw=(client.width()) / 2;
	int sw=480;


	addChild(new ppltk::Label(sw, y, 120, 30, "Targets: "));
	y+=35;
	for (int i=0;i < 15;i++) {
		addChild(new ppltk::Label(sw + 20, y, 80, 30, ppl7::ToString("Object %d: ", i + 1)));
		target_id[i]=new ppltk::SpinBox(sw + 100, y, 100, 30, object->targets[i].object_id);
		target_id[i]->setLimits(0, 65535);
		target_id[i]->setEventHandler(this);
		addChild(target_id[i]);

		ppltk::Frame* frame=new ppltk::Frame(sw + 210, y, 210, 30, ppltk::Frame::BorderStyle::NoBorder);
		frame->setTransparent(true);

		target_state_on[i]=new ppltk::RadioButton(0, 0, 50, 30, "on", object->targets[i].state == Switch::TargetState::enable);
		target_state_on[i]->setEventHandler(this);
		frame->addChild(target_state_on[i]);
		target_state_off[i]=new ppltk::RadioButton(50, 0, 60, 30, "off", object->targets[i].state == Switch::TargetState::disable);
		target_state_off[i]->setEventHandler(this);
		frame->addChild(target_state_off[i]);
		target_state_trigger[i]=new ppltk::RadioButton(100, 0, 90, 30, "trigger", object->targets[i].state == Switch::TargetState::trigger);
		target_state_trigger[i]->setEventHandler(this);
		frame->addChild(target_state_trigger[i]);
		addChild(frame);

		y+=30;
	}
	y=0;


	addChild(new ppltk::Label(0, y, 100, 30, "Switch-Style: "));
	switch_style=new ppltk::ComboBox(100, y, 300, 30);
	switch_style->add("Switch with lever", "0");
	switch_style->add("Switch with lever and top", "1");
	switch_style->add("Switch with small lever and top", "2");
	switch_style->add("Light switch", "3");
	switch_style->add("Lever", "4");
	switch_style->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->switch_style)));
	switch_style->setEventHandler(this);
	addChild(switch_style);
	y+=40;

	addChild(new ppltk::Label(0, y, 100, 30, "Flags: "));
	initial_state=new ppltk::CheckBox(100, y, 160, 30, "Initial state: on", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);

	current_state=new ppltk::CheckBox(260, y, 160, 30, "current state: on", object->current_state);
	current_state->setEventHandler(this);
	addChild(current_state);
	y+=35;



	visible_at_playtime=new ppltk::CheckBox(100, y, 160, 30, "switch is visible", object->visibleAtPlaytime);
	visible_at_playtime->setEventHandler(this);
	addChild(visible_at_playtime);

	one_time_switch=new ppltk::CheckBox(260, y, 160, 30, "one time switch", object->one_time_switch);
	one_time_switch->setEventHandler(this);
	addChild(one_time_switch);
	y+=35;

	auto_toggle_on_collision=new ppltk::CheckBox(100, y, 220, 30, "auto toggle on collision", object->auto_toggle_on_collision);
	auto_toggle_on_collision->setEventHandler(this);
	addChild(auto_toggle_on_collision);
	y+=35;

	addChild(new ppltk::Label(0, y, 80, 30, "Colors:"));
	current_element_label=new ppltk::Label(170, y, 300, 30, "Base");
	addChild(current_element_label);
	y+=35;
	ColorPalette& palette=GetColorPalette();
	colorframe=new Decker::ui::ColorSelectionFrame(170, y, 300, 300, palette);
	colorframe->setEventHandler(this);
	this->addChild(colorframe);

	button_color_base=new ppltk::Button(0, y, 100, 30, "Base");
	button_color_base->setEventHandler(this);
	addChild(button_color_base);
	frame_color_base=new ppltk::Frame(105, y, 60, 30, ppltk::Frame::Inset);
	frame_color_base->setBackgroundColor(palette.getColor(object->color_base));
	addChild(frame_color_base);
	y+=35;

	button_color_lever=new ppltk::Button(0, y, 100, 30, "Lever");
	button_color_lever->setEventHandler(this);
	addChild(button_color_lever);
	frame_color_lever=new ppltk::Frame(105, y, 60, 30, ppltk::Frame::Inset);
	frame_color_lever->setBackgroundColor(palette.getColor(object->color_lever));
	addChild(frame_color_lever);
	y+=35;

	button_color_button=new ppltk::Button(0, y, 100, 30, "Button");
	button_color_button->setEventHandler(this);
	addChild(button_color_button);
	frame_color_button=new ppltk::Frame(105, y, 60, 30, ppltk::Frame::Inset);
	frame_color_button->setBackgroundColor(palette.getColor(object->color_button));
	addChild(frame_color_button);
	y+=35;
	setCurrentElement(Element::Base);


}

void SwitchDialog::setCurrentElement(Element element)
{
	current_element=element;
	switch (element) {
		case Element::Base:
			current_element_label->setText("Base");
			current_element_color_frame=frame_color_base;
			color_target=&object->color_base;
			break;
		case Element::Lever:
			current_element_label->setText("Lever");
			current_element_color_frame=frame_color_lever;
			color_target=&object->color_lever;
			break;
		case Element::Button:
			current_element_label->setText("Button");
			current_element_color_frame=frame_color_button;
			color_target=&object->color_button;
			break;
	}
	colorframe->setColorIndex(*color_target);
}


void SwitchDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	ppltk::Widget* widget=event->widget();
	if (widget == switch_style) {
		object->switch_style=static_cast<Switch::SwitchStyle>(switch_style->currentIdentifier().toInt());
		object->init();
	}
	if (widget == colorframe && color_target != NULL) {
		*color_target=value;
		if (current_element_color_frame)
			current_element_color_frame->setBackgroundColor(GetColorPalette().getColor(value));

	}
}

void SwitchDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	for (int i=0;i < 15;i++) {
		if (event->widget() == target_id[i]) {
			//handled=true;
			object->targets[i].object_id=(value & 0xffff);
		}
	}
}

void SwitchDialog::mouseDownEvent(ppltk::MouseEvent* event)
{
	ppltk::Widget* widget=event->widget();
	if (widget == button_color_base || widget == frame_color_base) setCurrentElement(Element::Base);
	else if (widget == button_color_lever || widget == frame_color_lever) setCurrentElement(Element::Lever);
	else if (widget == button_color_button || widget == frame_color_button) setCurrentElement(Element::Button);
	else Dialog::mouseDownEvent(event);
}



void SwitchDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (checked) {
		for (int i=0;i < 15;i++) {
			if (event->widget() == target_state_on[i]) {
				object->targets[i].state=Switch::TargetState::enable;
			} else 	if (event->widget() == target_state_off[i]) {
				object->targets[i].state=Switch::TargetState::disable;
			} else 	if (event->widget() == target_state_trigger[i]) {
				object->targets[i].state=Switch::TargetState::trigger;
			}
		}
	}
	if (event->widget() == visible_at_playtime) object->visibleAtPlaytime=checked;
	else if (event->widget() == initial_state) object->initial_state=checked;
	else if (event->widget() == current_state) object->toggle(checked);
	else if (event->widget() == one_time_switch) object->one_time_switch=checked;
	else if (event->widget() == auto_toggle_on_collision) object->auto_toggle_on_collision=checked;

}
void SwitchDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button == Dialog::Buttons::Reset) object->reset();
}


}	// EOF namespace Decker::Objects
