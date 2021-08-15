#include <map>
#include <list>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "animation.h"
class SDL;
class SDL_Renderer;
class SpriteTexture;

class Player;
class TileTypePlane;

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
		FloaterVertical=10,
		Door=8,
		FloaterHorizontal=9,
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
		StamperVertical=110
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
	Object *object;
	std::list<ppl7::grafix::Point> collision_points;
public:
	ppl7::grafix::Rect bounding_box_object;
	ppl7::grafix::Rect bounding_box_player;
	ppl7::grafix::Rect bounding_box_intersection;
	Collision();
	Collision(const Collision &other);
	void detect(Object *object, const std::list<ppl7::grafix::Point> &checkpoints, const Player &player);
	const std::list<ppl7::grafix::Point> &getCollisionPoints() const;
	Object *getObject() const;
	bool onFoot() const;
	bool objectTop() const;
	bool objectBottom() const;
	bool objectLeft() const;
	bool objectRight() const;
};

class Object
{
private:
	Type::ObjectType myType;
public:
	ppl7::grafix::PointF p;
	ppl7::grafix::PointF initial_p;
	SpriteTexture	*texture;
	//ppl7::tk::Widget *widget;
	ppl7::grafix::Rect boundary;
	uint32_t id;
	int sprite_set;
	int sprite_no;
	int sprite_no_representation;
	unsigned char save_size;
	unsigned char state_size;
	bool collisionDetection;
	bool visibleAtPlaytime;
	bool enabled;
	bool pixelExactCollision;
	bool spawned;	// not saved, deleted on collection
	bool deleteDefered;

	Object(Type::ObjectType type);
	virtual ~Object();
	Type::ObjectType type() const;
	ppl7::String typeName() const;
	void updateBoundary();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual size_t save(unsigned char *buffer, size_t size);
	virtual bool load(const unsigned char *buffer, size_t size);
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const;
	virtual void openUi();
	virtual void reset();
	virtual void toggle(bool enable, Object *source=NULL);
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
	AudioInstance *audio;
public:
	Vent();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
};

class WindEmitter : public Object
{
private:
	double next_birth;
public:
	WindEmitter();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);

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
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual size_t save(unsigned char *buffer, size_t size);
	virtual bool load(const unsigned char *buffer, size_t size);
	virtual void openUi();
	virtual void reset();
};

class Speaker : public Object
{
private:
	AudioInstance *audio;
public:
	int sample_id;
	int max_distance;
	float volume;

	Speaker();
	~Speaker();
	static Representation representation();
	void setSample(int id, float volume, int max_distance=1600);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual size_t save(unsigned char *buffer, size_t size);
	virtual bool load(const unsigned char *buffer, size_t size);
	virtual void openUi();
};

class Switch : public Object
{
private:
	//AudioInstance *audio;
public:
	//int sample_id;
	//int max_distance;
	//float volume;

	Switch();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual size_t save(unsigned char *buffer, size_t size);
	virtual bool load(const unsigned char *buffer, size_t size);
	virtual void openUi();
};



class CoinReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	CoinReward();
	static Representation representation();

	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
};

class GemReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;
public:
	GemReward();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
};

class CrystalReward : public Object
{
private:
	double next_animation;
	AnimationCycle animation;
public:
	CrystalReward();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
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
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
};

class KeyReward : public Object
{
private:
public:
	KeyReward();
	static Representation representation();
	virtual void handleCollision(Player *player, const Collision &collision);
};

class Medikit : public Object
{
private:
public:
	Medikit();
	static Representation representation();
	virtual void handleCollision(Player *player, const Collision &collision);
};

class SavePoint : public Object
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	SavePoint();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
};


class Trap : public Object
{
private:
public:
	Trap(Type::ObjectType type);
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
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
};

class LaserBarrier : public Trap
{
private:
	ppl7::grafix::Point start,end;
	double next_state;
	int state;
	int flicker;
	AudioInstance *audio;
public:
	LaserBarrier(Type::ObjectType type);
	~LaserBarrier();
	static Representation representation(Type::ObjectType type);
	virtual void draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const;
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
};

class StamperVertical : public Trap
{
private:
	double next_state;
	int state;
public:
	float time_active, time_inactive;
	bool auto_intervall;

	StamperVertical();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual size_t save(unsigned char *buffer, size_t size);
	virtual bool load(const unsigned char *buffer, size_t size);
	virtual void openUi();
	virtual void toggle(bool enable, Object *source=NULL);

};


class Fire : public Trap
{
private:
	double next_animation;
public:
	Fire();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
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
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual size_t save(unsigned char *buffer, size_t size);
	virtual bool load(const unsigned char *buffer, size_t size);
	virtual void openUi();

};


class Enemy : public Object
{
private:
public:
	Enemy(Type::ObjectType type);
};

class Rat : public Enemy
{
private:
	int state;
public:
	Rat();
	static Representation representation();
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);

};

class Skeleton : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	int velocity=1;
	int state;
public:
	Skeleton();
	static Representation representation();
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
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
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);

};

class Mushroom : public Enemy
{
private:
	AnimationCycle animation;
	double next_state, next_animation;
	int velocity=1;
	int state;
public:
	Mushroom();
	static Representation representation();
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
};

class Scarabeus : public Enemy
{
private:
	AnimationCycle animation;
	AudioInstance *audio;
	double next_state, next_animation;
	ppl7::grafix::PointF velocity;
	int state;
public:
	Scarabeus();
	static Representation representation();
	void update_animation();
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);

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
	virtual void draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const;
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);

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
	virtual void draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const;
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
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
	ppl7::grafix::Point velocity;
	AnimationCycle animation;
	AudioInstance *audio;
public:
	Floater(Type::ObjectType type);
	~Floater();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const;
	virtual void handleCollision(Player *player, const Collision &collision);
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
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const;
	virtual void handleCollision(Player *player, const Collision &collision);
	virtual size_t save(unsigned char *buffer, size_t size);
	virtual bool load(const unsigned char *buffer, size_t size);
	virtual void reset();
	virtual void openUi();

};


class ObjectSystem
{
private:
	uint32_t nextid;
	uint32_t next_spawn_id;
	int player_start;
	std::map<uint32_t,Object *> object_list;
	std::map<uint32_t,Object *> visible_object_map;
	SpriteTexture *spriteset[Spriteset::MaxSpritesets];


	SpriteTexture *getTexture(int sprite_set) const;

public:
	ObjectSystem();
	~ObjectSystem();
	void clear();
	void loadSpritesets(SDL &sdl);
	void addObject(Object *object);
	Object *getInstance(int object_type) const;
	void update(double time, TileTypePlane &ttplane, Player &player);
	void updateVisibleObjectList(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport);
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void drawEditMode(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void save(ppl7::FileObject &file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr &ba);
	void saveState(ppl7::FileObject &file, unsigned char id) const;
	void loadState(const ppl7::ByteArrayPtr &ba) const;
	Object *findMatchingObject(const ppl7::grafix::Point &p) const;
	Object *detectCollision(const std::list<ppl7::grafix::Point> &player);
	void drawSelectedSpriteOutline(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords, int id);
	void drawPlaceSelection(SDL_Renderer *renderer, const ppl7::grafix::Point &p, int object_type);
	void deleteObject(int id);
	ppl7::grafix::Point findPlayerStart() const;
	ppl7::grafix::Point nextPlayerStart();
	size_t count() const;
	size_t countVisible() const;

};

ObjectSystem *GetObjectSystem();

} // EOF namespace Decker::Objects
