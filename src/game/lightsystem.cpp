#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "decker.h"
//#include "light.h"


LightSystem::Light::Light()
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
    lightsystem=NULL;
    type=LightType::Static;
}

LightSystem::LightSystem(const ColorPalette& palette)
    : palette(palette)
{
    bLightsVisible=true;
    spriteset=NULL;
    maxid=0;
}

LightSystem::~LightSystem()
{

}

void LightSystem::setVisible(bool visible)
{
    bLightsVisible=visible;
}

bool LightSystem::isVisible() const
{
    return bLightsVisible;
}

void LightSystem::clear()
{
    light_list.clear();
    maxid=0;
}

void LightSystem::setSpriteset(SpriteTexture* spriteset, SpriteTexture* objects)
{
    this->spriteset=spriteset;
    this->tex_object=objects;
}

void LightSystem::addLight(int x, int y, int sprite_no, float scale_x, float scale_y, float angle, uint8_t color_index, uint8_t intensity, LightType type)
{
    //printf ("x=%d, y=%d\n",x,y);
    LightSystem::Light item;
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
    item.type=type;
    item.boundary=spriteset->spriteBoundary(sprite_no, scale_x, x, y);
    light_list.insert(std::pair<int, LightSystem::Light>(item.id, item));

}

void LightSystem::updateVisibleLightList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
    if (!bLightsVisible) return;
    //ppl7::PrintDebugTime("LightSystem::updateVisibleLightList %zd\n", light_list.size());
    visible_lights_map.clear();
    std::map<int, LightSystem::Light>::const_iterator it;
    int width=viewport.width();
    int height=viewport.height();
    for (it=light_list.begin();it != light_list.end();++it) {
        const LightSystem::Light& item=(it->second);
        int x=item.x - worldcoords.x;
        int y=item.y - worldcoords.y;
        //ppl7::PrintDebugTime("found light at %d:%d, ", item.x, item.y);
        if (x + item.boundary.width() > 0 && y + item.boundary.height() > 0
            && x - item.boundary.width() < width && y - item.boundary.height() < height) {
            uint32_t id=(uint32_t)(((uint32_t)item.y & 0xffff) << 16) | (uint32_t)((uint32_t)item.x & 0xffff);
            visible_lights_map.insert(std::pair<uint32_t, const LightSystem::Light&>(id, item));
            //ppl7::PrintDebugTime("adding to visible_lights_map\n");
        } else {
            //ppl7::PrintDebugTime("but we ignore it! bw=%d, bh=%d\n", item.boundary.width(), item.boundary.height());
            uint32_t id=(uint32_t)(((uint32_t)item.y & 0xffff) << 16) | (uint32_t)((uint32_t)item.x & 0xffff);
            visible_lights_map.insert(std::pair<uint32_t, const LightSystem::Light&>(id, item));
        }


    }

}

size_t LightSystem::count() const
{
    return light_list.size();
}

size_t LightSystem::countVisible() const
{
    return visible_lights_map.size();
}



void LightSystem::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
    if (!bLightsVisible) return;
    std::map<uint32_t, const LightSystem::Light&>::const_iterator it;
    for (it=visible_lights_map.begin();it != visible_lights_map.end();++it) {
        const LightSystem::Light& item=(it->second);
        ppl7::grafix::Color c=palette.getColor(item.color_index);
        c.setAlpha(item.intensity);
        spriteset->drawScaledWithAngle(renderer,
            item.x + viewport.x1 - worldcoords.x,
            item.y + viewport.y1 - worldcoords.y,
            item.sprite_no, item.scale_x, item.scale_y, item.angle,
            c);
    }
}

void LightSystem::drawObjects(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
    if (!bLightsVisible) return;
    std::map<uint32_t, const LightSystem::Light&>::const_iterator it;
    for (it=visible_lights_map.begin();it != visible_lights_map.end();++it) {
        const LightSystem::Light& item=(it->second);
        tex_object->draw(renderer,
            item.x + viewport.x1 - worldcoords.x,
            item.y + viewport.y1 - worldcoords.y,
            1);
    }
}

void LightSystem::drawSelectedLightOutline(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id)
{
    if (!bLightsVisible) return;
    std::map<int, LightSystem::Light>::const_iterator it;
    it=light_list.find(id);
    if (it != light_list.end()) {
        const LightSystem::Light& item=(it->second);
        spriteset->drawOutlinesWithAngle(renderer,
            item.x + viewport.x1 - worldcoords.x,
            item.y + viewport.y1 - worldcoords.y,
            item.sprite_no, item.scale_x, item.scale_y, item.angle);

    }
}

void LightSystem::drawSelectedLightObject(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id)
{
    if (!bLightsVisible) return;
    std::map<int, LightSystem::Light>::const_iterator it;
    it=light_list.find(id);
    if (it != light_list.end()) {
        const LightSystem::Light& item=(it->second);
        tex_object->draw(renderer,
            item.x + viewport.x1 - worldcoords.x,
            item.y + viewport.y1 - worldcoords.y,
            0);

    }
}


void LightSystem::deleteLight(int id)
{
    std::map<int, LightSystem::Light>::const_iterator it;
    it=light_list.find(id);
    if (it != light_list.end()) {
        light_list.erase(it);
    }
}

void LightSystem::modifyLight(const LightSystem::Light& item)
{
    std::map<int, LightSystem::Light>::iterator it;
    it=light_list.find(item.id);
    if (it != light_list.end()) {
        LightSystem::Light& intitem=(it->second);
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


bool LightSystem::findMatchingLight(const ppl7::grafix::Point& p, LightSystem::Light& light) const
{
    bool found_match=false;
    light.id=-1;
    if (!bLightsVisible) return false;
    //printf ("Try to find sprite\n");
    std::map<uint32_t, const LightSystem::Light&>::const_iterator it;
    for (it=visible_lights_map.begin();it != visible_lights_map.end();++it) {
        const LightSystem::Light& item=(it->second);
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

void LightSystem::save(ppl7::FileObject& file, unsigned char id) const
{
    if (light_list.size() == 0) return;
    unsigned char* buffer=(unsigned char*)malloc(light_list.size() * 21 + 6);
    ppl7::Poke32(buffer + 0, 0);
    ppl7::Poke8(buffer + 4, id);
    ppl7::Poke8(buffer + 5, 1);		// Version
    size_t p=6;
    std::map<int, LightSystem::Light>::const_iterator it;
    for (it=light_list.begin();it != light_list.end();++it) {
        const LightSystem::Light& item=(it->second);
        ppl7::Poke16(buffer + p, item.x);
        ppl7::Poke16(buffer + p + 2, item.y);
        ppl7::PokeFloat(buffer + p + 4, item.scale_x);
        ppl7::PokeFloat(buffer + p + 8, item.scale_y);
        ppl7::PokeFloat(buffer + p + 12, item.angle);
        ppl7::Poke16(buffer + p + 16, item.sprite_no);
        ppl7::Poke8(buffer + p + 18, item.color_index);
        ppl7::Poke8(buffer + p + 19, item.intensity);
        ppl7::Poke8(buffer + p + 20, static_cast<int>(item.type));
        p+=21;
    }
    ppl7::Poke32(buffer + 0, p);
    file.write(buffer, p);
    free(buffer);
}

void LightSystem::load(const ppl7::ByteArrayPtr& ba)
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
        printf("Can't load LightSystem, unknown version! [%d]\n", version);

    }
}
