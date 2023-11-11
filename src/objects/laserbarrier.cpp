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
	if (type == Type::LaserBeamHorizontal) return Representation(Spriteset::Laser, 1);
	return Representation(Spriteset::Laser, 0);
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
			if (always_on) return;
			state=0;
			if (audio) {
				getAudioPool().stopInstace(audio);
			}
			//next_state=time+(double)ppl7::rand(2000,4000)/1000.f;
			next_state=time + ppl7::randf(time_off_min, time_off_max);
			collisionDetection=false;
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
	ppl7::tk::ComboBox* color_scheme;
	ppl7::tk::ComboBox* on_start_state;
	ppl7::tk::CheckBox* initial_state;
	ppl7::tk::CheckBox* always_on;
	ppl7::tk::CheckBox* block_player;
	ppl7::tk::LineInput* time_on_min, * time_on_max;
	ppl7::tk::LineInput* time_off_min, * time_off_max;

	LaserBarrier* object;

public:
	LaserBarrierDialog(LaserBarrier* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
	//virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

void LaserBarrier::openUi()
{
	LaserBarrierDialog* dialog=new LaserBarrierDialog(this);
	GetGameWindow()->addChild(dialog);
}

LaserBarrierDialog::LaserBarrierDialog(LaserBarrier* object)
	: Decker::ui::Dialog(600, 400)
{
	this->object=object;
	setWindowTitle("LaserBarrier");
	addChild(new ppl7::tk::Label(0, 0, 120, 30, "Color-Scheme: "));
	addChild(new ppl7::tk::Label(0, 40, 120, 30, "Initial state: "));
	addChild(new ppl7::tk::Label(0, 80, 120, 30, "Flags: "));
	addChild(new ppl7::tk::Label(0, 200, 120, 30, "Time on: "));
	addChild(new ppl7::tk::Label(0, 240, 120, 30, "Time off: "));

	color_scheme=new ppl7::tk::ComboBox(120, 0, 300, 30);
	color_scheme->add("green", "0");
	color_scheme->add("blue", "1");
	color_scheme->add("red", "2");
	color_scheme->add("purple", "3");
	color_scheme->add("yellow", "4");
	color_scheme->setCurrentIdentifier(ppl7::ToString("%d", object->color_scheme));
	color_scheme->setEventHandler(this);
	addChild(color_scheme);

	on_start_state=new ppl7::tk::ComboBox(120, 40, 80, 30);
	on_start_state->add("on", "1");
	on_start_state->add("off", "0");
	on_start_state->setEventHandler(this);
	addChild(on_start_state);

	initial_state=new ppl7::tk::CheckBox(120, 80, 400, 30, "enabled", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);

	always_on=new ppl7::tk::CheckBox(120, 120, 400, 30, "always on", object->always_on);
	always_on->setEventHandler(this);
	addChild(always_on);

	block_player=new ppl7::tk::CheckBox(120, 160, 400, 30, "block player", object->block_player);
	block_player->setEventHandler(this);
	addChild(block_player);

	time_on_min=new ppl7::tk::LineInput(120, 200, 100, 30, ppl7::ToString("%0.3f", object->time_on_min));
	time_on_min->setEventHandler(this);
	addChild(time_on_min);

	time_on_max=new ppl7::tk::LineInput(225, 200, 100, 30, ppl7::ToString("%0.3f", object->time_on_max));
	time_on_max->setEventHandler(this);
	addChild(time_on_max);

	time_off_min=new ppl7::tk::LineInput(120, 240, 100, 30, ppl7::ToString("%0.3f", object->time_off_min));
	time_off_min->setEventHandler(this);
	addChild(time_off_min);

	time_off_max=new ppl7::tk::LineInput(225, 240, 100, 30, ppl7::ToString("%0.3f", object->time_off_max));
	time_off_max->setEventHandler(this);
	addChild(time_off_max);

}

void LaserBarrierDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == color_scheme) {
		object->color_scheme=color_scheme->currentIdentifier().toInt();
		object->init();
	} else if (event->widget() == on_start_state) {


	}
}

void LaserBarrierDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	if (event->widget() == initial_state) {
		object->initial_state=checked;
		object->reset();
	} else if (event->widget() == always_on) {
		object->always_on=checked;
		object->reset();
	} else if (event->widget() == block_player) {
		object->block_player=checked;
		object->reset();
	}
}

void LaserBarrierDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
	if (event->widget() == time_on_min) object->time_on_min=text.toFloat();
	else if (event->widget() == time_on_max) object->time_on_max=text.toFloat();
	else if (event->widget() == time_off_min) object->time_off_min=text.toFloat();
	else if (event->widget() == time_off_max) object->time_off_max=text.toFloat();
}


}	// EOF namespace Decker::Objects
