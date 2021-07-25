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
		Wind=13,
		Arrow=100,
		ThreeSpeers=101,
		Rat=102,
		HangingSpider=103,
		Skeleton=104,
		Mummy=105,
		LaserBeamHorizontal=106,
		LaserBeamVertical=107
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
		MaxSpritesets=4
	};
};

class Representation
{
public:
	Representation(int sprite_set, int sprite_no);
	int sprite_set;
	int sprite_no;
};
class Object;
class Collision
{
private:
	Object *object;
	std::list<ppl7::grafix::Point> collision_points;
public:
	Collision();
	Collision(const Collision &other);
	void detect(Object *object, const std::list<ppl7::grafix::Point> &checkpoints, const Player &player);
	const std::list<ppl7::grafix::Point> &getCollisionPoints() const;
	Object *getObject() const;
	bool onFoot() const;
};

class Object
{
private:
	Type::ObjectType myType;
public:
	ppl7::grafix::Point p;
	ppl7::grafix::Point initial_p;
	SpriteTexture	*texture;
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
	static Representation representation();
};


class PlayerStartPoint : public Object
{
private:
public:
	PlayerStartPoint();
	static Representation representation();
};


class Collectable : public Object
{
private:
public:
	int points;
	Collectable(Type::ObjectType type);
	virtual void handleCollision(Player *player, const Collision &collision);
};


class CoinReward : public Collectable
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	CoinReward();
	static Representation representation();

	virtual void update(double time, TileTypePlane &ttplane, Player &player);

};

class GemReward : public Collectable
{
private:
	double next_animation;
	AnimationCycle animation;
public:
	GemReward();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
};

class CrystalReward : public Collectable
{
private:
	double next_animation;
	AnimationCycle animation;
public:
	CrystalReward();
	static Representation representation();
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
};

class KeyReward : public Collectable
{
private:
public:
	KeyReward();
	static Representation representation();
};

class Medikit : public Collectable
{
private:
public:
	Medikit();
	static Representation representation();
	virtual void handleCollision(Player *player, const Collision &collision);
};

class SavePoint : public Collectable
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
public:
	LaserBarrier(Type::ObjectType type);
	static Representation representation(Type::ObjectType type);
	virtual void draw(SDL_Renderer *renderer, const ppl7::grafix::Point &coords) const;
	virtual void update(double time, TileTypePlane &ttplane, Player &player);
	virtual void handleCollision(Player *player, const Collision &collision);
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

public:
	Arrow();
	static Representation representation();
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
public:
	Floater(Type::ObjectType type);
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



class ObjectSystem
{
private:
	uint32_t nextid;
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
	size_t count() const;
	size_t countVisible() const;

};

} // EOF namespace Decker::Objects
