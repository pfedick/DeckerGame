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
	warp_to_id=0;
	next_animation=0.0f;
	state=DoorState::closed;
	color_frame=7; // Frame yellow in default palette
	color_door=2; // Door white
	color_background=0;
	door_type=DoorType::windowed;
	initial_open=false;
	collisionDetection=true;
	pixelExactCollision=false;
	sprite_set=Spriteset::Doors;
	sprite_no=0;
	sprite_no_representation=4;
	door_sprite_no=30;
	orientation=DoorOrientation::front;
	auto_opens_on_collision=true;
	can_close_again=false;
	use_background_color=false;
	cooldown=0.0f;
	cooldown_for_locked_door_text=0.0f;
	myLayer=Layer::BehindPlayer;
	text_for_closed_door_said=false;
	warpside=WarpSide::left;
	autowarp_when_open=false;
	close_when_passed=false;
	init();
}

void Door::reset()
{
	Object::reset();
	state=DoorState::closed;
	collisionDetection=true;
	next_animation=0.0f;
	text_for_closed_door_said=false;
	init();
}

void Door::init()
{
	door_sprite_no=5 + 60 * (int)door_type;
	if (door_type == DoorType::no_door) door_sprite_no=0;
	switch (orientation) {
	case DoorOrientation::right:
		sprite_no=0;
		door_sprite_no+=0;
		break;
	case DoorOrientation::left:
		sprite_no=2;
		door_sprite_no+=15;
		break;
	case DoorOrientation::front:
		sprite_no=4;
		door_sprite_no+=30;
		break;
	case DoorOrientation::back:
		sprite_no=4;
		door_sprite_no+=45;
		break;
	}

	if (state == DoorState::open) door_sprite_no+=14;
}

void Door::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	if (state == DoorState::opening && time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			door_sprite_no=new_sprite;
		}
		if (animation.isFinished()) {
			state=DoorState::open;
			if (warp_to_id == 0) collisionDetection=false;
		}
	} else if (state == DoorState::closing && time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			door_sprite_no=new_sprite;
		}
		if (animation.isFinished()) {
			state=DoorState::closed;
			collisionDetection=true;
		}
	}

}

void Door::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	const ColorPalette& palette=GetColorPalette();

	if (use_background_color == true &&
		(orientation == DoorOrientation::front || orientation == DoorOrientation::back)) {

		ppl7::grafix::Color bg=palette.getColor(color_background);
		SDL_BlendMode currentBlendMode;
		SDL_Rect r;
		r.x=p.x + coords.x - 2 * TILE_WIDTH;
		r.y=p.y + coords.y - 6 * TILE_HEIGHT;
		r.w=4 * TILE_WIDTH;
		r.h=6 * TILE_HEIGHT;
		SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, bg.red(), bg.green(), bg.blue(), 255);
		SDL_RenderFillRect(renderer, &r);
		SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
	}

	switch (orientation) {
	case DoorOrientation::left:
	case DoorOrientation::right:
	case DoorOrientation::front:

		texture->draw(renderer,
			p.x + coords.x,
			p.y + coords.y,
			sprite_no, palette.getColor(color_frame));

		if (door_type != DoorType::no_door) {
			texture->draw(renderer,
				p.x + coords.x,
				p.y + coords.y,
				door_sprite_no, palette.getColor(color_door));
		}
		if (orientation == DoorOrientation::right) {
			texture->draw(renderer,
				p.x + coords.x,
				p.y + coords.y,
				1, palette.getColor(color_frame));
		} else if (orientation == DoorOrientation::left) {
			texture->draw(renderer,
				p.x + coords.x,
				p.y + coords.y,
				3, palette.getColor(color_frame));
		}
		break;
	case DoorOrientation::back:
		texture->draw(renderer,
			p.x + coords.x,
			p.y + coords.y,
			door_sprite_no, palette.getColor(color_door));
		texture->draw(renderer,
			p.x + coords.x,
			p.y + coords.y,
			sprite_no, palette.getColor(color_frame));
		break;
	}
}

