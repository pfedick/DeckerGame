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
    };
private:
    ppl7::grafix::Point tail_p[GLIMMER_TAIL_LENGTH];
    int last_tail_index;
    SpriteTexture* texture;
    const SpriteTexture* lightmaps;
    bool enabled;
    Behavior behavior;
    float frame_rate_compensation;
    double time;
    Game& game;
    int streak_sprite;
    float streak_rotation;
    float streak_size;
    LightObject light;

    void drawObject(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const;

public:

    ppl7::grafix::PointF p;

    Glimmer(Game& game);
    ~Glimmer();
    void setSpriteResource(SpriteTexture& resource, const SpriteTexture& lightmaps);

    void setPosition(int x, int y);
    void setPosition(const ppl7::grafix::Point& position);
    void setEnabled(bool enable);
    void setBehavior(Behavior behavior);

    void update(double time, const TileTypePlane& world, Player& player, Decker::Objects::ObjectSystem& objects, float frame_rate_compensation);
    void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;




};


#endif // INCLUDE_GLIMMER_H_
