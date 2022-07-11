#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "player.h"
#include "widgets.h"

namespace Decker::Objects {



Representation Door::representation()
{
	return Representation(Spriteset::Doors, 20);
}


Door::Door()
	: Decker::Objects::Object(Type::Door)
{
	key_id=0;
	next_animation=0.0f;
	state=0;
	frame_type=10;
	door_type=1;
	initial_open=false;
	collisionDetection=true;
	pixelExactCollision=false;
	save_size+=7;
	sprite_set=Spriteset::Doors;
	sprite_no=20;
	sprite_no_representation=20;
	door_sprite_no=30;
	left_sided=false;
	init();
}

void Door::reset()
{
	Object::reset();
	state=0;
	collisionDetection=true;
	next_animation=0.0f;
	init();
}

void Door::init()
{
	sprite_no=2 * frame_type + (int)left_sided;
	door_sprite_no=30 + 30 * door_type + 15 * (int)left_sided;

	if (state == 2) door_sprite_no+=14;
}

void Door::update(double time, TileTypePlane& ttplane, Player& player)
{
	if (state == 1 && time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			door_sprite_no=new_sprite;
		}
		if (animation.isFinished()) {
			state=2;
			collisionDetection=false;
		}
	}

}

void Door::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	texture->draw(renderer,
		p.x + coords.x,
		p.y + coords.y,
		sprite_no);
	texture->draw(renderer,
		p.x + coords.x,
		p.y + coords.y,
		door_sprite_no);

}

void Door::handleCollision(Player* player, const Collision& collision)
{
	if (state < 2) {
		//printf ("Door::handleCollision\n");
		if (collision.objectRight() && player->x < p.x + 48) player->x=p.x + 48;
		if (collision.objectLeft() && player->x > p.x - 16) player->x=p.x - 16;
		if (state == 0) {
			//int keyboard=player->getKeyboardMatrix();
			if (player->isInInventory(key_id)) {
				state=1;
				animation.startSequence(door_sprite_no, door_sprite_no + 14, false, door_sprite_no + 14);
			}
		}
	}
}

size_t Door::save(unsigned char* buffer, size_t size)
{
	if (size < save_size) return 0;
	size_t bytes=Object::save(buffer, size);
	ppl7::Poke32(buffer + bytes, key_id);
	ppl7::Poke8(buffer + bytes + 4, frame_type);
	ppl7::Poke8(buffer + bytes + 5, door_type);
	int flags=0;
	if (initial_open) flags|=1;
	if (left_sided) flags|=2;
	ppl7::Poke8(buffer + bytes + 6, (unsigned char)initial_open);
	return bytes + 7;
}

size_t Door::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < save_size) return 0;
	key_id=ppl7::Peek32(buffer + bytes);
	frame_type=ppl7::Peek8(buffer + bytes + 4);
	door_type=ppl7::Peek8(buffer + bytes + 5);
	int flags=ppl7::Peek8(buffer + bytes + 6);
	initial_open=false;
	left_sided=false;
	if (flags & 1) initial_open=true;
	if (flags & 2) left_sided=true;
	init();
	return size;
}

class DoorDialog : public Decker::ui::Dialog
{
private:
	Decker::ui::ComboBox* frame_type;
	Decker::ui::ComboBox* door_type;
	Decker::ui::ComboBox* initial_state;
	ppl7::tk::Button* reset;
	Decker::ui::RadioButton* orientation_left;
	Decker::ui::RadioButton* orientation_right;
	ppl7::tk::LineInput* key_id;

	Door* object;

public:
	DoorDialog(Door* object);
	//~DoorDialog();

	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent* event);
};



void Door::openUi()
{
	DoorDialog* dialog=new DoorDialog(this);
	GetGameWindow()->addChild(dialog);
}

DoorDialog::DoorDialog(Door* object)
	: Decker::ui::Dialog(640, 480)
{
	this->object=object;
	setWindowTitle("Door");
	addChild(new ppl7::tk::Label(0, 0, 120, 30, "Frame-Color: "));
	addChild(new ppl7::tk::Label(0, 40, 120, 30, "Door-Type: "));
	addChild(new ppl7::tk::Label(0, 80, 120, 30, "Initial state: "));
	addChild(new ppl7::tk::Label(0, 120, 120, 30, "Orientation: "));
	addChild(new ppl7::tk::Label(0, 160, 120, 30, "Open with: "));
	addChild(new ppl7::tk::Label(220, 160, 120, 30, " (Object-ID)"));

	frame_type=new Decker::ui::ComboBox(120, 0, 400, 30);
	frame_type->add("black", "0");
	frame_type->add("dark stone grey", "3");
	frame_type->add("medium stone grey", "5");
	frame_type->add("white", "9");
	frame_type->add("blue", "1");
	frame_type->add("green", "4");
	frame_type->add("red", "7");
	frame_type->add("dark brown", "2");
	frame_type->add("reddish brown", "8");
	frame_type->add("orange", "6");
	frame_type->add("yellow", "10");
	frame_type->setCurrentIdentifier(ppl7::ToString("%d", object->frame_type));
	frame_type->setEventHandler(this);
	addChild(frame_type);

	door_type=new Decker::ui::ComboBox(120, 40, 400, 30);
	door_type->add("metal lattice bar", "0");
	door_type->add("white", "1");
	door_type->setCurrentIdentifier(ppl7::ToString("%d", object->door_type));
	door_type->setEventHandler(this);
	addChild(door_type);

	initial_state=new Decker::ui::ComboBox(120, 80, 400, 30);
	initial_state->add("closed", "0");
	initial_state->add("open", "1");
	initial_state->setCurrentIdentifier(ppl7::ToString("%d", (int)object->initial_open));
	initial_state->setEventHandler(this);
	addChild(initial_state);

	orientation_left=new Decker::ui::RadioButton(120, 120, 80, 30, "left");
	orientation_left->setEventHandler(this);
	addChild(orientation_left);

	orientation_right=new Decker::ui::RadioButton(200, 120, 80, 30, "right");
	orientation_right->setEventHandler(this);
	addChild(orientation_right);
	if (object->left_sided) orientation_left->setChecked(true);

	key_id=new ppl7::tk::LineInput(120, 160, 100, 30, ppl7::ToString("%d", object->key_id));
	key_id->setEventHandler(this);
	addChild(key_id);

	reset=new ppl7::tk::Button(0, 240, 80, 30, "Reset");
	reset->setEventHandler(this);
	addChild(reset);

}

void DoorDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == frame_type) {
		object->frame_type=frame_type->currentIdentifier().toInt();
		object->init();
	} else if (event->widget() == door_type) {
		object->door_type=door_type->currentIdentifier().toInt();
		object->init();
	} else if (event->widget() == initial_state) {
		object->init();
		object->initial_open=door_type->currentIdentifier().toBool();
	}

}

void DoorDialog::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == reset) object->reset();
	else Decker::ui::Dialog::mouseDownEvent(event);
}

void DoorDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	if (event->widget() == orientation_left && checked == true) {
		object->left_sided=true;
		object->init();
	} else if (event->widget() == orientation_right && checked == true) {
		object->left_sided=false;
		object->init();

	}
}

void DoorDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	if (event->widget() == key_id) {
		object->key_id=text.toInt();
		object->init();
	}
}




}	// EOF namespace Decker::Objects
