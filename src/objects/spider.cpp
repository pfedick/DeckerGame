#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


/*****************************************************************************************
 * SPIDER SPIT
 ****************************************************************************************/

class SpiderSpit : public Object
{
public:
	ppl7::grafix::Point velocity;
	float angle;
	float gravity;

	SpiderSpit();
	static Representation representation();
	virtual void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	virtual void handleCollision(Player* player, const Collision& collision);
};

SpiderSpit::SpiderSpit()
	:Object(Type::ObjectType::Arrow)
{
	collisionDetection=true;
	pixelExactCollision=false;
	sprite_set=Spriteset::Spider;
	sprite_no=35;
	sprite_no_representation=35;
	spawned=true;
	gravity=0.0f;
	angle=0.0f;
}

Representation SpiderSpit::representation()
{
	return Representation(Spriteset::Spider, 35);
}

void SpiderSpit::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (gravity < 5) gravity+=0.2 * frame_rate_compensation;

	float update=velocity.x * frame_rate_compensation;
	float mul=1.0f;
	if (update < 0) mul=-1.0f;


	for (float i=0.0f;i < fabs(update);i+=16.0f) {
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + (i * mul), p.y));
		if (t1 == TileType::Blocking) {
			deleteDefered=true;
			// TODO: flatsch
			getAudioPool().playOnce(AudioClip::bullet_hits_wall, p, 1600, 0.4f);

		} else if (p.x < 0 || p.x>65535 || p.y < 0 || p.y>65535) {
			deleteDefered=true;
		}
	}
	p.x+=update;
	p.y+=gravity;
	updateBoundary();
}

void SpiderSpit::handleCollision(Player* player, const Collision& collision)
{
	deleteDefered=true;
	player->hitBySpiderWeb();
}




/*****************************************************************************************
 * SPIDER
 ****************************************************************************************/


Representation Spider::representation()
{
	return Representation(Spriteset::Spider, 0);
}

Spider::Spider()
	:Enemy(Type::ObjectType::Spider)
{
	sprite_set=Spriteset::Spider;
	sprite_no=0;
	sprite_no_representation=0;
	state=ActionState::Falling;
	orientation=Orientation::Front;
	collisionDetection=true;
	next_animation=0.0f;
	next_state_change=ppl7::randf(0.1f, 2.0f);
	audio=NULL;
	collision_cooldown=0.0f;
	velocity=2.0f;
	velocity_falling=0.0f;
	next_attack_time=0.0f;
	attack_cooldown=3.0f;
	can_attack_player=false;
	initial_state=true;
}

Spider::~Spider()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

size_t Spider::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	uint8_t flags=0;
	if (initial_state) flags|=1;
	if (can_attack_player) flags|=2;
	ppl7::Poke8(buffer + bytes + 1, flags);	// reserved
	ppl7::PokeFloat(buffer + bytes + 2, velocity);
	ppl7::PokeFloat(buffer + bytes + 6, attack_cooldown);
	return bytes + 10;
}

size_t Spider::saveSize() const
{
	return Object::saveSize() + 10;
}

size_t Spider::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0) return 0;
	if (size < bytes + 1) return bytes;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	uint8_t flags=ppl7::Peek8(buffer + bytes + 1);
	initial_state=(flags & 1);
	can_attack_player=(flags & 2);
	enabled=initial_state;
	velocity=ppl7::PeekFloat(buffer + bytes + 2);
	attack_cooldown=ppl7::PeekFloat(buffer + bytes + 6);
	return size;
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

void Spider::handleCollision(Player* player, const Collision& collision)
{
	if (state == ActionState::Dead) return;
	if (player->velocity_move.y > 0.0f || player->gravity > 0.0f) {
		collisionDetection=false;
		splatterBlood(player->time, p);
		state=ActionState::Dead;
		animation.startSequence(29, 34, false, 34);
		next_state_change=player->time + 5.0f;
		player->addPoints(50);
		getAudioPool().playOnce(AudioClip::squash1, 0.3f);
		if (audio) {
			getAudioPool().stopInstace(audio);
			delete audio;
			audio=NULL;
		}
		return;
	}


	if (collision_cooldown < player->time) {
		player->dropHealth(5);
		collision_cooldown=player->time + 0.5f;
	}
}

