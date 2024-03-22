#ifndef INCLUDE_OBJECTS_H_
#define INCLUDE_OBJECTS_H_

#include <map>
#include <list>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "animation.h"
#include "particle.h"
#include "light.h"


class SDL;
struct SDL_Renderer;
class SpriteTexture;
class LightObject;

class Player;
class TileTypePlane;
class Waynet;

namespace ppl7::tk {
class Widget;
}

class AudioInstance;

namespace Decker::Objects {

class Type
{
public:
	// ID must be <256
	enum ObjectType {
		PlayerStartpoint=1,
		Savepoint=2,
		Medikit=3,
		Crystal=4,
		Diamond=5,
		Coin=6,
		Key=7,
		Door=8,
		FloaterHorizontal=9,
		FloaterVertical=10,
		BreakingGround=11,
		Fire=12,
		WindEmitter=13,
		Vent=14,
		Speaker=15,
		Particle=16,
		TouchEmitter=17,
		TreasureChest=18,
		WarpGate=19,
		ExtraLife=20,
		Switch=21,
		LevelEnd=22,
		RainEmitter=23,
		Apple=24,
		Cherry=25,
		ParticleEmitter=26,
		Oxygen=27,
		BreakingWall=28,
		Hammer=29,
		Cheese=30,
		VoiceTrigger=31,
		ObjectWatcher=32,
		Trigger=33,
		Flashlight=34,
		LightTrigger=35,
		LevelModificator=36,
		TouchPlateSwitch=37,
		ButtonSwitch=38,
		LightSignal=39,
		Arrow=100,
		ThreeSpeers=101,
		Rat=102,
		HangingSpider=103,
		Skeleton=104,
		Mummy=105,
		LaserBeamHorizontal=106,
		LaserBeamVertical=107,
		Mushroom=108,
		Scarabeus=109,
		Stamper=110,
		Wallenstein=111,
		Helena=112,
		Scorpion=113,
		Bat=114,
		Bird=115,
		Yeti=116,
		AutoGeorge=117,
		Ostrich=118,
		Fish=119,
		Piranha=120,
		Ghost=121,
		Zombie=122,
		FireCannon=123,
		Skull=124,
		SkullMaster=125
	};
	static ppl7::String name(Type::ObjectType type);
};

class Spriteset
{
public:
	enum SpritesetIds {
		GenericObjects=0,
		ThreeSpeers,
		Skeleton,
		Mummy,
		Vent,
		Mushroom,
		TreasureChest,
		Doors,
		Scarabeus,
		Laser,
		Wallenstein,
		Helena,
		Bat,
		Scorpion,
		Bird,
		LevelEnd,
		Yeti,
		George,
		Ostrich,
		ScorpionMetalic,
		Piranha,
		BreakingWall,
		Rat,
		Ghost,
		Zombie,
		StamperV2,
		Skull,
		SkullMaster,
		Switches,
		MaxSpritesets
	};
};

class Representation
{
public:
	Representation(int sprite_set, int sprite_no);
	int sprite_set;
	int sprite_no;
};

Representation getRepresentation(int object_type);

class Object;
class Collision
{
private:
	Object* object;
	std::list<ppl7::grafix::Point> collision_points;
public:
	ppl7::grafix::Rect bounding_box_object;
	ppl7::grafix::Rect bounding_box_player;
	ppl7::grafix::Rect bounding_box_intersection;
	float frame_rate_compensation;
	Collision();
	Collision(const Collision& other);
	void detect(Object* object, const std::list<ppl7::grafix::Point>& checkpoints, const Player& player);
	const std::list<ppl7::grafix::Point>& getCollisionPoints() const;
	Object* getObject() const;
	bool onFoot() const;
	bool objectTop() const;
	bool objectBottom() const;
	bool objectLeft() const;
	bool objectRight() const;
};

class Object
{
public:
	enum class Layer {
		BehindBricks=0,
		BeforeBricks=1,
		BeforePlayer=2,
		BehindPlayer=1
	};
private:
	Type::ObjectType myType;

public:
	Layer myLayer;

