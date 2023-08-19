#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "player.h"
#include "wallenstein.h"
#include "helena.h"

namespace Decker::Objects {

static ObjectSystem* object_system=NULL;

ObjectSystem* GetObjectSystem()
{
	return object_system;
}

ObjectSystem::ObjectSystem(Waynet* waynet)
{
	if (!object_system) object_system=this;
	nextid=1;
	next_spawn_id=1000000;
	for (int i=0;i < Spriteset::MaxSpritesets;i++) {
		spriteset[i]=new SpriteTexture();
	}
	player_start=0;
	this->waynet=waynet;
}

ObjectSystem::~ObjectSystem()
{
	clear();
	if (object_system == this) object_system=NULL;
	for (int i=0;i < Spriteset::MaxSpritesets;i++) {
		delete spriteset[i];
	}
}

void ObjectSystem::clear()
{
	visible_object_map.clear();
	std::map<uint32_t, Object*>::iterator it;
	while ((it=object_list.begin()) != object_list.end()) {
		Object* object=(it->second);
		object_list.erase(it);
		delete object;
	}
	nextid=1;
	next_spawn_id=1000000;
	player_start=0;
}

void ObjectSystem::loadSpritesets(SDL& sdl)
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	spriteset[Spriteset::GenericObjects]->enableOutlines(true);
	spriteset[Spriteset::GenericObjects]->enableMemoryBuffer(true);
	spriteset[Spriteset::GenericObjects]->load(sdl, "res/objects.tex");

	spriteset[Spriteset::ThreeSpeers]->enableOutlines(true);
	spriteset[Spriteset::ThreeSpeers]->enableMemoryBuffer(true);
	spriteset[Spriteset::ThreeSpeers]->load(sdl, "res/enemy_3speers.tex");

	spriteset[Spriteset::Mummy]->enableOutlines(true);
	spriteset[Spriteset::Mummy]->enableMemoryBuffer(true);
	spriteset[Spriteset::Mummy]->load(sdl, "res/mummy.tex");

	spriteset[Spriteset::Skeleton]->enableOutlines(true);
	spriteset[Spriteset::Skeleton]->enableMemoryBuffer(true);
	spriteset[Spriteset::Skeleton]->load(sdl, "res/skeleton.tex");

	spriteset[Spriteset::Vent]->enableOutlines(true);
	spriteset[Spriteset::Vent]->enableMemoryBuffer(true);
	spriteset[Spriteset::Vent]->load(sdl, "res/vent.tex");

	spriteset[Spriteset::Mushroom]->enableOutlines(true);
	spriteset[Spriteset::Mushroom]->enableMemoryBuffer(true);
	spriteset[Spriteset::Mushroom]->load(sdl, "res/mushroom.tex");

	spriteset[Spriteset::Scarabeus]->enableOutlines(true);
	spriteset[Spriteset::Scarabeus]->enableMemoryBuffer(true);
	spriteset[Spriteset::Scarabeus]->load(sdl, "res/scarabeus.tex");

	spriteset[Spriteset::TreasureChest]->enableOutlines(true);
	spriteset[Spriteset::TreasureChest]->enableMemoryBuffer(true);
	spriteset[Spriteset::TreasureChest]->load(sdl, "res/treasure_chest.tex");

	spriteset[Spriteset::Doors]->enableOutlines(true);
	spriteset[Spriteset::Doors]->enableMemoryBuffer(true);
	spriteset[Spriteset::Doors]->load(sdl, "res/doors.tex");

	spriteset[Spriteset::Laser]->enableOutlines(true);
	spriteset[Spriteset::Laser]->enableMemoryBuffer(true);
	spriteset[Spriteset::Laser]->load(sdl, "res/laser.tex");

	spriteset[Spriteset::StamperVertical]->enableOutlines(true);
	spriteset[Spriteset::StamperVertical]->enableMemoryBuffer(true);
	spriteset[Spriteset::StamperVertical]->load(sdl, "res/stamper_vertical.tex");

