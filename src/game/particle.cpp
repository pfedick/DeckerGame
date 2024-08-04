#include <ppl7.h>
#include <ppl7-grafix.h>
#include <math.h>
#include "particle.h"
#include "player.h"

static int particle_transparent[]={ 33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
    52,53,54,55,56,57,58,59,60,61,62 };

static int particle_white[]={ 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    25,26,27,28,29,30,31 };

static int snowflake_white[]={ 64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,
    83,84,85,86,87,88,89,90,91,92,93 };

static int snowflake_transparent[]={ 95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
    112,113,114,115,116,117,118,119,120,121,122,123,124 };

static int disc_white[]={ 126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
    144,145,146,147,148,149,150,151,152,153,154,155 };


Particle::ColorGradientItem::ColorGradientItem()
{
    age=0.0f;
}

Particle::ColorGradientItem::ColorGradientItem(float age, const ppl7::grafix::Color& color)
{
    this->age=age;
    this->color=color;
}

Particle::ScaleGradientItem::ScaleGradientItem()
{
    age=0.0f;
    scale=0.0f;
}

Particle::ScaleGradientItem::ScaleGradientItem(float age, float scale)
{
    this->age=age;
    this->scale=scale;
}


Particle::Particle()
{
    sprite_set=ParticleSpriteset::GenericParticles;
    sprite_no=1;
    death_time=1.0f;
    scale=1.0f;
    layer=Layer::BeforePlayer;
    useColorGradient=false;
    useScaleGradient=false;
    color_age_diff=scale_age_diff=0.0f;
    base_scale=0.0f;
    birth_time=next_animation=0.0f;
    color_mod.set(255, 255, 255, 255);
    visible=true;
}

Particle::~Particle()
{

}

void Particle::initColorGradient(const std::list<ColorGradientItem>& gradient)
{
    if (gradient.empty()) return;
    color_gradient=gradient;
    useColorGradient=true;
    current_color=color_gradient.front();
    color_gradient.pop_front();

    if (current_color.age == 0.0f) {
        if (color_gradient.size() > 0) {
            next_color=color_gradient.front();
            color_gradient.pop_front();
        } else {
            next_color=current_color;
            next_color.age=1.0;
        }
    } else {
        next_color=current_color;
        current_color.age=0.0f;
    }
    color_age_diff=next_color.age - current_color.age;
}

void Particle::initScaleGradient(const std::list<ScaleGradientItem>& gradient, float base_scale)
{
    if (gradient.empty()) return;
    this->base_scale=base_scale;
    scale_gradient=gradient;
    useScaleGradient=true;
    current_scale=scale_gradient.front();
    scale_gradient.pop_front();

    if (current_scale.age == 0.0f) {
        if (scale_gradient.size() > 0) {
            next_scale=scale_gradient.front();
            scale_gradient.pop_front();
        } else {
            next_scale=current_scale;
            next_scale.age=1.0;
        }
    } else {
        next_scale=current_scale;
        current_scale.age=0.0f;
    }
    scale_age_diff=next_scale.age - current_scale.age;
}

void Particle::initAnimation(Particle::Type type)
{
    sprite_set=ParticleSpriteset::GenericParticles;
    switch (type) {
        case Particle::Type::RotatingParticleTransparent:
            animation.startRandom(particle_transparent, sizeof(particle_transparent) / sizeof(int), true, 0);
            break;
        case Particle::Type::RotatingParticleWhite:
            animation.startRandom(particle_white, sizeof(particle_white) / sizeof(int), true, 0);
            break;
        case Particle::Type::RotatingSnowflakeTransparent:
            animation.startRandom(snowflake_transparent, sizeof(snowflake_transparent) / sizeof(int), true, 0);
            break;
        case Particle::Type::RotatingSnowflakeWhite:
            animation.startRandom(snowflake_white, sizeof(snowflake_white) / sizeof(int), true, 0);
            break;
        case Particle::Type::RotatingCylinder:
            animation.startRandom(disc_white, sizeof(disc_white) / sizeof(int), true, 0);
            break;
        case Particle::Type::StaticBulletSmall:
            animation.setStaticFrame(1);
            break;
        case Particle::Type::StaticBulletBig:
            animation.setStaticFrame(159);
            break;
        case Particle::Type::StaticParticle:
            animation.setStaticFrame(161);
            break;
        case Particle::Type::StaticParticleBig:
            animation.setStaticFrame(163);
            break;
        case Particle::Type::StaticCircle:
            animation.setStaticFrame(157);
            break;
        case Particle::Type::StaticCircleBig:
            animation.setStaticFrame(158);
            break;
        case Particle::Type::StaticStudSmall:
            animation.setStaticFrame(160);
            break;
        case Particle::Type::StaticStudBig:
            animation.setStaticFrame(162);
            break;
        case Particle::Type::SoftGradientSmall:
            animation.setStaticFrame(166);
            break;
        case Particle::Type::SoftGradientMedium:
            animation.setStaticFrame(167);
            break;
        case Particle::Type::SoftGradientBig:
            animation.setStaticFrame(168);
            break;

    };
}


