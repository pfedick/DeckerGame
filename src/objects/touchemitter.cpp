#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"

namespace Decker::Objects {


class TouchParticle : public Object
{
	friend class TouchEmitter;
private:
	ppl7::grafix::PointF velocity;
	double max_distance;
	Type::ObjectType emitted_object;
	Object* child;
public:
	TouchParticle(Type::ObjectType type);
	~TouchParticle();
	static Representation representation();
	virtual void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
};

class TouchEmitterDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::ComboBox* touch_type;
	ppl7::tk::ComboBox* object_type;
	ppl7::tk::ComboBox* direction;
	ppl7::tk::CheckBox* activator_top;
	ppl7::tk::CheckBox* activator_right;
	ppl7::tk::CheckBox* activator_bottom;
	ppl7::tk::CheckBox* activator_left;
	ppl7::tk::LineInput* max_toggles;
	ppl7::tk::Button* reset;

	TouchEmitter* object;

public:
	TouchEmitterDialog(TouchEmitter* object);
	~TouchEmitterDialog();

	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent* event);
};


Representation TouchEmitter::representation()
{
	return Representation(Spriteset::GenericObjects, 238);
}


TouchEmitter::TouchEmitter()
	:Object(Type::ObjectType::TouchEmitter)
{
	sprite_set=0;
	sprite_no=238;
	sprite_no_representation=238;
	toogle_count=0;
	max_toggles=1;
	direction=0;
	touchtype=0;
	emitted_object=Type::Crystal;
	pixelExactCollision=false;
	collisionDetection=true;
	next_touch_time=0.0f;
	state=State::waiting;
}

TouchEmitter::~TouchEmitter()
{

}

void TouchEmitter::reset()
{
	Object::reset();
	toogle_count=0;
	collisionDetection=true;
	next_touch_time=0.0f;
	state=State::waiting;
}

void TouchEmitter::init()
{
	visibleAtPlaytime=true;
	sprite_no=238;
	sprite_no_representation=238;
	if ((touchtype & 15) == 1) {
		sprite_no=237;
		sprite_no_representation=237;
	} else if ((touchtype & 15) == 2) {
		visibleAtPlaytime=false;
	} else if ((touchtype & 15) == 3 || (touchtype & 15) == 4) {
		visibleAtPlaytime=false;
		state=State::waiting;
	}
}


void TouchEmitter::emmitObject(double time)
{
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::impact1, 0.2f);
	next_touch_time=time + ppl7::randf(0.1f, 1.0f);
	toogle_count++;
	TouchParticle* particle=new TouchParticle(emitted_object);
	particle->p.x=ppl7::randf(p.x - 16.0f, p.x + 16.0f);
	particle->p.y=ppl7::randf(p.y - 16.0f, p.y + 16.0f);
	particle->initial_p=p;
	particle->spawned=true;
	particle->max_distance=ppl7::randf(90.0f, 120.0f);
	float v0=ppl7::randf(-0.4, 0.4);
	float v1=ppl7::randf(3.5, 6.5);
	//printf ("v0=%0.3f, v1=%0.3f\n",v0,v1);
	switch (direction) {
	case 0: particle->velocity.setPoint(v0, -v1); break;
	case 1: particle->velocity.setPoint(v1, v0); break;
	case 2: particle->velocity.setPoint(v0, v1); break;
	case 3: particle->velocity.setPoint(-v1, v0); break;
	default: particle->velocity.setPoint(v0, -v1); break;
	}
	GetObjectSystem()->addObject(particle);
}