	spriteset[Spriteset::Wallenstein]->enableOutlines(true);
	spriteset[Spriteset::Wallenstein]->enableMemoryBuffer(true);
	spriteset[Spriteset::Wallenstein]->load(sdl, "res/wallenstein.tex");

	spriteset[Spriteset::Helena]->enableOutlines(true);
	spriteset[Spriteset::Helena]->enableMemoryBuffer(true);
	spriteset[Spriteset::Helena]->load(sdl, "res/helena.tex");

	spriteset[Spriteset::Yeti]->enableOutlines(true);
	spriteset[Spriteset::Yeti]->enableMemoryBuffer(true);
	spriteset[Spriteset::Yeti]->load(sdl, "res/yeti.tex");

	spriteset[Spriteset::Bat]->enableOutlines(true);
	spriteset[Spriteset::Bat]->enableMemoryBuffer(true);
	spriteset[Spriteset::Bat]->load(sdl, "res/bat.tex");

	spriteset[Spriteset::Scorpion]->enableOutlines(true);
	spriteset[Spriteset::Scorpion]->enableMemoryBuffer(true);
	spriteset[Spriteset::Scorpion]->load(sdl, "res/scorpion.tex");

	spriteset[Spriteset::ScorpionMetalic]->enableOutlines(true);
	spriteset[Spriteset::ScorpionMetalic]->enableMemoryBuffer(true);
	spriteset[Spriteset::ScorpionMetalic]->load(sdl, "res/scorpion_metalic.tex");

	spriteset[Spriteset::Bird]->enableOutlines(true);
	spriteset[Spriteset::Bird]->enableMemoryBuffer(true);
	spriteset[Spriteset::Bird]->load(sdl, "res/bird.tex");

	spriteset[Spriteset::LevelEnd]->enableOutlines(true);
	spriteset[Spriteset::LevelEnd]->enableMemoryBuffer(true);
	spriteset[Spriteset::LevelEnd]->load(sdl, "res/levelend.tex");

	spriteset[Spriteset::George]->enableOutlines(true);
	spriteset[Spriteset::George]->enableMemoryBuffer(true);
	spriteset[Spriteset::George]->load(sdl, "res/george.tex");

	spriteset[Spriteset::Ostrich]->enableOutlines(true);
	spriteset[Spriteset::Ostrich]->enableMemoryBuffer(true);
	spriteset[Spriteset::Ostrich]->load(sdl, "res/ostrich.tex");

	spriteset[Spriteset::Piranha]->enableOutlines(true);
	spriteset[Spriteset::Piranha]->enableMemoryBuffer(true);
	spriteset[Spriteset::Piranha]->load(sdl, "res/piranha.tex");

	spriteset[Spriteset::BreakingWall]->enableOutlines(true);
	spriteset[Spriteset::BreakingWall]->enableMemoryBuffer(true);
	spriteset[Spriteset::BreakingWall]->load(sdl, "res/breakingwall.tex");

	spriteset[Spriteset::Rat]->enableOutlines(true);
	spriteset[Spriteset::Rat]->enableMemoryBuffer(true);
	spriteset[Spriteset::Rat]->load(sdl, "res/rat.tex");

	spriteset[Spriteset::Ghost]->enableOutlines(true);
	spriteset[Spriteset::Ghost]->enableMemoryBuffer(true);
	spriteset[Spriteset::Ghost]->load(sdl, "res/ghost.tex");

}

void ObjectSystem::addObject(Object* object)
{
	if (!object) return;
	if (object->spawned) {
		object->id=next_spawn_id;
		next_spawn_id++;
	} else {
		object->id=nextid;
		nextid++;
	}
	if (object->sprite_set < Spriteset::MaxSpritesets && this->spriteset[object->sprite_set] != NULL) {
		object->texture=this->spriteset[object->sprite_set];
		object->updateBoundary();
	}

	object_list.insert(std::pair<uint32_t, Object*>(object->id, object));
}