void Particle::update(double time, float frame_rate_compensation)
{
    // Color and scale gradients and animation must only be updated if particle is visible
    if (visible) {
        if (time > next_animation) {
            next_animation=time + 0.056f;
            animation.update();
            sprite_no=animation.getFrame();
        }
        if (useColorGradient) updateColorGradient();
        if (useScaleGradient) updateScaleGradient();
    }
    velocity.x+=gravity.x * weight * frame_rate_compensation;
    velocity.y+=gravity.y * weight * frame_rate_compensation;
    p.x+=velocity.x * frame_rate_compensation;
    p.y+=velocity.y * frame_rate_compensation;
}

void Particle::updateColorGradient()
{
    if (age > next_color.age) {
        current_color=next_color;
        if (color_gradient.size() > 0) {
            next_color=color_gradient.front();
            color_gradient.pop_front();
        } else {
            next_color.age=1.0f;
        }
        color_age_diff=next_color.age - current_color.age;
    }
    float n1=1.0f - ((age - current_color.age) / color_age_diff);
    float n2=1.0f - ((next_color.age - age) / color_age_diff);
    color_mod=multiplyWithAlpha(current_color.color, n1) + multiplyWithAlpha(next_color.color, n2);

}

void Particle::updateScaleGradient()
{
    if (age > next_scale.age) {
        current_scale=next_scale;
        if (scale_gradient.size() > 0) {
            next_scale=scale_gradient.front();
            scale_gradient.pop_front();
        } else {
            next_scale.age=1.0f;
        }
        scale_age_diff=next_scale.age - current_scale.age;
    }
    float n1=1.0f - ((age - current_scale.age) / scale_age_diff);
    float n2=1.0f - ((next_scale.age - age) / scale_age_diff);
    scale=base_scale * (current_scale.scale * n1 + next_scale.scale * n2);
}


/****************************************************************************************
 * Functions
 ****************************************************************************************/

//static float pi=3.1415926535f;
static float rad_pi=3.1415926535f / 180.0f;

float randf(float min, float max)
{
    if (max == min) return min;
    double range=max - min;
    double r=(((double)ppl7::rand(0, RAND_MAX)) / (double)RAND_MAX * range) + min;
    return r;
}


ppl7::grafix::PointF calculateVelocity(float speed, float direction)
{
    if (direction < 0.0f) direction+=360.0f;
    if (direction >= 360.0f) direction-=360.0f;

    ppl7::grafix::PointF velocity;
    if (direction == 0.0f) {
        velocity.x=0;
        velocity.y=-speed;
    } else if (direction == 90.0f) {
        velocity.x=speed;
        velocity.y=0;
    } else if (direction == 180.0f) {
        velocity.x=0;
        velocity.y=speed;
    } else if (direction == 270.0f) {
        velocity.x=-speed;
        velocity.y=0;
    } else {
        velocity.y=-speed * sinf((90 - direction) * rad_pi);
        velocity.x=speed * sinf(direction * rad_pi);
    }
    return velocity;
}

ppl7::grafix::PointF getBirthPosition(const ppl7::grafix::PointF& emitter, const EmitterType type, const ppl7::grafix::Size emitter_size, float rotation)
{
    if (type == EmitterType::Point) return emitter;
    ppl7::grafix::PointF p1;
    if (type == EmitterType::Rectangle) {
        p1.setPoint(randf(0.0, emitter_size.width) - emitter_size.width / 2,
            randf(0.0, emitter_size.height) - emitter_size.height / 2);
    } else {
        float d=randf(0.0, 359.99999);
        p1.setPoint((sinf(d * rad_pi) * randf(0.0f, (float)emitter_size.width / 2)),
            (cosf(d * rad_pi) * randf(0.0f, (float)emitter_size.height / 2)));
    }

    return p1 + emitter;

}

bool emitterInPlayerRange(const ppl7::grafix::PointF& emitter, const Player& player)
{
    double d=ppl7::grafix::Distance(ppl7::grafix::PointF(player.WorldCoords.x + player.Viewport.width() / 2,
        player.WorldCoords.y + player.Viewport.height() / 2), emitter);
    if (d > player.Viewport.width()) return false;
    return true;
}



bool emitterInPlayerRange(int plane, const ppl7::grafix::PointF& emitter, const Player& player)
{
    if (plane == static_cast<int>(PlaneId::Player)) return emitterInPlayerRange(emitter, player);

    //ppl7::PrintDebug("Emitter: %d:%d, World: %d:%d\n", (int)emitter.x, (int)emitter.y, player.WorldCoords.x, player.WorldCoords.y);

    ppl7::grafix::PointF pos=emitter;

    ppl7::grafix::Point coords=player.WorldCoords * planeFactor[plane];
    ppl7::grafix::Point pp=ppl7::grafix::Point(emitter) - coords + player.WorldCoords;
    pos=pp;
    double d=ppl7::grafix::Distance(ppl7::grafix::PointF(player.WorldCoords.x + player.Viewport.width() / 2,
        player.WorldCoords.y + player.Viewport.height() / 2), pos);
    if (d > player.Viewport.width()) return false;
    return true;
}