void TouchEmitter::handleCollision(Player* player, const Collision& collision)
{
	if ((touchtype & 15) == 3 || (touchtype & 15) == 4) return; // emitted by trigger
	// emit particle, which moves in direction 2 free tiles and then emits the final
	// object
	//printf ("TouchEmitter::handleCollision %d:%d\n", toogle_count,max_toggles);
	//printf ("next_touch_time=%0.3f, now=%0.3f\n",next_touch_time,now);
	if (toogle_count < max_toggles && next_touch_time < player->time) {
		bool touched=false;
		if ((touchtype & 0xf0) == 0xf0) touched=true;
		if ((touchtype & 0x10) && collision.objectTop() == true) touched=true;
		if ((touchtype & 0x20) && collision.objectRight() == true) touched=true;
		if ((touchtype & 0x40) && collision.objectBottom() == true) touched=true;
		if ((touchtype & 0x80) && collision.objectLeft() == true) touched=true;
		if (touched) {
			emmitObject(player->time);
		}
	}
	if (toogle_count >= max_toggles) {
		collisionDetection=false;
		enabled=false;
	}
}

void TouchEmitter::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (((touchtype & 15) == 3 || (touchtype & 15) == 4) && state == State::triggered) {
		if (toogle_count < max_toggles && next_touch_time < time) {
			emmitObject(time);
		} else if (toogle_count >= max_toggles && (touchtype & 15) == 4) {
			toogle_count=0;
			state=State::waiting;
		}
	}
}

void TouchEmitter::trigger(Object*)
{
	state=State::triggered;
}


size_t TouchEmitter::saveSize() const
{
	return Object::saveSize() + 6;
}

size_t TouchEmitter::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	ppl7::Poke8(buffer + bytes + 1, max_toggles);
	ppl7::Poke8(buffer + bytes + 2, direction);
	ppl7::Poke16(buffer + bytes + 3, emitted_object);
	ppl7::Poke8(buffer + bytes + 5, touchtype);
	return bytes + 6;
}

size_t TouchEmitter::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	max_toggles=ppl7::Peek8(buffer + bytes + 1);
	direction=ppl7::Peek8(buffer + bytes + 2);
	emitted_object=(Type::ObjectType)ppl7::Peek8(buffer + bytes + 3);
	touchtype=ppl7::Peek8(buffer + bytes + 5);
	init();
	return size;
}

void TouchEmitter::openUi()
{
	TouchEmitterDialog* dialog=new TouchEmitterDialog(this);
	GetGameWindow()->addChild(dialog);
}

TouchParticle::TouchParticle(Type::ObjectType type)
	: Object(Type::Particle)
{
	emitted_object=type;
	max_distance=100.0f;
	Representation repr=getRepresentation(type);
	sprite_no=repr.sprite_no;
	sprite_set=repr.sprite_set;
	sprite_no_representation=sprite_no;
	child=GetObjectSystem()->getInstance(type);
}

TouchParticle::~TouchParticle()
{
	delete child;
}

Representation TouchParticle::representation()
{
	return Representation(Spriteset::GenericObjects, 238);
}

void TouchParticle::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	p+=velocity * frame_rate_compensation;
	if (ppl7::grafix::Distance(p, initial_p) > max_distance) {
		deleteDefered=true;
		ObjectSystem* objs=GetObjectSystem();
		Object* object=objs->getInstance(emitted_object);
		object->p=p;
		object->initial_p=p;
		object->spawned=true;
		objs->addObject(object);
	} else {
		child->update(time, ttplane, player, frame_rate_compensation);
		sprite_no=child->sprite_no;
	}
}



