#include <ppl7.h>
#include <ppl7-grafix.h>
#include <SDL.h>
#include "decker.h"
#include "particle.h"

static ParticleSystem* particle_system=NULL;

ParticleSystem* GetParticleSystem()
{
    return particle_system;
}


ParticleSystem::ParticleSystem()
{
    if (!particle_system) particle_system=this;
    for (int i=0;i < ParticleSpriteset::MaxSpritesets;i++) {
        spriteset[i]=new SpriteTexture();
    }
    nextid=1;
}

ParticleSystem::~ParticleSystem()
{
    clear();
    if (particle_system == this) particle_system=NULL;
}

void ParticleSystem::clear()
{
    for (int i=0;i < static_cast<int>(Particle::Layer::maxLayer);i++) {
        visible_particle_map[i].clear();
    }
    std::map<uint64_t, Particle*>::iterator it;
    for (it=particle_map.begin();it != particle_map.end();++it) {
        delete it->second;
    }
    particle_map.clear();
    nextid=1;
}

void ParticleSystem::loadSpritesets(SDL& sdl)
{
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    spriteset[ParticleSpriteset::GenericParticles]->enableOutlines(false);
    spriteset[ParticleSpriteset::GenericParticles]->enableMemoryBuffer(false);
    spriteset[ParticleSpriteset::GenericParticles]->load(sdl, "res/particles.tex");
}

void ParticleSystem::addParticle(Particle* particle)
{
    if (!particle) return;
    if (particle->birth_time == 0.0f || particle->birth_time > particle->death_time) return;
    particle->life_time=particle->death_time - particle->birth_time;
    particle_map.insert(std::pair<uint64_t, Particle*>(nextid, particle));
    nextid++;
}

void ParticleSystem::deleteParticle(uint64_t id)
{
    std::map<uint64_t, Particle*>::const_iterator it;
    it=particle_map.find(id);
    if (it != particle_map.end()) {
        Particle* particle=it->second;
        particle_map.erase(it);
        delete particle;
    }
}

void ParticleSystem::update(double time, TileTypePlane& ttplane, Player& player, const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport)
{
    std::list<uint64_t> deleteme;
    std::map<uint64_t, Particle*>::iterator it;
    for (int i=0;i < static_cast<int>(Particle::Layer::maxLayer);i++) {
        visible_particle_map[i].clear();
    }
    int width=viewport.width();
    int height=viewport.height();
    for (it=particle_map.begin();it != particle_map.end();++it) {
        Particle* particle=it->second;
        if (time > particle->death_time) {
            deleteme.push_back(it->first);
        } else {
            particle->age=(time - particle->birth_time) / particle->life_time;
            particle->update(time, ttplane);
            int x=(int)particle->p.x - worldcoords.x;
            int y=(int)particle->p.y - worldcoords.y;
            if (x + 64 > 0 && y + 64 > 0 && x - 64 < width && y - 64 < height) {
                uint32_t id=(uint32_t)(((uint32_t)particle->p.y & 0xffff) << 16) | (uint32_t)((uint32_t)particle->p.x & 0xffff);
                visible_particle_map[static_cast<int>(particle->layer)].insert(std::pair<uint32_t, Particle*>(id, particle));
            }
        }
    }
    if (deleteme.size() > 0) {
        std::list<uint64_t>::iterator it;
        for (it=deleteme.begin();it != deleteme.end();++it) {
            deleteParticle(*it);
        }
    }
}

void ParticleSystem::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Particle::Layer layer) const
{
    std::map<uint32_t, Particle*>::const_iterator it;
    ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
    int l=static_cast<int>(layer);

    for (it=visible_particle_map[l].begin();it != visible_particle_map[l].end();++it) {
        const Particle* particle=it->second;
        if (particle->layer == layer) {
            spriteset[particle->sprite_set]->drawScaled(renderer,
                particle->p.x + coords.x,
                particle->p.y + coords.y,
                particle->sprite_no, particle->scale, particle->color_mod);
        }
    }
}

size_t ParticleSystem::count() const
{
    return particle_map.size();
}

size_t ParticleSystem::countVisible() const
{
    size_t c=0;
    for (int i=0;i < static_cast<int>(Particle::Layer::maxLayer);i++) {
        c+=visible_particle_map[i].size();
    }
    return c;
}


ppl7::String ParticleSystem::layerName(Particle::Layer layer)
{
    switch (static_cast<int>(layer)) {

    }

    switch (layer) {
    case Particle::Layer::BehindBricks: return "BehindBricks";
    case Particle::Layer::BeforePlayer: return "BeforePlayer";
    case Particle::Layer::BehindPlayer: return "BehindPlayer";
    case Particle::Layer::BackplaneFront: return "BackplaneFront";
    case Particle::Layer::BackplaneBack: return "BackplaneBack";
    case Particle::Layer::FrontplaneFront: return "FrontplaneFront";
    case Particle::Layer::FrontplaneBack: return "FrontplaneBack";
    default: return "unknown";
    }
    return "unknown";
}
