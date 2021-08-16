#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation StamperVertical::representation()
{
	return Representation(Spriteset::StamperVertical, 5);
}


StamperVertical::StamperVertical()
:Trap(Type::ObjectType::StamperVertical)
{
	sprite_set=Spriteset::StamperVertical;
	sprite_no=0;
	collisionDetection=true;
	visibleAtPlaytime=true;
	sprite_no_representation=5;
	pixelExactCollision=false;
	save_size+=11;
	next_state=0.0f;
	state=0;
	if (ppl7::rand(0,1)==1) {
		state=2;
		sprite_no=5;
	}
	initial_state=state;
	stamper_type=0;
	time_active=ppl7::randf(0.2f, 0.5f);
	time_inactive=ppl7::randf(0.2f, 0.5f);
	auto_intervall=true;
	next_animation=0.0f;
	init();
}

void StamperVertical::init()
{
	state=initial_state;
	sprite_no=stamper_type*20;
	if (state==2) sprite_no+=5;
}


void StamperVertical::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	Trap::draw(renderer,coords);
	/*
	ppl7::grafix::Rect source=texture->spriteBoundary(sprite_no,1.0,p.x+coords.x, p.y+coords.y);
	SDL_Rect r;
	r.x=source.x1;
	r.y=source.y1;
	r.w=source.width();
	r.h=source.height();
	SDL_SetRenderDrawColor(renderer,255,0,0,255);
	SDL_RenderFillRect(renderer,&r);
	 */
}



void StamperVertical::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (time>next_animation) {
		next_animation=time+0.03f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite!=sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	int start_sprite_no=stamper_type*20;
	if (state==0 && time>next_state) {
		state=1;
		animation.startSequence(start_sprite_no, start_sprite_no+5, false, start_sprite_no+5);
	} else if (state==1 && animation.isFinished()==true) {
		state=2;
		next_state=time+time_active;
	} else if (state==2 && time>next_state) {
		state=3;
		animation.startSequence(start_sprite_no+6, start_sprite_no+19, false, start_sprite_no+19);
	} else if (state==3 && animation.isFinished()==true) {
		state=0;
		next_state=time+time_inactive;
	}
}



void StamperVertical::handleCollision(Player *player, const Collision &collision)
{
	if (state==1) {
		player->dropHealth(100, Player::Smashed);
		return;
	}
	player->setZeroVelocity();
}

void StamperVertical::toggle(bool enabled, Object *source)
{

}

size_t StamperVertical::save(unsigned char *buffer, size_t size)
{
	if (size<save_size) return 0;
	size_t bytes=Object::save(buffer,size);
	int flags=0;
	if (auto_intervall) flags|=1;
	ppl7::Poke8(buffer+bytes,flags);
	ppl7::Poke8(buffer+bytes+1,initial_state);
	ppl7::Poke8(buffer+bytes+2,stamper_type);
	ppl7::PokeFloat(buffer+bytes+3,time_active);
	ppl7::PokeFloat(buffer+bytes+7,time_inactive);
	return bytes+11;
}

bool StamperVertical::load(const unsigned char *buffer, size_t size)
{
	if (!Object::load(buffer,size)) return false;
	if (size<save_size) return true;
	size_t bytes=9;
	int flags=ppl7::Peek8(buffer+bytes);
	auto_intervall=(bool)(flags&1);
	initial_state=ppl7::Peek8(buffer+bytes+1);
	stamper_type=ppl7::Peek8(buffer+bytes+2);
	time_active=ppl7::PeekFloat(buffer+bytes+3);
	time_inactive=ppl7::PeekFloat(buffer+bytes+7);
	init();
	return true;
}

class StamperDialog : public Decker::ui::Dialog
{
private:
	Decker::ui::ComboBox *stamper_type;
	Decker::ui::CheckBox *initial_state;
	Decker::ui::CheckBox *auto_intervall;
	ppl7::tk::LineInput *time_active;
	ppl7::tk::LineInput *time_inactive;
	StamperVertical *object;

public:
	StamperDialog(StamperVertical *object);
	virtual void valueChangedEvent(ppl7::tk::Event *event, int value);
	virtual void textChangedEvent(ppl7::tk::Event *event, const ppl7::String &text);
	virtual void toggledEvent(ppl7::tk::Event *event, bool checked);
	//virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
};


void StamperVertical::openUi()
{
	StamperDialog *dialog=new StamperDialog(this);
	GetGameWindow()->addChild(dialog);
}

StamperDialog::StamperDialog(StamperVertical *object)
: Decker::ui::Dialog(640,660)
{
	this->object=object;
	setWindowTitle("Stamper");
	addChild(new ppl7::tk::Label(0,0,120,30,"Stamper-Type: "));
	addChild(new ppl7::tk::Label(0,40,120,30,"Flags: "));
	addChild(new ppl7::tk::Label(0,140,120,30,"Time active: "));
	addChild(new ppl7::tk::Label(0,180,120,30,"Time inactive: "));

	stamper_type=new Decker::ui::ComboBox(120,0,400,30);
	stamper_type->add("4 Tiles yellow","0");
	stamper_type->add("2 Tiles yellow 1","1");
	stamper_type->add("2 Tiles yellow 2","2");
	stamper_type->setCurrentIdentifier(ppl7::ToString("%d",object->stamper_type));
	stamper_type->setEventHandler(this);
	addChild(stamper_type);

	initial_state=new Decker::ui::CheckBox(120,70,400,30,"Initial state: on", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);

	auto_intervall=new Decker::ui::CheckBox(120,100,400,30,"Auto Intervall", object->auto_intervall);
	auto_intervall->setEventHandler(this);
	addChild(auto_intervall);

	time_active=new ppl7::tk::LineInput(120,140,100,30,ppl7::ToString("%0.3f", object->time_active));
	time_active->setEventHandler(this);
	addChild(time_active);

	time_inactive=new ppl7::tk::LineInput(120,180,100,30,ppl7::ToString("%0.3f", object->time_inactive));
	time_inactive->setEventHandler(this);
	addChild(time_inactive);

}


void StamperDialog::valueChangedEvent(ppl7::tk::Event *event, int value)
{
	if (event->widget()==stamper_type) {
		object->stamper_type=stamper_type->currentIdentifier().toInt();
		object->init();
	}
}

void StamperDialog::textChangedEvent(ppl7::tk::Event *event, const ppl7::String &text)
{
	if (event->widget()==time_active) object->time_active=text.toFloat();
	else if (event->widget()==time_inactive) object->time_inactive=text.toFloat();
}

void StamperDialog::toggledEvent(ppl7::tk::Event *event, bool checked)
{
	if (event->widget()==initial_state) object->initial_state=checked;
	else if (event->widget()==auto_intervall) object->auto_intervall=checked;

}


}	// EOF namespace Decker::Objects
