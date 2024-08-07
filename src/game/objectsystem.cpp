#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "player.h"
#include "wallenstein.h"
#include "helena.h"

namespace Decker::Objects {

static ObjectSystem* object_system=NULL;

static uint8_t getDifficultyMatrix()
{
	switch (GetGame().config.difficulty) {
		case Config::DifficultyLevel::easy: return 1;
		case Config::DifficultyLevel::normal: return 2;
		case Config::DifficultyLevel::hard: return 4;
	}
	return 2;
}

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
	light_objects=new SpriteTexture();
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
	delete(light_objects);
}

void ObjectSystem::clear()
{
	for (int i=0;i < static_cast<int>(PlaneId::MaxPlaneId);i++) visible_object_map[i].clear();
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

	light_objects->enableOutlines(true);
	light_objects->enableMemoryBuffer(true);
	light_objects->load(sdl, "res/lightobjects.tex");


	spriteset[Spriteset::GenericObjects]->enableOutlines(true);
	spriteset[Spriteset::GenericObjects]->enableMemoryBuffer(true);
	spriteset[Spriteset::GenericObjects]->load(sdl, "res/objects.tex");
	spriteset[Spriteset::GenericObjects]->setPivot(299, 128, 149);
	spriteset[Spriteset::GenericObjects]->setPivot(300, 128, 236);
	spriteset[Spriteset::GenericObjects]->setPivot(1, 128, 228);
	spriteset[Spriteset::GenericObjects]->setPivot(2, 128, 228);

	spriteset[Spriteset::GenericObjects]->setPivot(212, 128, 128);
	spriteset[Spriteset::GenericObjects]->setPivot(213, 128, 128);

	spriteset[Spriteset::GenericObjects]->setPivot(297, 128, 220);	// Spawnpoint
	spriteset[Spriteset::GenericObjects]->setPivot(3, 128, 180);	// GlimmerNode



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

	spriteset[Spriteset::StamperV2]->enableOutlines(true);
	spriteset[Spriteset::StamperV2]->enableMemoryBuffer(true);
	spriteset[Spriteset::StamperV2]->load(sdl, "res/stamper_v2.tex");
	spriteset[Spriteset::StamperV2]->setPivot(19, 64, 268);
	spriteset[Spriteset::StamperV2]->setPivot(20, 64, 146);
	spriteset[Spriteset::StamperV2]->setPivot(21, 125, 238);
	spriteset[Spriteset::StamperV2]->setPivot(22, 1, 238);

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

	spriteset[Spriteset::Skull]->enableOutlines(true);
	spriteset[Spriteset::Skull]->enableMemoryBuffer(true);
	spriteset[Spriteset::Skull]->load(sdl, "res/skull.tex");

	spriteset[Spriteset::SkullMaster]->enableOutlines(true);
	spriteset[Spriteset::SkullMaster]->enableMemoryBuffer(true);
	spriteset[Spriteset::SkullMaster]->load(sdl, "res/skull_master.tex");

	spriteset[Spriteset::Ghost]->enableOutlines(true);
	spriteset[Spriteset::Ghost]->enableMemoryBuffer(true);
	spriteset[Spriteset::Ghost]->load(sdl, "res/ghost.tex");

	spriteset[Spriteset::Zombie]->enableOutlines(true);
	spriteset[Spriteset::Zombie]->enableMemoryBuffer(true);
	spriteset[Spriteset::Zombie]->load(sdl, "res/zombie.tex");
	for (int i=167;i <= 182;i++)spriteset[Spriteset::Zombie]->setPivot(i, 128, 97);

	spriteset[Spriteset::Switches]->enableOutlines(true);
	spriteset[Spriteset::Switches]->enableMemoryBuffer(true);
	spriteset[Spriteset::Switches]->load(sdl, "res/switches.tex");
	for (int i=1;i <= 6;i++) spriteset[Spriteset::Switches]->setPivot(i, 144, 250); // compat reason

	spriteset[Spriteset::Crates]->enableOutlines(true);
	spriteset[Spriteset::Crates]->enableMemoryBuffer(true);
	spriteset[Spriteset::Crates]->load(sdl, "res/crates.tex");

	spriteset[Spriteset::Spider]->enableOutlines(true);
	spriteset[Spriteset::Spider]->enableMemoryBuffer(true);
	spriteset[Spriteset::Spider]->load(sdl, "res/spider.tex");

	spriteset[Spriteset::MagicGround]->enableOutlines(true);
	spriteset[Spriteset::MagicGround]->enableMemoryBuffer(true);
	spriteset[Spriteset::MagicGround]->load(sdl, "res/magicground.tex");

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


void ObjectSystem::updateVisibleObjectsForPlane(PlaneId plane, const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
	std::map<uint32_t, Object*>::iterator it;
	std::list<uint32_t> deleteme;
	int width=viewport.width();
	int height=viewport.height();

	for (it=object_list.begin();it != object_list.end();++it) {
		Object* object=it->second;
		if (object->myPlane != plane) continue;
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
				uint64_t id=(((uint64_t)object->p.y & 0xffff) << 48) | (uint64_t)(((uint64_t)object->p.x & 0xffff) << 32) | (uint64_t)object->id;
				visible_object_map[static_cast<int>(plane)].insert(std::pair<uint32_t, Object*>(id, object));
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

void ObjectSystem::updateVisibleObjectList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
	for (int i=0;i < static_cast<int>(PlaneId::MaxPlaneId);i++) visible_object_map[i].clear();
	updateVisibleObjectsForPlane(PlaneId::Player, worldcoords * planeFactor[0], viewport);
	updateVisibleObjectsForPlane(PlaneId::Middle, worldcoords * planeFactor[4], viewport);
	updateVisibleObjectsForPlane(PlaneId::Far, worldcoords * planeFactor[2], viewport);
	updateVisibleObjectsForPlane(PlaneId::Horizon, worldcoords * planeFactor[5], viewport);
	updateVisibleObjectsForPlane(PlaneId::Near, worldcoords * planeFactor[6], viewport);
}

void ObjectSystem::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	std::map<uint32_t, Object*>::iterator it;
	uint8_t dm=getDifficultyMatrix();
	for (it=object_list.begin();it != object_list.end();++it) {
		Object* object=it->second;
		if (object->difficulty_matrix & dm) object->update(time, ttplane, player, frame_rate_compensation);
	}
}

void ObjectSystem::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, PlaneId plane, Object::Layer layer) const
{
	std::map<uint64_t, Object*>::const_iterator it;
	ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
	uint8_t dm=getDifficultyMatrix();
	for (it=visible_object_map[static_cast<int>(plane)].begin();it != visible_object_map[static_cast<int>(plane)].end();++it) {
		const Object* object=it->second;
		if (object->texture != NULL && object->enabled == true && object->visibleAtPlaytime == true && object->myLayer == layer && (object->difficulty_matrix & dm)) {
			object->draw(renderer, coords);
		}
	}
}

