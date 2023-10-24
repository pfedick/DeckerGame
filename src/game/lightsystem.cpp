#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "decker.h"
//#include "light.h"


LightObject::LightObject()
{
    id=0;
    x=0;
    y=0;
    sprite_no=0;
    scale_x=1.0f;
    scale_y=1.0f;
    angle=0.0f;
    color_index=0;
    intensity=255;
    flare_intensity=0;
    //lightsystem=NULL;
    myType=LightType::Static;
    enabled=true;
    initial_state=true;
    plane=static_cast<int>(LightPlaneId::Player);
    playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Player);
    has_lensflare=false;
    save_size=33;
}

LightObject::~LightObject()
{

}

size_t LightObject::save(unsigned char* buffer, size_t size) const
{
    if (size < save_size) return 0;
    ppl7::Poke8(buffer + 0, 1);	// Object-Header-Version
    ppl7::Poke8(buffer + 1, static_cast<int>(myType));
    ppl7::Poke32(buffer + 2, id);
    ppl7::Poke8(buffer + 6, plane);
    int flags=0;
    if (initial_state) flags|=1;
    if (has_lensflare) flags|=2;
    ppl7::Poke8(buffer + 7, flags);
    ppl7::Poke8(buffer + 8, color_index);
    ppl7::Poke8(buffer + 9, intensity);
    ppl7::Poke8(buffer + 10, flare_intensity);
    ppl7::Poke32(buffer + 11, x);
    ppl7::Poke32(buffer + 15, y);
    ppl7::PokeFloat(buffer + 19, scale_x);
    ppl7::PokeFloat(buffer + 23, scale_y);
    ppl7::PokeFloat(buffer + 27, angle);
    ppl7::Poke16(buffer + 31, sprite_no);
    return 33;
}

size_t LightObject::load(const unsigned char* buffer, size_t size)
{
    if (size < 33) return 0;
    int version=ppl7::Peek8(buffer + 0);
    if (version == 1) {
        myType=static_cast<LightType>(ppl7::Peek8(buffer + 1));
        id=ppl7::Peek32(buffer + 2);
        plane=ppl7::Peek8(buffer + 6);
        int flags=ppl7::Peek8(buffer + 7);
        initial_state=flags & 1;
        has_lensflare=flags & 2;
        enabled=initial_state;
        color_index=ppl7::Peek8(buffer + 8);
        intensity=ppl7::Peek8(buffer + 9);
        flare_intensity=ppl7::Peek8(buffer + 10);
        x=ppl7::Peek32(buffer + 11);
        y=ppl7::Peek32(buffer + 15);
        scale_x=ppl7::PeekFloat(buffer + 19);
        scale_y=ppl7::PeekFloat(buffer + 23);
        angle=ppl7::PeekFloat(buffer + 27);
        sprite_no=ppl7::Peek16(buffer + 31);
        return 33;
    }
    return 0;
}

LightLayer::LightLayer(const ColorPalette& palette)
    : palette(palette)
{
    bLightsVisible=true;
    spriteset=NULL;
    maxid=0;
}

LightLayer::~LightLayer()
{

}

void LightLayer::setVisible(bool visible)
{
    bLightsVisible=visible;
}

bool LightLayer::isVisible() const
{
    return bLightsVisible;
}

void LightLayer::clear()
{
    light_list.clear();
    maxid=0;
}

void LightLayer::setSpriteset(SpriteTexture* spriteset, SpriteTexture* objects)
{
    this->spriteset=spriteset;
    this->tex_object=objects;
}

void LightLayer::addLight(int x, int y, int sprite_no, float scale_x, float scale_y, float angle, uint8_t color_index, uint8_t intensity, LightType type)
{
    //printf ("x=%d, y=%d\n",x,y);
    LightObject item;
    maxid++;
    item.id=maxid;
    item.lightsystem=this;
    item.x=x;
    item.y=y;
    item.sprite_no=sprite_no;
    item.scale_x=scale_x;
    item.scale_y=scale_y;
    item.angle=angle;
    item.intensity=intensity;
    item.color_index=color_index;
    item.myType=type;
    item.boundary=spriteset->spriteBoundary(sprite_no, scale_x, x, y);
    light_list.insert(std::pair<int, LightObject>(item.id, item));

}