void ObjectSystem::deleteObject(int id)
{
	std::map<uint32_t, Object*>::const_iterator it;
	it=object_list.find(id);
	if (it != object_list.end()) {
		Object* object=it->second;
		object_list.erase(it);
		delete object;
	}
}

void ObjectSystem::updateVisibleObjectList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
	visible_object_map.clear();
	std::map<uint32_t, Object*>::iterator it;
	std::list<uint32_t> deleteme;
	int width=viewport.width();
	int height=viewport.height();
	for (it=object_list.begin();it != object_list.end();++it) {
		Object* object=it->second;
		if (object->deleteDefered) {
			deleteme.push_back(it->first);
		} else if (object->texture) {
			int x=object->p.x - worldcoords.x;
			int y=object->p.y - worldcoords.y;
			bool isVisible=false;
			if (x + object->boundary.width() > 0 && y + object->boundary.height() > 0
				&& x - object->boundary.width() < width && y - object->boundary.height() < height)
				isVisible=true;
			if (object->p != object->initial_p) {
				x=object->initial_p.x - worldcoords.x;
				y=object->initial_p.y - worldcoords.y;
				if (x > 0 && y > 0 && x < width && y < height) isVisible=true;
			}
			if (isVisible) {
				uint32_t id=(uint32_t)(((uint32_t)object->p.y & 0xffff) << 16) | (uint32_t)((uint32_t)object->p.x & 0xffff);
				visible_object_map.insert(std::pair<uint32_t, Object*>(id, object));
			}
		}
	}
	if (deleteme.size() > 0) {
		std::list<uint32_t>::iterator it;
		for (it=deleteme.begin();it != deleteme.end();++it) {
			deleteObject(*it);
		}
	}
}

void ObjectSystem::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	std::map<uint32_t, Object*>::iterator it;
	for (it=object_list.begin();it != object_list.end();++it) {
		Object* object=it->second;
		object->update(time, ttplane, player, frame_rate_compensation);
	}
}

void ObjectSystem::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Object::Layer layer) const
{
	std::map<uint32_t, Object*>::const_iterator it;
	ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
	for (it=visible_object_map.begin();it != visible_object_map.end();++it) {
		const Object* object=it->second;
		if (object->texture != NULL && object->enabled == true && object->visibleAtPlaytime == true && object->myLayer == layer) {
			object->draw(renderer, coords);
		}
	}
}

void ObjectSystem::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Object::Layer layer) const
{
	std::map<uint32_t, Object*>::const_iterator it;
	ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
	for (it=visible_object_map.begin();it != visible_object_map.end();++it) {
		const Object* object=it->second;
		if (object->type() == Decker::Objects::Type::Particle) {
			if (object->texture != NULL && object->myLayer == layer) {
				object->draw(renderer, coords);
			}
		} else {
			if (object->texture != NULL && object->myLayer == layer) {
				object->drawEditMode(renderer, coords);
			}
		}
	}
}


Object* ObjectSystem::findMatchingObject(const ppl7::grafix::Point& p) const
{
	Object* found_object=NULL;
	std::map<uint32_t, Object*>::const_iterator it;
	for (it=visible_object_map.begin();it != visible_object_map.end();++it) {
		Object* item=it->second;
		if (p.inside(item->initial_boundary) == true && item->spawned == false) {
			if (item->texture) {
				const ppl7::grafix::Drawable draw=item->texture->getDrawable(item->sprite_no_representation);
				if (draw.width()) {
					int x=p.x - item->initial_boundary.x1;
					int y=p.y - item->initial_boundary.y1;
					ppl7::grafix::Color c=draw.getPixel(x, y);
					if (c.alpha() > 92) {
						found_object=item;
					}
				}
			}
		}
	}
	return found_object;
}

