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

Metrics::Timer& Metrics::Timer::operator +=(const Metrics::Timer& other)
{
    duration+=other.duration;
    return *this;
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
    total_audiotracks=0;
    hearable_audiotracks=0;
    framecount=0;
    frame_rate_compensation=0.0f;
    frametime=0.0f;
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
    total_audiotracks=0;
    hearable_audiotracks=0;

    frame_rate_compensation=0.0f;
    frametime=0.0f;
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
    time_audioengine.clear();
}

void Metrics::newFrame()
{
    framecount++;
}

Metrics Metrics::getAverage() const
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
        m.time_audioengine.duration=time_audioengine.get() / framecount;

        m.fps=fps / framecount;
        m.total_sprites=total_sprites / framecount;
        m.visible_sprites=visible_sprites / framecount;
        m.total_objects=total_objects / framecount;
        m.visible_objects=visible_objects / framecount;
        m.total_particles=total_particles / framecount;
        m.visible_particles=visible_particles / framecount;
        m.total_audiotracks=total_audiotracks / framecount;
        m.hearable_audiotracks=hearable_audiotracks / framecount;

        m.frame_rate_compensation=frame_rate_compensation / framecount;
        m.frametime=frametime / framecount;
    }
    return m;
}


Metrics& Metrics::operator+=(const Metrics& other)
{
    framecount+=other.framecount;
    fps+=other.fps;
    total_sprites+=other.total_sprites;
    visible_sprites+=other.visible_sprites;
    total_objects+=other.total_objects;
    visible_objects+=other.visible_objects;
    total_particles+=other.total_particles;
    visible_particles+=other.visible_particles;
    total_audiotracks+=other.total_audiotracks;
    hearable_audiotracks+=other.hearable_audiotracks;
    time_frame+=other.time_frame;
    time_total+=other.time_total;
    time_draw_ui+=other.time_draw_ui;
    time_draw_world+=other.time_draw_world;
    time_events+=other.time_events;
    time_update_sprites+=other.time_update_sprites;
    time_update_objects+=other.time_update_objects;
    time_update_particles+=other.time_update_particles;
    time_particle_thread+=other.time_particle_thread;
    time_draw_background+=other.time_draw_background;
    time_draw_tsop+=other.time_draw_tsop;
    time_sprites+=other.time_sprites;
    time_objects+=other.time_objects;
    time_draw_particles+=other.time_draw_particles;
    time_plane+=other.time_plane;
    time_misc+=other.time_misc;
    time_audioengine+=other.time_audioengine;
    frame_rate_compensation+=other.frame_rate_compensation;
    frametime+=other.frametime;
    return *this;
}


void Metrics::print() const
{
    printf("==================== METRICS DUMP ==============================\n");
    printf("Total Frames collected: %d\n", framecount);
    Metrics avg=getAverage();
    printf("fps: %d, Frametime: %0.3f ms, FPS-Compensation: %0.3f\n", avg.fps, avg.frametime * 1000.0f, avg.frame_rate_compensation);
    printf("Total Sprites:   %6zu, visible sprites:   %6zu\n", avg.total_sprites, avg.visible_sprites);
    printf("Total Objects:   %6zu, visible Objects:   %6zu\n", avg.total_objects, avg.visible_objects);
    printf("Total Particles: %6zu, visible Particles: %6zu\n", avg.total_particles, avg.visible_particles);
    printf("Total Tracks:    %6zu, hearable Tracks:   %6zu\n", avg.total_audiotracks, avg.hearable_audiotracks);
    printf("\n");
    printf("Timer total: %6.3f ms, Time Frame: %6.3f ms\n", avg.time_total.get() * 1000.0f, avg.time_frame.get() * 1000.0f);
    printf("  draw userinterface:     %6.3f ms\n", avg.time_draw_ui.get() * 1000.0f);
    printf("  handle events:          %6.3f ms\n", avg.time_events.get() * 1000.0f);
    printf("  misc:                   %6.3f ms\n", avg.time_misc.get() * 1000.0f);
    printf("  draw the world:         %6.3f ms\n", avg.time_draw_world.get() * 1000.0f);
    printf("    update sprites:       %6.3f ms\n", avg.time_update_sprites.get() * 1000.0f);
    printf("    update objects:       %6.3f ms\n", avg.time_update_objects.get() * 1000.0f);
    printf("    update particles:     %6.3f ms\n", avg.time_update_particles.get() * 1000.0f);
    printf("    draw background:      %6.3f ms\n", avg.time_draw_background.get() * 1000.0f);
    printf("    draw tiles:           %6.3f ms\n", avg.time_plane.get() * 1000.0f);
    printf("    draw sprites:         %6.3f ms\n", avg.time_sprites.get() * 1000.0f);
    printf("    draw objects:         %6.3f ms\n", avg.time_objects.get() * 1000.0f);
    printf("    draw particles:       %6.3f ms\n", avg.time_draw_particles.get() * 1000.0f);
    printf("  particle update thread: %6.3f ms\n", avg.time_particle_thread.get() * 1000.0f);
    printf("  audio engine thread:    %6.3f ms\n", avg.time_audioengine.get() * 1000.0f);

    fflush(stdout);
}