void LightLayer::updateVisibleLightList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
    if (!bLightsVisible) return;
    //ppl7::PrintDebugTime("LightLayer::updateVisibleLightList %zd\n", light_list.size());
    visible_lights_map.clear();
    std::map<int, LightObject>::const_iterator it;
    int width=viewport.width();
    int height=viewport.height();
    for (it=light_list.begin();it != light_list.end();++it) {
        const LightObject& item=(it->second);
        int x=item.x - worldcoords.x;
        int y=item.y - worldcoords.y;
        //ppl7::PrintDebugTime("found light at %d:%d, ", item.x, item.y);
        if (x + item.boundary.width() > 0 && y + item.boundary.height() > 0
            && x - item.boundary.width() < width && y - item.boundary.height() < height) {
            uint32_t id=(uint32_t)(((uint32_t)item.y & 0xffff) << 16) | (uint32_t)((uint32_t)item.x & 0xffff);
            visible_lights_map.insert(std::pair<uint32_t, const LightObject&>(id, item));
            //ppl7::PrintDebugTime("adding to visible_lights_map\n");
        }
    }
}

size_t LightLayer::count() const
{
    return light_list.size();
}

size_t LightLayer::countVisible() const
{
    return visible_lights_map.size();
}



void LightLayer::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
    if (!bLightsVisible) return;
    std::map<uint32_t, const LightObject&>::const_iterator it;
    for (it=visible_lights_map.begin();it != visible_lights_map.end();++it) {
        const LightObject& item=(it->second);
        ppl7::grafix::Color c=palette.getColor(item.color_index);
        c.setAlpha(item.intensity);
        spriteset->drawScaledWithAngle(renderer,
            item.x + viewport.x1 - worldcoords.x,
            item.y + viewport.y1 - worldcoords.y,
            item.sprite_no, item.scale_x, item.scale_y, item.angle,
            c);
    }
}

void LightLayer::drawObjects(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
    if (!bLightsVisible) return;
    std::map<uint32_t, const LightObject&>::const_iterator it;
    for (it=visible_lights_map.begin();it != visible_lights_map.end();++it) {
        const LightObject& item=(it->second);
        tex_object->draw(renderer,
            item.x + viewport.x1 - worldcoords.x,
            item.y + viewport.y1 - worldcoords.y,
            1);
    }
}

void LightLayer::drawSelectedLightOutline(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id)
{
    if (!bLightsVisible) return;
    std::map<int, LightObject>::const_iterator it;
    it=light_list.find(id);
    if (it != light_list.end()) {
        const LightObject& item=(it->second);
        spriteset->drawOutlinesWithAngle(renderer,
            item.x + viewport.x1 - worldcoords.x,
            item.y + viewport.y1 - worldcoords.y,
            item.sprite_no, item.scale_x, item.scale_y, item.angle);

    }
}

void LightLayer::drawSelectedLightObject(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id)
{
    if (!bLightsVisible) return;
    std::map<int, LightObject>::const_iterator it;
    it=light_list.find(id);
    if (it != light_list.end()) {
        const LightObject& item=(it->second);
        tex_object->draw(renderer,
            item.x + viewport.x1 - worldcoords.x,
            item.y + viewport.y1 - worldcoords.y,
            0);

    }
}


void LightLayer::deleteLight(int id)
{
    std::map<int, LightObject>::const_iterator it;
    it=light_list.find(id);
    if (it != light_list.end()) {
        light_list.erase(it);
    }
}

void LightLayer::modifyLight(const LightObject& item)
{
    std::map<int, LightObject>::iterator it;
    it=light_list.find(item.id);
    if (it != light_list.end()) {
        LightObject& intitem=(it->second);
        intitem.x=item.x;
        intitem.y=item.y;
        intitem.scale_x=item.scale_x;
        intitem.scale_y=item.scale_y;
        intitem.angle=item.angle;
        intitem.intensity=item.intensity;
        intitem.color_index=item.color_index;
        intitem.boundary=spriteset->spriteBoundary(intitem.sprite_no,
            intitem.scale_x, intitem.x, intitem.y);
    }
}


bool LightLayer::findMatchingLight(const ppl7::grafix::Point& p, LightObject& light) const
{
    bool found_match=false;
    light.id=-1;
    if (!bLightsVisible) return false;
    //printf ("Try to find sprite\n");
    std::map<uint32_t, const LightObject&>::const_iterator it;
    for (it=visible_lights_map.begin();it != visible_lights_map.end();++it) {
        const LightObject& item=(it->second);
        if (p.x > item.x - 20 && p.x<item.x + 20 && p.y>item.y - 20 && p.y < item.y + 20) {
            //ppl7::PrintDebug("possible match: %d\n", item.id);
            ppl7::grafix::Rect objectboundary=tex_object->spriteBoundary(1, 1.0f, item.x, item.y);
            const ppl7::grafix::Drawable draw=tex_object->getDrawable(1);
            if (draw.width()) {
                int x=p.x - objectboundary.x1;
                int y=p.y - objectboundary.y1;
                ppl7::grafix::Color c=draw.getPixel(x, y);
                if (c.alpha() > 40) {
                    light=item;
                    found_match=true;
                    //ppl7::PrintDebug("Bingo: %d\n", item.id);
                }
            }
        }

    }
    return found_match;
}

