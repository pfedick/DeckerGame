#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

static int walk_cycle_left[]={ 0,1,2,3,4,5,6,7,8,9,10,11 };
static int walk_cycle_right[]={ 14,15,16,17,18,19,20,21,22,23,24,25 };
static int idle_left[]={ 27,28,29,30,31,32,33,34,35,36,37,38,39 };
static int idle_right[]={ 41,42,43,44,45,46,47,48,49,50,51,52,53,54 };
static int turn_from_left_to_right[]={ 56,57,58,59,60,61,62,63 };
static int turn_from_right_to_left[]={ 64,65,66,67,68,69,70,71 };


Representation Scorpion::representation()
{
	return Representation(Spriteset::Scorpion, 0);
}

Scorpion::Scorpion()
	:Enemy(Type::ObjectType::Scorpion)
{
	sprite_set=Spriteset::Scorpion;
	type=0;
	sprite_no=0;
	sprite_no_representation=0;
	state=ActionState::WaitLeft;
	collisionDetection=true;
	next_animation=0.0f;
	speed=2;
	next_state=ppl7::GetMicrotime() + (double)ppl7::rand(5, 20);
	audio=NULL;
	minspeed=1.5f;
	maxspeed=3.0f;
	max_speed_when_player_is_near=7.0f;
	player_activation_distance=500;
	min_idle_time=1.0f;
	max_idle_time=10.0f;
	speed_acceleration=0.05;
}

Scorpion::~Scorpion()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

void Scorpion::playAudio(int id, float volume)
{
	AudioPool& audiopool=getAudioPool();
	if (audio) {
		audiopool.stopInstace(audio);
		delete audio;
		audio=NULL;
	}
	audio=audiopool.getInstance(static_cast<AudioClip::Id>(id));
	if (audio) {
		audio->setVolume(volume);
		audio->setAutoDelete(false);
		audio->setLoop(true);
		audiopool.playInstance(audio);
	}
}

void Scorpion::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(10);
}

void Scorpion::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{

	if (time > next_animation) {
		if (type == 0) updateSpriteset(Spriteset::Scorpion);
		else updateSpriteset(Spriteset::ScorpionMetalic);
		//ppl7::PrintDebugTime("type=%d, spriteset=%d\n", type, (int)sprite_set);
		next_animation=time + 0.05f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}


	double dist=ppl7::grafix::Distance(p, player.position());
	if (dist < player_activation_distance && speed < max_speed_when_player_is_near) speed+=speed_acceleration * frame_rate_compensation;

	if (state == ActionState::WaitLeft) {	// do nothing, look to left
		state=ActionState::WalkLeft;
		animation.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
		speed=randf(minspeed, maxspeed);
		playAudio(AudioClip::scorpion_run, 0.5f);
	} else if (state == ActionState::WalkLeft) {	// walk left
		//if (audio)
		p.x-=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y + 6));
		if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || t2 != TileType::Blocking) {
			if (ppl7::rand(2, 3)==2) {
				state=ActionState::IdleLeft;
				animation.start(idle_left, sizeof(idle_left) / sizeof(int), true, 0);
				next_state=time + (double)ppl7::rand(min_idle_time, max_idle_time);
				playAudio(AudioClip::scorpion_breath, 0.2f);
			} else {
				state=ActionState::TurnRight;
				animation.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 63);
			}
		}
	} else if (state == ActionState::IdleLeft && time > next_state) {	// idle left
		state=ActionState::TurnRight;
		animation.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 63);
	} else if (state == ActionState::TurnRight && animation.isFinished()) {	// turn from left to right
		state=ActionState::WalkRight;
		animation.start(walk_cycle_right, sizeof(walk_cycle_right) / sizeof(int), true, 0);
		speed=randf(minspeed, maxspeed);
		playAudio(AudioClip::scorpion_run, 0.5f);
	} else if (state == ActionState::WalkRight) {
		p.x+=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y + 6));
		if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || t2 != TileType::Blocking) {
			if (ppl7::rand(5, 6) == 5) {
				state=ActionState::IdleRight;
				animation.start(idle_right, sizeof(idle_right) / sizeof(int), true, 0);
				next_state=time + (double)ppl7::rand(min_idle_time, max_idle_time);
				playAudio(AudioClip::scorpion_breath, 0.2f);
			} else {
				state=ActionState::TurnLeft;
				animation.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 71);
			}
		}
	} else if (state == ActionState::IdleRight && time > next_state) {	// idle right
		state=ActionState::TurnLeft;
		animation.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 71);

	} else if (state == ActionState::TurnLeft && animation.isFinished()) {	// turn from left to right
		state=ActionState::WalkLeft;
		animation.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
		speed=randf(minspeed, maxspeed);
		playAudio(AudioClip::scorpion_run, 0.5f);
	}
	if (audio) audio->setPositional(p, 960);
}