	ppl7::grafix::PointF p;
	ppl7::grafix::PointF initial_p;
	SpriteTexture* texture;
	//ppl7::tk::Widget *widget;
	ppl7::grafix::Rect boundary, initial_boundary;
	uint32_t id;
	ppl7::grafix::Color color_mod;
	int sprite_set;
	int sprite_no;
	int sprite_no_representation;
	uint8_t difficulty_matrix;
	float scale;
	bool collisionDetection;
	bool visibleAtPlaytime;
	bool enabled;
	bool pixelExactCollision;
	bool spawned;	// not saved, deleted on collection
	bool deleteDefered;

	explicit Object(Type::ObjectType type);
	virtual ~Object();
	Type::ObjectType type() const;
	ppl7::String typeName() const;
	void updateBoundary();
	void updateSpriteset(int spriteset);
	virtual void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	virtual size_t save(unsigned char* buffer, size_t size) const;
	virtual size_t load(const unsigned char* buffer, size_t size);
	virtual size_t saveSize() const;
	virtual void handleCollision(Player* player, const Collision& collision);
	virtual void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
	virtual void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
	virtual void openUi();
	virtual void reset();
	virtual void toggle(bool enable, Object* source=NULL);
	virtual void trigger(Object* source=NULL);
	static Representation representation();
};


class PlayerStartPoint : public Object
{
private:
public:
	PlayerStartPoint();
	static Representation representation();
};

class Vent : public Object
{
private:
	double next_animation;
	AnimationCycle animation;
	AudioInstance* audio;
public:
	Vent();
	~Vent();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
};

class WindEmitter : public Object
{
private:
	double next_birth;
public:
	WindEmitter();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

};

class RainEmitter : public Object
{
private:
	double next_birth;

	void createParticle(ParticleSystem* ps, const TileTypePlane& ttplane, double time);
public:
	Particle::Type particle_type;
	Particle::Layer particle_layer;
	ppl7::grafix::Color ParticleColor;
	int emitter_stud_width;
	int max_particle_birth_per_cycle;
	float birth_time_min, birth_time_max;
	float max_velocity_x;
	float min_velocity_y;
	float max_velocity_y;
	float scale_min, scale_max;
	float age_min, age_max;
	int flags;
	bool current_state;
	enum class Flags {
		initialStateEnabled=1,

	};

	RainEmitter();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void toggle(bool enable, Object* source=NULL) override;
	void trigger(Object* source=NULL);

};

class ParticleEmitter : public Object
{
private:
	double next_birth;

	void createParticle(ParticleSystem* ps, double time);
public:
	enum class Flags {
		useColorGradient=1,
		useScaleGradient=2,
		initialStateDisabled=4,

	};

	Particle::Type particle_type;
	EmitterType emitter_type;
	ppl7::grafix::Color ParticleColor;
	Particle::Layer particle_layer;
	ppl7::grafix::Size emitter_size;
	int flags;
	int min_birth_per_cycle, max_birth_per_cycle;
	float birth_time_min, birth_time_max;
	float min_velocity;
	float max_velocity;
	float scale_min, scale_max;
	float age_min, age_max;
	float direction, variation;
	float weight_min, weight_max;
	ppl7::grafix::PointF gravity;
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	bool current_state;


	ParticleEmitter();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void toggle(bool enable, Object* source=NULL) override;
	void trigger(Object* source=NULL);

	ppl7::String generateCode() const;

};


class TouchEmitter : public Object
{
private:
	unsigned char toogle_count;
	double next_touch_time;
	enum class State {
		waiting,
		triggered
	};

	void emmitObject(double time);

public:
	State state;
	unsigned char max_toggles;
	unsigned char direction;	// 0=up, 1=right, 2=down, 3=left
	unsigned char touchtype;	// Bit 0-3: Type, Bit 4-7: activation
								// Bit 4: top, Bit 5: right, Bit 6: bottom, Bit 7: left
	Type::ObjectType emitted_object;