void LightLayer::save(ppl7::FileObject& file, unsigned char id) const
{
    if (light_list.size() == 0) return;
    unsigned char* buffer=(unsigned char*)malloc(light_list.size() * 21 + 6);
    ppl7::Poke32(buffer + 0, 0);
    ppl7::Poke8(buffer + 4, id);
    ppl7::Poke8(buffer + 5, 1);		// Version
    size_t p=6;
    std::map<int, LightObject>::const_iterator it;
    for (it=light_list.begin();it != light_list.end();++it) {
        const LightObject& item=(it->second);
        ppl7::Poke16(buffer + p, item.x);
        ppl7::Poke16(buffer + p + 2, item.y);
        ppl7::PokeFloat(buffer + p + 4, item.scale_x);
        ppl7::PokeFloat(buffer + p + 8, item.scale_y);
        ppl7::PokeFloat(buffer + p + 12, item.angle);
        ppl7::Poke16(buffer + p + 16, item.sprite_no);
        ppl7::Poke8(buffer + p + 18, item.color_index);
        ppl7::Poke8(buffer + p + 19, item.intensity);
        ppl7::Poke8(buffer + p + 20, static_cast<int>(item.myType));
        p+=21;
    }
    ppl7::Poke32(buffer + 0, p);
    file.write(buffer, p);
    free(buffer);
}

void LightLayer::load(const ppl7::ByteArrayPtr& ba)
{
    clear();
    const char* buffer=ba.toCharPtr();
    int version=ppl7::Peek8(buffer);
    size_t p=1;
    if (version == 1) {
        while (p < ba.size()) {
            addLight(ppl7::Peek16(buffer + p),
                ppl7::Peek16(buffer + p + 2),
                ppl7::Peek16(buffer + p + 16),
                ppl7::PeekFloat(buffer + p + 4),
                ppl7::PeekFloat(buffer + p + 8),
                ppl7::PeekFloat(buffer + p + 12),
                ppl7::Peek8(buffer + p + 18),
                ppl7::Peek8(buffer + p + 19),
                static_cast<LightType>(ppl7::Peek8(buffer + p + 20)));
            p+=21;
        }
    } else {
        printf("Can't load LightLayer, unknown version! [%d]\n", version);

    }
}


LightSystem::LightSystem()
{
    lensflares=new SpriteTexture();
    light_objects=new SpriteTexture();
    lightmaps=new SpriteTexture();
    nextid=1;
}

LightSystem::~LightSystem()
{
    clear();
    delete lensflares;
    delete light_objects;
    delete lightmaps;
}

void LightSystem::loadSpritesets(SDL& sdl)
{
    lightmaps->enableOutlines(true);
    lightmaps->enableMemoryBuffer(true);
    lightmaps->setTextureBlendMode(SDL_BLENDMODE_ADD);
    lightmaps->load(sdl, "res/lightmaps.tex");
    lightmaps->setPivot(8, 256, 27);
    lightmaps->setPivot(9, 256, 27);
    lightmaps->setPivot(10, 256, 27);
    lightmaps->setPivot(11, 256, 27);
    lightmaps->setPivot(12, 256, 27);
    lightmaps->setPivot(13, 256, 27);

    light_objects->enableOutlines(true);
    light_objects->enableMemoryBuffer(true);
    light_objects->load(sdl, "res/lightobjects.tex");

    lensflares->enableOutlines(false);
    lensflares->enableMemoryBuffer(true);
    lensflares->load(sdl, "res/lensflares.tex");

}


void LightSystem::clear()
{
    std::map<uint32_t, LightObject*>::const_iterator it;
    for (it=light_map.begin();it != light_map.end();++it) {
        delete it->second;
    }
    light_map.clear();
    visible_light_map[static_cast<int>(LightPlaneId::Near)].clear();
    visible_light_map[static_cast<int>(LightPlaneId::Player)].clear();
    visible_light_map[static_cast<int>(LightPlaneId::Middle)].clear();
    visible_light_map[static_cast<int>(LightPlaneId::Far)].clear();
    visible_light_map[static_cast<int>(LightPlaneId::Horizon)].clear();
    nextid=1;

}