void Door::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	Door::draw(renderer, coords);
}

void Door::handleCollision(Player* player, const Collision& collision)
{
	Player::Keys keyboard=player->getKeyboardMatrix();
	if (state != DoorState::open) {
		if (orientation == DoorOrientation::left) {
			if (collision.objectLeft() && player->x > p.x - 48) player->x=p.x - 48;
			if (collision.objectRight() && player->x < p.x + 64) player->x=p.x + 64;
		} else if (orientation == DoorOrientation::right) {
			if (collision.objectRight() && player->x < p.x + 48) player->x=p.x + 48;
			if (collision.objectLeft() && player->x > p.x - 64) player->x=p.x - 64;
		}
		if (state == DoorState::closed) {
			//int keyboard=player->getKeyboardMatrix();
			if (auto_opens_on_collision && (key_id == 0 || player->isInInventory(key_id))) {
				state=DoorState::opening;
				if (door_type == DoorType::lattice_metalic || door_type == DoorType::lattice_opaque) getAudioPool().playOnce(AudioClip::metaldoor, 0.5f);
				else getAudioPool().playOnce(AudioClip::door_open, 0.5f);
				animation.startSequence(door_sprite_no, door_sprite_no + 14, false, door_sprite_no + 14);
			}
		}
		if (state == DoorState::closed) {
			if (!text_for_closed_door_said) {
				if (key_id != 0 && player->isInInventory(key_id))  player->speak(20);
				else if (key_id != 0) player->speak(6);
				text_for_closed_door_said=true;
			} else if (key_id != 0 && player->isInInventory(key_id) && player->time < cooldown_for_locked_door_text) {
				player->speak(7);
			}
			cooldown_for_locked_door_text=player->time + 20.0f;
		}
	}

	double now=ppl7::GetMicrotime();
	if (!(keyboard.matrix & KeyboardKeys::Action)) cooldown=0.0f;
	if (state == DoorState::closed) {
		if (warp_to_id > 0 && (keyboard.matrix & KeyboardKeys::Action)) {
			if (key_id == 0 || player->isInInventory(key_id)) {
				state=DoorState::opening;
				if (door_type == DoorType::lattice_metalic || door_type == DoorType::lattice_opaque) getAudioPool().playOnce(AudioClip::metaldoor, 0.5f);
				else getAudioPool().playOnce(AudioClip::door_open, 0.5f);

				animation.startSequence(door_sprite_no, door_sprite_no + 14, false, door_sprite_no + 14);
			}
		}
	} else if (state == DoorState::open && warp_to_id > 0 && cooldown < now) {
		if ((keyboard.matrix & KeyboardKeys::Action) || autowarp_when_open) {
			Door* target=static_cast<Door*>(GetObjectSystem()->getObject(warp_to_id));
			if (target != NULL && target->type() == Type::ObjectType::Door) {
				//ppl7::PrintDebugTime("Wir haben eine Zieltuer!\n");
				target->state=DoorState::open;
				target->init();
				if (close_when_passed) {
					target->state=DoorState::closing;
					getAudioPool().playOnce(AudioClip::door_close, 0.5f);
					target->animation.startSequence(target->door_sprite_no, target->door_sprite_no - 14, false, target->door_sprite_no - 14);
					target->cooldown=now + 2.0f;
					state=DoorState::closing;
					animation.startSequence(door_sprite_no, door_sprite_no - 14, false, door_sprite_no - 14);
				} else {
					target->cooldown=now + 1.0f;
				}
				if (target->orientation == DoorOrientation::left || target->orientation == DoorOrientation::right) {
					if (target->warpside == WarpSide::left) player->move(target->p.x - 2 * TILE_WIDTH, target->p.y);
					else player->move(target->p.x + 2 * TILE_WIDTH, target->p.y);
				} else {
					player->move(target->p.x, target->p.y);
				}
			}
		}
	}
}

