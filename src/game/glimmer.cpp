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
    light_size=1.0f;
    speed=0.0f;
    direction=0.0f;

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
    this->enabled=enable;
}

void Glimmer::setBehavior(Behavior behavior)
{
    this->behavior=behavior;
    if (behavior == Behavior::FollowPlayer) follow_player.state=FollowPlayer::State::Start;

}

void Glimmer::setPosition(int x, int y)
{
    setPosition(ppl7::grafix::PointF(x, y));
}

void Glimmer::setPosition(const ppl7::grafix::Point& position)
{
    p=position;
    for (int i=0;i < GLIMMER_TAIL_LENGTH;i++) tail_p[i]=p;
    speed=0;
    velocity.setPoint(0.0f, 0.0f);
}

static inline int ftoi_clamp(float value, int max)
{
    int r=(int)(value * (float)max);
    if (r > max) return max;
    return r;
}

void Glimmer::update(double time, const TileTypePlane& world, Player& player, Decker::Objects::ObjectSystem& objects, float frame_rate_compensation)
{
    if (!enabled) return;
    this->frame_rate_compensation=frame_rate_compensation;
    this->time=time;
    if (behavior == Behavior::FollowPlayer) updateFollowPlayer(player);

    // Update Tail
    last_tail_index++;
    if (last_tail_index == GLIMMER_TAIL_LENGTH) last_tail_index=0;
    tail_p[last_tail_index]=p;

    streak_rotation+=2.0f * frame_rate_compensation;
    if (streak_rotation >= 360.0f) streak_rotation-=360.0f;

    LightSystem& lights=game.getLightSystem();
    light.x=p.x;
    light.y=p.y;
    light.scale_x=light_size;
    light.scale_y=light_size;
    light.intensity=ftoi_clamp(light_size, 255);
    light.flare_intensity=ftoi_clamp(light_size, 127);
    lights.addObjectLight(&light);

}

void Glimmer::updateFollowPlayer(Player& player)
{
    ppl7::grafix::PointF player_p(player.x, player.getBoundingBox().y1 - 1 * TILE_HEIGHT);
    if (player.velocity_move.x > 0 || player.orientation == Player::PlayerOrientation::Right) {
        player_p.x+=2 * TILE_WIDTH;
        if (player.movement == Player::PlayerMovement::Run) {
            player_p.x+=8 * TILE_WIDTH;
        }
    } else if (player.velocity_move.x < 0 || player.orientation == Player::PlayerOrientation::Left) {
        player_p.x-=2 * TILE_WIDTH;
        if (player.movement == Player::PlayerMovement::Run) {
            player_p.x-=8 * TILE_WIDTH;
        }
    }
    double dist=ppl7::grafix::Distance(player_p, p);

    float dist_x=player_p.x - p.x;
    float dist_y=player_p.y - p.y;
    float acceleration=0.1f;

    float maxspeed=dist / 40.0f;
    if (maxspeed > 15.0f) maxspeed=15.0f;
    acceleration=speed / 10.0f;
    if (acceleration > 2.0f) acceleration=20.0f;
    if (acceleration < 0.1) acceleration=0.1f;

    if (follow_player.state == FollowPlayer::State::Start) {
        if (speed == 0.0f) {
            follow_player.state=FollowPlayer::State::Wait;
        } else {
            follow_player.state=FollowPlayer::State::Follow;
        }
    }
    if (follow_player.state == FollowPlayer::State::Wait && dist > 100.0f) {
        follow_player.state=FollowPlayer::State::Follow;
    }
    if (follow_player.state == FollowPlayer::State::Follow) {
        if (speed < maxspeed) {
            speed+=acceleration * frame_rate_compensation;
            if (speed > maxspeed) speed=maxspeed;
        } else if (speed > maxspeed) {
            speed-=acceleration * frame_rate_compensation;
            if (speed < maxspeed) speed=maxspeed;
        }
        if (dist < 20.0f) follow_player.state=FollowPlayer::State::Stop;
    }
    if (follow_player.state == FollowPlayer::State::Stop) {
        if (speed > 0.0f) speed-=acceleration * frame_rate_compensation;
        if (speed < 0.0f) {
            speed=0.0f;
            follow_player.state=FollowPlayer::State::Wait;
        }
    }

    ppl7::PrintDebug("state: %d, speed: %0.3f, acceleration: %0.3f\n", (int)follow_player.state, speed, acceleration);


    if (p.x < player_p.x) p.x+=speed;
    if (p.x > player_p.x) p.x-=speed;

    if (p.y < player_p.y) p.y+=speed;
    if (p.y > player_p.y) p.y-=speed;


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
    drawObject(renderer, coords);


    SDL_SetRenderTarget(renderer, rendertarget);

}
