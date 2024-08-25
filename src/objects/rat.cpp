#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Rat::representation()
{
	return Representation(Spriteset::Rat, 0);
}

static inline int clip(int value) {
	if (value > 255) return 255;
	return value;
}

Rat::Rat()
	:Enemy(Type::ObjectType::Rat)
{
	sprite_set=Spriteset::Rat;
	sprite_no=0;
	sprite_no_representation=0;
	state=RatState::idle;
	collisionDetection=true;
	audio=NULL;
	next_state=0.0f;
	speed=0.0f;
	next_animation=0.0f;
	min_speed=4.0f;
	max_speed=10.0f;
	min_idle=0.5f;
	max_idle=5.0f;
	int base=ppl7::rand(128, 255);
	color_mod.setColor(clip(ppl7::rand(base - 16, base + 16)), clip(ppl7::rand(base - 16, base + 16)),
		clip(ppl7::rand(base - 16, base + 16)), 255);
	look_ahead=ppl7::rand(32, 96);
	scale=ppl7::randf(0.8f, 1.2f);

}

Rat::~Rat()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

static void splatterBlood(double time, const ppl7::grafix::PointF& p)
{
	if (GetGame().config.noBlood) return;
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.005, 0.314));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.455, 1.000));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 0.010));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(255, 0, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(0.342, ppl7::grafix::Color(137, 0, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(157, 0, 0, 0)));
	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(107, 135);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.293, 0.764) + time;
		particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(49, 20), 0.000);
		particle->layer=Particle::Layer::BeforePlayer;
		particle->weight=randf(0.406, 0.642);
		particle->gravity.setPoint(0.000, 0.377);
		particle->velocity=calculateVelocity(randf(2.642, 5.849), 0.000 + randf(-54.340, 54.340));
		particle->scale=randf(0.300, 0.622);
		particle->color_mod.set(230, 0, 0, 255);
		particle->initAnimation(Particle::Type::StaticParticle);
		particle->initScaleGradient(scale_gradient, particle->scale);
		particle->initColorGradient(color_gradient);
		ps->addParticle(particle);
	}
}


void Rat::handleCollision(Player* player, const Collision& collision)
{
	if (state == RatState::dead) return;
	//Player::PlayerMovement movement=player->getMovement();
	//ppl7::PrintDebugTime("player v: %0.3f, gravity: %0.3f\n", player->velocity_move.y, player->gravity,
	//		(int)movement);
	if (player->x<p.x - 48 || player->x>p.x + 48) return;
	if (player->velocity_move.y > 0.0f || player->gravity > 0.0f) {
		if (ppl7::rand(0, 1) == 0 && GetGame().config.difficulty != Config::DifficultyLevel::easy) {
			// run away
			speed=max_speed + 2.0f;
			next_state=player->time + 10.0f;
			if (state == RatState::walk_left || state == RatState::wait_left || state == RatState::turn_right_to_left) {
				state=RatState::walk_right;
				animation.startSequence(15, 29, true, 15);

			} else {
				state=RatState::walk_left;
				animation.startSequence(0, 14, true, 0);
			}
			player->dropHealth(2);
			collisionDetection=false;
			getAudioPool().playOnce(AudioClip::rat_squeek, 0.5f);
			return;
		}
		collisionDetection=false;
		splatterBlood(player->time, p);
		//enabled=false;
		if (state == RatState::walk_left || state == RatState::wait_left || state == RatState::turn_right_to_left)
			sprite_no=61;
		else sprite_no=62;
		state=RatState::dead;

		player->addPoints(50);
		getAudioPool().playOnce(AudioClip::squash1, 0.3f);
		if (audio) {
			getAudioPool().stopInstace(audio);
			delete audio;
			audio=NULL;
		}
	} else {
		if (player->hasSpecialObject(Type::ObjectType::Cheese)) {
			if (!player->hasSpoken(9)) player->speak(9, 1.0f);
			return;
		}
		player->dropHealth(2);
	}
}