static void drawId(SDL_Renderer* renderer, SpriteTexture* spriteset, int x, int y, uint32_t as)
{
	ppl7::String s;
	s.setf("%d", as);
	int w=(int)s.size() * 10;
	x-=w / 2;
	for (size_t p=0;p < s.size();p++) {
		int num=s[p] - 48 + 3;
		spriteset->draw(renderer, x, y, num);
		x+=10;
	}
}

void ObjectSystem::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, PlaneId plane, Object::Layer layer) const
{
	std::map<uint64_t, Object*>::const_iterator it;
	ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
	for (it=visible_object_map[static_cast<int>(plane)].begin();it != visible_object_map[static_cast<int>(plane)].end();++it) {
		const Object* object=it->second;
		if (object->type() == Decker::Objects::Type::Particle) {
			if (object->texture != NULL && object->myLayer == layer) {
				object->draw(renderer, coords);
			}
		} else {
			if (object->texture != NULL && object->myLayer == layer) {
				object->drawEditMode(renderer, coords);
				drawId(renderer, light_objects, object->p.x + coords.x, object->p.y + coords.y, object->id);
				if (object->p != object->initial_p && object->spawned == false) drawId(renderer, light_objects, object->initial_p.x + coords.x, object->initial_p.y + coords.y, object->id);
			}
		}
	}
}


