#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "audiopool.h"

namespace Decker::Objects {

Representation LaserBarrier::representation(Type::ObjectType type)
{
	if (type==Type::LaserBeamHorizontal) return Representation(Spriteset::GenericObjects, 213);
	return Representation(Spriteset::GenericObjects, 212);
}

LaserBarrier::LaserBarrier(Type::ObjectType type)
: Trap(type)
{
	sprite_set=Spriteset::Laser;
	if (type==Type::LaserBeamHorizontal) {
		sprite_no=1;
		sprite_no_representation=1;
	} else {
		sprite_no=0;
		sprite_no_representation=0;
	}
	next_state=0.0f;
	state=ppl7::rand(0,1);
	start_state=(bool)state;
	pixelExactCollision=false;
	flicker=0;
	audio=NULL;
	time_on_min=1.0f;
	time_off_min=1.0f;
	time_on_max=1.0f;
	time_off_max=1.0f;
	initial_state=true;
	always_on=false;
	block_player=false;
	color_scheme=0;
	save_size+=18;
	init();
}

LaserBarrier::~LaserBarrier()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void LaserBarrier::init()
{
	state=start_state;
	if (type()==Type::LaserBeamHorizontal) {
		sprite_no=1+color_scheme*2;
		sprite_no_representation=1+color_scheme*2;
	} else {
		sprite_no=0+color_scheme*2;
		sprite_no_representation=0+color_scheme*2;
	}
	if (always_on) {
		state=0;
	}

}

size_t LaserBarrier::save(unsigned char *buffer, size_t size)
{
	if (size<save_size) return 0;
	size_t bytes=Object::save(buffer,size);
	int flags=0;
	if (initial_state) flags|=1;
	if (always_on) flags|=2;
	if (block_player) flags|=4;
	if (start_state) flags|=8;

	ppl7::Poke8(buffer+bytes,flags);
	ppl7::Poke8(buffer+bytes+1,color_scheme);
	ppl7::PokeFloat(buffer+bytes+2,time_on_min);
	ppl7::PokeFloat(buffer+bytes+6,time_off_min);
	ppl7::PokeFloat(buffer+bytes+10,time_on_max);
	ppl7::PokeFloat(buffer+bytes+14,time_off_max);
	return bytes+18;
}

bool LaserBarrier::load(const unsigned char *buffer, size_t size)
{
	if (!Object::load(buffer,size)) return false;
	if (size<save_size) return true;
	size_t bytes=9;
	int flags=ppl7::Peek8(buffer+bytes);
	color_scheme=(unsigned char)(ppl7::Peek8(buffer+bytes+1)&0xff);
	time_on_min=ppl7::PeekFloat(buffer+bytes+2);
	time_off_min=ppl7::PeekFloat(buffer+bytes+6);
	time_on_max=ppl7::PeekFloat(buffer+bytes+10);
	time_off_max=ppl7::PeekFloat(buffer+bytes+14);
	initial_state=(bool)(flags&1);
	always_on=(bool)(flags&2);
	block_player=(bool)(flags&4);
	start_state=(bool)(flags&8);
	init();
	return true;
}


void LaserBarrier::draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const
{
	if (state==0) return;
	if ((flicker&3)==0) return;
	if (type()==Type::LaserBeamHorizontal) {
		for (int i=start.x;i<end.x;i+=32)
			texture->draw(renderer,
					i+coords.x+32/2,
					start.y+coords.y,
					sprite_no);
	} else {
		for (int i=start.y;i<end.y;i+=32)
			texture->draw(renderer,
					start.x+coords.x,
					i+coords.y+16,
					sprite_no);
	}

}

void LaserBarrier::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (!enabled) return;
	flicker++;
	if (next_state<time) {
		if (state==0) {
			AudioPool &pool=getAudioPool();
			if (!audio) {
				audio=pool.getInstance(AudioClip::electric);
				audio->setVolume(0.05f);
				audio->setPositional(p, 1600);
				audio->setLoop(true);
			}
			if (audio) {
				audio->setPositional(p, 1600);
				pool.playInstance(audio);
			}

			state=1;
			//next_state=time+(double)ppl7::rand(4000,8000)/1000.0f;
			next_state=time+ppl7::randf(time_on_min,time_on_max);
			collisionDetection=true;
			if (type()==Type::LaserBeamHorizontal) {
				// find left end
				start.y=p.y;
				start.x=((int)p.x/TILE_WIDTH)*TILE_WIDTH;
				end=start;
				while (start.x>0 && ttplane.getType(start)!=TileType::Blocking)
					start.x-=TILE_WIDTH;
				start.x+=TILE_WIDTH;
				if (start.x<0)start.x=0;
				// find right end
				while (end.x<65000 && ttplane.getType(end)!=TileType::Blocking)
					end.x+=TILE_WIDTH;
				// update Boundary by Points
				boundary.x1=start.x;
				boundary.y1=start.y-15;
				boundary.x2=end.x;
				boundary.y2=end.y+15;
			} else {
				// find upper end
				start.x=p.x;
				start.y=((int)p.y/TILE_HEIGHT)*TILE_HEIGHT;
				end=start;
				while (start.y>0 && ttplane.getType(start)!=TileType::Blocking)
					start.y-=TILE_HEIGHT;
				start.y+=TILE_HEIGHT;
				if (start.y<0)start.y=0;
				// find lower end
				while (end.y<65000 && ttplane.getType(end)!=TileType::Blocking)
					end.y+=TILE_HEIGHT;
				//printf ("p=%d, start: %d, end: %d\n",p.y, start.y, end.y);
				// update Boundary by Points
				boundary.x1=start.x-15;
				boundary.y1=start.y;
				boundary.x2=end.x+15;
				boundary.y2=end.y;

			}
		} else {
			if (always_on) return;
			state=0;
			if (audio) {
				getAudioPool().stopInstace(audio);
			}
			//next_state=time+(double)ppl7::rand(2000,4000)/1000.f;
			next_state=time+ppl7::randf(time_off_min,time_off_max);
			collisionDetection=false;
		}
	}

}