Object* ObjectSystem::detectCollision(const std::list<ppl7::grafix::Point>& player)
{
	std::map<uint32_t, Object*>::const_iterator it;
	std::list<ppl7::grafix::Point>::const_iterator p_it;
	for (it=visible_object_map.begin();it != visible_object_map.end();++it) {
		Object* item=it->second;
		if (item->texture != NULL && item->collisionDetection == true && item->enabled == true) {
			for (p_it=player.begin();p_it != player.end();++p_it) {
				if ((*p_it).inside(item->boundary)) {
					//printf ("inside boundary\n");
					if (item->pixelExactCollision == false) return item;
					const ppl7::grafix::Drawable draw=item->texture->getDrawable(item->sprite_no);
					if (draw.width()) {
						int x=(*p_it).x - item->boundary.x1;
						int y=(*p_it).y - item->boundary.y1;
						ppl7::grafix::Color c=draw.getPixel(x, y);
						if (c.alpha() > 92) {
							return item;
						}
					}
				}
			}
		}
	}
	return NULL;
}

Object* ObjectSystem::getObject(uint32_t object_id)
{
	std::map<uint32_t, Object*>::iterator it;
	it=object_list.find(object_id);
	if (it != object_list.end()) return it->second;
	return NULL;
}


void ObjectSystem::drawSelectedSpriteOutline(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id)
{
	std::map<uint32_t, Object*>::const_iterator it;
	it=object_list.find(id);
	if (it != object_list.end()) {
		const Object* item=it->second;
		if (item->texture) {
			item->texture->drawOutlines(renderer,
				item->initial_p.x + viewport.x1 - worldcoords.x,
				item->initial_p.y + viewport.y1 - worldcoords.y,
				item->sprite_no_representation, item->scale);
		}
	}
}

Representation getRepresentation(int object_type)
{
	switch (object_type) {
	case Type::PlayerStartpoint: return PlayerStartPoint::representation();
	case Type::Savepoint: return SavePoint::representation();
	case Type::Medikit: return Medikit::representation();
	case Type::Diamond: return GemReward::representation();
	case Type::Crystal: return CrystalReward::representation();
	case Type::Coin: return CoinReward::representation();
	case Type::Apple: return AppleReward::representation();
	case Type::Cherry: return CherryReward::representation();
	case Type::ExtraLife: return ExtraLife::representation();
	case Type::Key: return KeyReward::representation();
	case Type::Arrow: return Arrow::representation();
	case Type::ThreeSpeers: return ThreeSpeers::representation();
	case Type::Rat: return Rat::representation();
	case Type::HangingSpider: return HangingSpider::representation();
	case Type::FloaterHorizontal: return FloaterHorizontal::representation();
	case Type::FloaterVertical: return FloaterVertical::representation();
	case Type::Skeleton: return Skeleton::representation();
	case Type::Ghost: return Ghost::representation();
	case Type::Mummy: return Mummy::representation();
	case Type::Fire: return Fire::representation();
	case Type::BreakingGround: return BreakingGround::representation();
	case Type::LaserBeamHorizontal: return LaserBarrier::representation(Type::LaserBeamHorizontal);
	case Type::LaserBeamVertical: return LaserBarrier::representation(Type::LaserBeamVertical);
	case Type::Vent: return Vent::representation();
	case Type::WindEmitter: return WindEmitter::representation();
	case Type::RainEmitter: return RainEmitter::representation();
	case Type::ParticleEmitter: return ParticleEmitter::representation();
	case Type::Speaker: return Speaker::representation();
	case Type::TouchEmitter: return TouchEmitter::representation();
	case Type::Mushroom: return Mushroom::representation();
	case Type::TreasureChest: return TreasureChest::representation();
	case Type::Door: return Door::representation();
	case Type::Scarabeus: return Scarabeus::representation();
	case Type::Switch: return Switch::representation();
	case Type::StamperVertical: return StamperVertical::representation();
	case Type::Wallenstein: return Wallenstein::representation();
	case Type::Helena: return Helena::representation();
	case Type::Bat: return Bat::representation();
	case Type::Bird: return Bird::representation();
	case Type::Yeti: return Yeti::representation();
	case Type::Scorpion: return Scorpion::representation();
	case Type::LevelEnd: return LevelEnd::representation();
	case Type::AutoGeorge: return AutoGeorge::representation();
	case Type::Ostrich: return Ostrich::representation();
	case Type::Oxygen: return OxygenTank::representation();
	case Type::Fish: return Fish::representation();
	case Type::Piranha: return Piranha::representation();
	case Type::BreakingWall: return BreakingWall::representation();
	case Type::Hammer: return Hammer::representation();
	case Type::Cheese: return Cheese::representation();
	case Type::VoiceTrigger: return VoiceTrigger::representation();
	case Type::ObjectWatcher: return ObjectWatcher::representation();
	case Type::Trigger: return Trigger::representation();
	default: return Object::representation();
	}
}


