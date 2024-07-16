#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "light.h"
#include "audiopool.h"

namespace Decker::Objects {

Representation LaserBarrier::representation(Type::ObjectType type)
{
	if (type == Type::LaserBeamHorizontal) return Representation(Spriteset::GenericObjects, 213);
	return Representation(Spriteset::GenericObjects, 212);
}

LaserBarrier::LaserBarrier(Type::ObjectType type)
	: Trap(type)
{
	sprite_set=Spriteset::Laser;
	if (type == Type::LaserBeamHorizontal) {
		sprite_no=1;
		sprite_no_representation=1;
	} else {
		sprite_no=0;
		sprite_no_representation=0;
	}
	next_state=0.0f;
	state=ppl7::rand(0, 1);
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
	init();
}

LaserBarrier::~LaserBarrier()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
	clearLights();
}

void LaserBarrier::init()
{
	state=start_state;
	if (type() == Type::LaserBeamHorizontal) {
		sprite_no=1 + color_scheme * 2;
		sprite_no_representation=1 + color_scheme * 2;
	} else {
		sprite_no=0 + color_scheme * 2;
		sprite_no_representation=0 + color_scheme * 2;
	}
	if (always_on) {
		state=0;
	}


	switch (color_scheme) {
		case 0:
			light_color.set(0, 255, 0, 255);
			break;
		case 1:
			light_color.set(0, 0, 255, 255);
			break;
		case 2:
			light_color.set(255, 0, 0, 255);
			break;
		case 3:
			light_color.set(255, 0, 255, 255);
			break;
		case 4:
			light_color.set(255, 255, 0, 255);
			break;

	}

}

size_t LaserBarrier::saveSize() const
{
	return Object::saveSize() + 19;
}

size_t LaserBarrier::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	int flags=0;
	if (initial_state) flags|=1;
	if (always_on) flags|=2;
	if (block_player) flags|=4;
	if (start_state) flags|=8;

	ppl7::Poke8(buffer + bytes + 1, flags);
	ppl7::Poke8(buffer + bytes + 2, color_scheme);
	ppl7::PokeFloat(buffer + bytes + 3, time_on_min);
	ppl7::PokeFloat(buffer + bytes + 7, time_off_min);
	ppl7::PokeFloat(buffer + bytes + 11, time_on_max);
	ppl7::PokeFloat(buffer + bytes + 15, time_off_max);
	return bytes + 19;
}

size_t LaserBarrier::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	int flags=ppl7::Peek8(buffer + bytes + 1);
	color_scheme=(unsigned char)ppl7::Peek8(buffer + bytes + 2);
	time_on_min=ppl7::PeekFloat(buffer + bytes + 3);
	time_off_min=ppl7::PeekFloat(buffer + bytes + 7);
	time_on_max=ppl7::PeekFloat(buffer + bytes + 11);
	time_off_max=ppl7::PeekFloat(buffer + bytes + 15);
	initial_state=(bool)(flags & 1);
	always_on=(bool)(flags & 2);
	block_player=(bool)(flags & 4);
	start_state=(bool)(flags & 8);
	init();
	return size;
}
/*

size_t LaserBarrier::load1(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load1(buffer, size);
	if (bytes == 0) return 0;
	int flags=ppl7::Peek8(buffer + bytes);
	color_scheme=(unsigned char)(ppl7::Peek8(buffer + bytes + 1) & 0xff);
	time_on_min=ppl7::PeekFloat(buffer + bytes + 2);
	time_off_min=ppl7::PeekFloat(buffer + bytes + 6);
	time_on_max=ppl7::PeekFloat(buffer + bytes + 10);
	time_off_max=ppl7::PeekFloat(buffer + bytes + 14);
	initial_state=(bool)(flags & 1);
	always_on=(bool)(flags & 2);
	block_player=(bool)(flags & 4);
	start_state=(bool)(flags & 8);
	init();
	return size;
}
*/


