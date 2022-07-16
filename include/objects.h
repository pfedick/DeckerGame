#ifndef INCLUDE_OBJECTS_H_
#define INCLUDE_OBJECTS_H_

#include <map>
#include <list>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "animation.h"
class SDL;
struct SDL_Renderer;
class SpriteTexture;

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
		StamperVertical=110,
		Wallenstein=111,
		Helena=112,
		Scorpion=113,
		Bat=114,
		Bird=115
	};
	static ppl7::String name(Type::ObjectType type);
};

class Spriteset
{
public:
	enum SpritesetIds {
		GenericObjects=0,
		ThreeSpeers=1,
		Skeleton=2,
		Mummy=3,
		Vent=4,
		Mushroom=5,
		TreasureChest=6,
		Doors=7,
		Scarabeus=8,
		Laser=9,
		StamperVertical=10,
		Wallenstein=11,
		Helena=12,
		Bat=13,
		Scorpion=14,
		Bird,
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
	float scale;
	unsigned char save_size;
	unsigned char state_size;
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
	virtual void update(double time, TileTypePlane& ttplane, Player& player);
	virtual size_t save(unsigned char* buffer, size_t size);
	virtual size_t load(const unsigned char* buffer, size_t size);
	virtual void handleCollision(Player* player, const Collision& collision);
	virtual void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
	virtual void openUi();
	virtual void reset();
	virtual void toggle(bool enable, Object* source=NULL);
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
	void update(double time, TileTypePlane& ttplane, Player& player) override;
};

class WindEmitter : public Object
{
private:
	double next_birth;
public:
	WindEmitter();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player) override;

};

class RainEmitter : public Object
{
private:
	double next_birth;

	void createParticle(const TileTypePlane& ttplane);
public:
	enum class ParticleType {
		Rain=0,
		ParticleWhite,
		SnowflakeWhite,
		SnowflakeTransparent
	};
	ParticleType type;
	ppl7::grafix::Color ParticleColor;
	int emitter_stud_width;
	int max_particle_birth_per_cycle;
	float birth_time_min, birth_time_max;
	float max_velocity_x;
	float min_velocity_y;
	float max_velocity_y;
	float scale_min, scale_max;


	RainEmitter();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player) override;

	size_t save(unsigned char* buffer, size_t size) override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;

};


class TouchEmitter : public Object
{
private:
	unsigned char toogle_count;
	double next_touch_time;

public:
	unsigned char max_toggles;
	unsigned char direction;	// 0=up, 1=right, 2=down, 3=left
	unsigned char touchtype;	// Bit 0-3: Type, Bit 4-7: actication
								// Bit 4: top, Bit 5: right, Bit 6: bottom, Bit 7: left
	Type::ObjectType emitted_object;

	TouchEmitter();
	~TouchEmitter();
	static Representation representation();
	void init();
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) override;
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

	Speaker();
	~Speaker();
	static Representation representation();
	void setSample(int id, float volume, int max_distance=1600);
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	size_t save(unsigned char* buffer, size_t size) override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
};

class Switch : public Object
{
private:
	double cooldown;
	int state;
	bool current_state;

	void notify_targets();

public:
	class TargetObject
	{
	public:
		TargetObject();
		int object_id;
		bool enable;
	};
	bool initial_state;
	bool one_time_switch;
	bool auto_toggle_on_collision;
	unsigned char color_scheme;

	TargetObject targets[10];

	Switch();
	static Representation representation();
	void init();
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void reset() override;
};



class CoinReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	CoinReward();
	static Representation representation();

	void update(double time, TileTypePlane& ttplane, Player& player) override;
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

	void update(double time, TileTypePlane& ttplane, Player& player) override;
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

	void update(double time, TileTypePlane& ttplane, Player& player) override;
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

	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void handleCollision(Player* player, const Collision& collision) override;
};


class GemReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;
public:
	GemReward();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class CrystalReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;
public:
	CrystalReward();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player) override;
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
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class KeyReward : public Object
{
private:
public:
	KeyReward();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
};

class Medikit : public Object
{
private:
public:
	Medikit();
	static Representation representation();
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
	void update(double time, TileTypePlane& ttplane, Player& player) override;
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
	AnimationCycle animation;

public:
	ThreeSpeers();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void handleCollision(Player* player, const Collision& collision) override;
};

class LaserBarrier : public Trap
{
private:
	ppl7::grafix::Point start, end;
	double next_state;
	int state;
	int flicker;
	AudioInstance* audio;
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
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void handleCollision(Player* player, const Collision& collision) override;
	void toggle(bool enable, Object* source=NULL) override;
	size_t save(unsigned char* buffer, size_t size) override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;


};

