#include <map>
#include <ppl7.h>
#include <ppl7-grafix.h>

class SDL;
class SDL_Renderer;
class SpriteTexture;


namespace Decker::Objects {

class Type
{
public:
	enum ObjectType {
		Coin,
		Gem,
		Medikit,
		Savepoint,
		ThreeSpeers,
		Rat,

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
	float scale;
	int sprite_set;
	int sprite_no;
	Object(Type::ObjectType type);
	Type::ObjectType type() const;
	ppl7::String typeName() const;
};

class Collectable : public Object
{
private:
public:
};


class CoinReward : public Collectable
{
private:
public:
};

class GemReward : public Collectable
{
private:
public:
};

class Medikit : public Collectable
{
private:
public:
};

class SavePoint : public Collectable
{
private:
public:
};


class Trap : public Object
{
private:
public:
};

class ThreeSpeers : public Trap
{
private:
public:
};

class LaserBarrier : public Trap
{
private:
public:
};


class Enemy : public Object
{
private:
public:
};

class Rat : public Enemy
{
private:
public:
};

class HangingSpider : public Enemy
{
private:
public:
};

class ObjectSystem
{
private:
	std::map<uint32_t,Object *> object_list;
	std::map<uint32_t,Object *> visible_object_map;
	SpriteTexture *spriteset[Spriteset::MaxSpritesets];

public:
	ObjectSystem();
	~ObjectSystem();
	void clear();
	void loadSpritesets(SDL &sdl);
	void addObject(Object *object);
	void updateVisibleObjectList(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport);
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void save(ppl7::FileObject &file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr &ba);
	void saveState(ppl7::FileObject &file, unsigned char id) const;
	void loadState(const ppl7::ByteArrayPtr &ba) const;
	Object *findMatchingObject(const ppl7::grafix::Point &p) const;
	void drawSelectedSpriteOutline(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords, int id);


	size_t count() const;
	size_t countVisible() const;

};

} // EOF namespace Decker::Objects
