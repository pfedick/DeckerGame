#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "audiopool.h"

namespace Decker::Objects {

static int walk_cycle_left[]={ 1,2,3,4,5,6,7,8 };
static int walk_cycle_right[]={ 10,11,12,13,14,15,16,17 };
static int turn_from_left_to_right[]={ 22,23,24,25,26 };
static int turn_from_right_to_left[]={ 18,19,20,21,22 };
static int turn_from_mid_to_left[]={ 27,20,21,22 };
static int death_animation[]={ 79,80,81,82,83,84,85,86,87,88 };


Representation Mushroom::representation()
{
	return Representation(Spriteset::Mushroom, 27);
}

Mushroom::Mushroom()
	:Enemy(Type::ObjectType::Mushroom)
{
	sprite_set=Spriteset::Mushroom;
	sprite_no=27;
	animation.setStaticFrame(27);
	sprite_no_representation=27;
	next_state=ppl7::GetMicrotime() + (double)ppl7::rand(5, 20);
	state=ActionState::Falling;
	next_animation=0.0f;
	collisionDetection=true;
	animation.start(turn_from_mid_to_left, sizeof(turn_from_mid_to_left) / sizeof(int), false, 0);
	lightmap.color.set(200, 200, 255, 255);
	lightmap.sprite_no=1;
	lightmap.scale_x=1.0f;
	lightmap.scale_y=1.0f;
	lightmap.custom_texture=NULL;
	lightmap.intensity=255;
	lightmap.plane=static_cast<int>(LightPlaneId::Player);
	lightmap.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);
	max_gravity_y=ppl7::randf(1.0f, 2.0f);
	gravity_x=0.0f;
	initial_state=true;

}

void Mushroom::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (!enabled) return;
	if (time > next_animation) {
		next_animation=time + 0.07f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	lightmap.x=p.x;
	lightmap.y=p.y;
	lightmap.custom_texture=texture;
	lightmap.sprite_no=sprite_no + 89;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&lightmap);

	if (state == ActionState::Start && animation.isFinished()) {
		state=ActionState::WalkLeft;
		animation.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
	} else if (state == ActionState::WalkLeft) {	// walk left
		p.x-=1 * frame_rate_compensation;
		updateBoundary();
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 20, p.y - 6));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 20, p.y + 2));
		if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || t2 != TileType::Blocking) {
			state=ActionState::WaitForTurnRight;
			animation.setStaticFrame(0);
			next_state=time + (double)ppl7::rand(1, 5);
		}
	} else if (state == ActionState::WaitForTurnRight && time > next_state) {
		getAudioPool().playOnce(AudioClip::skeleton_turn, p, 1600, 1.0f);
		animation.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 26);
		state=ActionState::TurnRight;
	} else if (state == ActionState::TurnRight && animation.isFinished()) {
		state=ActionState::WalkRight;
		animation.start(walk_cycle_right, sizeof(walk_cycle_right) / sizeof(int), true, 9);

	} else if (state == ActionState::WalkRight) {
		p.x+=1 * frame_rate_compensation;
		updateBoundary();
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 20, p.y - 6));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 20, p.y + 2));
		if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || t2 != TileType::Blocking) {
			state=ActionState::WaitForTurnLeft;
			animation.setStaticFrame(9);
			next_state=time + (double)ppl7::rand(1, 5);
		}
	} else if (state == ActionState::WaitForTurnLeft && time > next_state) {
		getAudioPool().playOnce(AudioClip::skeleton_turn, p, 1600, 1.0f);
		animation.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 22);
		state=ActionState::TurnLeft;
	} else if (state == ActionState::TurnLeft) {
		if (animation.isFinished()) {
			state=ActionState::Start;
			next_state=time + 5.0f;
		}
	} else if (state == ActionState::Dead && time > next_state) {
		enabled=false;
		state=ActionState::Disabled;
	} else if (state == ActionState::Falling) {
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 2));
		if (t1 != TileType::NonBlocking) state=ActionState::Start;
		else {
			if (velocity.y < max_gravity_y) {
				velocity.y+=0.2 * frame_rate_compensation;
				if (velocity.y > max_gravity_y) velocity.y=max_gravity_y;
			}
			p+=velocity;
		}
	}
	if (state != ActionState::Falling && state != ActionState::Dead && state != ActionState::Disabled) {
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 2));
		if (t1 == TileType::NonBlocking) {
			state=ActionState::Falling;
			velocity.y=0.0f;
		}

	}
}


static void emmitParticles(double time, const ppl7::grafix::PointF& p)
{
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.286));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(107, 145, 104, 29)));
	color_gradient.push_back(Particle::ColorGradientItem(0.279, ppl7::grafix::Color(155, 255, 160, 29)));
	color_gradient.push_back(Particle::ColorGradientItem(0.653, ppl7::grafix::Color(125, 237, 216, 21)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(84, 100, 69, 0)));

	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(200, 300);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.104, 1.235) + time;
		particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(49, 1), 0.000);
		particle->layer=Particle::Layer::BeforePlayer;
		particle->weight=randf(0.000, 0.000);
		particle->gravity.setPoint(0.000, 0.000);
		particle->velocity=calculateVelocity(randf(1.396, 3.981), 0.000 + randf(-10.189, 10.189));
		particle->scale=randf(0.723, 1.799);
		particle->color_mod.set(210, 255, 203, 29);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		particle->initScaleGradient(scale_gradient, particle->scale);
		particle->initColorGradient(color_gradient);
		ps->addParticle(particle);
	}
}