void Rat::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	//ppl7::PrintDebugTime("rat %d: state=%d, next_state=%0.3f, time=%0.3f, sprite: %d, animation: %d\n", id, (int)state, next_state, time, sprite_no, animation.isFinished());
	if (time > next_animation && state != RatState::dead) {
		next_animation=time + 0.033f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	if (ppl7::rand(0, 20) == 0) speed=ppl7::randf(min_speed, max_speed);
	if (state == RatState::idle) {
		// Fix initial position from older savegames
		while (ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1)) != TileType::NonBlocking) p.y--;
		while (ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1)) != TileType::Blocking) p.y++;


		int r=ppl7::rand(0, 1);
		if (r == 0) {
			state=RatState::walk_left;
			animation.startSequence(0, 14, true, 0);
			speed=ppl7::randf(min_speed, max_speed);
			next_state=time + ppl7::randf(1.0f, 10.0f);
		} else {
			state=RatState::walk_right;
			animation.startSequence(15, 29, true, 15);
			speed=ppl7::randf(min_speed, max_speed);
			next_state=time + ppl7::randf(1.0f, 10.0f);
		}
	} else if (state == RatState::walk_left) {
		p.x-=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - look_ahead, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - look_ahead, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking || next_state < time) {
			collisionDetection=true;
			state=static_cast<RatState>(ppl7::rand(static_cast<int>(RatState::wait_left),
				static_cast<int>(RatState::turn_left_to_right)));
			if (state == RatState::wait_left) {
				animation.startSequence(30, 39, true, 30);
				next_state=time + ppl7::randf(min_idle, max_idle);
			} else animation.startSequence(55, 59, false, 60);
			look_ahead=ppl7::rand(32, 96);
		}
	} else if (state == RatState::wait_left && time > next_state) {
		animation.startSequence(55, 59, false, 15);
		state=RatState::turn_left_to_right;
	} else if (state == RatState::turn_left_to_right && animation.isFinished()) {
		state=RatState::walk_right;
		speed=ppl7::randf(min_speed, max_speed);
		next_state=time + ppl7::randf(1.0f, 10.0f);
		animation.startSequence(15, 29, true, 15);
	} else if (state == RatState::walk_right) {
		p.x+=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + look_ahead, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + look_ahead, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking || next_state < time) {
			collisionDetection=true;
			state=static_cast<RatState>(ppl7::rand(static_cast<int>(RatState::wait_right),
				static_cast<int>(RatState::turn_right_to_left)));
			if (state == RatState::wait_right) {
				animation.startSequence(40, 49, true, 40);
				next_state=time + ppl7::randf(min_idle, max_idle);
			} else animation.startSequence(50, 54, false, 0);
			look_ahead=ppl7::rand(32, 96);
		}
	} else if (state == RatState::wait_right && time > next_state) {
		animation.startSequence(50, 54, false, 0);
		state=RatState::turn_right_to_left;
	} else if (state == RatState::turn_right_to_left && animation.isFinished()) {
		state=RatState::walk_left;
		next_state=time + ppl7::randf(1.0f, 10.0f);
		speed=ppl7::randf(min_speed, max_speed);
		animation.startSequence(0, 14, true, 0);
	}
	if (!audio && state != RatState::dead) {
		AudioPool& pool=getAudioPool();
		audio=pool.getInstance(AudioClip::rat_noise);
		if (audio) {
			audio->setVolume(0.2f);
			audio->setAutoDelete(false);
			audio->setLoop(true);
			audio->startRandom();
			audio->setPositional(p, 960);
			pool.playInstance(audio);
		}
	} else if (audio) {
		audio->setPositional(p, 960);
	}
}


size_t Rat::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	ppl7::Poke32(buffer + bytes + 1, color_mod.color());
	ppl7::PokeFloat(buffer + bytes + 5, scale);
	ppl7::PokeFloat(buffer + bytes + 9, min_speed);
	ppl7::PokeFloat(buffer + bytes + 13, max_speed);
	ppl7::PokeFloat(buffer + bytes + 17, min_idle);
	ppl7::PokeFloat(buffer + bytes + 21, max_idle);
	return bytes + 25;
}

size_t Rat::saveSize() const
{
	return Object::saveSize() + 25;
}

size_t Rat::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0) return 0;
	if (size < bytes + 1) {
		// Version 0
		color_mod.setColor(255, 255, 255, 255);
		scale=1.0f;
		return size;
	}
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	color_mod.setColor(ppl7::Peek32(buffer + bytes + 1));
	scale=ppl7::PeekFloat(buffer + bytes + 5);
	min_speed=ppl7::PeekFloat(buffer + bytes + 9);
	max_speed=ppl7::PeekFloat(buffer + bytes + 13);
	min_idle=ppl7::PeekFloat(buffer + bytes + 17);
	max_idle=ppl7::PeekFloat(buffer + bytes + 21);
	return size;
}

