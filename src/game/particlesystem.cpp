#include <ppl7.h>
#include <ppl7-grafix.h>
#include <SDL.h>
#include "decker.h"
#include "particle.h"


//#define DEBUGOUT

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
    active_map=0;
    update_thread.ps=this;
    update_thread.threadStart();
}

ParticleSystem::~ParticleSystem()
{
    update_thread.mutex.signal();
    update_thread.threadStop();
    clear();
    for (int i=0;i < ParticleSpriteset::MaxSpritesets;i++) {
        delete spriteset[i];
    }

    if (particle_system == this) particle_system=NULL;
}

void ParticleSystem::clear()
{
    waitForUpdateThreadFinished();
    for (int i=0;i < static_cast<int>(Particle::Layer::maxLayer);i++) {
        visible_particle_map[0][i].clear();
        visible_particle_map[1][i].clear();
    }
    std::map<uint64_t, Particle*>::iterator it;
    for (it=particle_map.begin();it != particle_map.end();++it) {
        delete it->second;
    }
    new_particles.clear();
    particles_to_delete.clear();
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
    new_particles.insert(std::pair<uint64_t, Particle*>(nextid, particle));
    nextid++;
}

void ParticleSystem::deleteParticle(uint64_t id)
{
    std::map<uint64_t, Particle*>::const_iterator it;
    it=particle_map.find(id);
    if (it != particle_map.end()) {
        Particle* particle=it->second;
        particle_map.erase(it);
        particle->sprite_set=1234567;
        delete particle;
    }
}

void ParticleSystem::update(double time, TileTypePlane& ttplane, Player& player, const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport, float frame_rate_compensation)
{
    if (update_thread.isRunning()) {
        ppl7::PrintDebugTime("Particle Update Thread too slow!\n");
        while (update_thread.isRunning()) ppl7::MSleep(1);
    }
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::update: started, map=%d\n", ppl7::ThreadID(), active_map);
#endif
    cleanupParticles();
    update_thread.frame_rate_compensation=frame_rate_compensation;
    update_thread.time=time;
    update_thread.ttplane=&ttplane;
    update_thread.player=&player;
    update_thread.worldcoords.x=worldcoords.x;
    update_thread.worldcoords.y=worldcoords.y;
    update_thread.viewport=viewport;
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::update: set visible map to %d\n", ppl7::ThreadID(), active_map);
#endif
    update_thread.setVisibleParticleMapAndContinue(visible_particle_map[active_map]);
    active_map=(active_map + 1) & 1;
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::update:  ended, draw on map: %d, send signal to ParticleUpdateThread\n", ppl7::ThreadID(), active_map);
#endif

}

double ParticleSystem::waitForUpdateThreadFinished()
{
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::waitForUpdateThreadFinished\n", ppl7::ThreadID());
#endif
    while (update_thread.isRunning()) ppl7::MSleep(1);
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::waitForUpdateThreadFinished => OK\n", ppl7::ThreadID());
#endif

    return update_thread.getThreadDuration();
}

void ParticleSystem::cleanupParticles()
{
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::cleanupParticles, particles to delete: %zd, insert: %zd\n", ppl7::ThreadID(), particles_to_delete.size(), new_particles.size());
#endif
    if (particles_to_delete.size() > 0) {
        //ppl7::PrintDebugTime("deleting %zd particles\n", particles_to_delete.size());
        std::list<uint64_t>::iterator dit;
        for (dit=particles_to_delete.begin();dit != particles_to_delete.end();++dit) {
            deleteParticle(*dit);
        }
        particles_to_delete.clear();
    }
    // Insert new Particles
    std::map<uint64_t, Particle*>::const_iterator it;
    for (it=new_particles.begin();it != new_particles.end();++it) {
        particle_map.insert(std::pair<uint64_t, Particle*>(it->first, it->second));
    }
    new_particles.clear();
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::cleanupParticles => DONE\n", ppl7::ThreadID());
#endif

}


