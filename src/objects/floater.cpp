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
			p+=velocity * frame_rate_compensation;
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
			p+=velocity * frame_rate_compensation;
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
		} else if (state == 1) {
			velocity.x=0;
			velocity.y=4;
			p+=velocity * frame_rate_compensation;
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
				thuruster1.intensity=ppl7::rand(128, 255);
				thuruster1.flare_intensity=ppl7::rand(64, 128);
				thuruster2.intensity=ppl7::rand(128, 255);
				thuruster2.flare_intensity=ppl7::rand(64, 128);
				next_animation=time + 0.06f;
			}
		} else if (state == 3) {
			velocity.x=0;
			velocity.y=-4;
			p+=velocity * frame_rate_compensation;
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
		player->x+=velocity.x * frame_rate_compensation;
		player->y+=velocity.y * frame_rate_compensation;
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
	ppl7::Poke8(buffer + bytes + 2, initial_state);
	return bytes + 3;
}

size_t Floater::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	floater_type=ppl7::Peek8(buffer + bytes + 1);
	initial_state=(bool)(ppl7::Peek8(buffer + bytes + 2) & 1);
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