class StamperVertical : public Trap
{
private:
	AnimationCycle animation;
	double next_state;
	double next_animation;
	int state;
public:
	float time_active, time_inactive;
	bool auto_intervall;
	unsigned char initial_state;
	unsigned char stamper_type;

	StamperVertical();
	static Representation representation();
	void init();
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;
	void toggle(bool enable, Object* source=NULL) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;

};


class Fire : public Trap
{
private:
	double next_animation;
public:
	Fire();
	static Representation representation();
	void update(double time, TileTypePlane& ttplane, Player& player) override;
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
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	size_t save(unsigned char* buffer, size_t size) override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void openUi() override;

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
	int state;
public:
	Rat();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player) override;

};

class Scorpion : public Enemy
{
private:
	AnimationCycle animation;
	double next_animation;
	int state;
public:
	Scorpion();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player) override;

};

class Bat : public Enemy
{
private:
	AnimationCycle animation;
	double next_animation;
	float velocity;
	int state;
public:
	Bat();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player) override;

};

class Bird : public Enemy
{
private:
	AnimationCycle animation;
	double next_animation;
	float velocity;
	int state;
public:
	Bird();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player) override;

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
	void update(double time, TileTypePlane& ttplane, Player& player) override;
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
	void update(double time, TileTypePlane& ttplane, Player& player) override;

};

class Mushroom : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	//int velocity;
	int state;
public:
	Mushroom();
	static Representation representation();
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player) override;
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
	void update(double time, TileTypePlane& ttplane, Player& player) override;

};


class HangingSpider : public Enemy
{
private:
	double next_state;
	int state;
	double velocity;
public:
	HangingSpider();
	static Representation representation();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void handleCollision(Player* player, const Collision& collision) override;
	void update(double time, TileTypePlane& ttplane, Player& player) override;

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
	void update(double time, TileTypePlane& ttplane, Player& player) override;
};

class Floater : public Object
{
	friend class FloaterVertical;
	friend class FloaterHorizontal;
private:
	int direction;
	double next_state, next_animation;
	int state;
	int flame_sprite1, flame_sprite2;
	bool current_state;
	ppl7::grafix::Point velocity;
	AnimationCycle animation;
	AudioInstance* audio;
public:
	bool initial_state;
	unsigned char floater_type;

	explicit Floater(Type::ObjectType type);
	~Floater();
	void init();
	void reset() override;
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const  override;
	void handleCollision(Player* player, const Collision& collision) override;
	void toggle(bool enable, Object* source=NULL) override;
	size_t save(unsigned char* buffer, size_t size) override;
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

public:
	uint32_t key_id;
	unsigned char frame_type;
	unsigned char door_type;
	int state;
	bool initial_open;
	bool left_sided;

	Door();
	static Representation representation();
	void init();
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void reset() override;
	void openUi() override;

};

class LevelEnd : public Object
{
private:
	AnimationCycle animation;
	double next_animation;

public:
	uint32_t key_id;
	unsigned char frame_type;
	unsigned char door_type;
	int state;
	bool initial_open;
	bool left_sided;

	LevelEnd();
	static Representation representation();
	void init();
	void update(double time, TileTypePlane& ttplane, Player& player) override;
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void handleCollision(Player* player, const Collision& collision) override;
	size_t save(unsigned char* buffer, size_t size) override;
	size_t load(const unsigned char* buffer, size_t size) override;
	void reset() override;
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
	SpriteTexture* getTexture(int sprite_set) const;

public:
	ObjectSystem(Waynet* waynet);
	~ObjectSystem();
	void clear();
	void loadSpritesets(SDL& sdl);
	void addObject(Object* object);
	Object* getInstance(int object_type) const;
	void update(double time, TileTypePlane& ttplane, Player& player);
	void updateVisibleObjectList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Object::Layer layer) const;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Object::Layer layer) const;
	void save(ppl7::FileObject& file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr& ba);
	void saveState(ppl7::FileObject& file, unsigned char id) const;
	void loadState(const ppl7::ByteArrayPtr& ba) const;
	Object* getObject(uint32_t object_id);
	Object* findMatchingObject(const ppl7::grafix::Point& p) const;
	Object* detectCollision(const std::list<ppl7::grafix::Point>& player);
	void drawSelectedSpriteOutline(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id);
	void drawPlaceSelection(SDL_Renderer* renderer, const ppl7::grafix::Point& p, int object_type);
	void deleteObject(int id);
	ppl7::grafix::Point findPlayerStart() const;
	ppl7::grafix::Point nextPlayerStart();
	size_t count() const;
	size_t countVisible() const;
	Waynet& getWaynet();

};

ObjectSystem* GetObjectSystem();

} // EOF namespace Decker::Objects

#endif // INCLUDE_OBJECTS_H_