void Door::trigger(Object* source)
{
	if (state == DoorState::closed) {
		state=DoorState::opening;
		if (door_type == DoorType::lattice_metalic || door_type == DoorType::lattice_opaque) getAudioPool().playOnce(AudioClip::metaldoor, 0.5f);
		else getAudioPool().playOnce(AudioClip::door_open, 0.5f);
		animation.startSequence(door_sprite_no, door_sprite_no + 14, false, door_sprite_no + 14);
	}
}

size_t Door::saveSize() const
{
	return Object::saveSize() + 15;
}

size_t Door::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 2);		// Object Version

	ppl7::Poke32(buffer + bytes + 1, key_id);
	ppl7::Poke32(buffer + bytes + 5, warp_to_id);
	ppl7::Poke8(buffer + bytes + 9, static_cast<int>(orientation));
	ppl7::Poke8(buffer + bytes + 10, static_cast<int>(door_type));
	int flags=0;
	if (initial_open) flags|=1;
	if (auto_opens_on_collision) flags|=2;
	if (can_close_again) flags|=4;
	if (use_background_color) flags|=8;
	if (warpside == WarpSide::right) flags|=16;
	if (autowarp_when_open) flags|=32;
	if (close_when_passed) flags|=64;

	ppl7::Poke8(buffer + bytes + 11, (unsigned char)flags);
	ppl7::Poke8(buffer + bytes + 12, color_frame);
	ppl7::Poke8(buffer + bytes + 13, color_door);
	ppl7::Poke8(buffer + bytes + 14, color_background);
	return bytes + 15;
}

void Door::load_v1(const unsigned char* buffer, size_t size)
{
	key_id=ppl7::Peek32(buffer + 1);
	warp_to_id=0;

	int old_frame_type=ppl7::Peek8(buffer + 5);
	switch (old_frame_type) {
	case 0: // black
		color_frame=9;
		break;
	case 1: // blue
		color_frame=8;
		break;
	case 2: // dark brown
		color_frame=12;
		break;
	case 3: // dark stone grey
		color_frame=4;
		break;
	case 4: // green
		color_frame=5;
		break;
	case 5: // medium stone grey
		color_frame=3;
		break;
	case 6: // orange
		color_frame=14;
		break;
	case 7: // red
		color_frame=6;
		break;
	case 8: // reddish brown
		color_frame=13;
		break;
	case 9: // white
		color_frame=2;
		break;
	default: // 10 = yellow
		color_frame=7;
		break;
	}

	int old_door_type=ppl7::Peek8(buffer + 6);
	switch (old_door_type) {
	case 0: // metal lattice bar
		door_type=DoorType::lattice_metalic;
		color_door=2;
		break;
	default: // white
		door_type=DoorType::windowed;
		color_door=2;
	}

	int flags=ppl7::Peek8(buffer + 7);
	initial_open=false;
	orientation=DoorOrientation::right;
	if (flags & 1) {
		initial_open=true;
		state=DoorState::open;
	}
	if (flags & 2) {
		orientation=DoorOrientation::left;
		p.x-=16;
		initial_p.x-=16;
	} else {
		p.x+=16;
		initial_p.x+=16;
	}
	auto_opens_on_collision=true;
	can_close_again=false;
	use_background_color=false;

}




size_t Door::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	myLayer=Layer::BehindPlayer;
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	warpside=WarpSide::left;
	if (version < 1) return 0;
	if (version == 1) {
		load_v1(buffer + bytes, size - bytes);
	} else if (version == 2) {
		key_id=ppl7::Peek32(buffer + bytes + 1);
		warp_to_id=ppl7::Peek32(buffer + bytes + 5);
		orientation=(DoorOrientation)ppl7::Peek8(buffer + bytes + 9);
		door_type=(DoorType)ppl7::Peek8(buffer + bytes + 10);
		int flags=ppl7::Peek8(buffer + bytes + 11);
		initial_open=false;
		if (flags & 1) {
			initial_open=true;
			state=DoorState::open;
		}
		auto_opens_on_collision=(flags & 2);
		can_close_again=(flags & 4);
		use_background_color=(flags & 8);
		if (flags & 16) warpside=WarpSide::right;
		autowarp_when_open= (flags & 32);
		close_when_passed= (flags & 64);

		color_frame=ppl7::Peek8(buffer + bytes + 12);
		color_door=ppl7::Peek8(buffer + bytes + 13);
		color_background=ppl7::Peek8(buffer + bytes + 14);
	}

	init();
	return size;
}