void LightSystem::updateVisibleLightList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
    visible_light_map[static_cast<int>(LightPlaneId::Near)].clear();
    visible_light_map[static_cast<int>(LightPlaneId::Player)].clear();
    visible_light_map[static_cast<int>(LightPlaneId::Middle)].clear();
    visible_light_map[static_cast<int>(LightPlaneId::Far)].clear();
    visible_light_map[static_cast<int>(LightPlaneId::Horizon)].clear();

    std::map<uint32_t, LightObject*>::const_iterator it;
    //int width=viewport.width();
    //int height=viewport.height();
    for (it=light_map.begin();it != light_map.end();++it) {
        LightObject* item=(it->second);
        /* TODO: unsloved Bug!!!
        int x=item->x - worldcoords.x;
        int y=item->y - worldcoords.y;
        //ppl7::PrintDebugTime("found light at %d:%d, ", item.x, item.y);
        if (x + item->boundary.width() > 0 && y + item->boundary.height() > 0
            && x - item->boundary.width() < width && y - item->boundary.height() < height) {
            */
        addObjectLight(item);
        //}
    }
}


void LightSystem::addObjectLight(LightObject* light)
{
    uint32_t id=(uint32_t)(((uint32_t)light->y & 0xffff) << 16) | (uint32_t)((uint32_t)light->x & 0xffff);
    visible_light_map[static_cast<int>(light->plane)].insert(std::pair<uint32_t, LightObject*>(id, light));
}

void LightSystem::addLight(LightObject* light)
{
    light->boundary=lightmaps->spriteBoundary(light->sprite_no, light->scale_x, light->x, light->y);
    light->id=nextid;
    nextid++;
    light_map.insert(std::pair<uint32_t, LightObject*>(light->id, light));
}

LightObject* LightSystem::getLight(uint32_t light_id)
{
    std::map<uint32_t, LightObject*>::iterator it;
    it=light_map.find(light_id);
    if (it != light_map.end()) return it->second;
    return NULL;
}

void LightSystem::deleteLight(uint32_t light_id)
{
    std::map<uint32_t, LightObject*>::iterator it;
    it=light_map.find(light_id);
    if (it != light_map.end()) {
        delete it->second;
        light_map.erase(it);
    }
}

size_t LightSystem::count() const
{
    return light_map.size();
}


void LightSystem::save(ppl7::FileObject& file, unsigned char id) const
{
    if (light_map.size() == 0) return;
    std::map<uint32_t, LightObject*>::const_iterator it;
    size_t buffersize=0;
    for (it=light_map.begin();it != light_map.end();++it) {
        LightObject* object=it->second;
        buffersize+=object->save_size + 4;
    }
    unsigned char* buffer=(unsigned char*)malloc(buffersize + 5);
    ppl7::Poke32(buffer + 0, 0);
    ppl7::Poke8(buffer + 4, id);
    size_t p=5;
    for (it=light_map.begin();it != light_map.end();++it) {
        LightObject* object=it->second;
        ppl7::Poke32(buffer + p, object->save_size + 4);
        size_t bytes_saved=object->save(buffer + p + 4, object->save_size);
        if (bytes_saved == object->save_size && bytes_saved > 0) {
            p+=object->save_size + 4;
        }
    }
    ppl7::Poke32(buffer + 0, p);
    file.write(buffer, p);
    free(buffer);
}

void LightSystem::load(const ppl7::ByteArrayPtr& ba)
{
    clear();
    size_t p=0;
    const unsigned char* buffer=(const unsigned char*)ba.toCharPtr();
    while (p < ba.size()) {
        int save_size=ppl7::Peek32(buffer + p);
        //int type=ppl7::Peek16(buffer + p + 5);
        LightObject* light=new LightObject();
        if (light) {
            if (light->load(buffer + p + 4, save_size - 4)) {
                if (light->id >= nextid) nextid=light->id + 1;
                light->boundary=lightmaps->spriteBoundary(light->sprite_no, light->scale_x, light->x, light->y);
                light_map.insert(std::pair<uint32_t, LightObject*>(light->id, light));
            } else {
                delete light;
            }
        }
        p+=save_size;
    }
}


void LightSystem::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane, LightPlayerPlaneMatrix pplane) const
{

}

void LightSystem::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane, LightPlayerPlaneMatrix pplane) const
{

}

void LightSystem::drawLensFlares(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane, LightPlayerPlaneMatrix pplane) const
{

}
