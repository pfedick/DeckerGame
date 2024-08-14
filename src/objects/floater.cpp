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
	initial_state=true;
	start_direction_a=false;
	current_state=true;
	floater_type=0;
	/* Particles */
	next_birth=0.0f;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.063));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.068, 0.235));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.320, 0.353));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.604, 0.241));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(255, 255, 0, 220)));
	color_gradient.push_back(Particle::ColorGradientItem(0.149, ppl7::grafix::Color(255, 99, 0, 101)));
	color_gradient.push_back(Particle::ColorGradientItem(0.311, ppl7::grafix::Color(40, 41, 43, 101)));
	color_gradient.push_back(Particle::ColorGradientItem(0.428, ppl7::grafix::Color(11, 14, 12, 69)));
	color_gradient.push_back(Particle::ColorGradientItem(0.590, ppl7::grafix::Color(126, 127, 130, 87)));
	color_gradient.push_back(Particle::ColorGradientItem(0.775, ppl7::grafix::Color(255, 255, 255, 23)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(114, 116, 116, 0)));
	particle_velocity_correction=0;

	thuruster1.color.set(255, 200, 0, 255);
	thuruster1.sprite_no=12;
	thuruster1.scale_x=0.8f;
	thuruster1.scale_y=0.5f;
	thuruster1.scale_y=0.5f;
	thuruster1.intensity=64;
	thuruster1.plane=static_cast<int>(LightPlaneId::Player);
	thuruster1.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::All);
	thuruster1.has_lensflare=true;
	thuruster1.flarePlane=static_cast<int>(LightPlayerPlaneMatrix::Player);
	thuruster1.flare_intensity=128;
	thuruster1.flare_useLightColor=true;

	thuruster2=thuruster1;

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
	state=0;
	if (start_direction_a) state=2;
}