void ParticleSystem::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Particle::Layer layer) const
{
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::draw => DONE\n", ppl7::ThreadID());
#endif
    std::map<uint32_t, Particle*>::const_iterator it;
    ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
    int l=static_cast<int>(layer);
    //ppl7::PrintDebugTime("   draw with active_map=%d\n", active_map);

    for (it=visible_particle_map[active_map][l].begin();it != visible_particle_map[active_map][l].end();++it) {
        const Particle* particle=it->second;
        if (particle->sprite_set <= 2) {
            spriteset[particle->sprite_set]->drawScaled(renderer,
                particle->p.x + coords.x,
                particle->p.y + coords.y,
                particle->sprite_no, particle->scale, particle->color_mod);

        } else {
            ppl7::PrintDebugTime("[%llu] Found invalid particle!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", ppl7::ThreadID());
        }
    }
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleSystem::draw => DONE\n", ppl7::ThreadID());
#endif

}

size_t ParticleSystem::count() const
{
    return particle_map.size();
}

size_t ParticleSystem::countVisible() const
{
    size_t c=0;
    for (int i=0;i < static_cast<int>(Particle::Layer::maxLayer);i++) {
        c+=visible_particle_map[active_map][i].size();
    }
    return c;
}


ppl7::String ParticleSystem::layerName(Particle::Layer layer)
{
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


ParticleUpdateThread::ParticleUpdateThread()
{
    ps=NULL;
    time=0.0f;
    ttplane=NULL;
    player=NULL;
    visible_particle_map=NULL;
    frame_rate_compensation=1.0f;
    thread_duration=0.0f;
    thread_running=false;
}

double ParticleUpdateThread::getThreadDuration() const
{
    return thread_duration;
}

void ParticleUpdateThread::setVisibleParticleMapAndContinue(std::map<uint32_t, Particle*>* visible_particle_map)
{
    datamutex.lock();
    this->visible_particle_map=visible_particle_map;
    for (int i=0;i < static_cast<int>(Particle::Layer::maxLayer);i++) {
        this->visible_particle_map[i].clear();
    }
    thread_running=true;
    datamutex.unlock();
    mutex.signal();
}

bool ParticleUpdateThread::isRunning() const
{
    return thread_running;
}

void ParticleUpdateThread::run()
{
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleUpdateThread STARTED\n", ppl7::ThreadID());
#endif
    thread_running=false;
    while (!threadShouldStop()) {
#ifdef DEBUGOUT
        ppl7::PrintDebugTime("[%llu] ParticleUpdateThread waiting for signal\n", ppl7::ThreadID());
#endif

        mutex.wait();
        datamutex.lock();
        thread_running=true;
#ifdef DEBUGOUT
        ppl7::PrintDebugTime("[%llu] ParticleUpdateThread running\n", ppl7::ThreadID());
#endif

        double thread_start_time=ppl7::GetMicrotime();
        if (visible_particle_map) {
            float left=worldcoords.x - 64;
            float top=worldcoords.y - 64;
            float right=worldcoords.x + viewport.width() + 64;
            float bottom=worldcoords.y + viewport.height() + 64;
            std::map<uint64_t, Particle*>::iterator it;
            for (it=ps->particle_map.begin();it != ps->particle_map.end();++it) {
                Particle* particle=it->second;
                if (time <= particle->death_time) {
                    particle->age=(time - particle->birth_time) / particle->life_time; // Rises from 0.0f to 1.0f
                    particle->update(time, frame_rate_compensation);
                    if (particle->p.x > left && particle->p.y > top && particle->p.x < right && particle->p.y < bottom) {
                        uint32_t id=(uint32_t)(((uint32_t)particle->p.y & 0xffff) << 16) | (uint32_t)((uint32_t)particle->p.x & 0xffff);
                        visible_particle_map[static_cast<int>(particle->layer)].insert(std::pair<uint32_t, Particle*>(id, particle));
                        particle->visible=true;
                    } else {
                        particle->visible=false;
                    }
                } else {
                    ps->particles_to_delete.push_back(it->first);
                }
            }
        }
        thread_duration=ppl7::GetMicrotime() - thread_start_time;
        thread_running=false;
        datamutex.unlock();
        //ppl7::PrintDebugTime("    ParticleUpdateThread: sleep\n");

        //printf("ParticleUpdateThread signaled\n");
    }
#ifdef DEBUGOUT
    ppl7::PrintDebugTime("[%llu] ParticleUpdateThread ENDED\n", ppl7::ThreadID());
#endif

}
