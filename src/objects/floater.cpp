#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

Floater::Floater(Type::ObjectType type)
	: Object(type)
{
	sprite_no_representation=115;
	direction=0;
	collisionDetection=true;
	next_animation=0.0f;
	next_state=ppl7::GetMicrotime() + 5.0f;
	state=0;
	flame_sprite1=152;
	flame_sprite2=152;
	audio=NULL;
	initial_state=enabled;
	current_state=enabled;
	floater_type=0;
	save_size+=2;
	init();
}

Floater::~Floater()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void Floater::init()
{
	if (type() == Type::ObjectType::FloaterHorizontal) {
		sprite_no_representation=115;
		sprite_no=115;
	} else {
		sprite_no_representation=116;
		sprite_no=116;
	}
	if (floater_type == 1) {
		sprite_no_representation+=2;
		sprite_no+=2;
	}
}


void Floater::update(double time, TileTypePlane& ttplane, Player& player)
{
	if (current_state == false && (state == 0 || state == 2)) return;
	AudioPool& pool=getAudioPool();
	if (!audio) {
		audio=pool.getInstance(AudioClip::thruster);
		audio->setVolume(0.3f);
		audio->setPositional(p, 1800);
		audio->setLoop(true);
	}
	audio->setPositional(p, 1800);
	if (direction == 0) {		// horizontal
		if (next_state < time && (state == 0 || state == 2)) {
			state++;
			pool.playInstance(audio);
		} else if (state == 1) {
			velocity.x=4;
			velocity.y=0;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 64, p.y));
			if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker) {
				velocity.setPoint(0, 0);
				state=2;
				next_state=time + 5.0f;
				pool.stopInstace(audio);
			}
			if (time > next_animation) {
				flame_sprite1=ppl7::rand(152, 161);
				next_animation=time + 0.06f;
			}
		} else if (state == 3) {
			velocity.x=-4;
			velocity.y=0;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 64, p.y));
			if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker) {
				velocity.setPoint(0, 0);
				state=0;
				pool.stopInstace(audio);
				next_state=time + 5.0f;
			}
			if (time > next_animation) {
				flame_sprite1=ppl7::rand(162, 171);
				next_animation=time + 0.06f;
			}
		}
	} else { // vertical
		if (next_state < time && (state == 0 || state == 2)) {
			state++;
			pool.playInstance(audio);
		} else if (state == 1) {
			velocity.x=0;
			velocity.y=4;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 64));
			if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker) {
				velocity.setPoint(0, 0);
				state=2;
				next_state=time + 5.0f;
				pool.stopInstace(audio);
			}
			if (time > next_animation) {
				flame_sprite1=ppl7::rand(172, 181);
				flame_sprite2=ppl7::rand(172, 181);
				next_animation=time + 0.06f;
			}
		} else if (state == 3) {
			velocity.x=0;
			velocity.y=-4;
			p+=velocity;
			updateBoundary();
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y - 64));
			if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker) {
				velocity.setPoint(0, 0);
				state=0;
				pool.stopInstace(audio);
				next_state=time + 5.0f;
			}
			if (time > next_animation) {
				flame_sprite1=ppl7::rand(172, 181);
				flame_sprite2=ppl7::rand(172, 181);
				next_animation=time + 0.06f;
			}
		}
	}
}

void Floater::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	if (direction == 0) {
		if (state == 1) texture->draw(renderer,
			p.x + coords.x - 128 - 32,
			p.y + coords.y,
			flame_sprite1);
		else if (state == 3) texture->draw(renderer,
			p.x + coords.x + 128 + 32,
			p.y + coords.y,
			flame_sprite1);
	} else if (direction == 1 && (state == 1 || state == 3)) {
		texture->draw(renderer,
			p.x + coords.x - 16,
			p.y + coords.y + 112,
			flame_sprite1);
		texture->draw(renderer,
			p.x + coords.x + 16,
			p.y + coords.y + 112,
			flame_sprite2);
	}
	Object::draw(renderer, coords);
}

void Floater::handleCollision(Player* player, const Collision& collision)
{
	if (collision.onFoot()) {
		//printf ("collision with floater\n");
		player->setStandingOnObject(this);
		player->x+=velocity.x;
		player->y+=velocity.y;
	}
}

void Floater::toggle(bool enable, Object* source)
{
	current_state=enable;
	//printf ("Floater::toggle %d: %d\n", id, (int)enable);
}

void Floater::reset()
{
	Object::reset();
	current_state=initial_state;
	state=0;
	if (audio) {
		AudioPool& pool=getAudioPool();
		pool.stopInstace(audio);
	}
}

size_t Floater::save(unsigned char* buffer, size_t size)
{
	if (size < save_size) return 0;
	size_t bytes=Object::save(buffer, size);
	ppl7::Poke8(buffer + bytes, floater_type);
	ppl7::Poke8(buffer + bytes + 1, initial_state);
	return bytes + 2;
}

size_t Floater::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	//printf("lade floater, basisbytes: %d, size: %d, save_size=%d\n", bytes, size, save_size);
	if (bytes == 0 || size < save_size) return 0;
	floater_type=ppl7::Peek8(buffer + bytes);
	initial_state=(bool)(ppl7::Peek8(buffer + bytes + 1) & 1);
	current_state=initial_state;
	//printf("floater-type: %d, initial_state=%d\n", floater_type, initial_state);
	init();
	return size;
}

class FloaterDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::ComboBox* floater_type;
	ppl7::tk::CheckBox* initial_state;
	Floater* object;

public:
	FloaterDialog(Floater* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	//virtual void textChangedEvent(ppl7::tk::Event *event, const ppl7::String &text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
	//virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

void Floater::openUi()
{
	FloaterDialog* dialog=new FloaterDialog(this);
	GetGameWindow()->addChild(dialog);
}

FloaterDialog::FloaterDialog(Floater* object)
	: Decker::ui::Dialog(400, 200)
{
	this->object=object;
	setWindowTitle("Floater");
	addChild(new ppl7::tk::Label(0, 0, 120, 30, "Floater-Type: "));
	addChild(new ppl7::tk::Label(0, 40, 120, 30, "Initial state: "));

	floater_type=new ppl7::tk::ComboBox(120, 0, 400, 30);
	floater_type->add("studded", "0");
	floater_type->add("flat", "1");
	floater_type->setCurrentIdentifier(ppl7::ToString("%d", object->floater_type));
	floater_type->setEventHandler(this);
	addChild(floater_type);

	initial_state=new ppl7::tk::CheckBox(120, 40, 400, 30, "enabled", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);

}

void FloaterDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == floater_type) {
		object->floater_type=floater_type->currentIdentifier().toInt();
		object->init();
		object->reset();
	}
}

void FloaterDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	if (event->widget() == initial_state) {
		object->initial_state=checked;
		object->reset();

	}
}


Representation FloaterHorizontal::representation()
{
	return Representation(Spriteset::GenericObjects, 115);
}


Representation FloaterVertical::representation()
{
	return Representation(Spriteset::GenericObjects, 116);
}

FloaterHorizontal::FloaterHorizontal()
	: Floater(Type::FloaterHorizontal)
{
	sprite_no_representation=115;
	sprite_no=115;
	direction=0;
}


FloaterVertical::FloaterVertical()
	: Floater(Type::FloaterVertical)
{
	sprite_no_representation=116;
	sprite_no=116;
	direction=1;
}




}	// EOF namespace Decker::Objects