class RatDialog : public Decker::ui::Dialog
{
private:
	Decker::ui::ColorSliderWidget* color;
	ppltk::DoubleHorizontalSlider* scale;
	ppltk::DoubleHorizontalSlider* min_speed;
	ppltk::DoubleHorizontalSlider* max_speed;
	ppltk::DoubleHorizontalSlider* min_idle;
	ppltk::DoubleHorizontalSlider* max_idle;

	Rat* object;

public:
	RatDialog(Rat* object);

	virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, double value);
};



void Rat::openUi()
{
	RatDialog* dialog=new RatDialog(this);
	GetGameWindow()->addChild(dialog);
}

RatDialog::RatDialog(Rat* object)
	: Decker::ui::Dialog(600, 400, Dialog::Buttons::OK)
{
	ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle("Rat");
	int col1=120;
	int col2=(client.width()) / 2 + 40;

	int sw=col2 - col1;
	int y=0;

	// Color
	addChild(new ppltk::Label(0, y, col1, 30, "Color:"));

	color=new Decker::ui::ColorSliderWidget(col1, y, client.width() - col1, 4 * 35, false);
	color->setEventHandler(this);
	addChild(color);
	y+=4 * 35;

	// Scale
	addChild(new ppltk::Label(0, y, col1, 30, "Scale:"));
	scale=new ppltk::DoubleHorizontalSlider(col1, y, client.width() - col1, 30);
	scale->setEventHandler(this);
	scale->setLimits(0.5f, 1.5f);
	scale->enableSpinBox(true, 0.01f, 3, 80);
	addChild(scale);
	y+=35;

	// Speed
	addChild(new ppltk::Label(0, y, col1, 30, "Speed min:"));
	min_speed=new ppltk::DoubleHorizontalSlider(col1, y, sw, 30);
	min_speed->setEventHandler(this);
	min_speed->setLimits(2.0f, 16.0f);
	min_speed->enableSpinBox(true, 0.01f, 3, 80);
	addChild(min_speed);
	addChild(new ppltk::Label(col2, y, 40, 30, "max:"));
	max_speed=new ppltk::DoubleHorizontalSlider(col2 + 40, y, sw, 30);
	max_speed->setEventHandler(this);
	max_speed->setLimits(2.0f, 16.0f);
	max_speed->enableSpinBox(true, 0.01f, 3, 80);
	addChild(max_speed);
	y+=35;

	// Idle
	addChild(new ppltk::Label(0, y, col1, 30, "Idle min:"));
	min_idle=new ppltk::DoubleHorizontalSlider(col1, y, sw, 30);
	min_idle->setEventHandler(this);
	min_idle->setLimits(0.0f, 10.0f);
	min_idle->enableSpinBox(true, 0.01f, 3, 80);
	addChild(min_idle);
	addChild(new ppltk::Label(col2, y, 40, 30, "max:"));
	max_idle=new ppltk::DoubleHorizontalSlider(col2 + 40, y, sw, 30);
	max_idle->setEventHandler(this);
	max_idle->setLimits(0.0f, 10.0f);
	max_idle->enableSpinBox(true, 0.01f, 3, 80);
	addChild(max_idle);
	y+=35;


	color->setColor(object->color_mod);
	scale->setValue(object->scale);
	min_speed->setValue(object->min_speed);
	max_speed->setValue(object->max_speed);
	min_idle->setValue(object->min_idle);
	max_idle->setValue(object->max_idle);

}


void RatDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	ppltk::Widget* widget=event->widget();
	if (widget == color) {
		object->color_mod=color->color();
	}

}
void RatDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	ppltk::Widget* widget=event->widget();
	if (widget == min_speed) {
		object->min_speed=value;
		if (value > max_speed->value()) max_speed->setValue(value);
	} else if (widget == max_speed) {
		object->max_speed=value;
		if (value < min_speed->value()) min_speed->setValue(value);
	} else if (widget == min_idle) {
		object->min_idle=value;
		if (value > max_idle->value()) max_idle->setValue(value);
	} else if (widget == max_idle) {
		object->max_idle=value;
		if (value < min_idle->value()) min_idle->setValue(value);
	} else if (widget == scale) {
		object->scale=(float)value;
	}
}



}	// EOF namespace Decker::Objects