SpriteTexture* ObjectSystem::getTexture(int sprite_set) const
{
	if (sprite_set >= 0 && sprite_set < Spriteset::MaxSpritesets)
		return spriteset[sprite_set];
	return NULL;
}

void ObjectSystem::drawPlaceSelection(SDL_Renderer* renderer, const ppl7::grafix::Point& p, int object_type)
{
	Representation repr=getRepresentation(object_type);
	if (repr.sprite_set >= 0) {
		SpriteTexture* texture=getTexture(repr.sprite_set);
		if (texture) {
			texture->draw(renderer,
				p.x,
				p.y,
				repr.sprite_no);
			texture->drawOutlines(renderer, p.x, p.y,
				repr.sprite_no, 1.0f);
		}
	}
}

Object* ObjectSystem::getInstance(int object_type) const
{
	switch (object_type) {
	case Type::ThreeSpeers: return new ThreeSpeers();
	case Type::Coin: return new CoinReward();
	case Type::Apple: return new AppleReward();
	case Type::Cherry: return new CherryReward();
	case Type::ExtraLife: return new ExtraLife();
	case Type::Crystal: return new CrystalReward();
	case Type::Diamond: return new GemReward();
	case Type::Medikit: return new Medikit();
	case Type::Savepoint: return new SavePoint();
	case Type::Key: return new KeyReward();
	case Type::PlayerStartpoint: return new PlayerStartPoint();
	case Type::FloaterHorizontal: return new FloaterHorizontal();
	case Type::FloaterVertical: return new FloaterVertical();
	case Type::Arrow: return new Arrow();
	case Type::Rat: return new Rat();
	case Type::Skeleton: return new Skeleton();
	case Type::Ghost: return new Ghost();
	case Type::Mummy: return new Mummy();
	case Type::HangingSpider: return new HangingSpider();
	case Type::Fire: return new Fire();
	case Type::BreakingGround: return new BreakingGround();
	case Type::LaserBeamHorizontal: return new LaserBarrier(Type::LaserBeamHorizontal);
	case Type::LaserBeamVertical: return new LaserBarrier(Type::LaserBeamVertical);
	case Type::WindEmitter: return new WindEmitter();
	case Type::RainEmitter: return new RainEmitter();
	case Type::ParticleEmitter: return new ParticleEmitter();
	case Type::Vent: return new Vent();
	case Type::Speaker: return new Speaker();
	case Type::TouchEmitter: return new TouchEmitter();
	case Type::Mushroom: return new Mushroom();
	case Type::TreasureChest: return new TreasureChest();
	case Type::Door: return new Door();
	case Type::Scarabeus: return new Scarabeus();
	case Type::Switch: return new Switch();
	case Type::StamperVertical: return new StamperVertical();
	case Type::Wallenstein: return new Wallenstein();
	case Type::Helena: return new Helena();
	case Type::Yeti: return new Yeti();
	case Type::Bat: return new Bat();
	case Type::Bird: return new Bird();
	case Type::Scorpion: return new Scorpion();
	case Type::LevelEnd: return new LevelEnd();
	case Type::AutoGeorge: return new AutoGeorge();
	case Type::Ostrich: return new Ostrich();
	case Type::Oxygen: return new OxygenTank();
	case Type::Fish: return new Fish();
	case Type::Piranha: return new Piranha();
	case Type::BreakingWall: return new BreakingWall();
	case Type::Hammer: return new Hammer();
	case Type::Cheese: return new Cheese();
	case Type::VoiceTrigger: return new VoiceTrigger();
	case Type::ObjectWatcher: return new ObjectWatcher();
	case Type::Trigger: return new Trigger();
	}
	return NULL;
}

