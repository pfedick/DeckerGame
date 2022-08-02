#ifndef INCLUDE_METRICS_H_
#define INCLUDE_METRICS_H_

#include <ppl7.h>

class Metrics
{
private:
    int framecount;
public:

    class Timer
    {
        friend class Metrics;
    private:
        double start_time;
        double duration;
        uint64_t s;
        uint64_t f;

    public:
        Timer();
        void start();
        void stop();
        void clear();
        double get() const;
        void addDuration(double d);

        Timer& operator +=(const Timer& other);
    };
    Metrics();
    void clear();
    void newFrame();
    void print() const;
    Metrics getAverage() const;
    Metrics& operator +=(const Metrics& other);

    Timer time_frame;
    Timer time_total;
    Timer time_draw_ui;
    Timer time_draw_world;
    Timer time_events;
    Timer time_update_sprites;
    Timer time_update_objects;
    Timer time_update_particles;
    Timer time_particle_thread;
    Timer time_draw_background;
    Timer time_draw_tsop;
    Timer time_sprites;
    Timer time_objects;
    Timer time_draw_particles;
    Timer time_plane;
    Timer time_misc;


    int fps;
    size_t total_sprites;
    size_t visible_sprites;
    size_t total_objects;
    size_t visible_objects;
    size_t total_particles;
    size_t visible_particles;

};


#endif
