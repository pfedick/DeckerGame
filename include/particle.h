#ifndef INCLUDE_PARTICLE_H_
#define INCLUDE_PARTICLE_H_

#include <map>
#include <list>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "animation.h"
#include <atomic>
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

enum class EmitterType {
    Point=0,
    Rectangle,
    Ellipse
};

typedef struct {
    float x;
    float y;
} PARTICLE_POINT;

typedef struct {
    PARTICLE_POINT p;
    PARTICLE_POINT velocity;
    PARTICLE_POINT gravity;
    float weight;
} PARTICLE;

class Particle
{
    friend class ParticleSystem;
    friend class ParticleUpdateThread;
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
        StaticBulletBig,
        StaticStudSmall,
        StaticStudBig
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
    bool visible;
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
    unsigned int sprite_set;
    unsigned int sprite_no;
    float scale;
    float weight;
    ppl7::grafix::Color color_mod;

    ppl7::grafix::PointF p;
    ppl7::grafix::PointF velocity;
    ppl7::grafix::PointF gravity;

    Particle();
    virtual ~Particle();
    virtual void update(double time, float frame_rate_compensation);

    void initAnimation(Particle::Type type);
    void initColorGradient(const std::list<ColorGradientItem>& gradient);
    void initScaleGradient(const std::list<ScaleGradientItem>& gradient, float base_scale);
};

class ParticleSystem;

class ParticleToDraw
{
public:
    ppl7::grafix::Color color_mod;
    ppl7::grafix::PointF p;
    float scale;
};

class ParticleUpdateThread : public ppl7::Thread
{
    friend class ParticleSystem;
private:
    ParticleSystem& ps;
    double time;
    TileTypePlane* ttplane;
    Player* player;
    ppl7::grafix::PointF worldcoords;
    ppl7::grafix::Rect viewport;
    float frame_rate_compensation;
    double thread_duration;
    std::map<uint32_t, Particle*>* visible_particle_map;
    std::atomic_bool thread_running;

public:
    ppl7::Mutex mutex;

    ParticleUpdateThread(ParticleSystem& ps);
    void run() override;
    bool isRunning() const;
    double getThreadDuration() const;
    void setVisibleParticleMap(std::map<uint32_t, Particle*>* visible_particle_map);

};

class ParticleSystem
{
    friend class ParticleUpdateThread;
private:
    uint64_t nextid;
    SpriteTexture* spriteset[ParticleSpriteset::MaxSpritesets];
    std::map<uint64_t, Particle*> particle_map;
    std::map<uint64_t, Particle*> new_particles;
    std::map<uint32_t, Particle*> visible_particle_map[2][static_cast<int>(Particle::Layer::maxLayer)];
    std::list<uint64_t> particles_to_delete;
    void deleteParticle(uint64_t id);
    void cleanupParticles();
    int active_map;
    ParticleUpdateThread update_thread=ParticleUpdateThread(*this);
public:
    ParticleSystem();
    ~ParticleSystem();
    void clear();
    void loadSpritesets(SDL& sdl);
    void addParticle(Particle* particle);
    void update(double time, TileTypePlane& ttplane, Player& player, const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport, float frame_rate_compensation);
    double waitForUpdateThreadFinished();
    void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, Particle::Layer layer) const;
    size_t count() const;
    size_t countVisible() const;
    static ppl7::String layerName(Particle::Layer layer);


};

ParticleSystem* GetParticleSystem();
float randf(float min, float max);
ppl7::grafix::PointF calculateVelocity(float speed, float direction);
ppl7::grafix::PointF getBirthPosition(const ppl7::grafix::PointF& emitter, const EmitterType type, const ppl7::grafix::Size emitter_size, float rotation=0.0f);
bool emitterInPlayerRange(const ppl7::grafix::PointF& emitter, const Player& player);
#endif