void ObjectSystem::save(ppl7::FileObject& file, unsigned char id) const
{
	if (object_list.size() == 0) return;
	std::map<uint32_t, Object*>::const_iterator it;
	size_t buffersize=0;
	for (it=object_list.begin();it != object_list.end();++it) {
		Object* object=it->second;
		if (!object->spawned)
			buffersize+=object->saveSize() + 4;
	}
	unsigned char* buffer=(unsigned char*)malloc(buffersize + 5);
	ppl7::Poke32(buffer + 0, 0);
	ppl7::Poke8(buffer + 4, id);
	size_t p=5;
	for (it=object_list.begin();it != object_list.end();++it) {
		Object* object=it->second;
		if (!object->spawned) {
			size_t object_size=object->saveSize();
			ppl7::Poke32(buffer + p, object_size + 4);
			size_t bytes_saved=object->save(buffer + p + 4, object_size);
			if (bytes_saved == object_size && bytes_saved > 0) {
				p+=object_size + 4;
			}
		}
	}
	ppl7::Poke32(buffer + 0, p);
	file.write(buffer, p);
	free(buffer);
}

void ObjectSystem::load(const ppl7::ByteArrayPtr& ba)
{
	clear();
	size_t p=0;
	const unsigned char* buffer=(const unsigned char*)ba.toCharPtr();
	//printf("chunk size=%zd\n",ba.size());
	while (p < ba.size()) {
		int save_size=ppl7::Peek32(buffer + p);
		int type=ppl7::Peek16(buffer + p + 5);
		Object* object=getInstance(type);
		//printf("try to load object of type %d (%s), size: %d\n", type, (const char*)object->typeName(), save_size);
		//ppl7::HexDump(buffer + p + 1, save_size - 1);
		if (object) {
			if (object->load(buffer + p + 4, save_size - 4)) {
				if (object->id >= nextid) nextid=object->id + 1;
				if (object->sprite_set < Spriteset::MaxSpritesets && this->spriteset[object->sprite_set] != NULL) {
					object->texture=this->spriteset[object->sprite_set];
					object->updateBoundary();
				}
				object_list.insert(std::pair<uint32_t, Object*>(object->id, object));
			}
		}
		p+=save_size;
	}
	//printf ("nextid=%d\n",nextid);
}


ppl7::grafix::Point ObjectSystem::findPlayerStart() const
{
	std::map<uint32_t, Object*>::const_iterator it;
	for (it=object_list.begin();it != object_list.end();++it) {
		Object* object=it->second;
		if (object->type() == Decker::Objects::Type::PlayerStartpoint)
			return object->p;
	}
	return ppl7::grafix::Point(0, 0);
}

ppl7::grafix::Point ObjectSystem::nextPlayerStart()
{
	int c=0;
	player_start++;
	std::map<uint32_t, Object*>::const_iterator it;
	for (it=object_list.begin();it != object_list.end();++it) {
		Object* object=it->second;
		if (object->type() == Decker::Objects::Type::PlayerStartpoint) {
			if (c == player_start) {
				return object->p;
			}
			c++;
		}
	}
	player_start=0;
	return findPlayerStart();
}

void ObjectSystem::resetPlayerStart()
{
	player_start=0;
}