size_t Scorpion::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	ppl7::Poke8(buffer + bytes + 1, type);
	ppl7::Poke16(buffer + bytes + 2, player_activation_distance);
	ppl7::PokeFloat(buffer + bytes + 4, minspeed);
	ppl7::PokeFloat(buffer + bytes + 8, maxspeed);
	ppl7::PokeFloat(buffer + bytes + 12, max_speed_when_player_is_near);
	ppl7::PokeFloat(buffer + bytes + 16, min_idle_time);
	ppl7::PokeFloat(buffer + bytes + 20, max_idle_time);
	ppl7::PokeFloat(buffer + bytes + 24, speed_acceleration);
	return bytes + 28;
}

size_t Scorpion::saveSize() const
{
	return Object::saveSize() + 28;
}

size_t Scorpion::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	type=ppl7::Peek8(buffer + bytes + 1);
	player_activation_distance=ppl7::Peek16(buffer + bytes + 2);
	minspeed=ppl7::PeekFloat(buffer + bytes + 4);
	maxspeed=ppl7::PeekFloat(buffer + bytes + 8);
	max_speed_when_player_is_near=ppl7::PeekFloat(buffer + bytes + 12);
	min_idle_time=ppl7::PeekFloat(buffer + bytes + 16);
	max_idle_time=ppl7::PeekFloat(buffer + bytes + 20);
	speed_acceleration=ppl7::PeekFloat(buffer + bytes + 24);
	return size;
}


class ScorpionDialog : public Decker::ui::Dialog
{
private:
	ppltk::ComboBox* scorpion_type;
	ppltk::HorizontalSlider* player_distance;
	ppltk::DoubleHorizontalSlider* minspeed;
	ppltk::DoubleHorizontalSlider* maxspeed;
	ppltk::DoubleHorizontalSlider* max_speed_when_player_is_near;
	ppltk::DoubleHorizontalSlider* min_idle_time;
	ppltk::DoubleHorizontalSlider* max_idle_time;
	ppltk::DoubleHorizontalSlider* speed_acceleration;
	Scorpion* object;

	void setValuesToUi(const Scorpion* object);

public:
	ScorpionDialog(Scorpion* object);
	//virtual void selectionChangedEvent(ppltk::Event* event) override;
	virtual void valueChangedEvent(ppltk::Event* event, int value);
	virtual void valueChangedEvent(ppltk::Event* event, double value) override;
	virtual void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
};

void Scorpion::openUi()
{
	ScorpionDialog* dialog=new ScorpionDialog(this);
	GetGameWindow()->addChild(dialog);
}

ScorpionDialog::ScorpionDialog(Scorpion* object)
	: Decker::ui::Dialog(600, 400, Dialog::Buttons::OK | Dialog::Buttons::CopyAndPaste)
{
	this->object=object;
	this->setWindowTitle("Scorpion");
	ppl7::grafix::Rect client=clientRect();
	int y=0;
	addChild(new ppltk::Label(0, y, 100, 30, "Scorpion-Type:"));
	scorpion_type=new ppltk::ComboBox(100, y, 150, 30);
	scorpion_type->add("normal", "0");
	scorpion_type->add("metalic", "1");
	scorpion_type->setEventHandler(this);
	addChild(scorpion_type);
	y+=35;

	int sw=(client.width() - 140 - 20);

	addChild(new ppltk::Label(0, y, 140, 30, "Minimum speed:"));
	minspeed=new ppltk::DoubleHorizontalSlider(140, y, sw, 30);
	minspeed->setEventHandler(this);
	minspeed->setLimits(1.0f, 20.0f);
	minspeed->enableSpinBox(true, 0.1f, 3, 80);
	addChild(minspeed);
	y+=35;

	addChild(new ppltk::Label(0, y, 140, 30, "Maximum speed:"));
	maxspeed=new ppltk::DoubleHorizontalSlider(140, y, sw, 30);
	maxspeed->setEventHandler(this);
	maxspeed->setLimits(1.0f, 20.0f);
	maxspeed->enableSpinBox(true, 0.1f, 3, 80);
	addChild(maxspeed);
	y+=35;

	addChild(new ppltk::Label(0, y, 140, 30, "Min idle time:"));
	min_idle_time=new ppltk::DoubleHorizontalSlider(140, y, sw, 30);
	min_idle_time->setEventHandler(this);
	min_idle_time->setLimits(0.0f, 20.0f);
	min_idle_time->enableSpinBox(true, 0.1f, 3, 80);
	addChild(min_idle_time);
	y+=35;

	addChild(new ppltk::Label(0, y, 140, 30, "Max idle time:"));
	max_idle_time=new ppltk::DoubleHorizontalSlider(140, y, sw, 30);
	max_idle_time->setEventHandler(this);
	max_idle_time->setLimits(0.0f, 20.0f);
	max_idle_time->enableSpinBox(true, 0.1f, 3, 80);
	addChild(max_idle_time);
	y+=35;

	addChild(new ppltk::Label(0, y, 140, 30, "Player distance:"));
	player_distance=new ppltk::HorizontalSlider(140, y, sw, 30);
	player_distance->setEventHandler(this);
	player_distance->setLimits(100, 1920);
	player_distance->enableSpinBox(true, 100, 80);
	addChild(player_distance);
	y+=35;

	addChild(new ppltk::Label(0, y, 300, 30, "Max speed when player is near:"));
	y+=35;
	max_speed_when_player_is_near=new ppltk::DoubleHorizontalSlider(140, y, sw, 30);
	max_speed_when_player_is_near->setEventHandler(this);
	max_speed_when_player_is_near->setLimits(1.0f, 20.0f);
	max_speed_when_player_is_near->enableSpinBox(true, 0.1f, 3, 80);
	addChild(max_speed_when_player_is_near);
	y+=35;
	addChild(new ppltk::Label(0, y, 300, 30, "Speed acceleration:"));
	speed_acceleration=new ppltk::DoubleHorizontalSlider(140, y, sw, 30);
	speed_acceleration->setEventHandler(this);
	speed_acceleration->setLimits(0.01f, 2.0f);
	speed_acceleration->enableSpinBox(true, 0.01f, 2, 80);
	addChild(speed_acceleration);
	y+=35;


	setValuesToUi(object);
}

