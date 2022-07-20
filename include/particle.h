#ifndef INCLUDE_PARTICLE_H_
#define INCLUDE_PARTICLE_H_

#include <map>
#include <list>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "animation.h"
class TileTypePlane;
class Player;
class SDL;
struct SDL_Renderer;
class SpriteTexture;

class Particle
{
    friend class ParticleEmitter;
public:
    class Velocity
    {
    public:
        Velocity() {
            x=0.0f;
            y=0.0f;
        }
        float x;
        float y;
    };
    class ScaleGradientItem
    {
    public:
        float age;
        float time;
    };
    class ColorGradientItem
    {
    public:
        float age;
        ppl7::grafix::Color color;
    };


private:
    double next_animation;
    double death_time;
    AnimationCycle animation;
    Velocity velocity;
    Velocity pf;
    float weight;
    float direction;
    float v;
    std::list<ColorGradientItem>color_gradient;
public:
    ppl7::grafix::PointF p;
    Particle();
    void update(double time, TileTypePlane& ttplane, Player& player);
};


class ParticleSystem
{
public:
    enum class Layer {
        BehindBricks=0,
        BeforeBricks=1,
        BeforePlayer=2,
        BehindPlayer=1
    };
public:
    ParticleSystem();
    ~ParticleSystem();
    void clear();
    void loadSpritesets(SDL& sdl);
    void addParticle(Layer layer, Particle* particle);
    void update(double time, TileTypePlane& ttplane, Player& player);
    void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, ParticleSystem::Layer layer) const;
    size_t count() const;
};

ParticleSystem* GetParticleSystem();

#endif