void LaserBarrier::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	if (state == 0) return;
	if ((flicker & 7) == 0) return;
	if (type() == Type::LaserBeamHorizontal) {
		for (int i=start.x;i < end.x;i+=32)
			texture->draw(renderer,
				i + coords.x + 16,
				start.y + coords.y,
				sprite_no);
	} else {
		for (int i=start.y;i < end.y;i+=TILE_HEIGHT)
			texture->draw(renderer,
				start.x + coords.x,
				i + coords.y + 16,
				sprite_no);
	}
}

void LaserBarrier::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	ppl7::grafix::Color c(255, 255, 255, 192);
	if (state != 0 && (flicker & 7) != 0) {
		if (type() == Type::LaserBeamHorizontal) {
			for (int i=start.x;i < end.x;i+=32)
				texture->draw(renderer,
					i + coords.x + 16,
					start.y + coords.y,
					sprite_no, c);
		} else {
			for (int i=start.y;i < end.y;i+=TILE_HEIGHT)
				texture->draw(renderer,
					start.x + coords.x,
					i + coords.y + 16,
					sprite_no, c);
		}
	}
	Trap::drawEditMode(renderer, coords);

}


void LaserBarrier::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	if (!enabled) return;
	flicker++;
	if (next_state < time) {
		if (state == 0) {
			AudioPool& pool=getAudioPool();
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
			next_state=time + ppl7::randf(time_on_min, time_on_max);
			collisionDetection=true;
			if (type() == Type::LaserBeamHorizontal) {
				// find left end
				start.y=p.y;
				start.x=((int)p.x / TILE_WIDTH) * TILE_WIDTH;
				end=start;
				while (start.x > 0 && ttplane.getType(start) != TileType::Blocking)
					start.x-=TILE_WIDTH;
				start.x+=TILE_WIDTH;
				if (start.x < 0)start.x=0;
				// find right end
				while (end.x < 65000 && ttplane.getType(end) != TileType::Blocking)
					end.x+=TILE_WIDTH;
				// update Boundary by Points
				boundary.x1=start.x;
				boundary.y1=start.y - 15;
				boundary.x2=end.x;
				boundary.y2=end.y + 15;
			} else {
				// find upper end
				start.x=p.x;
				start.y=((int)p.y / TILE_HEIGHT) * TILE_HEIGHT;
				end=start;
				while (start.y > 0 && ttplane.getType(start) != TileType::Blocking)
					start.y-=TILE_HEIGHT;
				start.y+=TILE_HEIGHT;
				if (start.y < 0)start.y=0;
				// find lower end
				while (end.y < 65000 && ttplane.getType(end) != TileType::Blocking)
					end.y+=TILE_HEIGHT;
				//printf ("p=%d, start: %d, end: %d\n",p.y, start.y, end.y);
				// update Boundary by Points
				boundary.x1=start.x - 15;
				boundary.y1=start.y;
				boundary.x2=end.x + 15;
				boundary.y2=end.y;

			}
		} else {
			if (!always_on) {
				state=0;
				if (audio) {
					getAudioPool().stopInstace(audio);
				}
				//next_state=time+(double)ppl7::rand(2000,4000)/1000.f;
				next_state=time + ppl7::randf(time_off_min, time_off_max);
				collisionDetection=false;
			}
		}
	}
	updateLightMaps();

}

void LaserBarrier::clearLights()
{
	std::list<LightObject*>::iterator it;
	for (it=light_list.begin();it != light_list.end();++it) {
		delete (*it);
	}
	light_list.clear();
}