size_t ObjectSystem::count() const
{
	return object_list.size();
}

size_t ObjectSystem::countVisible() const
{
	return visible_object_map.size();
}

Waynet& ObjectSystem::getWaynet()
{
	return *waynet;
}

void ObjectSystem::getObjectCounter(std::map<int, size_t>& object_counter) const
{
	object_counter.clear();
	std::map<uint32_t, Object*>::const_iterator it;
	for (it=object_list.begin();it != object_list.end();++it) {
		const Object* obj=(*it).second;
		if (obj) {
			object_counter[obj->type()]++;
			if (obj->type() == Type::ObjectType::TouchEmitter) {
				int t=((const TouchEmitter*)obj)->emitted_object;
				object_counter[t]+=((const TouchEmitter*)obj)->max_toggles;
			}
		}
	}

}


bool ObjectSystem::findObjectsInRange(const ppl7::grafix::PointF& p, double range, std::list <Object*>& objects)
{
	objects.clear();
	std::map<uint32_t, Object*>::const_iterator it;
	for (it=visible_object_map.begin();it != visible_object_map.end();++it) {
		double dist=ppl7::grafix::Distance((*it).second->p, p);
		if (dist < range) objects.push_back((*it).second);
	}
	if (objects.size() > 0) return true;
	return false;
}




Collision::Collision()
{
	object=NULL;
}

Collision::Collision(const Collision& other)
{
	object=other.object;
	collision_points=other.collision_points;
}


void Collision::detect(Object* object, const std::list<ppl7::grafix::Point>& checkpoints, const Player& player)
{
	collision_points.clear();
	this->object=object;
	std::list<ppl7::grafix::Point>::const_iterator p_it;
	const ppl7::grafix::Drawable draw=object->texture->getDrawable(object->sprite_no);
	for (p_it=checkpoints.begin();p_it != checkpoints.end();++p_it) {
		if (draw.width()) {
			int x=(*p_it).x - object->boundary.x1;
			int y=(*p_it).y - object->boundary.y1;
			ppl7::grafix::Color c=draw.getPixel(x, y);
			if (c.alpha() > 92) {
				collision_points.push_back(ppl7::grafix::Point((*p_it).x - player.x, (*p_it).y - player.y));
			}
		}
	}
}

const std::list<ppl7::grafix::Point>& Collision::getCollisionPoints() const
{
	return collision_points;
}

Object* Collision::getObject() const
{
	return object;
}

bool Collision::onFoot() const
{
	int height=bounding_box_object.height();
	if (height > 2 * TILE_HEIGHT) height=height * 2 / 3;
	else height=height / 2;
	if (bounding_box_intersection.y2 <= bounding_box_object.y2 - height) {
		return true;
	}
	return false;
}

bool Collision::objectBottom() const
{
	if (bounding_box_player.y2 > bounding_box_object.y2 &&
		bounding_box_player.y1<bounding_box_object.y2 &&
		bounding_box_player.y2>bounding_box_object.y2) {
		return true;
	}
	return false;
}

bool Collision::objectTop() const
{
	if (bounding_box_player.y2<bounding_box_object.y2 &&
		bounding_box_player.y2>bounding_box_object.y1 &&
		bounding_box_player.y1 < bounding_box_object.y1) {
		return true;
	}
	return false;

}

bool Collision::objectLeft() const
{
	if (bounding_box_player.x2<bounding_box_object.x2 &&
		bounding_box_player.x2>bounding_box_object.x1 &&
		bounding_box_player.x1 < bounding_box_object.x1) {
		return true;
	}
	return false;
}

bool Collision::objectRight() const
{
	if (bounding_box_player.x1 > bounding_box_object.x1 &&
		bounding_box_player.x1<bounding_box_object.x2 &&
		bounding_box_player.x2>bounding_box_object.x2) {
		return true;
	}
	return false;
}



}	// EOF namespace Decker::Objects