void LaserBarrier::handleCollision(Player *player, const Collision &collision)
{
	player->dropHealth(10);
}

void LaserBarrier::toggle(bool enable, Object *source)
{
	//printf ("LaserBarrier::toggle %d: %d\n", id, (int)enable);
	this->enabled=enable;
	if (!enable) {
		if (audio) {
			getAudioPool().stopInstace(audio);
		}
	}
}

class LaserBarrierDialog : public Decker::ui::Dialog
{
private:
	Decker::ui::ComboBox *color_scheme;
	Decker::ui::ComboBox *on_start_state;
	Decker::ui::CheckBox *initial_state;
	Decker::ui::CheckBox *always_on;
	Decker::ui::CheckBox *block_player;
	ppl7::tk::LineInput *time_on_min, *time_on_max;
	ppl7::tk::LineInput *time_off_min, *time_off_max;

	LaserBarrier *object;

public:
	LaserBarrierDialog(LaserBarrier *object);
	virtual void valueChangedEvent(ppl7::tk::Event *event, int value);
	virtual void textChangedEvent(ppl7::tk::Event *event, const ppl7::String &text);
	virtual void toggledEvent(ppl7::tk::Event *event, bool checked);
	//virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

void LaserBarrier::openUi()
{
	LaserBarrierDialog *dialog=new LaserBarrierDialog(this);
	GetGameWindow()->addChild(dialog);
}

LaserBarrierDialog::LaserBarrierDialog(LaserBarrier *object)
: Decker::ui::Dialog(600,400)
{
	this->object=object;
	setWindowTitle("LaserBarrier");
	addChild(new ppl7::tk::Label(0,0,120,30,"Color-Scheme: "));
	addChild(new ppl7::tk::Label(0,40,120,30,"Initial state: "));
	addChild(new ppl7::tk::Label(0,80,120,30,"Flags: "));
	addChild(new ppl7::tk::Label(0,200,120,30,"Time on: "));
	addChild(new ppl7::tk::Label(0,240,120,30,"Time off: "));

	color_scheme=new Decker::ui::ComboBox(120,0,300,30);
	color_scheme->add("green","0");
	color_scheme->add("blue","1");
	color_scheme->add("red","2");
	color_scheme->add("purple","3");
	color_scheme->add("yellow","4");
	color_scheme->setCurrentIdentifier(ppl7::ToString("%d",object->color_scheme));
	color_scheme->setEventHandler(this);
	addChild(color_scheme);

	on_start_state=new Decker::ui::ComboBox(120,40,80,30);
	on_start_state->add("on","1");
	on_start_state->add("off","0");
	on_start_state->setEventHandler(this);
	addChild(on_start_state);

	initial_state=new Decker::ui::CheckBox(120,80,400,30,"enabled", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);

	always_on=new Decker::ui::CheckBox(120,120,400,30,"always on", object->always_on);
	always_on->setEventHandler(this);
	addChild(always_on);

	block_player=new Decker::ui::CheckBox(120,160,400,30,"block player", object->block_player);
	block_player->setEventHandler(this);
	addChild(block_player);

	time_on_min=new ppl7::tk::LineInput(120,200,100,30,ppl7::ToString("%0.3f", object->time_on_min));
	time_on_min->setEventHandler(this);
	addChild(time_on_min);

	time_on_max=new ppl7::tk::LineInput(225,200,100,30,ppl7::ToString("%0.3f", object->time_on_max));
	time_on_max->setEventHandler(this);
	addChild(time_on_max);

	time_off_min=new ppl7::tk::LineInput(120,240,100,30,ppl7::ToString("%0.3f", object->time_off_min));
	time_off_min->setEventHandler(this);
	addChild(time_off_min);

	time_off_max=new ppl7::tk::LineInput(225,240,100,30,ppl7::ToString("%0.3f", object->time_off_max));
	time_off_max->setEventHandler(this);
	addChild(time_off_max);

}

void LaserBarrierDialog::valueChangedEvent(ppl7::tk::Event *event, int value)
{
	if (event->widget()==color_scheme) {
		object->color_scheme=color_scheme->currentIdentifier().toInt();
		object->init();
	} else if (event->widget()==on_start_state) {


	}
}

void LaserBarrierDialog::toggledEvent(ppl7::tk::Event *event, bool checked)
{
	if (event->widget()==initial_state) {
		object->initial_state=checked;
		object->reset();
	} else if (event->widget()==always_on) {
		object->always_on=checked;
		object->reset();
	} else if (event->widget()==block_player) {
		object->block_player=checked;
		object->reset();
	}
}

void LaserBarrierDialog::textChangedEvent(ppl7::tk::Event *event, const ppl7::String &text)
{
	if (event->widget()==time_on_min) object->time_on_min=text.toFloat();
	else if (event->widget()==time_on_max) object->time_on_max=text.toFloat();
	else if (event->widget()==time_off_min) object->time_off_min=text.toFloat();
	else if (event->widget()==time_off_max) object->time_off_max=text.toFloat();
}


}	// EOF namespace Decker::Objects
