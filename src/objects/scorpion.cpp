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
	if (ppl7::rand(0, 2) == 1) {
		sprite_set=Spriteset::ScorpionMetalic;
		type=1;
	}
	sprite_no=0;
	sprite_no_representation=0;
	state=0;
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
		next_animation=time + 0.05f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}


	double dist=ppl7::grafix::Distance(p, player.position());
	if (dist < player_activation_distance && speed < max_speed_when_player_is_near) speed+=0.05 * frame_rate_compensation;

	if (state == 0) {	// do nothing, look to left
		state=1;
		animation.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
		speed=randf(minspeed, maxspeed);
		playAudio(AudioClip::scorpion_run, 0.5f);
	} else if (state == 1) {	// walk left
		if (audio)

			p.x-=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 60, p.y + 6));
		if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || t2 != TileType::Blocking) {
			state=ppl7::rand(2, 3);
			if (state == 2) {
				animation.start(idle_left, sizeof(idle_left) / sizeof(int), true, 0);
				next_state=time + (double)ppl7::rand(min_idle_time, max_idle_time);
				playAudio(AudioClip::scorpion_breath, 0.2f);
			} else {
				state=3;
				animation.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 63);
			}
		}
	} else if (state == 2 && time > next_state) {	// idle left
		state=3;
		animation.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 63);
	} else if (state == 3 && animation.isFinished()) {	// turn from left to right
		state=4;
		animation.start(walk_cycle_right, sizeof(walk_cycle_right) / sizeof(int), true, 0);
		speed=randf(minspeed, maxspeed);
		playAudio(AudioClip::scorpion_run, 0.5f);
	} else if (state == 4) {
		p.x+=speed * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y - 12));
		TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 60, p.y + 6));
		if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || t2 != TileType::Blocking) {
			state=ppl7::rand(5, 6);
			if (state == 5) {
				animation.start(idle_right, sizeof(idle_right) / sizeof(int), true, 0);
				next_state=time + (double)ppl7::rand(min_idle_time, max_idle_time);
				playAudio(AudioClip::scorpion_breath, 0.2f);
			} else {
				state=6;
				animation.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 71);
			}
		}
	} else if (state == 5 && time > next_state) {	// idle right
		state=6;
		animation.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 71);

	} else if (state == 6 && animation.isFinished()) {	// turn from left to right
		state=1;
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
	return bytes + 24;
}

size_t Scorpion::saveSize() const
{
	return Object::saveSize() + 24;
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
	return size;
}


class ScorpionDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::ComboBox* type;
	ppl7::tk::LineInput* min_cooldown;
	ppl7::tk::LineInput* max_cooldown;
	ppl7::tk::LineInput* distance;
	Arrow* arrow_trap;

public:
	ScorpionDialog(Scorpion* object);
	~ScorpionDialog();
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
};

void Scorpion::openUi()
{
	//ScorpionDialog* dialog=new ScorpionDialog(this);
	//GetGameWindow()->addChild(dialog);
}



}	// EOF namespace Decker::Objects