void LaserBarrier::updateLightMaps()
{
	clearLights();
	//ppl7::PrintDebug("LaserBarrier::updateLightMaps, state: %d\n", state);
	if (state == 0) return;
	if ((flicker & 7) == 0) return;

	LightSystem& lights=GetGame().getLightSystem();
	if (type() == Type::LaserBeamHorizontal) {
		for (int i=start.x;i < end.x;i+=32) {
			LightObject* light=new LightObject();
			light->x=i + 16;
			light->y=start.y;
			light->sprite_no=17;
			light->playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Player);
			light_list.push_back(light);
			lights.addObjectLight(light);

			LightObject* shine_up=new LightObject();
			shine_up->x=light->x;
			shine_up->y=light->y - 1;
			shine_up->sprite_no=18;
			shine_up->angle=180;
			shine_up->scale_x=0.945;
			shine_up->scale_y=0.5;
			shine_up->color=light_color;
			shine_up->intensity=128;
			shine_up->playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);
			light_list.push_back(shine_up);
			lights.addObjectLight(shine_up);

			LightObject* shine_down=new LightObject();
			shine_down->x=light->x;
			shine_down->y=light->y + 1;
			shine_down->sprite_no=18;
			shine_down->angle=0;
			shine_down->scale_x=0.945;
			shine_down->scale_y=0.5;
			shine_down->color=light_color;
			shine_down->intensity=128;
			shine_down->playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);
			light_list.push_back(shine_down);
			lights.addObjectLight(shine_down);

		}
	} else {

		for (int i=start.y;i < end.y;i+=TILE_HEIGHT) {
			LightObject* light=new LightObject();
			light->x=start.x;
			light->y=i + 19;
			light->sprite_no=16;
			light->playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Player);
			light_list.push_back(light);
			lights.addObjectLight(light);

			LightObject* shine_left=new LightObject();
			shine_left->x=light->x - 1;
			shine_left->y=light->y;
			shine_left->sprite_no=19;
			shine_left->angle=270;
			shine_left->scale_y=0.5;
			shine_left->scale_x=1;
			shine_left->color=light_color;
			shine_left->intensity=128;
			shine_left->playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);
			light_list.push_back(shine_left);
			lights.addObjectLight(shine_left);

			LightObject* shine_right=new LightObject();
			shine_right->x=light->x + 1;
			shine_right->y=light->y;
			shine_right->sprite_no=19;
			shine_right->angle=90;
			shine_right->scale_y=0.5;
			shine_right->scale_x=1;
			shine_right->color=light_color;
			shine_right->intensity=128;
			shine_right->playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);
			light_list.push_back(shine_right);
			lights.addObjectLight(shine_right);
		}
	}

}

void LaserBarrier::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(10);
}

void LaserBarrier::toggle(bool enable, Object* source)
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
	ppltk::CheckBox* initialStateEnabled, * currentState;
	ppltk::ComboBox* color_scheme;
	ppltk::CheckBox* always_on;
	ppltk::CheckBox* block_player;
	ppltk::DoubleHorizontalSlider* time_on_min, * time_on_max;
	ppltk::DoubleHorizontalSlider* time_off_min, * time_off_max;

	LaserBarrier* object;

public:
	LaserBarrierDialog(LaserBarrier* object);
	virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, double value);
	virtual void toggledEvent(ppltk::Event* event, bool checked);
};

void LaserBarrier::openUi()
{
	LaserBarrierDialog* dialog=new LaserBarrierDialog(this);
	GetGameWindow()->addChild(dialog);
}