class DoorDialog : public Decker::ui::Dialog
{
private:
/*
	ppl7::tk::ComboBox* frame_type;
	ppl7::tk::ComboBox* door_type;
	ppl7::tk::ComboBox* initial_state;
	ppl7::tk::Button* reset;
	ppl7::tk::RadioButton* orientation_left;
	ppl7::tk::RadioButton* orientation_right;
	ppl7::tk::LineInput* key_id;
	*/
	ppl7::tk::ComboBox* door_type;
	ppl7::tk::ComboBox* door_orientation;
	ppl7::tk::CheckBox* auto_opens_on_collision;
	ppl7::tk::CheckBox* can_close_again;
	ppl7::tk::CheckBox* use_background_color;
	ppl7::tk::CheckBox* autowarp_when_open;
	ppl7::tk::CheckBox* close_when_passed;
	ppl7::tk::ComboBox* warp_side;


	ppl7::tk::RadioButton* initial_state_closed;
	ppl7::tk::RadioButton* initial_state_open;
	ppl7::tk::RadioButton* current_state_closed;
	ppl7::tk::RadioButton* current_state_open;

	ppl7::tk::SpinBox* key_id;
	ppl7::tk::SpinBox* warp_target_id;

	Decker::ui::ColorSelectionFrame* colorframe;
	ppl7::tk::Button* button_color_background;
	ppl7::tk::Button* button_color_frame;
	ppl7::tk::Button* button_color_door;

	ppl7::tk::Frame* frame_color_background;
	ppl7::tk::Frame* frame_color_frame;
	ppl7::tk::Frame* frame_color_door;


	ppl7::tk::Label* current_element_label;
	ppl7::tk::Frame* current_element_color_frame;
	int* color_target;

	enum class Element {
		Background,
		Frame,
		Door
	};
	Element current_element;

	Door* object;

	void setCurrentElement(Element element);

public:
	DoorDialog(Door* object);
	//~DoorDialog();

	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int64_t value);
	//virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent* event);
	virtual void dialogButtonEvent(Dialog::Buttons button) override;

};



void Door::openUi()
{
	DoorDialog* dialog=new DoorDialog(this);
	GetGameWindow()->addChild(dialog);
}

