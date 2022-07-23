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

class ParticleSpriteset
{
public:
    enum SpritesetIds {
        GenericParticles=0,
        MaxSpritesets
    };
};

class Particle
{
    friend class ParticleSystem;
public:
    enum class Layer {
        BehindBricks=0,
        BehindPlayer=1,
        BeforePlayer=2,
        BackplaneFront=3,
        BackplaneBack=4,
        FrontplaneFront=5,
        FrontplaneBack=6,
        maxLayer
    };
    enum class Type {
        RotatingParticleTransparent=0,
        RotatingParticleWhite,
        RotatingSnowflakeTransparent,
        RotatingSnowflakeWhite,
        RotatingCylinder,
        StaticParticle,
        StaticParticleBig,
        StaticCircle,
        StaticCircleBig,
        StaticBulletSmall,
        StaticBulletBig
    };


    class ScaleGradientItem
    {
    public:
        float age;
        float scale;

        ScaleGradientItem();
        ScaleGradientItem(float age, float scale);
    };
    class ColorGradientItem
    {
    public:
        float age;
        ppl7::grafix::Color color;

        ColorGradientItem(float age, const ppl7::grafix::Color& color);
        ColorGradientItem();
    };


private:
    double next_animation;
    float life_time;
    float age;
    AnimationCycle animation;

    //float weight;
    //float direction;

    // Color gradient
    bool useColorGradient;
    std::list<ColorGradientItem>color_gradient;
    ColorGradientItem current_color;
    ColorGradientItem next_color;
    float color_age_diff;

    // Scale gradient
    bool useScaleGradient;
    std::list<ScaleGradientItem>scale_gradient;
    ScaleGradientItem current_scale;
    ScaleGradientItem next_scale;
    float scale_age_diff;
    float base_scale;

    void updateColorGradient();
    void updateScaleGradient();

public:
    double death_time;
    double birth_time;

    Layer layer;
    int sprite_set;
    int sprite_no;
    float scale;
    float weight;
    ppl7::grafix::Color color_mod;

    ppl7::grafix::PointF p;
    ppl7::grafix::PointF velocity;

    Particle();
    virtual ~Particle();
    virtual void update(double time, TileTypePlane& ttplane);

    void initAnimation(Particle::Type type);
    void initColorGradient(const std::list<ColorGradientItem>& gradient);
    void initScaleGradient(const std::list<ScaleGradientItem>& gradient, float base_scale);
};


class ParticleSystem
{
private:
    uint64_t nextid;
    SpriteTexture* spriteset[ParticleSpriteset::MaxSpritesets];
    std::map<uint64_t, Particle*> particle_map;
    std::map<uint32_t, Particle*> visible_particle_map[static_cast<int>(Particle::Layer::maxLayer)];
    void deleteParticle(uint64_t id);
public:
    ParticleSystem();
    ~ParticleSystem();
    void clear();
    void loadSpritesets(SDL& sdl);
    void addParticle(Particle* particle);
    void update(double time, TileTypePlane& ttplane, Player& player, const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
    void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Particle::Layer layer) const;
    size_t count() const;
    size_t countVisible() const;
    static ppl7::String layerName(Particle::Layer layer);
};

ParticleSystem* GetParticleSystem();

#endif
