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
        FlyToAndStop
    };
private:

    enum class MoveState {
        Start,
        Wait,
        Move,
        Stop,
        Grow,
        Shrink
    };
    MoveState movestate;


    ppl7::grafix::Point tail_p[GLIMMER_TAIL_LENGTH];
    int last_tail_index;
    SpriteTexture* texture;
    const SpriteTexture* lightmaps;
    bool enabled;
    bool draw_enabled;
    Behavior behavior;
    float frame_rate_compensation;
    double time;
    double action_start_time;
    Game& game;
    float streak_rotation;
    float streak_size;
    float light_size;
    float scale;
    float speed;
    float direction;
    float maxspeed;
    ppl7::grafix::PointF velocity;
    ppl7::grafix::PointF target_coords;
    LightObject light;
    uint32_t next_node;

    void drawObject(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;
    void updateVelocity();
    void moveTo(const ppl7::grafix::PointF& target);

    void updateFollowPlayer(Player& player);
    void updateAwaken();
    void updateAppear();
    void updateDisappear();
    void updateFlyTo();
    void updateWait();

    void checkCollisionWithOtherObjects();


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


    void flyTo(const ppl7::grafix::PointF& target, float maxSpeed, bool stop_at_target);
    void followPlayer();
    void appear();
    void awaken();
    void disappear();
    void wait(const ppl7::grafix::PointF& target);
    void setNextNode(uint32_t id);



};


#endif // INCLUDE_GLIMMER_H_