DoorDialog::DoorDialog(Door* object)
	: Decker::ui::Dialog(600, 600, Dialog::Buttons::OK | Dialog::Buttons::CopyAndPaste)
{
	current_element=Element::Background;
	current_element_color_frame=NULL;
	color_target=NULL;
	this->object=object;
	setWindowTitle("Door");
	ppl7::grafix::Rect client=clientRect();
	int y=0;
	//int sw=(client.width()) / 2;

	addChild(new ppl7::tk::Label(0, y, 150, 30, "Door Type:"));
	door_type=new ppl7::tk::ComboBox(150, y, 300, 30);
	door_type->add("windowed door", "0");
	door_type->add("flat door", "1");
	door_type->add("metal lattice", "2");
	door_type->add("colored lattice", "3");
	door_type->add("no door", "4");
	door_type->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->door_type)));
	door_type->setEventHandler(this);
	addChild(door_type);

	y+=35;
	addChild(new ppl7::tk::Label(0, y, 150, 30, "Orientation:"));
	door_orientation=new ppl7::tk::ComboBox(150, 40, 300, 30);
	door_orientation->add("left", "1");
	door_orientation->add("right", "0");
	door_orientation->add("open to front", "2");
	door_orientation->add("open to back", "3");
	door_orientation->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->orientation)));
	door_orientation->setEventHandler(this);
	addChild(door_orientation);
	y+=35;

	ppl7::tk::Frame* frame=new ppl7::tk::Frame(0, y, 310, 30, ppl7::tk::Frame::NoBorder);
	addChild(frame);

	frame->addChild(new ppl7::tk::Label(0, 0, 150, 30, "initial state:"));

	initial_state_closed=new ppl7::tk::RadioButton(150, 0, 80, 30, "closed");
	initial_state_closed->setEventHandler(this);
	frame->addChild(initial_state_closed);

	initial_state_open=new ppl7::tk::RadioButton(230, 0, 80, 30, "open");
	initial_state_open->setEventHandler(this);
	frame->addChild(initial_state_open);

	frame=new ppl7::tk::Frame(310, y, 240, 30, ppl7::tk::Frame::NoBorder);
	addChild(frame);
	frame->addChild(new ppl7::tk::Label(0, 0, 80, 30, "current:"));

	current_state_closed=new ppl7::tk::RadioButton(80, 0, 80, 30, "closed");
	current_state_closed->setEventHandler(this);
	frame->addChild(current_state_closed);

	current_state_open=new ppl7::tk::RadioButton(160, 0, 80, 30, "open");
	current_state_open->setEventHandler(this);
	frame->addChild(current_state_open);


	y+=35;

	addChild(new ppl7::tk::Label(0, y, 150, 30, "unlock with Object ID:"));
	key_id=new ppl7::tk::SpinBox(150, y, 100, 30);
	key_id->setLimits(0, 65535);
	key_id->setEventHandler(this);
	addChild(key_id);

	addChild(new ppl7::tk::Label(280, y, 150, 30, "transfer to object ID:"));
	warp_target_id=new ppl7::tk::SpinBox(430, y, 100, 30);
	warp_target_id->setLimits(0, 65535);
	warp_target_id->setEventHandler(this);
	addChild(warp_target_id);

	y+=40;
	addChild(new ppl7::tk::Label(0, y, 80, 30, "Colors:"));
	current_element_label=new ppl7::tk::Label(client.width() - 300, y, 300, 30, "Background");
	addChild(current_element_label);
	y+=35;

	ColorPalette& palette=GetColorPalette();
	colorframe=new Decker::ui::ColorSelectionFrame(client.width() - 300, y, 300, 300, palette);
	colorframe->setEventHandler(this);
	this->addChild(colorframe);

	button_color_background=new ppl7::tk::Button(30, y, 100, 30, "Background");
	button_color_background->setEventHandler(this);
	addChild(button_color_background);
	frame_color_background=new ppl7::tk::Frame(135, y, 60, 30, ppl7::tk::Frame::Inset);
	frame_color_background->setBackgroundColor(palette.getColor(object->color_background));
	addChild(frame_color_background);
	y+=35;

	button_color_frame=new ppl7::tk::Button(30, y, 100, 30, "Doorframe");
	button_color_frame->setEventHandler(this);
	addChild(button_color_frame);
	frame_color_frame=new ppl7::tk::Frame(135, y, 60, 30, ppl7::tk::Frame::Inset);
	frame_color_frame->setBackgroundColor(palette.getColor(object->color_frame));
	addChild(frame_color_frame);
	y+=35;

	button_color_door=new ppl7::tk::Button(30, y, 100, 30, "Door");
	button_color_door->setEventHandler(this);
	addChild(button_color_door);
	frame_color_door=new ppl7::tk::Frame(135, y, 60, 30, ppl7::tk::Frame::Inset);
	frame_color_door->setBackgroundColor(palette.getColor(object->color_door));
	addChild(frame_color_door);
	y+=35;

	use_background_color=new ppl7::tk::CheckBox(0, y, client.width(), 30, "use Background color");
	use_background_color->setEventHandler(this);
	addChild(use_background_color);
	y+=35;

	auto_opens_on_collision=new ppl7::tk::CheckBox(0, y, client.width(), 30, "automatically opens on collision");
	auto_opens_on_collision->setEventHandler(this);
	addChild(auto_opens_on_collision);
	y+=35;

	can_close_again=new ppl7::tk::CheckBox(0, y, client.width(), 30, "can be closed again");
	can_close_again->setEventHandler(this);
	addChild(can_close_again);
	y+=35;

	close_when_passed=new ppl7::tk::CheckBox(0, y, client.width(), 30, "close when passed", object->close_when_passed);
	close_when_passed->setEventHandler(this);
	addChild(close_when_passed);
	y+=35;

	autowarp_when_open=new ppl7::tk::CheckBox(0, y, client.width(), 30, "auto warp when open", object->autowarp_when_open);
	autowarp_when_open->setEventHandler(this);
	addChild(autowarp_when_open);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 140, 30, "Warp to door side:"));
	warp_side=new ppl7::tk::ComboBox(140, y, 100, 30);
	warp_side->add("left", "0");
	warp_side->add("right", "1");
	warp_side->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->warpside)));
	warp_side->setEventHandler(this);
	addChild(warp_side);


	if (object->state == Door::DoorState::closed || object->state == Door::DoorState::closing) {
		current_state_closed->setChecked(true);
		//current_state_open->setChecked(false);
	} else {
		//current_state_closed->setChecked(false);
		current_state_open->setChecked(true);
	}
	if (object->initial_open) {
		//initial_state_closed->setChecked(false);
		initial_state_open->setChecked(true);
	} else {
		initial_state_closed->setChecked(true);
		//initial_state_open->setChecked(false);
	}

	key_id->setValue(object->key_id);
	warp_target_id->setValue(object->warp_to_id);
	use_background_color->setChecked(object->use_background_color);
	auto_opens_on_collision->setChecked(object->auto_opens_on_collision);
	can_close_again->setChecked(object->can_close_again);

	setCurrentElement(Element::Background);

}