	TouchEmitter();
	~TouchEmitter();
	static Representation representation();
	void init();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void trigger(Object* source=NULL);
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void reset() override;
};

class Speaker : public Object
{
private:
	AudioInstance* audio;
public:
	int sample_id;
	int max_distance;
	float volume;

	int flags;
	bool current_state;
	enum class Flags {
		initialStateEnabled=1
	};


	Speaker();
	~Speaker();
	static Representation representation();
	void setSample(int id, float volume, int max_distance=1600);
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void toggle(bool enable, Object* source=NULL) override;
};

class Switch : public Object
{
private:
	double cooldown;
	//int state;

	void notify_targets();

public:
	enum class TargetState {
		disable=0,
		enable=1,
		trigger=2
	};

	enum class SwitchStyle {
		SwitchWithLever=0,
		SwitchWithLeverAndTop=1,
		SwitchWithSmallLeverAndTop=2,
		LightSwitch=3,
		Lever=4
	};

	class TargetObject
	{
	public:
		TargetObject();
		int object_id;
		TargetState state;
	};

	bool current_state;
	bool initial_state;
	bool one_time_switch;
	bool auto_toggle_on_collision;
	SwitchStyle switch_style;
	int color_base;
	int color_lever;
	int color_button;


	TargetObject targets[15];

	Switch();
	static Representation representation();
	void init();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;

	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void reset() override;
	void toggle(bool enable, Object* source=NULL) override;
	void trigger(Object* source=NULL) override;
};



class CoinReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	CoinReward();
	static Representation representation();

	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class AppleReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	AppleReward();
	static Representation representation();

	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class CherryReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	CherryReward();
	static Representation representation();

	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class ExtraLife : public Object
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	ExtraLife();
	static Representation representation();

	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};


class GemReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;
	LightObject light_glow;
public:
	GemReward();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class CrystalReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;
	LightObject light_glow;
public:
	CrystalReward();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class TreasureChest : public Object
{
private:
	double next_animation;
	int state;
	AnimationCycle animation;
public:
	TreasureChest();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class KeyReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;
public:
	enum class KeyType {
		silver=0,
		golden=1,
		colored=2
	};
	KeyType key_type;
	int color_modification;
	KeyReward();
	static Representation representation();
	void init();
	void updateColor();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
};

class Medikit : public Object
{
private:
public:
	Medikit();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
};

class OxygenTank : public Object
{
private:
	double next_birth;
	double respawn_time;
	std::list<Particle::ScaleGradientItem>scale_gradient;

public:
	OxygenTank();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};


class SavePoint : public Object
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	SavePoint();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};


class Trap : public Object
{
private:
public:
	explicit Trap(Type::ObjectType type);
};

class ThreeSpeers : public Trap
{
private:
	double next_state, next_animation;
	int state;

	//float active_time, inactive_time;

	AnimationCycle animation;


public:
	int speer_type;

	ThreeSpeers();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;

};

class LaserBarrier : public Trap
{
private:
	ppl7::grafix::Point start, end;
	double next_state;
	int state;
	int flicker;
	AudioInstance* audio;
	std::list<LightObject*> light_list;
	ppl7::grafix::Color light_color;
	void updateLightMaps();
	void clearLights();
public:
	float time_on_min, time_off_min;
	float time_on_max, time_off_max;
	unsigned char color_scheme;
	bool initial_state;
	bool always_on;
	bool block_player;
	bool start_state;


	explicit LaserBarrier(Type::ObjectType type);
	~LaserBarrier();
	static Representation representation(Type::ObjectType type);
	void init();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	void toggle(bool enable, Object* source=NULL) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;


};

class Stamper : public Trap
{
private:
	enum class State {
		Open=0,
		Closing,
		Closed,
		Opening
	};
	State state;