Object* ObjectSystem::findMatchingObjectOnPlane(PlaneId plane, const ppl7::grafix::Point& p) const
{
	Object* found_object=NULL;
	std::map<uint64_t, Object*>::const_iterator it;
	for (it=visible_object_map[static_cast<int>(plane)].begin();it != visible_object_map[static_cast<int>(plane)].end();++it) {
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

Object* ObjectSystem::findMatchingObject(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Point& p) const
{
	Object* found_object=NULL;
	found_object=findMatchingObjectOnPlane(PlaneId::Near, p + worldcoords * planeFactor[6]);
	if (found_object) return found_object;

	found_object=findMatchingObjectOnPlane(PlaneId::Player, p + worldcoords * planeFactor[0]);
	if (found_object) return found_object;

	found_object=findMatchingObjectOnPlane(PlaneId::Middle, p + worldcoords * planeFactor[4]);
	if (found_object) return found_object;

	found_object=findMatchingObjectOnPlane(PlaneId::Far, p + worldcoords * planeFactor[2]);
	if (found_object) return found_object;

	found_object=findMatchingObjectOnPlane(PlaneId::Horizon, p + worldcoords * planeFactor[5]);
	if (found_object) return found_object;
	return NULL;
}

bool ObjectSystem::checkCollisionWithObject(const std::list<ppl7::grafix::Point>& checkpoints, const Object* object)
{
	std::list<ppl7::grafix::Point>::const_iterator p_it;
	for (p_it=checkpoints.begin();p_it != checkpoints.end();++p_it) {
		if ((*p_it).inside(object->boundary)) {
			//printf ("inside boundary\n");
			if (object->pixelExactCollision == false) return true;
			else {
				const ppl7::grafix::Drawable draw=object->texture->getDrawable(object->sprite_no);
				if (draw.width()) {
					int x=(*p_it).x - object->boundary.x1;
					int y=(*p_it).y - object->boundary.y1;
					ppl7::grafix::Color c=draw.getPixel(x, y);
					if (c.alpha() > 92) return true;
				}
			}
		}
	}
	return false;
}

void ObjectSystem::detectCollision(const std::list<ppl7::grafix::Point>& checkpoints, std::list<Object*>& object_list)
{
	std::map<uint64_t, Object*>::const_iterator it;
	std::list<ppl7::grafix::Point>::const_iterator p_it;
	uint8_t dm=getDifficultyMatrix();
	for (it=visible_object_map[static_cast<int>(PlaneId::Player)].begin();it != visible_object_map[static_cast<int>(PlaneId::Player)].end();++it) {
		Object* item=it->second;
		if (item->texture != NULL && item->collisionDetection == true && item->enabled == true && (item->difficulty_matrix & dm)) {
			if (ObjectSystem::checkCollisionWithObject(checkpoints, item)) object_list.push_back(item);
		}
	}
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
		case Type::Peach: return PeachReward::representation();
		case Type::Cherry: return CherryReward::representation();
		case Type::ExtraLife: return ExtraLife::representation();
		case Type::Key: return KeyReward::representation();
		case Type::Arrow: return Arrow::representation();
		case Type::FireCannon: return FireCannon::representation();
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
		case Type::Stamper: return Stamper::representation();
		case Type::Wallenstein: return Wallenstein::representation();
		case Type::Helena: return Helena::representation();
		case Type::Bat: return Bat::representation();
		case Type::Bird: return Bird::representation();
		case Type::Yeti: return Yeti::representation();
		case Type::Zombie: return Zombie::representation();
		case Type::Scorpion: return Scorpion::representation();
		case Type::LevelEnd: return LevelEnd::representation();
		case Type::AutoGeorge: return AutoGeorge::representation();
		case Type::Ostrich: return Ostrich::representation();
		case Type::Oxygen: return OxygenTank::representation();
		case Type::Fish: return Fish::representation();
		case Type::Piranha: return Piranha::representation();
		case Type::Skull: return Skull::representation();
		case Type::SkullMaster: return SkullMaster::representation();
		case Type::BreakingWall: return BreakingWall::representation();
		case Type::Hammer: return Hammer::representation();
		case Type::Flashlight: return Flashlight::representation();
		case Type::Cheese: return Cheese::representation();
		case Type::VoiceTrigger: return VoiceTrigger::representation();
		case Type::ObjectWatcher: return ObjectWatcher::representation();
		case Type::Trigger: return Trigger::representation();
		case Type::LightTrigger: return LightTrigger::representation();
		case Type::LevelModificator: return LevelModificator::representation();
		case Type::LightSignal: return LightSignal::representation();
		case Type::ButtonSwitch: return ButtonSwitch::representation();
		case Type::TouchPlateSwitch: return TouchPlateSwitch::representation();
		case Type::Crate: return Crate::representation();
		case Type::Spider: return Spider::representation();
		case Type::PowerCell: return PowerCell::representation();
		case Type::SpawnPoint: return SpawnPoint::representation();
		case Type::MagicGround: return MagicGround::representation();
		case Type::GlimmerNode: return GlimmerNode::representation();
		case Type::ItemTaker: return ItemTaker::representation();
		case Type::DamageTrigger: return DamageTrigger::representation();

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
		case Type::Peach: return new PeachReward();
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
		case Type::FireCannon: return new FireCannon();
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
		case Type::Stamper: return new Stamper();
		case Type::Wallenstein: return new Wallenstein();
		case Type::Helena: return new Helena();
		case Type::Yeti: return new Yeti();
		case Type::Zombie: return new Zombie();
		case Type::Bat: return new Bat();
		case Type::Bird: return new Bird();
		case Type::Scorpion: return new Scorpion();
		case Type::LevelEnd: return new LevelEnd();
		case Type::AutoGeorge: return new AutoGeorge();
		case Type::Ostrich: return new Ostrich();
		case Type::Oxygen: return new OxygenTank();
		case Type::Fish: return new Fish();
		case Type::Piranha: return new Piranha();
		case Type::Skull: return new Skull();
		case Type::SkullMaster: return new SkullMaster();
		case Type::BreakingWall: return new BreakingWall();
		case Type::Hammer: return new Hammer();
		case Type::Flashlight: return new Flashlight();
		case Type::Cheese: return new Cheese();
		case Type::VoiceTrigger: return new VoiceTrigger();
		case Type::ObjectWatcher: return new ObjectWatcher();
		case Type::Trigger: return new Trigger();
		case Type::LightTrigger: return new LightTrigger();
		case Type::LevelModificator: return new LevelModificator();
		case Type::LightSignal: return new LightSignal();
		case Type::TouchPlateSwitch: return new TouchPlateSwitch();
		//case Type::ButtonSwitch: return new ButtonSwitch();
		case Type::Crate: return new Crate();
		case Type::Spider: return new Spider();
		case Type::PowerCell: return new PowerCell();
		case Type::SpawnPoint: return new SpawnPoint();
		case Type::MagicGround: return new MagicGround();
		case Type::GlimmerNode: return new GlimmerNode();
		case Type::ItemTaker: return new ItemTaker();
		case Type::DamageTrigger: return new DamageTrigger();

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
		/*
		if (type == Objects::Type::Ghost) {
			printf("try to load object of type %d (%s), size: %d\n", type, (const char*)object->typeName(), save_size);
			ppl7::HexDump(buffer + p + 1, save_size - 1);
			fflush(stdout);
		}
		*/
		if (object) {
			if (object->load(buffer + p + 4, save_size - 4)) {
				if (object->id >= nextid) nextid=object->id + 1;
				if (object->sprite_set < Spriteset::MaxSpritesets && this->spriteset[object->sprite_set] != NULL) {
					object->texture=this->spriteset[object->sprite_set];
					object->updateBoundary();
				}
				object_list.insert(std::pair<uint32_t, Object*>(object->id, object));
			} else {
				delete object;
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
		if (object->type() == Decker::Objects::Type::PlayerStartpoint && object->myPlane == PlaneId::Player)
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
		if (object->type() == Decker::Objects::Type::PlayerStartpoint && object->myPlane == PlaneId::Player) {
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
	size_t count=0;
	for (int i=0;i < static_cast<int>(PlaneId::MaxPlaneId);i++) {
		count+=visible_object_map[i].size();
	}
	return count;
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
			} else if (obj->type() == Type::ObjectType::SpawnPoint) {
				int t=((const SpawnPoint*)obj)->emitted_object;
				object_counter[t]+=((const TouchEmitter*)obj)->max_toggles;
			}
		}
	}
}


bool ObjectSystem::findObjectsInRange(const ppl7::grafix::PointF& p, double range, std::list <Object*>& objects)
{
	objects.clear();
	std::map<uint64_t, Object*>::const_iterator it;
	for (it=visible_object_map[static_cast<int>(PlaneId::Player)].begin();it != visible_object_map[static_cast<int>(PlaneId::Player)].end();++it) {
		double dist=ppl7::grafix::Distance((*it).second->p, p);
		if (dist < range) objects.push_back((*it).second);
	}
	if (objects.size() > 0) return true;
	return false;
}

static void getCheckPoints(const Object* object, std::list<ppl7::grafix::Point>& checkpoints)
{
	const ppl7::grafix::Drawable& draw=object->texture->getDrawable(object->sprite_no);
	ppl7::grafix::Rect boundary=object->texture->spriteBoundary(object->sprite_no, 1.0f, object->p.x, object->p.y);
	if (!draw.width()) return;
	int stepx=boundary.width() / 16;
	int stepy=boundary.height() / 16;
	for (int py=boundary.y1;py < boundary.y2;py+=stepx) {
		for (int px=boundary.x1;px < boundary.x2;px+=stepy) {
			ppl7::grafix::Color c=draw.getPixel(px - boundary.x1, py - boundary.y1);
			if (c.alpha() > 92) {
				checkpoints.push_back(ppl7::grafix::Point(px, py));
			}
		}
	}
}

static bool checkCollision(const Object* obj1, const std::list<ppl7::grafix::Point> checkpoints1, const Object* obj2) {
	if (obj1->pixelExactCollision == false && obj2->pixelExactCollision == false) return true;
	ppl7::grafix::Rect intersection=obj1->boundary.intersected(obj2->boundary);
	std::list<ppl7::grafix::Point>::const_iterator it;
	if (obj1->pixelExactCollision == false) {
		std::list<ppl7::grafix::Point> checkpoints2;
		getCheckPoints(obj2, checkpoints2);
		//ppl7::PrintDebug("we have %d checkpoints\n", (int)checkpoints2.size());
		for (it=checkpoints2.begin();it != checkpoints2.end();++it) {
			if (it->inside(intersection)) return true;
		}
	} else if (obj2->pixelExactCollision == false) {
		for (it=checkpoints1.begin();it != checkpoints1.end();++it) {
			if (it->inside(intersection)) return true;
		}
	} else {
		ppl7::PrintDebug("checkCollision with two pixelExcact objects not implemented yet\n");
	}
	return false;
}



void ObjectSystem::detectObjectCollision(const Object* object, std::list<Object*>& collision_object_list)
{
	collision_object_list.clear();
	std::list<ppl7::grafix::Point> checkpoints;
	if (object->pixelExactCollision) getCheckPoints(object, checkpoints);
	std::map<uint32_t, Object*>::const_iterator it;
	for (it=object_list.begin();it != object_list.end();++it) {
		if (it->second != object && it->second->enabled == true && it->second->visibleAtPlaytime == true && it->second->myPlane == object->myPlane) {
			if (object->boundary.intersects(it->second->boundary)) {
				if (checkCollision(object, checkpoints, it->second))
					collision_object_list.push_back(it->second);
			}
		}
	}
}

void ObjectSystem::detectObjectCollision(const ppl7::grafix::Rect& boundary, std::list<Object*>& collision_object_list)
{
	//ppl7::PrintDebug("ObjectSystem::detectObjectCollision\n");
	collision_object_list.clear();
	std::map<uint32_t, Object*>::const_iterator it;
	for (it=object_list.begin();it != object_list.end();++it) {
		if (it->second->type() == Decker::Objects::Type::GlimmerNode) {
			//ppl7::PrintDebug("checking against GlimmerNode: %d, rect: %d:%d - %d:%d\n", it->second->id,
			//it->second->boundary.x1, it->second->boundary.y1, it->second->boundary.x2, it->second->boundary.y2);
			if (it->second->enabled == true && it->second->myPlane == PlaneId::Player) {
				if (boundary.intersects(it->second->boundary)) {
					collision_object_list.push_back(it->second);
				}
			}
		}
	}
}




}	// EOF namespace Decker::Objects