void DoorDialog::setCurrentElement(Element element)
{
	current_element=element;
	switch (element) {
	case Element::Background:
		current_element_label->setText("Background");
		current_element_color_frame=frame_color_background;
		color_target=&object->color_background;
		break;
	case Element::Frame:
		current_element_label->setText("Doorframe");
		current_element_color_frame=frame_color_frame;
		color_target=&object->color_frame;
		break;
	case Element::Door:
		current_element_label->setText("Door");
		current_element_color_frame=frame_color_door;
		color_target=&object->color_door;
		break;
	}

	colorframe->setColorIndex(*color_target);
}


void DoorDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	ppl7::tk::Widget* widget=event->widget();
	if (widget == use_background_color) {
		object->use_background_color=checked;
	} else if (widget == auto_opens_on_collision) {
		object->auto_opens_on_collision=checked;
	} else if (widget == can_close_again) {
		object->can_close_again=checked;
	} else if (widget == current_state_closed) {
		//current_state_open->setChecked(!checked);
		if (checked) object->state=Door::DoorState::closed;
		else object->state=Door::DoorState::open;
		object->init();
	} else if (widget == current_state_open) {
		//current_state_closed->setChecked(!checked);
		if (checked) object->state=Door::DoorState::open;
		else object->state=Door::DoorState::closed;
		object->init();
	} else if (widget == initial_state_closed) {
		//initial_state_open->setChecked(!checked);
		if (checked) {
			object->initial_open=false;
		} else {
			object->initial_open=true;
		}
		object->init();
	} else if (widget == initial_state_open) {
		//initial_state_closed->setChecked(!checked);
		if (checked) {
			object->initial_open=true;
		} else {
			object->initial_open=false;
		}
		object->init();
	} else if (widget == autowarp_when_open) {
		object->autowarp_when_open=checked;
	} else if (widget == close_when_passed) {
		object->close_when_passed=close_when_passed;
	}

}

void DoorDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	ppl7::tk::Widget* widget=event->widget();
	if (widget == door_type) {
		object->door_type=static_cast<Door::DoorType>(door_type->currentIdentifier().toInt());
		object->init();
	} else if (widget == door_orientation) {
		object->orientation=static_cast<Door::DoorOrientation>(door_orientation->currentIdentifier().toInt());
		object->init();
	} else if (widget == warp_side) {
		object->warpside=static_cast<Door::WarpSide>(warp_side->currentIdentifier().toInt());
		object->init();

	}
	if (widget == colorframe && color_target != NULL) {
		*color_target=value;
		if (current_element_color_frame)
			current_element_color_frame->setBackgroundColor(GetColorPalette().getColor(value));
	}
}


void DoorDialog::valueChangedEvent(ppl7::tk::Event* event, int64_t value)
{
	ppl7::tk::Widget* widget=event->widget();
	if (widget == key_id) object->key_id=value;
	if (widget == warp_target_id) object->warp_to_id=value;

}

void DoorDialog::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	ppl7::tk::Widget* widget=event->widget();
	if (widget == button_color_background || widget == frame_color_background) setCurrentElement(Element::Background);
	else if (widget == button_color_door) setCurrentElement(Element::Door);
	else if (widget == button_color_frame) setCurrentElement(Element::Frame);
	else Dialog::mouseDownEvent(event);
}



static Door clipboard;

void DoorDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button == Dialog::Buttons::Copy) {
		clipboard.color_frame=object->color_frame;
		clipboard.color_door=object->color_door;
		clipboard.color_background=object->color_background;
		clipboard.key_id=object->key_id;
		clipboard.warp_to_id=object->warp_to_id;
		clipboard.orientation=object->orientation;
		clipboard.door_type=object->door_type;
		clipboard.state=object->state;
		clipboard.initial_open=object->initial_open;
		clipboard.auto_opens_on_collision=object->auto_opens_on_collision;
		clipboard.can_close_again=object->can_close_again;
		clipboard.use_background_color=object->use_background_color;

	} else if (button == Dialog::Buttons::Paste) {
		object->color_frame=clipboard.color_frame;
		object->color_door=clipboard.color_door;
		object->color_background=clipboard.color_background;
		object->key_id=clipboard.key_id;
		object->warp_to_id=clipboard.warp_to_id;
		object->orientation=clipboard.orientation;
		object->door_type=clipboard.door_type;
		object->state=clipboard.state;
		object->initial_open=clipboard.initial_open;
		object->auto_opens_on_collision=clipboard.auto_opens_on_collision;
		object->can_close_again=clipboard.can_close_again;
		object->use_background_color=clipboard.use_background_color;
		object->init();
	}
}


#ifdef DOORv1
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

	frame_type=new ppl7::tk::ComboBox(120, 0, 400, 30);
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

	door_type=new ppl7::tk::ComboBox(120, 40, 400, 30);
	door_type->add("metal lattice bar", "0");
	door_type->add("white", "1");
	door_type->setCurrentIdentifier(ppl7::ToString("%d", object->door_type));
	door_type->setEventHandler(this);
	addChild(door_type);

	initial_state=new ppl7::tk::ComboBox(120, 80, 400, 30);
	initial_state->add("closed", "0");
	initial_state->add("open", "1");
	initial_state->setCurrentIdentifier(ppl7::ToString("%d", (int)object->initial_open));
	initial_state->setEventHandler(this);
	addChild(initial_state);

	orientation_left=new ppl7::tk::RadioButton(120, 120, 80, 30, "left");
	orientation_left->setEventHandler(this);
	addChild(orientation_left);

	orientation_right=new ppl7::tk::RadioButton(200, 120, 80, 30, "right");
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

#endif



}	// EOF namespace Decker::Objects