void Floater::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	this->frame_rate_compensation=frame_rate_compensation;
	if (current_state == false && (state == 0 || state == 2)) return;
	AudioPool& pool=getAudioPool();
	if (!audio) {
		audio=pool.getInstance(AudioClip::thruster);
		audio->setVolume(0.3f);
		audio->setPositional(p, 1800);
		audio->setLoop(true);
	}
	LightSystem& lights=GetGame().getLightSystem();
	audio->setPositional(p, 1800);
	if (direction == 0) {		// horizontal
		if (next_state < time && (state == 0 || state == 2)) {
			state++;
			pool.playInstance(audio);
		} else if (state == 1) {
			velocity.x=4;
			velocity.y=0;
			p+=(velocity * frame_rate_compensation);
			if (next_birth < time) {
				particle_velocity_correction=0.0f;
				emmitParticles(time, player, ppl7::grafix::PointF(p.x - 64, p.y - 20), 270);
				next_birth=time + randf(0.020, 0.085);
			}
			updateBoundary();
			thuruster1.x=p.x - 74;
			thuruster1.y=p.y - 25;
			thuruster1.angle=90;
			lights.addObjectLight(&thuruster1);

			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 64, p.y));
			if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker) {
				velocity.setPoint(0, 0);
				state=2;
				next_state=time + 5.0f;
				pool.stopInstace(audio);
			}
			if (time > next_animation) {
				flame_sprite1=ppl7::rand(152, 161);
				thuruster1.intensity=ppl7::rand(128, 255);
				thuruster1.flare_intensity=ppl7::rand(64, 128);
				next_animation=time + 0.06f;
			}
		} else if (state == 3) {
			velocity.x=-4;
			velocity.y=0;
			p+=(velocity * frame_rate_compensation);
			if (next_birth < time) {
				particle_velocity_correction=0.0f;
				emmitParticles(time, player, ppl7::grafix::PointF(p.x + 64, p.y - 20), 90);
				next_birth=time + randf(0.020, 0.085);
			}
			updateBoundary();
			thuruster1.x=p.x + 74;
			thuruster1.y=p.y - 24;
			thuruster1.angle=270;
			lights.addObjectLight(&thuruster1);
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 64, p.y));
			if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker) {
				velocity.setPoint(0, 0);
				state=0;
				pool.stopInstace(audio);
				next_state=time + 5.0f;
			}
			if (time > next_animation) {
				flame_sprite1=ppl7::rand(162, 171);
				thuruster1.intensity=ppl7::rand(128, 255);
				thuruster1.flare_intensity=ppl7::rand(64, 128);
				next_animation=time + 0.06f;
			}
		}
	} else { // vertical
		if (next_state < time && (state == 0 || state == 2)) {
			state++;
			pool.playInstance(audio);
		} else if (state == 1) {	// down
			velocity.x=0;
			velocity.y=4;
			p+=(velocity * frame_rate_compensation);
			if (next_birth < time) {
				particle_velocity_correction=velocity.y;
				emmitParticles(time, player, ppl7::grafix::PointF(p.x - 16, p.y - 20), 180);
				emmitParticles(time, player, ppl7::grafix::PointF(p.x + 16, p.y - 20), 180);
				next_birth=time + randf(0.020, 0.085);
			}
			updateBoundary();
			thuruster1.x=p.x - 16;
			thuruster1.y=p.y - 8;
			thuruster2.x=p.x + 16;
			thuruster2.y=p.y - 8;
			thuruster1.angle=0;
			thuruster2.angle=0;
			lights.addObjectLight(&thuruster1);
			lights.addObjectLight(&thuruster2);

			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 32));
			if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker) {
				velocity.setPoint(0, 0);
				state=2;
				next_state=time + 5.0f;
				pool.stopInstace(audio);
			}
			if (time > next_animation) {
				flame_sprite1=ppl7::rand(172, 181);
				flame_sprite2=ppl7::rand(172, 181);
				thuruster1.intensity=ppl7::rand(128, 255);
				thuruster1.flare_intensity=ppl7::rand(64, 128);
				thuruster2.intensity=ppl7::rand(128, 255);
				thuruster2.flare_intensity=ppl7::rand(64, 128);
				next_animation=time + 0.06f;
			}
		} else if (state == 3) {
			velocity.x=0;
			velocity.y=-4;
			p+=(velocity * frame_rate_compensation);
			if (next_birth < time) {
				particle_velocity_correction=0.0f;
				emmitParticles(time, player, ppl7::grafix::PointF(p.x - 16, p.y - 20), 180);
				emmitParticles(time, player, ppl7::grafix::PointF(p.x + 16, p.y - 20), 180);
				next_birth=time + randf(0.020, 0.085);
			}
			updateBoundary();
			thuruster1.x=p.x - 16;
			thuruster1.y=p.y - 8;
			thuruster2.x=p.x + 16;
			thuruster2.y=p.y - 8;
			thuruster1.angle=0;
			thuruster2.angle=0;
			lights.addObjectLight(&thuruster1);
			lights.addObjectLight(&thuruster2);

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
				thuruster1.intensity=ppl7::rand(128, 255);
				thuruster1.flare_intensity=ppl7::rand(64, 128);
				thuruster2.intensity=ppl7::rand(128, 255);
				thuruster2.flare_intensity=ppl7::rand(64, 128);
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
			p.y + coords.y - 10,
			flame_sprite1);
		else if (state == 3) texture->draw(renderer,
			p.x + coords.x + 128 + 32,
			p.y + coords.y - 10,
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
		player->x+=(velocity.x * frame_rate_compensation);
		player->y+=(velocity.y * frame_rate_compensation);
		if (player->y > p.y - 58) player->y=p.y - 58;
	}
}

void Floater::toggle(bool enable, Object* source)
{
	current_state=enable;
	//printf ("Floater::toggle %d: %d\n", id, (int)enable);
}

void Floater::trigger(Object* source)
{
	current_state=!current_state;
}

void Floater::reset()
{
	Object::reset();
	current_state=initial_state;
	p=initial_p;
	state=0;
	if (start_direction_a) state=2;
	if (audio) {
		AudioPool& pool=getAudioPool();
		pool.stopInstace(audio);
	}
}

size_t Floater::saveSize() const
{
	return Object::saveSize() + 3;
}


size_t Floater::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	ppl7::Poke8(buffer + bytes + 1, floater_type);
	int flags=0;
	if (initial_state) flags|=1;
	if (start_direction_a) flags|=2;
	ppl7::Poke8(buffer + bytes + 2, flags);
	return bytes + 3;
}

size_t Floater::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	floater_type=ppl7::Peek8(buffer + bytes + 1);
	int flags=ppl7::Peek8(buffer + bytes + 2);
	initial_state=(bool)(flags & 1);
	start_direction_a=(bool)(flags & 2);
	current_state=initial_state;
	//printf("floater-type: %d, initial_state=%d\n", floater_type, initial_state);
	init();
	return size;
}

