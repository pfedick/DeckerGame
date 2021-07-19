#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"

namespace Decker::Objects {

ObjectSystem::ObjectSystem()
{
	nextid=1;
	for (int i=0;i<Spriteset::MaxSpritesets;i++) {
		spriteset[i]=new SpriteTexture();
	}
}

ObjectSystem::~ObjectSystem()
{
	clear();
}

void ObjectSystem::clear()
{
	visible_object_map.clear();
	std::map<uint32_t, Object *>::iterator it;
	while ((it=object_list.begin())!=object_list.end()) {
		Object *object=(it->second);
		object_list.erase(it);
		delete object;
	}
	nextid=1;
}

void ObjectSystem::loadSpritesets(SDL &sdl)
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"linear");
	spriteset[Spriteset::GenericObjects]->enableOutlines(true);
	spriteset[Spriteset::GenericObjects]->enableMemoryBuffer(true);
	spriteset[Spriteset::GenericObjects]->load(sdl,"res/objects.tex");

	spriteset[Spriteset::ThreeSpeers]->enableOutlines(true);
	spriteset[Spriteset::ThreeSpeers]->enableMemoryBuffer(true);
	spriteset[Spriteset::ThreeSpeers]->load(sdl,"res/enemy_3speers.tex");

}

void ObjectSystem::addObject(Object *object)
{
	if (!object) return;
	object->id=nextid;
	nextid++;
	if (object->sprite_set<Spriteset::MaxSpritesets && this->spriteset[object->sprite_set]!=NULL) {
		object->texture=this->spriteset[object->sprite_set];
		object->updateBoundary();
	}

	object_list.insert(std::pair<uint32_t,Object*>(object->id,object));
}

void ObjectSystem::deleteObject(int id)
{
	std::map<uint32_t,Object *>::const_iterator it;
	it=object_list.find(id);
	if (it!=object_list.end()) {
		Object *object=it->second;
		object_list.erase(it);
		delete object;
	}
}

void ObjectSystem::updateVisibleObjectList(const ppl7::grafix::Point &worldcoords, const ppl7::grafix::Rect &viewport)
{
	visible_object_map.clear();
	std::map<uint32_t, Object *>::iterator it;
	int width=viewport.width();
	int height=viewport.height();
	for (it=object_list.begin();it!=object_list.end();++it) {
		Object *object=it->second;
		if (object->texture) {
			int x=object->p.x-worldcoords.x;
			int y=object->p.y-worldcoords.y;
			if (x+object->boundary.width()>0 && y+object->boundary.height()>0
					&& x-object->boundary.width()<width && y-object->boundary.height()<height ) {
				uint32_t id=(uint32_t)((object->p.y&0xffff)<<16)|(uint32_t)(object->p.x&0xffff);
				visible_object_map.insert(std::pair<uint32_t,Object *>(id,object));
			}
		}
	}
}

void ObjectSystem::update(double time)
{
	std::map<uint32_t, Object *>::iterator it;
	for (it=object_list.begin();it!=object_list.end();++it) {
		Object *object=it->second;
		object->update(time);
	}
}

void ObjectSystem::draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	std::map<uint32_t,Object *>::const_iterator it;
	for (it=visible_object_map.begin();it!=visible_object_map.end();++it) {
		const Object *object=it->second;
		if (object->texture) {
			object->texture->draw(renderer,
					object->p.x+viewport.x1-worldcoords.x,
					object->p.y+viewport.y1-worldcoords.y,
					object->sprite_no);
		}
	}
}

Object *ObjectSystem::findMatchingObject(const ppl7::grafix::Point &p) const
{
	Object *found_object=NULL;
	std::map<uint32_t,Object *>::const_iterator it;
	for (it=visible_object_map.begin();it!=visible_object_map.end();++it) {
		Object *item=it->second;
		if (p.inside(item->boundary)) {
			if (item->texture) {
				const ppl7::grafix::Drawable draw=item->texture->getDrawable(item->sprite_no);
				if (draw.width()) {
					int x=p.x-item->boundary.x1;
					int y=p.y-item->boundary.y1;
					ppl7::grafix::Color c=draw.getPixel(x, y);
					if (c.alpha()>92) {
						found_object=item;
					}
				}
			}
		}
	}
	return found_object;
}