void Mushroom::handleCollision(Player* player, const Collision& collision)
{
	Player::PlayerMovement movement=player->getMovement();
	if (collision.onFoot() == true && movement == Player::Falling) {
		animation.start(death_animation, sizeof(death_animation) / sizeof(int), false, 100);
		state=ActionState::Dead;
		collisionDetection=false;
		//enabled=false;
		player->addPoints(50);
		getAudioPool().playOnce(AudioClip::squash1, 0.5f);
		emmitParticles(player->time, p);
		//AudioPool &audio=getAudioPool();
		//audio.playOnce(AudioClip::skeleton_death);

	} else {
		player->dropHealth(1);
	}
}

void Mushroom::toggle(bool enable, Object* source)
{
	if (state != ActionState::Disabled) {
		this->enabled=enable;
	}
}

void Mushroom::trigger(Object* source)
{
	if (state != ActionState::Disabled) {
		if (enabled) toggle(false, source);
		else toggle(true, source);
	}
}

size_t Mushroom::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 2);		// Object Version

	uint8_t flags=0;
	if (initial_state) flags|=1;
	ppl7::Poke8(buffer + bytes + 1, flags);	// reserved
	ppl7::PokeFloat(buffer + bytes + 2, gravity_x);
	ppl7::PokeFloat(buffer + bytes + 6, max_gravity_y);
	return bytes + 10;
}
size_t Mushroom::saveSize() const
{
	return Object::saveSize() + 10;
}

size_t Mushroom::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0) return 0;
	if (size < bytes + 1) return bytes;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 2) return 0;
	uint8_t flags=ppl7::Peek8(buffer + bytes + 1);
	initial_state=(flags & 1);
	enabled=initial_state;
	gravity_x=ppl7::PeekFloat(buffer + bytes + 2);
	velocity.x=gravity_x;
	max_gravity_y=ppl7::PeekFloat(buffer + bytes + 6);
	return size;
}



/**********************************************************************************************
 * Mushroom Dialog
 **********************************************************************************************/

class MushroomDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::DoubleHorizontalSlider* max_gravity_y;
	ppl7::tk::DoubleHorizontalSlider* gravity_x;
	ppl7::tk::CheckBox* current_state;
	ppl7::tk::CheckBox* initial_state;

	Mushroom* object;

public:
	MushroomDialog(Mushroom* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, double value) override;
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked) override;
};

void Mushroom::openUi()
{
	MushroomDialog* dialog=new MushroomDialog(this);
	GetGameWindow()->addChild(dialog);
}

MushroomDialog::MushroomDialog(Mushroom* object)
	: Decker::ui::Dialog(500, 300)
{
	this->object=object;
	setWindowTitle(ppl7::ToString("Mushroom, ID: %d", object->id));
	ppl7::grafix::Rect client=clientRect();
	int y=0;
	int col1=100;
	int w=client.width() - col1 - 10;
	int col2=client.width() / 2;
	int col3=col2 + 100;

	addChild(new ppl7::tk::Label(0, y, 100, 30, "initial State:"));
	initial_state=new ppl7::tk::CheckBox(col1, y, 120, 30, "enabled", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);
	addChild(new ppl7::tk::Label(col2, y, 100, 30, "current State:"));
	current_state=new ppl7::tk::CheckBox(col3, y, 120, 30, "enabled", object->enabled);
	current_state->setEventHandler(this);
	addChild(current_state);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 100, 30, "Gravity:"));
	max_gravity_y=new ppl7::tk::DoubleHorizontalSlider(col1, y, w, 30);
	max_gravity_y->setLimits(1.0f, 4.0f);
	max_gravity_y->enableSpinBox(true, 0.1f, 1, 80);
	max_gravity_y->setValue(object->max_gravity_y);
	max_gravity_y->setEventHandler(this);
	addChild(max_gravity_y);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 100, 30, "Gravity x:"));
	gravity_x=new ppl7::tk::DoubleHorizontalSlider(col1, y, w, 30);
	gravity_x->setLimits(0.0f, 360.0f);
	gravity_x->enableSpinBox(true, 1.0f, 0, 80);
	gravity_x->setValue(object->gravity_x);
	gravity_x->setEventHandler(this);
	addChild(gravity_x);
	y+=35;
}

void MushroomDialog::valueChangedEvent(ppl7::tk::Event* event, double value)
{
	if (event->widget() == max_gravity_y) {
		object->max_gravity_y=value;
	} else 	if (event->widget() == gravity_x) {
		object->gravity_x=value;
	}

}

void MushroomDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	if (event->widget() == initial_state) {
		object->initial_state=checked;
	} else if (event->widget() == current_state) {
		object->enabled=checked;
	}
}



}	// EOF namespace Decker::Objects
