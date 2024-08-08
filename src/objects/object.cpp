#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"

namespace Decker::Objects {

ppl7::String Type::name(Type::ObjectType type)
{
	switch (type) {
		case ObjectType::PlayerStartpoint: return ppl7::String("PlayerStartpoint");
		case ObjectType::Savepoint: return ppl7::String("Savepoint");
		case ObjectType::Medikit: return ppl7::String("Medikit");

		case ObjectType::Crystal: return ppl7::String("Crystal");
		case ObjectType::Diamond: return ppl7::String("Diamond");
		case ObjectType::Coin: return ppl7::String("Coin");
		case ObjectType::Cherry: return ppl7::String("Cherry");
		case ObjectType::Apple: return ppl7::String("Apple");
		case ObjectType::Key: return ppl7::String("Key");
		case ObjectType::Door: return ppl7::String("Door");
		case ObjectType::FloaterHorizontal: return ppl7::String("FloaterHorizontal");
		case ObjectType::FloaterVertical: return ppl7::String("FloaterVertical");
		case ObjectType::BreakingGround: return ppl7::String("BreakingGround");

		case ObjectType::Fire: return ppl7::String("Fire");
		case ObjectType::WindEmitter: return ppl7::String("WindEmitter");
		case ObjectType::RainEmitter: return ppl7::String("RainEmitter");
		case ObjectType::ParticleEmitter: return ppl7::String("ParticleEmitter");
		case ObjectType::Vent: return ppl7::String("Vent");
		case ObjectType::Speaker: return ppl7::String("Speaker");
		case ObjectType::Particle: return ppl7::String("Particle");
		case ObjectType::TouchEmitter: return ppl7::String("TouchEmitter");
		case ObjectType::TreasureChest: return ppl7::String("TreasureChest");
		case ObjectType::WarpGate: return ppl7::String("WarpGate");
		case ObjectType::ExtraLife: return ppl7::String("ExtraLife");
		case ObjectType::Switch: return ppl7::String("Switch");
		case ObjectType::LevelEnd: return ppl7::String("LevelEnd");


		case ObjectType::Arrow: return ppl7::String("Arrow");
		case ObjectType::ThreeSpeers: return ppl7::String("ThreeSpeers");
		case ObjectType::Rat: return ppl7::String("Rat");
		case ObjectType::HangingSpider: return ppl7::String("HangingSpider");
		case ObjectType::Skeleton: return ppl7::String("Skeleton");
		case ObjectType::Mummy: return ppl7::String("Mummy");
		case ObjectType::LaserBeamHorizontal: return ppl7::String("LaserBeamHorizontal");
		case ObjectType::LaserBeamVertical: return ppl7::String("LaserBeamVertical");
		case ObjectType::Mushroom: return ppl7::String("Mushroom");
		case ObjectType::Scarabeus: return ppl7::String("Scarabeus");
		case ObjectType::Stamper: return ppl7::String("Stamper");
		case ObjectType::Wallenstein: return ppl7::String("Wallenstein");
		case ObjectType::Helena: return ppl7::String("Helena");
		case ObjectType::Yeti: return ppl7::String("Yeti");
		case ObjectType::Scorpion: return ppl7::String("Scorpion");
		case ObjectType::Bat: return ppl7::String("Bat");
		case ObjectType::Bird: return ppl7::String("Bird");
		case ObjectType::AutoGeorge: return ppl7::String("AutoGeorge");
		case ObjectType::Ostrich: return ppl7::String("Ostrich");

		default: return ppl7::String("unknown object type: %d", static_cast<int>(type));
	}
}

Representation::Representation(int sprite_set, int sprite_no)
{
	this->sprite_set=sprite_set;
	this->sprite_no=sprite_no;
}


Object::Object(Type::ObjectType type)
{
	myPlane=PlaneId::Player;
	myType=type;
	sprite_set=0;
	sprite_no=0;
	sprite_no_representation=0;
	id=0;
	texture=NULL;
	collisionDetection=false;
	visibleAtPlaytime=true;
	enabled=true;
	isInViewport=false;
	alwaysUpdate=true;
	//save_size=14;
	pixelExactCollision=true;
	spawned=false;
	deleteDefered=false;
	myLayer=Layer::BehindPlayer;
	scale=1.0f;
	rotation=0.0f;
	difficulty_matrix=255;
	color_mod.set(255, 255, 255, 255);
}

Object::~Object()
{

}

Type::ObjectType Object::type() const
{
	return myType;
}

ppl7::String Object::typeName() const
{
	return Type::name(myType);
}


void Object::updateBoundary()
{
	if (texture) {
		boundary=texture->spriteBoundary(sprite_no, 1.0f, p.x, p.y);
		initial_boundary=texture->spriteBoundary(sprite_no_representation, 1.0f, initial_p.x, initial_p.y);
	}
}


Representation Object::representation()
{
	return Representation(-1, 0);
}

void Object::update(double, TileTypePlane&, Player&, float)
{

}

size_t Object::save(unsigned char* buffer, size_t size) const
{
	if (size < 17) return 0;
	ppl7::Poke8(buffer + 0, 3);	// Object-Header-Version
	ppl7::Poke16(buffer + 1, myType);
	ppl7::Poke8(buffer + 3, static_cast<int>(myLayer));
	ppl7::Poke32(buffer + 4, id);
	ppl7::Poke32(buffer + 8, initial_p.x);
	ppl7::Poke32(buffer + 12, initial_p.y);
	ppl7::Poke8(buffer + 16, difficulty_matrix);
	ppl7::Poke8(buffer + 17, static_cast<int>(myPlane));
	return 18;
}

size_t Object::saveSize() const
{
	return 18;
}

size_t Object::load(const unsigned char* buffer, size_t size)
{

	if (size < 16) return 0;
	int version=ppl7::Peek8(buffer + 0);
	if (version < 1 || version > 3) return 0;
	myLayer=static_cast<Layer>(ppl7::Peek8(buffer + 3));
	id=ppl7::Peek32(buffer + 4);
	if (static_cast<int>(myLayer) > 2) {
		ppl7::PrintDebug("Waring, found object with id %d and obsolete layer: %d\n", id, static_cast<int>(myLayer));
		return 0;
	}
	initial_p.x=ppl7::Peek32(buffer + 8);
	initial_p.y=ppl7::Peek32(buffer + 12);
	p=initial_p;
	if (version == 1) return 16;
	difficulty_matrix=ppl7::Peek8(buffer + 16);
	if (version == 2) return 17;
	myPlane=static_cast<PlaneId>(ppl7::Peek8(buffer + 17));
	return 18;
}

void Object::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	if (!spawned) {
		texture->drawScaled(renderer,
			initial_p.x + coords.x,
			initial_p.y + coords.y,
			sprite_no_representation, scale, color_mod);
	}

	ppl7::grafix::Color c=color_mod;
	c.setAlpha(90);
	texture->drawScaled(renderer,
		p.x + coords.x,
		p.y + coords.y,
		sprite_no, scale, c);

}

void Object::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	if (scale == 1.0f && rotation == 0.0f)
		texture->draw(renderer,
			p.x + coords.x,
			p.y + coords.y,
			sprite_no, color_mod);
	else
		texture->drawScaledWithAngle(renderer,
			p.x + coords.x,
			p.y + coords.y,
			sprite_no, scale, scale, rotation, color_mod);
}

void Object::handleCollision(Player* player, const Collision& collision)
{

}

void Object::openUi()
{

}

void Object::reset()
{
	if (!spawned) enabled=true;
}

void Object::toggle(bool enabled, Object*)
{
	this->enabled=enabled;
}

void Object::trigger(Object*)
{
	enabled=!enabled;
}

bool Object::isEnabled() const
{
	return enabled;
}


void Object::updateSpriteset(int spriteset)
{
	if (spriteset != this->sprite_set) {
		sprite_set=spriteset;
		texture=GetObjectSystem()->getTexture(sprite_set);
	}

}

}	// EOF namespace Decker::Objects
