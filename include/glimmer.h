#ifndef INCLUDE_GLIMMER_H_
#define INCLUDE_GLIMMER_H_
#include <ppl7-grafix.h>
#include "decker.h"
#include "light.h"

#define GLIMMER_TAIL_LENGTH 20

class Glimmer
{
public:
    enum class Behavior {
        Wait,
        FollowPlayer,
        Invisible,
        Appear,
        Disappear,
        Awaken,
        FlyTo,
        FlyToAndStop,
        FlyToPlayer,
        Glimmer,
        Agree,
        Disagree,
        IncreaseLight,
        DecreaseLight
    };
private:

    enum class MoveState {
        Start,
        Wait,
        Move,
        Stop,
        Grow,
        Shrink,
    };
    MoveState movestate;
    AudioInstance* audio;

    double next_birth;
    std::list<Particle::ColorGradientItem>color_gradient;


    ppl7::grafix::Point tail_p[GLIMMER_TAIL_LENGTH];
    int last_tail_index;
    SpriteTexture* texture;
    const SpriteTexture* lightmaps;
    bool enabled;
    bool draw_enabled;
    bool break_for_direction_change;
    Behavior behavior;
    float frame_rate_compensation;
    double time;
    double action_start_time;
    double action_timeout;
    float duration;
    Game& game;
    float streak_rotation;
    float streak_size;
    float light_size;
    float scale;
    float speed;
    float direction;
    float maxspeed;
    float glimmer_angle;
    ppl7::grafix::PointF velocity;
    ppl7::grafix::PointF target_coords;
    ppl7::grafix::PointF start_coords;
    LightObject light;
    uint32_t next_node;
    int emote_counter;

    void drawObject(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
    void updateVelocity();
    void moveTo(const ppl7::grafix::PointF& target);

    void updateFollowPlayer(Player& player);
    void updateAwaken();
    void updateAppear();
    void updateDisappear();
    void updateFlyTo();
    void updateWait();
    void updateFlyToPlayer(Player& player);
    void updateGlimmer();
    void updateAgree();
    void updateDisagree();
    void updateIncreaseLight();
    void updateDecreaseLight();

    void triggerNextNode();

    void checkCollisionWithOtherObjects();
    void emmitParticles(double time, const Player& player);
    void glimmerParticles(double time, const Player& player);


public:

    ppl7::grafix::PointF p;

    Glimmer(Game& game);
    ~Glimmer();
    void setSpriteResource(SpriteTexture& resource, const SpriteTexture& lightmaps);

    void setPosition(int x, int y);
    void setPosition(const ppl7::grafix::Point& position);
    void setEnabled(bool enable);

    void update(double time, const TileTypePlane& world, Player& player, Decker::Objects::ObjectSystem& objects, float frame_rate_compensation);
    void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;


    void flyTo(const ppl7::grafix::PointF& target, float maxSpeed, bool stop_at_target, bool break_for_direction_change);
    void followPlayer();
    void appear();
    void awaken();
    void disappear();
    void glimmer();
    void agree();
    void disagree();
    void increaseLight();
    void decreaseLight();
    void wait(const ppl7::grafix::PointF& target, float duration);
    void setNextNode(uint32_t id);
    void flyToPlayer(float maxSpeed);



};


#endif // INCLUDE_GLIMMER_H_
