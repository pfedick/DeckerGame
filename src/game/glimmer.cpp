#include "glimmer.h"
#include "player.h"

Glimmer::Glimmer(Game& game)
    :game(game)
{
    for (int i=0;i < GLIMMER_TAIL_LENGTH;i++) {
        tail_p[i].x=-500.0f;
        tail_p[i].y=-500.0f;
    }
    last_tail_index=0;
    texture=NULL;
    enabled=false;
    behavior=Behavior::Wait;
    frame_rate_compensation=1.0f;
    streak_rotation=0.0f;
    streak_size=1.0f;
    streak_sprite=0;

    light.color.set(255, 255, 255, 255);
    light.sprite_no=0;
    light.scale_x=1.0f;
    light.scale_y=1.0f;
    light.intensity=255;
    light.plane=static_cast<int>(LightPlaneId::Player);
    light.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::All);
    light.has_lensflare=true;
    light.flarePlane=static_cast<int>(LightPlayerPlaneMatrix::Player);
    light.flare_intensity=255;
    light.flare_useLightColor=true;
}

Glimmer::~Glimmer()
{

}

void Glimmer::setSpriteResource(SpriteTexture& resource, const SpriteTexture& lightmaps)
{
    texture=&resource;
    this->lightmaps=&lightmaps;
}

void Glimmer::setEnabled(bool enable)
{
    //experimental
    //this->enabled=enable;
}

void Glimmer::setBehavior(Behavior behavior)
{
    this->behavior=behavior;
}

void Glimmer::setPosition(int x, int y)
{
    setPosition(ppl7::grafix::PointF(x, y));
}

void Glimmer::setPosition(const ppl7::grafix::Point& position)
{
    p=position;
    for (int i=0;i < GLIMMER_TAIL_LENGTH;i++) tail_p[i]=p;
}

void Glimmer::update(double time, const TileTypePlane& world, Player& player, Decker::Objects::ObjectSystem& objects, float frame_rate_compensation)
{
    if (!enabled) return;
    this->frame_rate_compensation=frame_rate_compensation;
    this->time=time;
    if (p.x < player.x) p.x++;
    if (p.x > player.x) p.x--;

    int y=player.y - 5 * TILE_HEIGHT;
    if (p.y < y) p.y++;
    if (p.y > y) p.y--;



    last_tail_index++;
    if (last_tail_index == GLIMMER_TAIL_LENGTH) last_tail_index=0;
    tail_p[last_tail_index]=p;

    streak_rotation+=2.0f * frame_rate_compensation;
    if (streak_rotation >= 360.0f) streak_rotation-=360.0f;

    LightSystem& lights=game.getLightSystem();
    light.x=p.x;
    light.y=p.y;
    lights.addObjectLight(&light);

}

void Glimmer::drawObject(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
    //draw tail
    int index=last_tail_index;
    ppl7::grafix::Point glimmer_p=p;
    ppl7::grafix::Color color(255, 255, 255, 255);
    for (int i=0;i < GLIMMER_TAIL_LENGTH;i++) {
        if (glimmer_p != tail_p[index]) {
            color.setAlpha(255 - i * (255 / GLIMMER_TAIL_LENGTH));
            texture->draw(renderer,
                tail_p[index].x + coords.x,
                tail_p[index].y + coords.y, 3, color);
        }
        index--;
        if (index < 0) index=GLIMMER_TAIL_LENGTH - 1;
    }
    color.set(255, 255, 255, 255);


    // draw streaks
    texture->drawScaledWithAngle(renderer,
        p.x + coords.x,
        p.y + coords.y, 1, streak_size, streak_size, streak_rotation, color);

    // draw streaks
    texture->drawScaledWithAngle(renderer,
        p.x + coords.x,
        p.y + coords.y, 2, streak_size, streak_size, streak_rotation, color);
    texture->drawScaledWithAngle(renderer,
        p.x + coords.x,
        p.y + coords.y, 2, streak_size, streak_size, streak_rotation, color);


    // draw glimmer body
    texture->draw(renderer,
        p.x + coords.x,
        p.y + coords.y, 3, color);
}

void Glimmer::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
    if (!enabled) return;
    ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
    texture->setTextureBlendMode(SDL_BLENDMODE_BLEND);
    drawObject(renderer, coords);


    SDL_Texture* lighttarget=game.getLightRenderTarget();
    if (!lighttarget) return;
    SDL_Texture* rendertarget=SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, lighttarget);
    texture->setTextureBlendMode(SDL_BLENDMODE_ADD);
    //drawObject(renderer, coords);

    /*
    lightmaps->drawScaled(renderer,
        p.x + coords.x,
        p.y + coords.y, 0, 1.0f);
        */

    SDL_SetRenderTarget(renderer, rendertarget);

}
