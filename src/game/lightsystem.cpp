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
    save_size=34;
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
    ppl7::Poke8(buffer + 7, playerPlane);
    int flags=0;
    if (initial_state) flags|=1;
    if (has_lensflare) flags|=2;
    ppl7::Poke8(buffer + 8, flags);
    ppl7::Poke8(buffer + 9, color_index);
    ppl7::Poke8(buffer + 10, intensity);
    ppl7::Poke8(buffer + 11, flare_intensity);
    ppl7::Poke32(buffer + 12, x);
    ppl7::Poke32(buffer + 16, y);
    ppl7::PokeFloat(buffer + 20, scale_x);
    ppl7::PokeFloat(buffer + 24, scale_y);
    ppl7::PokeFloat(buffer + 28, angle);
    ppl7::Poke16(buffer + 32, sprite_no);
    return 34;
}

size_t LightObject::load(const unsigned char* buffer, size_t size)
{
    if (size < 34) return 0;
    int version=ppl7::Peek8(buffer + 0);
    if (version == 1) {
        myType=static_cast<LightType>(ppl7::Peek8(buffer + 1));
        id=ppl7::Peek32(buffer + 2);
        plane=ppl7::Peek8(buffer + 6);
        playerPlane=ppl7::Peek8(buffer + 7);
        int flags=ppl7::Peek8(buffer + 8);
        initial_state=flags & 1;
        has_lensflare=flags & 2;
        enabled=initial_state;
        color_index=ppl7::Peek8(buffer + 9);
        intensity=ppl7::Peek8(buffer + 10);
        flare_intensity=ppl7::Peek8(buffer + 11);
        x=ppl7::Peek32(buffer + 12);
        y=ppl7::Peek32(buffer + 16);
        scale_x=ppl7::PeekFloat(buffer + 20);
        scale_y=ppl7::PeekFloat(buffer + 24);
        angle=ppl7::PeekFloat(buffer + 28);
        sprite_no=ppl7::Peek16(buffer + 32);
        return 34;
    }
    return 0;
}


void LightSystem::loadLegacyLightLayer(const ppl7::ByteArrayPtr& ba, LightPlaneId plane, int pplane)
{
    const char* buffer=ba.toCharPtr();
    int version=ppl7::Peek8(buffer);
    size_t p=1;
    if (version == 1) {
        while (p < ba.size()) {
            LightObject* light=new LightObject();
            light->plane=static_cast<int>(plane);
            light->playerPlane=static_cast<int>(pplane);
            light->x=ppl7::Peek16(buffer + p);
            light->y=ppl7::Peek16(buffer + p + 2);
            light->sprite_no=ppl7::Peek16(buffer + p + 16);
            light->scale_x= ppl7::PeekFloat(buffer + p + 4);
            light->scale_y= ppl7::PeekFloat(buffer + p + 8);
            light->angle= ppl7::PeekFloat(buffer + p + 12);
            light->color_index=ppl7::Peek8(buffer + p + 18);
            light->intensity=ppl7::Peek8(buffer + p + 19);
            light->myType=static_cast<LightType>(ppl7::Peek8(buffer + p + 20));
            addLight(light);
            p+=21;
        }
    } else {
        printf("Can't load LightLayer, unknown version! [%d]\n", version);

    }
}


LightSystem::LightSystem(const ColorPalette& palette)
    : palette(palette)
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
    int width=viewport.width();
    int height=viewport.height();
    for (it=light_map.begin();it != light_map.end();++it) {
        LightObject* item=(it->second);

        int x=item->x - worldcoords.x * planeFactor[item->plane];
        int y=item->y - worldcoords.y * planeFactor[item->plane];
        //ppl7::PrintDebugTime("found light at %d:%d, ", item.x, item.y);
        if (x + item->boundary.width() > 0 && y + item->boundary.height() > 0
            && x - item->boundary.width() < width && y - item->boundary.height() < height) {

            //addObjectLight(item);
            uint32_t id=(uint32_t)(((uint32_t)item->y & 0xffff) << 16) | (uint32_t)((uint32_t)item->x & 0xffff);
            visible_light_map[static_cast<int>(item->plane)].insert(std::pair<uint32_t, LightObject*>(id, item));

        }
    }
}