	AnimationCycle animation;
	double next_state;
	double next_animation;
	float acceleration;
	float position;
	AudioInstance* audio_drag_up;

	void updateStamperBoundary();

	void drawDown(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
	void drawUp(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
	void drawLeft(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
	void drawRight(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;

public:

	enum class Orientation {
		down=0,
		up=1,
		left=2,
		right=3
	};

	float time_active, time_inactive;
	bool auto_intervall;
	unsigned char initial_state;		// <=== Refactor to boolean and Flag
	unsigned char stamper_type;
	unsigned char teeth_type;
	Orientation orientation;
	int color_stamper;
	int color_teeth;


	Stamper();
	~Stamper();
	static Representation representation();
	void init();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void toggle(bool enable, Object* source=NULL) override;
	void trigger(Object* source=NULL) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
};


class Fire : public Trap
{
private:
	double next_animation;
public:
	Fire();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class Arrow : public Trap
{
private:
	int state;
	double next_state;
	double min_cooldown_state;

	void fire();

public:
	int direction;	// 0=up, 1=right, 2=down, 3=left
	int player_activation_distance;
	float min_cooldown_time;
	float max_cooldown_time;

	Arrow();
	static Representation representation();
	void changeDirection(int new_direction);
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;

};

class Fireball : public Object
{
private:
	double next_birth;
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	void emmitParticles(double time, const Player& player);

	LightObject light_ball;
	LightObject light_shine;
	AudioInstance* audio;

public:
	ppl7::grafix::PointF velocity;
	float direction;
	float gravity;
	float player_damage;
	int min_particles;
	int max_particles;

	Fireball();
	~Fireball();
	static Representation representation();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;

	virtual void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	virtual void handleCollision(Player* player, const Collision& collision);
};


class FireCannon : public Trap
{
private:
	double next_state;
	void fire();
public:
	float direction;
	float min_cooldown_time;
	float max_cooldown_time;
	float speed;
	bool current_state_on;
	bool initial_state_on;

	FireCannon();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void toggle(bool enable, Object* source=NULL) override;
	void trigger(Object* source=NULL) override;


};



class Enemy : public Object
{
private:
public:
	explicit Enemy(Type::ObjectType type);
};

class Rat : public Enemy
{
private:
	AnimationCycle animation;
	double next_animation;
	double next_state;
	int look_ahead;

	AudioInstance* audio;

	enum class RatState {
		idle=0,
		walk_left,
		wait_left,
		turn_left_to_right,
		walk_right,
		wait_right,
		turn_right_to_left,
		dead
	};
	RatState state;
	float speed;

public:
	float min_speed;
	float max_speed;
	float min_idle;
	float max_idle;

	Rat();
	~Rat();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
};

class Scorpion : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	AudioInstance* audio;
	int state;
	float speed;

	void playAudio(int id, float volume);
public:
	int type;
	int player_activation_distance;
	float minspeed, maxspeed, max_speed_when_player_is_near;
	float min_idle_time, max_idle_time;
	float speed_acceleration;

	Scorpion();
	~Scorpion();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;

};

class Bat : public Enemy
{
private:
	AnimationCycle animation;
	double next_animation;
	float velocity;
	int state;
	AudioInstance* audio;
	LightObject eyes_light;
public:
	Bat();
	~Bat();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

};

class Bird : public Enemy
{
private:
	enum class BirdState
	{
		FlyFront=0,
		FlyLeft,
		FlyRight,
		FlyBack,
		AttackLeft,
		AttackRight,
		FlyOnPlace,
		FlyUp,
		WaitThenFlyUp
	};

	AnimationCycle animation;
	double next_animation;
	double next_state_change;
	float velocity;
	BirdState state;
	AudioInstance* audio;
	void changeState(BirdState state);
public:
	Bird();
	~Bird();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

};


class Skeleton : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	//int velocity;
	int state;
public:
	Skeleton();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
};

class Ghost : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	double change_height;
	enum class State {
		Stand,
		TurnToLeft,
		TurnToRight,
		FlyLeft,
		FlyLeftToRight,
		FlyRight,
		FlyRightToLeft,
		LandLeft,
		LandRight,
		TurnToMid,
		Dead,
	};
	State state;
	float glow;
	float glowtarget;
	float glowspeed;
	float height;
	float target_height;
	bool land;
	AudioInstance* audio;
	//double next_glowtime;
	void update_glow(double time, float frame_rate_compensation);
	void update_animation(double time);
public:
	Ghost();
	~Ghost();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
};



class Mummy : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	int velocity=1;
	int state;
public:
	Mummy();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

};


class Ostrich : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	AudioInstance* audio;
	int state;
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	void emmitParticles(double time, const Player& player, float angle);


public:
	Ostrich();
	~Ostrich();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
};


