#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"
#include "audiopool.h"
namespace Decker::Objects {




class ArrowFired : public Object
{
	friend class Arrow;
private:
	ppl7::grafix::Point velocity;
public:
	ArrowFired();
	static Representation representation();
	virtual void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	virtual void handleCollision(Player* player, const Collision& collision);
};



Representation Arrow::representation()
{
	return Representation(Spriteset::GenericObjects, 35);
}

ArrowFired::ArrowFired()
	:Object(Type::ObjectType::Arrow)
{
	collisionDetection=true;
}

Representation ArrowFired::representation()
{
	return Representation(Spriteset::GenericObjects, 35);
}

void ArrowFired::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	p+=velocity * frame_rate_compensation;
	updateBoundary();
	TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y));
	if (t1 == TileType::Blocking) {
		deleteDefered=true;
		getAudioPool().playOnce(AudioClip::light_switch1, p, 1600, 0.4f);

	} else if (p.x < 0 || p.x>65535 || p.y < 0 || p.y>65535) {
		deleteDefered=true;
	}
}

void ArrowFired::handleCollision(Player* player, const Collision& collision)
{
	deleteDefered=true;
	player->dropHealth(20);
}


Arrow::Arrow()
	:Trap(Type::ObjectType::Arrow)
{
	sprite_set=0;
	sprite_no=35;
	sprite_no_representation=35;
	collisionDetection=false;
	visibleAtPlaytime=false;
	direction=0;
	state=0;
	player_activation_distance=200;
	min_cooldown_time=15.0f;
	max_cooldown_time=min_cooldown_time;
	min_cooldown_state=0.0f;
	next_state=ppl7::GetMicrotime() + (float)ppl7::rand(5000, 15000) / 1000.0f;
}


void Arrow::changeDirection(int new_direction)
{
	if (new_direction != direction) {
		direction=new_direction;
		switch (direction) {
			case 0: sprite_no=35; break;
			case 1: sprite_no=36; break;
			case 2: sprite_no=119; break;
			case 3: sprite_no=37; break;
		}
		sprite_no_representation=sprite_no;
		this->updateBoundary();
	}
}

void Arrow::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (state == 0) {
		double dist=ppl7::grafix::Distance(p, player.position());
		if (next_state < time || (dist < player_activation_distance && min_cooldown_state < time)) {
			state++;
			if (state == 1) {
				next_state=time + ppl7::rand(min_cooldown_time * 1000, max_cooldown_time * 1000) / 1000;
				if (GetGame().config.difficulty == Config::DifficultyLevel::easy) min_cooldown_state=time + ppl7::randf(0.600, 1.400);
				else min_cooldown_state=time + ppl7::randf(0.300, 0.800);
				state=0;
			}
			// Emit a new arrow
			fire();
		}
	}
}

void Arrow::fire()
{
	ArrowFired* particle=new ArrowFired();
	particle->p=p;
	particle->initial_p=p;
	particle->spawned=true;
	particle->sprite_no=sprite_no;
	particle->sprite_set=sprite_set;
	particle->sprite_no_representation=sprite_no;
	switch (direction) {
		case 0: particle->velocity.setPoint(0, -12); break;
		case 1: particle->velocity.setPoint(12, 0); break;
		case 2: particle->velocity.setPoint(0, 12); break;
		case 3: particle->velocity.setPoint(-12, 0); break;
		default: particle->velocity.setPoint(0, -12); break;
	}
	GetObjectSystem()->addObject(particle);
	getAudioPool().playOnce(AudioClip::arrow_swoosh, p, 1600, 0.4f);
}

size_t Arrow::saveSize() const
{
	return Object::saveSize() + 12;
}

size_t Arrow::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	ppl7::Poke8(buffer + bytes + 1, direction);
	ppl7::Poke16(buffer + bytes + 2, player_activation_distance);
	ppl7::PokeFloat(buffer + bytes + 4, min_cooldown_time);
	ppl7::PokeFloat(buffer + bytes + 8, max_cooldown_time);
	return bytes + 12;
}

