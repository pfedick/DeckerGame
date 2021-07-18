#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"

namespace Decker::Objects {

ObjectSystem::ObjectSystem()
{
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
	object->id=(uint32_t)object_list.size();
	if (object->sprite_set<Spriteset::MaxSpritesets && this->spriteset[object->sprite_set]!=NULL) {
		object->texture=this->spriteset[object->sprite_set];
		object->updateBoundary();
	}

	object_list.insert(std::pair<uint32_t,Object*>(object->id,object));
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

}

void ObjectSystem::draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	std::map<uint32_t,Object *>::const_iterator it;
	for (it=visible_object_map.begin();it!=visible_object_map.end();++it) {
		const Object *object=it->second;
		if (object->texture) {
			object->texture->drawScaled(renderer,
					object->p.x+viewport.x1-worldcoords.x,
					object->p.y+viewport.y1-worldcoords.y,
					object->sprite_no, object->scale);
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
					ppl7::grafix::Color c=draw.getPixel(x/item->scale, y/item->scale);
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
					item->sprite_no, item->scale);
		}
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
