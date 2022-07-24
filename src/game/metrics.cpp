#include <ppl7.h>
#include "decker.h"
#include <SDL.h>


Metrics::Timer::Timer()
{
    start_time=duration=0.0f;
    s=0;
    f=SDL_GetPerformanceFrequency();
}

void Metrics::Timer::start()
{
    //start_time=ppl7::GetMicrotime();
    s=SDL_GetPerformanceCounter();

}

void Metrics::Timer::stop()
{
    uint64_t d=(SDL_GetPerformanceCounter() - s);
    duration+=(double)d / (double)f;
    //duration+=(ppl7::GetMicrotime() - start_time);

}

void Metrics::Timer::clear()
{
    start_time=duration=0.0f;
    s=0;
}

double Metrics::Timer::get() const
{
    return duration;
}

void Metrics::Timer::addDuration(double d)
{
    duration+=d;
}


Metrics::Metrics()
{
    fps=0;
    total_sprites=0;
    visible_sprites=0;
    total_objects=0;
    visible_objects=0;
    total_particles=0;
    visible_particles=0;
    framecount=0;
}

void Metrics::clear()
{
    framecount=0;
    fps=0;
    total_sprites=0;
    visible_sprites=0;
    total_objects=0;
    visible_objects=0;
    total_particles=0;
    visible_particles=0;
    time_frame.clear();
    time_total.clear();
    time_draw_ui.clear();
    time_draw_world.clear();
    time_events.clear();
    time_update_sprites.clear();
    time_update_objects.clear();
    time_update_particles.clear();
    time_particle_thread.clear();
    time_draw_background.clear();
    time_draw_tsop.clear();
    time_sprites.clear();
    time_objects.clear();
    time_draw_particles.clear();
    time_plane.clear();
    time_misc.clear();
}

void Metrics::newFrame()
{
    framecount++;
}

Metrics Metrics::getAverage()
{
    Metrics m;
    if (framecount) {
        m.framecount=1;
        m.time_frame.duration=time_frame.get() / framecount;
        m.time_total.duration=time_total.get() / framecount;
        m.time_draw_ui.duration=time_draw_ui.get() / framecount;
        m.time_draw_world.duration=time_draw_world.get() / framecount;
        m.time_events.duration=time_events.get() / framecount;
        m.time_update_sprites.duration=time_update_sprites.get() / framecount;
        m.time_update_objects.duration=time_update_objects.get() / framecount;
        m.time_update_particles.duration=time_update_particles.get() / framecount;
        m.time_particle_thread.duration=time_particle_thread.get() / framecount;
        m.time_draw_background.duration=time_draw_background.get() / framecount;
        m.time_draw_tsop.duration=time_draw_tsop.get() / framecount;
        m.time_sprites.duration=time_sprites.get() / framecount;
        m.time_objects.duration=time_objects.get() / framecount;
        m.time_draw_particles.duration=time_draw_particles.get() / framecount;
        m.time_plane.duration=time_plane.get() / framecount;
        m.time_misc.duration=time_misc.get() / framecount;

        m.fps=fps / framecount;
        m.total_sprites=total_sprites / framecount;
        m.visible_sprites=visible_sprites / framecount;
        m.total_objects=total_objects / framecount;
        m.visible_objects=visible_objects / framecount;
        m.total_particles=total_particles / framecount;
        m.visible_particles=visible_particles / framecount;
    }
    return m;
}