void ScorpionDialog::setValuesToUi(const Scorpion* object)
{
	scorpion_type->setCurrentIdentifier(ppl7::ToString("%d", object->type));
	minspeed->setValue(object->minspeed);
	maxspeed->setValue(object->maxspeed);
	min_idle_time->setValue(object->min_idle_time);
	max_idle_time->setValue(object->max_idle_time);
	max_speed_when_player_is_near->setValue(object->max_speed_when_player_is_near);
	player_distance->setValue(object->player_activation_distance);
	speed_acceleration->setValue(object->speed_acceleration);
}

void ScorpionDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with int value\n");
	ppltk::Widget* widget=event->widget();
	if (widget == scorpion_type) {
		object->type=scorpion_type->currentIdentifier().toInt();
	}
}

void ScorpionDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with double value\n");
	ppltk::Widget* widget=event->widget();
	if (widget == minspeed) {
		object->minspeed=value;
		if (object->minspeed > object->maxspeed) {
			object->maxspeed=value;
			maxspeed->setValue(object->maxspeed);
		}
	} else if (widget == maxspeed) {
		object->maxspeed=value;
		if (object->maxspeed < object->minspeed) {
			object->minspeed=value;
			minspeed->setValue(object->minspeed);
		}
	} else if (widget == min_idle_time) {
		object->min_idle_time=value;
		if (object->min_idle_time > object->max_idle_time) {
			object->max_idle_time=value;
			max_idle_time->setValue(object->max_idle_time);
		}
	} else if (widget == max_idle_time) {
		object->max_idle_time=value;
		if (object->max_idle_time < object->min_idle_time) {
			object->min_idle_time=value;
			min_idle_time->setValue(object->min_idle_time);
		}
	} else if (widget == max_speed_when_player_is_near) {
		object->max_speed_when_player_is_near=value;
	} else if (widget == speed_acceleration) {
		object->speed_acceleration=value;
	}
}

void ScorpionDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with double value\n");
	ppltk::Widget* widget=event->widget();
	if (widget == player_distance) {
		object->player_activation_distance=(int)value;
	}
}

static Scorpion clipboard;

void ScorpionDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button == Dialog::Buttons::Copy) {
		clipboard.type=object->type;
		clipboard.player_activation_distance=object->player_activation_distance;
		clipboard.minspeed=object->minspeed;
		clipboard.maxspeed=object->maxspeed;
		clipboard.max_speed_when_player_is_near=object->max_speed_when_player_is_near;
		clipboard.min_idle_time=object->min_idle_time;
		clipboard.max_idle_time=object->max_idle_time;
		clipboard.speed_acceleration=object->speed_acceleration;
	} else if (button == Dialog::Buttons::Paste) {
		object->type=clipboard.type;
		object->player_activation_distance=clipboard.player_activation_distance;
		object->minspeed=clipboard.minspeed;
		object->maxspeed=clipboard.maxspeed;
		object->max_speed_when_player_is_near=clipboard.max_speed_when_player_is_near;
		object->min_idle_time=clipboard.min_idle_time;
		object->max_idle_time=clipboard.max_idle_time;
		object->speed_acceleration=clipboard.speed_acceleration;
	}
}

}	// EOF namespace Decker::Objects