class Mushroom : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	ppl7::grafix::PointF velocity;
	enum class ActionState {
		Start,
		WalkLeft,
		WaitForTurnRight,
		TurnRight,
		WalkRight,
		WaitForTurnLeft,
		TurnLeft,
		Dead,
		Disabled,
		Falling
	};
	ActionState state;
	LightObject lightmap;
public:
	float gravity_x;
	float max_gravity_y;
	bool initial_state;

	Mushroom();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

	void toggle(bool enable, Object* source=NULL) override;
	void trigger(Object* source) override;

	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;


};

class Scarabeus : public Enemy
{
private:
	AnimationCycle animation;
	AudioInstance* audio;
	double next_state, next_animation;
	ppl7::grafix::PointF velocity;
	int state;
public:
	Scarabeus();
	~Scarabeus();
	static Representation representation();
	void update_animation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

};

class Fish : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	float speed;
	int state;
public:
	Fish();
	//~Fish();
	static Representation representation();
	//void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;

};

class Piranha : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	float speed;
	int state;
public:
	Piranha();
	//~Fish();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

};

class Skull : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	double fire_cooldown, next_roll, voice_cooldown;
	float health;
	int state;
	ppl7::grafix::PointF bounce_start;
	enum class ActionState {
		Frozen,
		Wait,
		Bouncing,
		Turn,
		Attack,
		GoBackToOrigin,
		Stop,
		Dead
	};
	enum class Orientation {
		Front,
		Left,
		Right
	};
	Orientation orientation;
	ActionState aState;
	LightObject light;
	LightObject shine;
	ppl7::grafix::PointF velocity;

	void updateBouncing(double time, TileTypePlane& ttplane, float frame_rate_compensation);
	void updateGoBackToOrigin(double time, float frame_rate_compensation);
	void newState(double time, TileTypePlane& ttplane, Player& player);
	void updateStop(double time, float frame_rate_compensation);
	void updateAttack(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	void die(double time);
	void turn();
	void fire(double time, Player& player);
	void stopHorizontalVelocity(float frame_rate_compensation);
	void goToTarget(double time, float frame_rate_compensation, const ppl7::grafix::PointF& target);
public:
	Skull();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void freeze(bool flag);

};