void LightSystem::addObjectLight(LightObject* light)
{
    light->boundary=lightmaps->spriteBoundary(light->sprite_no, light->scale_x, light->x, light->y);
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

size_t LightSystem::countVisible() const
{
    size_t total=0;
    for (int i=0;i < static_cast<int>(LightPlaneId::Max);i++) total+=visible_light_map[i].size();
    return total;
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

void LightSystem::setVisible(LightPlaneId plane, bool visible)
{
    visibility[static_cast<int>(plane)]=visible;
}

void LightSystem::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane, LightPlayerPlaneMatrix pplane) const
{
    if (!visibility[static_cast<int>(plane)]) return;
    std::map<uint32_t, LightObject*>::const_iterator it;
    for (it=visible_light_map[static_cast<int>(plane)].begin();it != visible_light_map[static_cast<int>(plane)].end();++it) {
        const LightObject* item=(it->second);
        if (plane != LightPlaneId::Player || (item->playerPlane & static_cast<int>(pplane))) {
            ppl7::grafix::Color c=palette.getColor(item->color_index);
            c.setAlpha(item->intensity);
            lightmaps->drawScaledWithAngle(renderer,
                item->x + viewport.x1 - worldcoords.x,
                item->y + viewport.y1 - worldcoords.y,
                item->sprite_no, item->scale_x, item->scale_y, item->angle,
                c);
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

void LightSystem::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane) const
{
    if (!visibility[static_cast<int>(plane)]) return;
    std::map<uint32_t, LightObject*>::const_iterator it;
    for (it=visible_light_map[static_cast<int>(plane)].begin();it != visible_light_map[static_cast<int>(plane)].end();++it) {
        const LightObject* item=(it->second);
        if (static_cast<uint8_t>(plane) == item->plane) {
            int x=item->x + viewport.x1 - worldcoords.x;
            int y=item->y + viewport.y1 - worldcoords.y;
            light_objects->draw(renderer,
                x,
                y,
                1);
            drawId(renderer, light_objects, x, y, item->id);

        }
    }

}

void LightSystem::drawLensFlares(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane, LightPlayerPlaneMatrix pplane) const
{
    if (!visibility[static_cast<int>(plane)]) return;
    std::map<uint32_t, LightObject*>::const_iterator it;
    for (it=visible_light_map[static_cast<int>(plane)].begin();it != visible_light_map[static_cast<int>(plane)].end();++it) {
        const LightObject* item=(it->second);
        if (plane != LightPlaneId::Player || (item->playerPlane & static_cast<int>(pplane))) {
            if (item->has_lensflare) {
                if (plane == LightPlaneId::Player) {
                    if (pplane == LightPlayerPlaneMatrix::Player && item->playerPlane & 4) continue;
                }
                //ppl7::grafix::Color c=palette.getColor(item->color_index);
                ppl7::grafix::Color c(255, 255, 255, 255);
                int x=item->x + viewport.x1 - worldcoords.x;
                int y= item->y + viewport.y1 - worldcoords.y;
                float dist=ppl7::grafix::Distance(ppl7::grafix::Point(x, y), ppl7::grafix::Point(1920 / 2, 1080 / 2));
                if (dist < 1300) c.setAlpha(255 - (dist * 200.0f / 1300.0f));
                //ppl7::PrintDebugTime("distance: %0.3f\n", dist);
                int sprite=0;
                //if (item->flare_intensity != 255) sprite=item->flare_intensity;
                sprite=7 - (item->flare_intensity * 7 / 255);
                lensflares->draw(renderer, x, y, sprite, c);
                if (dist < 700) {
                    float v=1.0f - dist / 700.0f;
                    if (dist < 30) v=v * dist / 30;
                    v=v * item->flare_intensity / 255.0f;
                    int xd=1920 / 2 - x;
                    int yd=1080 / 2 - y;
                    lensflares->drawScaled(renderer, 960 - xd * 3 / 4, 540 - yd * 3 / 3, 8, 0.2f, ppl7::grafix::Color(128, 128, 0, 30.0f * v));
                    lensflares->drawScaled(renderer, 960 - xd * 2 / 4, 540 - yd * 2 / 3, 8, 0.3f, ppl7::grafix::Color(128, 128, 0, 40.0f * v));
                    lensflares->drawScaled(renderer, 960 - xd * 1 / 4, 540 - yd * 1 / 3, 8, 0.4f, ppl7::grafix::Color(128, 0, 0, 60.0f * v));
                    lensflares->drawScaled(renderer, 960, 540, 8, 0.3f, ppl7::grafix::Color(255, 255, 255, 50.0f * v));
                    lensflares->drawScaled(renderer, 960 + xd * 1 / 4, 540 + yd * 1 / 4, 8, 0.6f, ppl7::grafix::Color(255, 255, 255, 80.0f * v));
                    lensflares->drawScaled(renderer, 960 + xd * 2 / 4, 540 + yd * 2 / 4, 8, 0.8f, ppl7::grafix::Color(128, 64, 0, 30.0f * v));
                    lensflares->drawScaled(renderer, 960 + xd * 3 / 4, 540 + yd * 3 / 4, 8, 0.6f, ppl7::grafix::Color(32, 32, 192, 70.0f * v));
                    lensflares->drawScaled(renderer, 960 + xd * 4 / 4, 540 + yd * 4 / 4, 8, 1.0f, ppl7::grafix::Color(32, 192, 32, 30.0f * v));
                }


            }
        }
    }
}


void LightSystem::drawSelectedLight(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id)
{
    std::map<uint32_t, LightObject*>::const_iterator it;
    it=light_map.find(id);
    if (it == light_map.end()) return;
    const LightObject* item=it->second;
    if (!visibility[item->plane]) return;
    lightmaps->drawOutlinesWithAngle(renderer,
        item->x + viewport.x1 - worldcoords.x,
        item->y + viewport.y1 - worldcoords.y,
        item->sprite_no, item->scale_x, item->scale_y, item->angle);

    light_objects->draw(renderer,
        item->x + viewport.x1 - worldcoords.x,
        item->y + viewport.y1 - worldcoords.y,
        2);

}


LightObject* LightSystem::findMatchingLight(const ppl7::grafix::Point& p, LightPlaneId plane)
{
    if (!visibility[static_cast<int>(plane)]) return NULL;
    //printf ("Try to find sprite\n");
    LightObject* found_match=NULL;
    std::map<uint32_t, LightObject*>::iterator it;
    for (it=visible_light_map[static_cast<int>(plane)].begin();it != visible_light_map[static_cast<int>(plane)].end();++it) {
        LightObject* item=(it->second);
        if (p.x > item->x - 20 && p.x<item->x + 20 && p.y>item->y - 20 && p.y < item->y + 20) {
            //ppl7::PrintDebug("possible match: %d\n", item.id);
            ppl7::grafix::Rect objectboundary=light_objects->spriteBoundary(1, 1.0f, item->x, item->y);
            const ppl7::grafix::Drawable draw=light_objects->getDrawable(1);
            if (draw.width()) {
                int x=p.x - objectboundary.x1;
                int y=p.y - objectboundary.y1;
                ppl7::grafix::Color c=draw.getPixel(x, y);
                if (c.alpha() > 40) {
                    found_match=item;
                    //ppl7::PrintDebug("Bingo: %d\n", item.id);
                }
            }
        }

    }
    return found_match;
}