size_t Arrow::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	changeDirection((int)ppl7::Peek8(buffer + bytes + 1));
	player_activation_distance=ppl7::Peek16(buffer + bytes + 2);
	min_cooldown_time=ppl7::PeekFloat(buffer + bytes + 4);
	max_cooldown_time=ppl7::PeekFloat(buffer + bytes + 8);
	return size;
}


class ArrowDialog : public Decker::ui::Dialog
{
private:
	ppltk::ComboBox* direction;
	ppltk::DoubleHorizontalSlider* min_cooldown;
	ppltk::DoubleHorizontalSlider* max_cooldown;
	ppltk::HorizontalSlider* distance;
	Arrow* object;

public:
	ArrowDialog(Arrow* object);
	~ArrowDialog();
	virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
	virtual void valueChangedEvent(ppltk::Event* event, double value);
	//virtual void textChangedEvent(ppltk::Event* event, const ppl7::String& text);
};

void Arrow::openUi()
{
	ArrowDialog* dialog=new ArrowDialog(this);
	GetGameWindow()->addChild(dialog);
}


ArrowDialog::ArrowDialog(Arrow* object)
	: Decker::ui::Dialog(510, 250)
{
	this->object=object;
	setWindowTitle("Arrow Trap");

	int y=0;

	addChild(new ppltk::Label(0, y, 120, 30, "Direction: "));
	direction=new ppltk::ComboBox(120, y, 360, 30);
	direction->add("up", "0");
	direction->add("right", "1");
	direction->add("down", "2");
	direction->add("left", "3");
	direction->setCurrentIdentifier(ppl7::ToString("%d", object->direction));
	direction->setEventHandler(this);
	addChild(direction);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Cooldown time: "));
	addChild(new ppltk::Label(120, y, 50, 30, "min:"));
	min_cooldown=new ppltk::DoubleHorizontalSlider(180, y, 300, 30);
	min_cooldown->setLimits(0.0f, 16.0f);
	min_cooldown->setValue(object->min_cooldown_time);
	min_cooldown->enableSpinBox(true, 0.1f, 3, 100);
	min_cooldown->setEventHandler(this);
	addChild(min_cooldown);
	y+=35;

	addChild(new ppltk::Label(120, y, 50, 30, "max:"));
	max_cooldown=new ppltk::DoubleHorizontalSlider(180, y, 300, 30);
	max_cooldown->setLimits(0.0f, 16.0f);
	max_cooldown->setValue(object->max_cooldown_time);
	max_cooldown->enableSpinBox(true, 0.1f, 3, 100);
	max_cooldown->setEventHandler(this);
	addChild(max_cooldown);
	y+=35;


	addChild(new ppltk::Label(0, y, 120, 30, "player distance: "));


	distance=new ppltk::HorizontalSlider(120, y, 360, 30);
	distance->setLimits(0, 1000);
	distance->setValue(object->player_activation_distance);
	distance->enableSpinBox(true, 50, 100);
	distance->setEventHandler(this);
	addChild(distance);

}

ArrowDialog::~ArrowDialog()
{

}

void ArrowDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	if (event->widget() == direction) {
		object->changeDirection(direction->currentIdentifier().toInt());
	}
}

void ArrowDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	if (event->widget()==distance) {
		object->player_activation_distance=value;
	}
}

void ArrowDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	if (event->widget()==min_cooldown) {
		object->min_cooldown_time=value;
		if (object->min_cooldown_time>object->max_cooldown_time) max_cooldown->setValue(value);

	} else 	if (event->widget()==max_cooldown) {
		object->max_cooldown_time=value;
		if (object->max_cooldown_time<object->min_cooldown_time) min_cooldown->setValue(value);

	}
}


}	// EOF namespace Decker::Objects