void ObjectSystem::drawSelectedSpriteOutline(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords, int id)
{
	std::map<uint32_t,Object *>::const_iterator it;
	it=object_list.find(id);
	if (it!=object_list.end()) {
		const Object *item=it->second;
		if (item->texture) {
			item->texture->drawOutlines(renderer,
					item->p.x+viewport.x1-worldcoords.x,
					item->p.y+viewport.y1-worldcoords.y,
					item->sprite_no, 1.0f);
		}
	}
}

Representation getRepresentation(int object_type)
{
	switch (object_type) {
	case Type::Savepoint: return SavePoint::representation();
	case Type::Medikit: return Medikit::representation();
	case Type::Diamond: return GemReward::representation();
	case Type::Crystal: return CrystalReward::representation();
	case Type::Coin: return CoinReward::representation();
	case Type::Key: return KeyReward::representation();
	case Type::Arrow: return Arrow::representation();
	case Type::ThreeSpeers: return ThreeSpeers::representation();
	case Type::Rat: return Rat::representation();
	case Type::HangingSpider: return HangingSpider::representation();
	case Type::FloaterHorizontal: return FloaterHorizontal::representation();
	case Type::FloaterVertical: return FloaterVertical::representation();
	default: return Object::representation();
	}
}


SpriteTexture *ObjectSystem::getTexture(int sprite_set) const
{
	if (sprite_set>=0 && sprite_set<Spriteset::MaxSpritesets)
		return spriteset[sprite_set];
	return NULL;
}

void ObjectSystem::drawPlaceSelection(SDL_Renderer *renderer, const ppl7::grafix::Point &p, int object_type)
{
	Representation repr=getRepresentation(object_type);
	if (repr.sprite_set>=0) {
		SpriteTexture *texture=getTexture(repr.sprite_set);
		if (texture) {
			texture->draw(renderer,
					p.x,
					p.y,
					repr.sprite_no);
			texture->drawOutlines(renderer,p.x,p.y,
					repr.sprite_no, 1.0f);
		}
	}
}

Object * ObjectSystem::getInstance(int object_type) const
{
	switch (object_type) {
	case Type::ThreeSpeers: return new ThreeSpeers();
	case Type::Coin: return new CoinReward();
	case Type::Crystal: return new CrystalReward();
	case Type::Diamond: return new GemReward();

	}
	return NULL;
}

void ObjectSystem::save(ppl7::FileObject &file, unsigned char id) const
{
	if (object_list.size()==0) return;
	std::map<uint32_t,Object *>::const_iterator it;
	size_t buffersize=0;
	for (it=object_list.begin();it!=object_list.end();++it) {
		Object *object=it->second;
		buffersize+=object->save_size+1;
	}
	unsigned char *buffer=(unsigned char*)malloc(buffersize+5);
	ppl7::Poke32(buffer+0,0);
	ppl7::Poke8(buffer+4,id);
	size_t p=5;
	for (it=object_list.begin();it!=object_list.end();++it) {
		Object *object=it->second;
		ppl7::Poke8(buffer+p,object->save_size+1);
		size_t bytes_saved=object->save(buffer+p+1,object->save_size);
		if (bytes_saved==object->save_size && bytes_saved>0) {
			p+=object->save_size+1;
			//printf ("saved object %d with size %d\n",object->id, object->save_size+1);
		}
	}
	ppl7::Poke32(buffer+0,p);
	file.write(buffer,p);
	free(buffer);
}

void ObjectSystem::load(const ppl7::ByteArrayPtr &ba)
{
	clear();
	size_t p=0;
	const unsigned char *buffer=(const unsigned char*)ba.toCharPtr();
	//printf("chunk size=%zd\n",ba.size());
	while (p<ba.size()) {
		int save_size=ppl7::Peek8(buffer+p);
		int type=ppl7::Peek8(buffer+p+1);
		Object *object=getInstance(type);
		if (object) {
			if (object->load(buffer+p+1,save_size-1)) {
				if (object->id>=nextid) nextid=object->id+1;
				if (object->sprite_set<Spriteset::MaxSpritesets && this->spriteset[object->sprite_set]!=NULL) {
					object->texture=this->spriteset[object->sprite_set];
					object->updateBoundary();
				}
				object_list.insert(std::pair<uint32_t,Object*>(object->id,object));
				//printf ("found object of type %d with size %d\n",type,save_size);
			}
		}
		p+=save_size;
	}
}


size_t ObjectSystem::count() const
{
	return object_list.size();
}

size_t ObjectSystem::countVisible() const
{
	return visible_object_map.size();
}


}	// EOF namespace Decker::Objects