class SkullMaster : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation, next_birth;
	double fire_cooldown, voice_cooldown, collision_cooldown, flashlight_cooldown;
	float health;
	int wait_state;
	int suspend_count;
	float bounce_distance;
	float bounce_speed;
	float max_bounce_velocity;
	bool skullsDropped;

	enum class SuspendState {
		Start,
		Retreat,
		Comeback
	};

	enum class ActionState {
		Wait,
		Turn,
		Attack,
		Suspend,
		Die,
		Dead
	};
	enum class Orientation {
		Front,
		FrontDown,
		Left,
		LeftDown,
		Right,
		RightDown
	};
	Orientation orientation;
	ActionState aState;
	SuspendState sState;
	LightObject lightmap;
	LightObject shine;
	ppl7::grafix::PointF velocity;
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;

	void reset();
	void die(double time);
	void turn(Orientation target);
	void fire(double time, Player& player);
	void dropSkulls();
	void dropSkull(float direction);
	void dropFireball(float direction, const ppl7::grafix::PointF& p);

	void update_bounce(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	void update_wait(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	void update_attack(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	void update_die(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	void update_suspend(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);

public:
	SkullMaster();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
};


class HangingSpider : public Enemy
{
private:
	double next_state;
	int state;
	double velocity;
	LightObject eyes_light;
public:
	HangingSpider();
	static Representation representation();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;

};

class BreakingGround : public Object
{
private:
	double next_animation;
	int state;
	int layers;
	int fragment_y;
	double velocity;
public:
	BreakingGround();
	static Representation representation();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
};

class BreakingWall : public Object
{
private:
	AnimationCycle animation;
	double next_animation;
	int state;
public:
	int wall_color;

	BreakingWall();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void breakWall(Player* player);
};

class Hammer : public Object
{
private:
	AnimationCycle animation;
	double next_animation;
public:
	Hammer();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
};

class Cheese : public Object
{
private:
	AnimationCycle animation;
	double next_animation;
public:
	Cheese();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
};

class Flashlight : public Object
{
private:
	AnimationCycle animation;
	double next_animation;
public:
	Flashlight();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
};

class Floater : public Object
{
	friend class FloaterVertical;
	friend class FloaterHorizontal;
private:
	int direction;
	double next_state, next_animation;
	double next_birth;
	float frame_rate_compensation;
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	ppl7::grafix::PointF emmit_position;
	float emmit_angle;
	float particle_velocity_correction;
	int state;
	int flame_sprite1, flame_sprite2;
	bool current_state;
	ppl7::grafix::PointF velocity;
	AnimationCycle animation;
	AudioInstance* audio;

	LightObject thuruster1;
	LightObject thuruster2;


	void emmitParticles(double time, const Player& player, const ppl7::grafix::PointF p, float angle);
public:
	bool initial_state;
	unsigned char floater_type;

	explicit Floater(Type::ObjectType type);
	~Floater();
	void init();
	void reset() override;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const  override;
	void handleCollision(Player* player, const Collision& collision) override;
	void toggle(bool enable, Object* source=NULL) override;
	void trigger(Object* source) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;

};


class FloaterHorizontal : public Floater
{
private:
public:
	FloaterHorizontal();
	static Representation representation();
};

class FloaterVertical : public Floater
{
private:
public:
	FloaterVertical();
	static Representation representation();
};


class Door : public Object
{
private:
	AnimationCycle animation;
	double next_animation;
	int door_sprite_no;
	double cooldown;
	bool text_for_closed_door_said;
	double cooldown_for_locked_door_text;

	void load_v1(const unsigned char* buffer, size_t size);

public:
	enum class DoorOrientation {
		right=0,
		left=1,
		front=2,
		back=3
	};

	enum class DoorType {
		windowed=0,
		flat=1,
		lattice_metalic=2,
		lattice_opaque=3,
		no_door=4
	};

	enum class DoorState {
		closed=0,
		opening=1,
		open=2,
		closing=3
	};
	enum class WarpSide {
		left=0,
		right
	};

	int color_frame;
	int color_door;
	int color_background;
	uint32_t key_id;
	uint32_t warp_to_id;
	DoorOrientation orientation;
	DoorType door_type;
	DoorState state;
	WarpSide warpside;
	bool initial_open;
	bool auto_opens_on_collision;
	bool can_close_again;
	bool use_background_color;
	bool autowarp_when_open;
	bool close_when_passed;


	Door();
	static Representation representation();
	void init();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void reset() override;
	void openUi() override;
	void trigger(Object* source) override;

};

class LevelEnd : public Object
{
private:
	AnimationCycle animation;
	double next_animation;
	double cooldown;

public:
	enum class State
	{
		Inactive,
		Active
	};
	enum class Flags
	{
		initialStateActive=1,
		useBackgroundColorWhenActive=2,
		transferOnCollision=4,
	};
	uint32_t key_id;
	Flags flags;
	State state;
	ppl7::String next_level;
	AudioInstance* audio;
	int color_doorframe;
	int color_background;
	int color_puddle;
	int color_details_doorframe;
	int color_details_arch;
	int color_stairs;
	int background_alpha;

	LevelEnd();
	~LevelEnd();
	static Representation representation();
	void init();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	//void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void toggle(bool enable, Object* source=NULL) override;
	void trigger(Object* source) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;

};


class VoiceTrigger : public Object
{
private:
	enum class State {
		waiting_for_activation,
		activated,
		waiting_for_initial_delay,
		speaking,
		waiting_for_trigger_delay,
		finished,
		disabled
	};
	double cooldown;
	int trigger_count;
	State state;
	double activeInitialDeleay;

	void notifyTargets() const;

public:
	ppl7::grafix::Point range;
	ppl7::String context;		// for translation context
	uint16_t speechId;
	bool singleTrigger;
	bool triggeredByCollision;
	bool requireKeypress;
	bool pauseWorld;
	bool initialStateEnabled;
	float cooldownUntilNextTrigger;
	float volume;
	float initialDelay;
	float triggerDeleay;

	class TargetObject
	{
	public:
		uint16_t object_id=0;
		bool enable=true;
	};
	TargetObject triggerObjects[5];


	VoiceTrigger();
	~VoiceTrigger();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	virtual void toggle(bool enable, Object* source=NULL) override;
	virtual void trigger(Object* source=NULL) override;

	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void openUi() override;
	void reset();
};

class ObjectWatcher : public Object
{
private:


public:
	class WatchObject
	{
	public:
		uint16_t object_id=0;
		bool expectedState=false;
	};

	class TriggerObject
	{
	public:
		uint16_t object_id=0;
	};
	WatchObject watchObjects[10];
	TriggerObject triggerObjects[5];

	ObjectWatcher();
	~ObjectWatcher();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void reset();
	void notifyTargets() const;
	//virtual void toggle(bool enable, Object* source=NULL) override;
	//virtual void trigger(Object* source=NULL) override;
	void openUi() override;

};

class Trigger : public Object
{
private:
	enum class State {
		waiting_for_activation,
		activated,
		waiting_for_trigger_delay,
		finished,
		disabled
	};
	State state;
	double cooldown;
	double triggerDeleayTime;
	int trigger_count;
	double last_collision_time;
	void notifyTargets() const;
public:
	ppl7::grafix::Point range;
	bool multiTrigger;
	bool triggeredByCollision;
	bool initialStateEnabled;
	float cooldownUntilNextTrigger;
	float triggerDeleay;
	uint16_t maxTriggerCount;


	class TargetObject
	{
	public:
		uint16_t object_id=0;
	};
	TargetObject triggerObjects[10];

	Trigger();
	~Trigger();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void reset();
	virtual void toggle(bool enable, Object* source=NULL) override;
	virtual void trigger(Object* source=NULL) override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void openUi() override;

	void test();
};

class LightTrigger : public Object
{
private:
	enum class State {
		waiting_for_activation,
		activated,
		waiting_for_trigger_delay,
		finished,
		disabled
	};
	State state;
	double cooldown;
	double triggerDeleayTime;
	int trigger_count;
	double last_collision_time;
	void notifyTargets() const;
public:
	ppl7::grafix::Point range;
	bool multiTrigger;
	bool triggeredByCollision;
	bool initialStateEnabled;
	float cooldownUntilNextTrigger;
	float triggerDeleay;
	uint16_t maxTriggerCount;


	class TargetObject
	{
	public:
		uint32_t light_id=0;
	};
	TargetObject triggerObjects[10];

	LightTrigger();
	~LightTrigger();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void reset();
	virtual void toggle(bool enable, Object* source=NULL) override;
	virtual void trigger(Object* source=NULL) override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void openUi() override;

	void test();
};

class LevelModificator : public Object
{
private:
	enum class State {
		waiting_for_activation,
		in_transition,
		disabled
	};
	State state;
public:
	enum class BackgroundType {
		Image,
		Color
	};
	ppl7::grafix::Point range;
	bool initialStateEnabled;
	bool loadLevelDefault;
	bool changeBackground;
	bool changeGlobalLighting;
	bool changeSong;
	BackgroundType backgroundType;
	ppl7::String BackgroundImage;
	ppl7::String Song;
	ppl7::grafix::Color BackgroundColor;
	ppl7::grafix::Color GlobalLighting;
	float transitionTime;


	LevelModificator();
	~LevelModificator();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void reset();
	virtual void toggle(bool enable, Object* source=NULL) override;
	virtual void trigger(Object* source=NULL) override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void openUi() override;
	void test();
};

class TouchPlateSwitch : public Object
{
private:
public:
	TouchPlateSwitch();
	~TouchPlateSwitch();

	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;

	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;

};

class ButtonSwitch : public Object
{
private:
public:
	ButtonSwitch();
	~ButtonSwitch();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	void handleCollision(Player* player, const Collision& collision) override;
	virtual void toggle(bool enable, Object* source=NULL) override;
	virtual void trigger(Object* source=NULL) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;


	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
};

class LightSignal : public Object
{
private:
	LightObject led;
	double next_state;
	bool blink_state;
public:
	bool initialState;
	bool currentState;
	bool dualColor;
	bool blink;
	ppl7::grafix::Color color_on;
	ppl7::grafix::Color color_off;

	LightSignal();
	~LightSignal();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation) override;
	//void handleCollision(Player* player, const Collision& collision) override;
	virtual void toggle(bool enable, Object* source=NULL) override;
	virtual void trigger(Object* source=NULL) override;


	size_t save(unsigned char* buffer, size_t size) const override;
	size_t saveSize() const override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
};



class ObjectSystem
{
private:
	uint32_t nextid;
	uint32_t next_spawn_id;
	int player_start;
	std::map<uint32_t, Object*> object_list;
	std::map<uint32_t, Object*> visible_object_map;
	SpriteTexture* spriteset[Spriteset::MaxSpritesets];
	Waynet* waynet;

public:
	ObjectSystem(Waynet* waynet);
	~ObjectSystem();
	void clear();
	void loadSpritesets(SDL& sdl);
	void addObject(Object* object);
	Object* getInstance(int object_type) const;
	void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	void updateVisibleObjectList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Object::Layer layer) const;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Object::Layer layer) const;
	void save(ppl7::FileObject& file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr& ba);
	Object* getObject(uint32_t object_id);
	Object* findMatchingObject(const ppl7::grafix::Point& p) const;
	//Object* detectCollision(const std::list<ppl7::grafix::Point>& player);
	void detectCollision(const std::list<ppl7::grafix::Point>& player, std::list<Object*>& object_list);
	void drawSelectedSpriteOutline(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id);
	void drawPlaceSelection(SDL_Renderer* renderer, const ppl7::grafix::Point& p, int object_type);
	void deleteObject(int id);
	bool findObjectsInRange(const ppl7::grafix::PointF& p, double range, std::list <Object*>& objects);
	ppl7::grafix::Point findPlayerStart() const;
	ppl7::grafix::Point nextPlayerStart();
	SpriteTexture* getTexture(int sprite_set) const;
	void resetPlayerStart();
	size_t count() const;
	size_t countVisible() const;
	Waynet& getWaynet();
	void getObjectCounter(std::map<int, size_t>& object_counter) const;

};

ObjectSystem* GetObjectSystem();

} // EOF namespace Decker::Objects

#endif // INCLUDE_OBJECTS_H_
