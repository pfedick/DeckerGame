#include <map>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "animation.h"
class SDL;
class SDL_Renderer;
class SpriteTexture;

namespace Decker::Objects {

class Type
{
public:
	enum ObjectType {
		PlayerStartpoint=1,
		Savepoint=2,
		Medikit=3,
		Crystal=100,
		Diamond=101,
		Coin=102,
		Key=103,
		Arrow=200,
		ThreeSpeers=201,
		Rat=300,
		HangingSpider=301,
		FloaterHorizontal=500,
		FloaterVertical=501,
		Door=502
	};
	static ppl7::String name(Type::ObjectType type);
};

class Spriteset
{
public:
	enum SpritesetIds {
		GenericObjects=0,
		ThreeSpeers=1,
		MaxSpritesets=2
	};
};

class Representation
{
public:
	Representation(int sprite_set, int sprite_no);
	int sprite_set;
	int sprite_no;
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
	unsigned char save_size;
	unsigned char state_size;
	bool collsionDetection;

	Object(Type::ObjectType type);
	virtual ~Object();
	Type::ObjectType type() const;
	ppl7::String typeName() const;
	void updateBoundary();
	virtual void update(double time);
	virtual size_t save(unsigned char *buffer, size_t size);
	virtual bool load(const unsigned char *buffer, size_t size);

	static Representation representation();
};

class Collectable : public Object
{
private:
public:
	Collectable(Type::ObjectType type);
};


class CoinReward : public Collectable
{
private:
	double next_animation;
	AnimationCycle animation;

public:
	CoinReward();
	static Representation representation();

	virtual void update(double time);

};

class GemReward : public Collectable
{
private:
public:
	GemReward();
	static Representation representation();
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
};

class SavePoint : public Collectable
{
private:
public:
	SavePoint();
	static Representation representation();
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
	virtual void update(double time);
};

class LaserBarrier : public Trap
{
private:
public:
	LaserBarrier();
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
public:
	Rat();
	static Representation representation();
};

class HangingSpider : public Enemy
{
private:
public:
	HangingSpider();
	static Representation representation();
};


class FloaterHorizontal : public Object
{
private:
public:
	FloaterHorizontal();
	static Representation representation();
};

class FloaterVertical : public Object
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
	void update(double time);
	void updateVisibleObjectList(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport);
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void save(ppl7::FileObject &file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr &ba);
	void saveState(ppl7::FileObject &file, unsigned char id) const;
	void loadState(const ppl7::ByteArrayPtr &ba) const;
	Object *findMatchingObject(const ppl7::grafix::Point &p) const;
	void drawSelectedSpriteOutline(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords, int id);
	void drawPlaceSelection(SDL_Renderer *renderer, const ppl7::grafix::Point &p, int object_type);

	size_t count() const;
	size_t countVisible() const;

};

} // EOF namespace Decker::Objects