TouchEmitterDialog::TouchEmitterDialog(TouchEmitter* object)
	: Decker::ui::Dialog(640, 480)
{
	this->object=object;
	setWindowTitle("TouchEmitter");
	addChild(new ppl7::tk::Label(0, 0, 120, 30, "Touch-Type: "));
	addChild(new ppl7::tk::Label(0, 40, 120, 30, "Activation: "));
	addChild(new ppl7::tk::Label(0, 120, 120, 30, "Emitted object: "));
	addChild(new ppl7::tk::Label(0, 160, 120, 30, "direction: "));
	addChild(new ppl7::tk::Label(0, 200, 120, 30, "max toggles: "));

	touch_type=new ppl7::tk::ComboBox(120, 0, 400, 30);
	touch_type->add("2 x 4", "0");
	touch_type->add("2 x 2", "1");
	touch_type->add("invisible", "2");
	touch_type->add("triggered", "3");
	touch_type->add("retriggerable", "4");
	touch_type->setCurrentIdentifier(ppl7::ToString("%d", object->touchtype & 15));
	touch_type->setEventHandler(this);
	addChild(touch_type);

	activator_top=new ppl7::tk::CheckBox(180, 40, 60, 30, "top", object->touchtype & 16);
	activator_top->setEventHandler(this);
	addChild(activator_top);
	activator_right=new ppl7::tk::CheckBox(240, 60, 70, 30, "right", object->touchtype & 32);
	activator_right->setEventHandler(this);
	addChild(activator_right);
	activator_bottom=new ppl7::tk::CheckBox(180, 80, 84, 30, "bottom", object->touchtype & 64);
	activator_bottom->setEventHandler(this);
	addChild(activator_bottom);
	activator_left=new ppl7::tk::CheckBox(120, 60, 60, 30, "left", object->touchtype & 128);
	activator_left->setEventHandler(this);
	addChild(activator_left);


	object_type=new ppl7::tk::ComboBox(120, 120, 400, 30);
	object_type->setEventHandler(this);
	object_type->add("Medikit", ppl7::ToString("%d", Type::Medikit));
	object_type->add("Crystal", ppl7::ToString("%d", Type::Crystal));
	object_type->add("Diamond", ppl7::ToString("%d", Type::Diamond));
	object_type->add("Coin", ppl7::ToString("%d", Type::Coin));
	object_type->add("ExtraLife", ppl7::ToString("%d", Type::ExtraLife));
	//object_type->add("ExtraLife",ppl7::ToString("%d",Type::ExtraLife));
	object_type->setCurrentIdentifier(ppl7::ToString("%d", object->emitted_object));
	addChild(object_type);

	direction=new ppl7::tk::ComboBox(120, 160, 400, 30);
	direction->setEventHandler(this);
	direction->add("up", "0");
	direction->add("right", "1");
	direction->add("down", "2");
	direction->add("left", "3");
	direction->setCurrentIdentifier(ppl7::ToString("%d", object->direction));
	addChild(direction);

	max_toggles=new ppl7::tk::LineInput(120, 200, 80, 30, ppl7::ToString("%d", object->max_toggles));
	max_toggles->setEventHandler(this);
	addChild(max_toggles);

	reset=new ppl7::tk::Button(0, 240, 80, 30, "Reset");
	reset->setEventHandler(this);
	addChild(reset);

}

TouchEmitterDialog::~TouchEmitterDialog()
{

}

void TouchEmitterDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	// checkboxes
	//printf ("TouchEmitterDialog::toggledEvent\n");
	if (event->widget() == activator_top) {
		object->touchtype&=(255 - 16);
		if (checked)object->touchtype|=16;
	} else 	if (event->widget() == activator_right) {
		object->touchtype&=(255 - 32);
		if (checked)object->touchtype|=32;
	} else 	if (event->widget() == activator_bottom) {
		object->touchtype&=(255 - 64);
		if (checked)object->touchtype|=64;
	} else 	if (event->widget() == activator_left) {
		object->touchtype&=(255 - 128);
		if (checked)object->touchtype|=128;
	}

}

void TouchEmitterDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	// combobox
	//printf ("TouchEmitterDialog::valueChangedEvent\n");
	if (event->widget() == touch_type) {
		object->touchtype&=0xf0;
		object->touchtype|=(unsigned char)(touch_type->currentIdentifier().toInt() & 0x0f);
		object->init();
	} else if (event->widget() == object_type) {
		object->emitted_object=(Type::ObjectType)object_type->currentIdentifier().toInt();
	} else if (event->widget() == direction) {
		object->direction=(unsigned char)direction->currentIdentifier().toInt();
	}

}

void TouchEmitterDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	// input box
	//printf ("TouchEmitterDialog::textChangedEvent\n");
	if (event->widget() == max_toggles) {
		object->max_toggles=(unsigned char)(text.toUnsignedInt() & 0xff);
	}
}

void TouchEmitterDialog::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == reset) object->reset();
	else Decker::ui::Dialog::mouseDownEvent(event);
}


}	// EOF namespace Decker::Objects