void Floater::emmitParticles(double time, const Player& player, const ppl7::grafix::PointF p, float angle)
{

	ParticleSystem* ps=GetParticleSystem();
	if (!emitterInPlayerRange(p, player)) return;
	int new_particles=ppl7::rand(33, 67);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.293, 0.858) + time;
		particle->p=p;
		particle->layer=Particle::Layer::BehindBricks;
		particle->weight=randf(0.000, 0.000);
		particle->gravity.setPoint(0.000, 0.000);
		particle->velocity=calculateVelocity(randf(2.462, 5.102) + particle_velocity_correction, angle + randf(-10.189, 10.189));
		particle->scale=randf(0.554, 1.422);
		particle->color_mod.set(226, 126, 69, 41);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		particle->initScaleGradient(scale_gradient, particle->scale);
		particle->initColorGradient(color_gradient);
		ps->addParticle(particle);
	}

}


class FloaterDialog : public Decker::ui::Dialog
{
private:
	ppltk::ComboBox* floater_type;
	ppltk::CheckBox* initial_state, * current_state;
	ppltk::RadioButton* direction_a, * direction_b;

	Floater* object;

public:
	FloaterDialog(Floater* object);
	virtual void valueChangedEvent(ppltk::Event* event, int value);
	//virtual void textChangedEvent(ppltk::Event *event, const ppl7::String &text);
	virtual void toggledEvent(ppltk::Event* event, bool checked);
	//virtual void mouseDownEvent(ppltk::MouseEvent *event);
};

void Floater::openUi()
{
	FloaterDialog* dialog=new FloaterDialog(this);
	GetGameWindow()->addChild(dialog);
}

FloaterDialog::FloaterDialog(Floater* object)
	: Decker::ui::Dialog(400, 230)
{
	this->object=object;
	ppl7::grafix::Rect client=clientRect();
	setWindowTitle(ppl7::ToString("Floater, Object ID: %u", object->id));
	int y=0;
	int sw=client.width() / 2;

	addChild(new ppltk::Label(0, y, 120, 30, "Floater-Type: "));
	floater_type=new ppltk::ComboBox(120, y, client.width() - 120, 30);
	floater_type->add("studded", "0");
	floater_type->add("flat", "1");
	floater_type->setCurrentIdentifier(ppl7::ToString("%d", object->floater_type));
	floater_type->setEventHandler(this);
	addChild(floater_type);
	y+=35;


	initial_state=new ppltk::CheckBox(0, y, sw, 30, "initial state");
	initial_state->setEventHandler(this);
	initial_state->setChecked(object->initial_state);
	addChild(initial_state);
	current_state=new ppltk::CheckBox(sw, y, sw, 30, "current state");
	current_state->setEventHandler(this);
	current_state->setChecked(object->current_state);
	addChild(current_state);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Start direction: "));
	ppltk::Frame* dirframe=new ppltk::Frame(120, y, client.width() - 120, 35, ppltk::Frame::BorderStyle::NoBorder);
	addChild(dirframe);
	ppl7::String label_a="left", label_b="right";
	if (object->type() == Type::FloaterVertical) {
		label_a="up";
		label_b="down";
	}
	direction_a=new ppltk::RadioButton(0, 0, 70, 30, label_a, (object->start_direction_a == true));
	direction_a->setEventHandler(this);
	dirframe->addChild(direction_a);
	direction_b=new ppltk::RadioButton(70, 0, 70, 30, label_b, (object->start_direction_a == false));
	direction_b->setEventHandler(this);
	dirframe->addChild(direction_b);
	y+=35;


}

void FloaterDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	if (event->widget() == floater_type) {
		object->floater_type=floater_type->currentIdentifier().toInt();
		object->init();
		object->reset();
	}
}

void FloaterDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (event->widget() == initial_state) {
		object->initial_state=checked;
		object->reset();
	} else if (event->widget() == current_state) {
		object->current_state=checked;
		object->reset();
	} else if (event->widget() == direction_a && checked == true) {
		object->start_direction_a=true;
		object->reset();
	} else if (event->widget() == direction_b && checked == true) {
		object->start_direction_a=false;
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