LaserBarrierDialog::LaserBarrierDialog(LaserBarrier* object)
	: Decker::ui::Dialog(600, 400)
{
	//ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle(ppl7::ToString("LaserBarrier, Object ID: %u", object->id));
	int y=0;
	//int sw=client.width() / 2;
	initialStateEnabled=new ppltk::CheckBox(0, y, 120, 30, "initial state");
	initialStateEnabled->setChecked(object->initial_state);
	initialStateEnabled->setEventHandler(this);
	addChild(initialStateEnabled);
	currentState=new ppltk::CheckBox(120, y, 130, 30, "current state");
	currentState->setChecked(object->enabled);
	currentState->setEventHandler(this);
	addChild(currentState);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Color-Scheme: "));
	color_scheme=new ppltk::ComboBox(120, y, 300, 30);
	color_scheme->add("green", "0");
	color_scheme->add("blue", "1");
	color_scheme->add("red", "2");
	color_scheme->add("purple", "3");
	color_scheme->add("yellow", "4");
	color_scheme->setCurrentIdentifier(ppl7::ToString("%d", object->color_scheme));
	color_scheme->setEventHandler(this);
	addChild(color_scheme);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Flags: "));

	always_on=new ppltk::CheckBox(120, y, 400, 30, "always on", object->always_on);
	always_on->setEventHandler(this);
	addChild(always_on);
	y+=35;

	block_player=new ppltk::CheckBox(120, y, 400, 30, "block player", object->block_player);
	block_player->setEventHandler(this);
	addChild(block_player);
	y+=35;


	addChild(new ppltk::Label(0, y, 120, 30, "Time on: "));

	addChild(new ppltk::Label(120, y, 50, 30, "min:"));
	time_on_min=new ppltk::DoubleHorizontalSlider(170, y, 400, 30);
	time_on_min->setLimits(0.0f, 5.0f);
	time_on_min->setValue(object->time_on_min);
	time_on_min->enableSpinBox(true, 0.1f, 3, 100);
	time_on_min->setEventHandler(this);
	addChild(time_on_min);
	y+=35;

	addChild(new ppltk::Label(120, y, 50, 30, "max:"));
	time_on_max=new ppltk::DoubleHorizontalSlider(170, y, 400, 30);
	time_on_max->setLimits(0.0f, 5.0f);
	time_on_max->setValue(object->time_on_max);
	time_on_max->enableSpinBox(true, 0.1f, 3, 100);
	time_on_max->setEventHandler(this);
	addChild(time_on_max);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Time off: "));

	addChild(new ppltk::Label(120, y, 50, 30, "min:"));
	time_off_min=new ppltk::DoubleHorizontalSlider(170, y, 400, 30);
	time_off_min->setLimits(0.0f, 5.0f);
	time_off_min->setValue(object->time_off_min);
	time_off_min->enableSpinBox(true, 0.1f, 3, 100);
	time_off_min->setEventHandler(this);
	addChild(time_off_min);
	y+=35;

	addChild(new ppltk::Label(120, y, 50, 30, "max:"));
	time_off_max=new ppltk::DoubleHorizontalSlider(170, y, 400, 30);
	time_off_max->setLimits(0.0f, 5.0f);
	time_off_max->setValue(object->time_off_max);
	time_off_max->enableSpinBox(true, 0.1f, 3, 100);
	time_off_max->setEventHandler(this);
	addChild(time_off_max);
	y+=35;


}

void LaserBarrierDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	if (event->widget() == color_scheme) {
		object->color_scheme=color_scheme->currentIdentifier().toInt();
		object->init();
	}
}

void LaserBarrierDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (event->widget() == initialStateEnabled) {
		object->initial_state=checked;
		object->reset();
	} else if (event->widget() == currentState) {
		object->enabled=checked;
		object->reset();
	} else if (event->widget() == always_on) {
		object->always_on=checked;
		object->reset();
	} else if (event->widget() == block_player) {
		object->block_player=checked;
		object->reset();
	}
}

void LaserBarrierDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	if (event->widget() == time_on_min) {
		object->time_on_min=value;
		if (object->time_on_min > object->time_on_max) time_on_max->setValue(object->time_on_min);
	} else if (event->widget() == time_on_max) {
		object->time_on_max=value;
		if (object->time_on_max < object->time_on_min) time_on_min->setValue(object->time_on_max);
	} else if (event->widget() == time_off_min) {
		object->time_off_min=value;
		if (object->time_off_min > object->time_off_max) time_off_max->setValue(object->time_off_min);
	} else if (event->widget() == time_off_max) {
		object->time_off_max=value;
		if (object->time_off_max < object->time_off_min) time_off_min->setValue(object->time_off_max);
	}


}


}	// EOF namespace Decker::Objects
