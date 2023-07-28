#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "audiopool.h"


namespace Decker::Objects {

//static int trap_activation[]={ 18,19,20,21,22 };
//static int trap_deactivation[]={ 23,24,25,26,27,28,29,30,31,32,33,34 };


Representation ThreeSpeers::representation()
{
	return Representation(Spriteset::ThreeSpeers, 0);
}

static int getRealSprite(int sprite_no, int speer_type)
{
	if (speer_type == 0) return sprite_no + 18;
	if (speer_type == 1) return sprite_no + 1;
	return speer_type * 17 + sprite_no + 1;
}

ThreeSpeers::ThreeSpeers()
	: Trap(Type::ObjectType::ThreeSpeers)
{
	sprite_set=Spriteset::ThreeSpeers;
	animation.setStaticFrame(0);
	sprite_no=0;
	next_animation=next_state=0.0f;
	state=0;
	next_state=ppl7::GetMicrotime() + ppl7::randf(3.0f, 7.0f);
	collisionDetection=false;
	sprite_no_representation=0;
	speer_type=0;
}

void ThreeSpeers::update(double time, TileTypePlane&, Player&, float)
{
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		int new_sprite=getRealSprite(animation.getFrame(), speer_type);
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			if (animation.isFinished() == true && collisionDetection == true && state == 0)
				collisionDetection=false;
			updateBoundary();
		}
	}
	if (state == 0 && next_state < time) {
		getAudioPool().playOnce(AudioClip::threespeers_activation, p, 1600, 0.5f);
		getAudioPool().playOnce(AudioClip::arrow_swoosh, p, 1600, 1.0f);
		getAudioPool().playOnce(AudioClip::break1, p, 1600, 0.5f);
		next_state=time + ppl7::rand(2, 5);
		animation.startSequence(0, 4, false, 4);
		sprite_no=getRealSprite(animation.getFrame(), speer_type);
		state=1;
		collisionDetection=true;
		updateBoundary();
	}
	if (state == 1 && next_state < time) {
		getAudioPool().playOnce(AudioClip::stone_drag_short, p, 1600, 1.0f);
		next_state=time + ppl7::rand(3, 6);
		animation.startSequence(5, 16, false, 0);
		//animation.start(trap_deactivation, sizeof(trap_deactivation) / sizeof(int), false, 18);
		sprite_no=getRealSprite(animation.getFrame(), speer_type);
		state=0;
		collisionDetection=false;
		updateBoundary();

	}

}

void ThreeSpeers::handleCollision(Player* player, const Collision&)
{
	player->dropHealth(2);
}


size_t ThreeSpeers::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	ppl7::Poke8(buffer + bytes + 1, speer_type);
	return bytes + 2;
}

size_t ThreeSpeers::saveSize() const
{
	return Object::saveSize() + 2;
}

size_t ThreeSpeers::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0) return 0;
	if (size <= bytes) return bytes;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	speer_type=ppl7::Peek8(buffer + bytes + 1);
	return size;
}

class ThreeSpeersDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::ComboBox* speer_type;
	ThreeSpeers* object;

public:
	ThreeSpeersDialog(ThreeSpeers* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
};

void ThreeSpeers::openUi()
{
	ThreeSpeersDialog* dialog=new ThreeSpeersDialog(this);
	GetGameWindow()->addChild(dialog);
}

ThreeSpeersDialog::ThreeSpeersDialog(ThreeSpeers* object)
	: Decker::ui::Dialog(400, 200)
{
	this->object=object;
	setWindowTitle("Three Speers");
	addChild(new ppl7::tk::Label(0, 0, 120, 30, "Speer-Type: "));


	speer_type=new ppl7::tk::ComboBox(120, 0, 400, 30);
	speer_type->add("Speers with Skeleton", "0");
	speer_type->add("Speers only", "1");
	speer_type->add("Speers from ceiling", "2");
	speer_type->add("Speers from left", "3");
	speer_type->add("Speers from right", "4");
	speer_type->setCurrentIdentifier(ppl7::ToString("%d", object->speer_type));
	speer_type->setEventHandler(this);
	addChild(speer_type);


}

void ThreeSpeersDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == speer_type) {
		object->speer_type=speer_type->currentIdentifier().toInt();
	}
}





}	// EOF namespace Decker::Objects