void Spider::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	//AudioPool& audiopool=getAudioPool();

	if (time > next_animation) {
		next_animation=time + 0.03f;
		animation.update();
		if (animation.getFrame() != sprite_no) {
			sprite_no=animation.getFrame();
		}
	}

	double dist=ppl7::grafix::Distance(p, player.position());


	//ppl7::PrintDebug("Spider state: %d\n", static_cast<int>(state));
	if (state == ActionState::Stand && orientation == Orientation::Front && next_state_change < time) {
		state=ActionState::Walk;
		if (ppl7::rand(0, 1) == 0) {
			orientation=Orientation::Left;
			animation.startSequence(2, 14, true, 2);
		} else {
			orientation=Orientation::Right;
			animation.startSequence(16, 28, true, 16);

		}
	} else if (state == ActionState::Falling) {
		if (velocity_falling < 12.0f) {
			velocity_falling+=0.2 * frame_rate_compensation;
			if (velocity_falling > 12.0f) velocity_falling=12.0f;
		}
		//ppl7::PrintDebug("Falling: %0.3f\n", velocity_falling);
		for (int yy=p.y;yy <= p.y + velocity_falling;yy++) {
			TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, yy));
			if (t1 != TileType::NonBlocking) {
				state=ActionState::Stand;
				orientation=Orientation::Front;
				p.y=yy - 1;
				next_state_change=ppl7::randf(0.1f, 2.0f);
				break;
			}
		}
		if (state == ActionState::Falling) p.y+=velocity_falling;

	} else if (state == ActionState::Walk && orientation == Orientation::Left) {
		p.x-=velocity * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=ActionState::Stand;
			orientation=Orientation::Left;
			animation.setStaticFrame(1);
			next_state_change=time + randf(0.5, 5.0f);
		}

	} else 	if (state == ActionState::Walk && orientation == Orientation::Right) {
		p.x+=velocity * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y + 6));
		if (t1 != TileType::NonBlocking || t2 != TileType::Blocking) {
			state=ActionState::Stand;
			orientation=Orientation::Right;
			animation.setStaticFrame(15);
			next_state_change=time + randf(0.5, 5.0f);
		}
	} else if (state == ActionState::Stand && orientation == Orientation::Left && time > next_state_change) {
		state=ActionState::Walk;
		orientation = Orientation::Right;
		animation.startSequence(16, 28, true, 16);
	} else if (state == ActionState::Stand && orientation == Orientation::Right && time > next_state_change) {
		state=ActionState::Walk;
		orientation = Orientation::Left;
		animation.startSequence(2, 14, true, 2);
	} else if (state == ActionState::Dead && next_state_change < time) {
		enabled=false;
	}
	updateBoundary();
	/*
	if (!audio) {
		audio=audiopool.getInstance(AudioClip::crow_flying);
		if (audio) {
			audio->setVolume(1.0f);
			audio->setAutoDelete(false);
			audio->setLoop(true);
			audio->setPositional(p, 1600);
			audiopool.playInstance(audio);
		}
	} else if (audio) {
		audio->setPositional(p, 1600);
	}
	*/

}

void Spider::attack(double time, Player& player)
{
	attack_cooldown=time + 1.0f;

}


/**********************************************************************************************
 * Spider Dialog
 **********************************************************************************************/

class SpiderDialog : public Decker::ui::Dialog
{
private:
	ppltk::DoubleHorizontalSlider* velocity;
	ppltk::DoubleHorizontalSlider* attack_cooldown;
	ppltk::CheckBox* current_state;
	ppltk::CheckBox* initial_state;
	ppltk::CheckBox* can_attack_player;

	Spider* object;

public:
	SpiderDialog(Spider* object);
	virtual void valueChangedEvent(ppltk::Event* event, double value) override;
	virtual void toggledEvent(ppltk::Event* event, bool checked) override;
};

void Spider::openUi()
{
	SpiderDialog* dialog=new SpiderDialog(this);
	GetGameWindow()->addChild(dialog);
}

SpiderDialog::SpiderDialog(Spider* object)
	: Decker::ui::Dialog(500, 300)
{
	this->object=object;
	setWindowTitle(ppl7::ToString("Spider, ID: %d", object->id));
	ppl7::grafix::Rect client=clientRect();
	int y=0;
	int col1=100;
	int w=client.width() - col1 - 10;
	int col2=client.width() / 2;
	int col3=col2 + 100;

	addChild(new ppltk::Label(0, y, 100, 30, "initial State:"));
	initial_state=new ppltk::CheckBox(col1, y, 120, 30, "enabled", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);
	addChild(new ppltk::Label(col2, y, 100, 30, "current State:"));
	current_state=new ppltk::CheckBox(col3, y, 120, 30, "enabled", object->enabled);
	current_state->setEventHandler(this);
	addChild(current_state);
	y+=35;

	can_attack_player=new ppltk::CheckBox(0, y, 200, 30, "can attack player", object->can_attack_player);
	can_attack_player->setEventHandler(this);
	addChild(can_attack_player);
	y+=35;

	addChild(new ppltk::Label(0, y, 100, 30, "Velocity:"));
	velocity=new ppltk::DoubleHorizontalSlider(col1, y, w, 30);
	velocity->setLimits(1.0f, 6.0f);
	velocity->enableSpinBox(true, 0.1f, 1, 80);
	velocity->setValue(object->velocity);
	velocity->setEventHandler(this);
	addChild(velocity);
	y+=35;

	addChild(new ppltk::Label(0, y, 100, 30, "Attack cooldown:"));
	attack_cooldown=new ppltk::DoubleHorizontalSlider(col1, y, w, 30);
	attack_cooldown->setLimits(0.0f, 10.0f);
	attack_cooldown->enableSpinBox(true, 0.1f, 1, 100);
	attack_cooldown->setValue(object->attack_cooldown);
	attack_cooldown->setEventHandler(this);
	addChild(attack_cooldown);
	y+=35;
}


void SpiderDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	if (event->widget() == velocity) {
		object->velocity=value;
	} else 	if (event->widget() == attack_cooldown) {
		object->attack_cooldown=value;
	}

}

void SpiderDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (event->widget() == initial_state) {
		object->initial_state=checked;
	} else if (event->widget() == current_state) {
		object->enabled=checked;
	} else if (event->widget() == can_attack_player) {
		object->can_attack_player=checked;
	}
}



}	// EOF namespace Decker::Objects
