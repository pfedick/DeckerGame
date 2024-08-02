#include "objects.h"
#include "glimmer.h"
#include "player.h"


static float rad_pi=3.1415926535f / 180.0f;

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
    behavior=Behavior::Invisible;
    frame_rate_compensation=1.0f;
    streak_rotation=0.0f;
    streak_size=1.0f;
    light_size=1.0f;
    scale=1.0f;
    speed=0.0f;
    direction=0.0f;
    maxspeed=15.0f;
    next_node=0;

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
    //ppl7::PrintDebug("Glimmer state: %d\n", static_cast<int>(behavior));
    switch (behavior) {
        case Behavior::FollowPlayer:
            updateFollowPlayer(player);
            break;
        case Behavior::Appear:
            updateAppear();
            break;
        case Behavior::Disappear:
            updateDisappear();
            break;
        case Behavior::FlyTo:
        case Behavior::FlyToAndStop:
            updateFlyTo();
            break;
        case Behavior::Wait:
            updateWait();
            break;

        case Behavior::Invisible:
            return;
        case Behavior::Awaken:
            updateAwaken();
            break;
    }



    p+=velocity;

    // Update Tail
    last_tail_index++;
    if (last_tail_index == GLIMMER_TAIL_LENGTH) last_tail_index=0;
    tail_p[last_tail_index]=p;

    streak_rotation+=2.0f * frame_rate_compensation;
    if (streak_rotation >= 360.0f) streak_rotation-=360.0f;

    if (behavior != Behavior::Invisible) {
        LightSystem& lights=game.getLightSystem();
        light.x=p.x;
        light.y=p.y;
        light.scale_x=light_size * scale;
        light.scale_y=light_size * scale;
        light.intensity=ftoi_clamp(light_size * scale, 255);
        light.flare_intensity=ftoi_clamp(light_size * scale, 127);
        lights.addObjectLight(&light);
    }
    checkCollisionWithOtherObjects();

}

void Glimmer::checkCollisionWithOtherObjects()
{
    std::list<Decker::Objects::Object*> object_list;
    ppl7::grafix::Rect boundary((int)p.x - 16, (int)p.y - 16, 32, 32);
    Decker::Objects::GetObjectSystem()->detectObjectCollision(boundary, object_list);
    std::list<Decker::Objects::Object*>::iterator it;
    for (it=object_list.begin();it != object_list.end();++it) {
        if ((*it)->type() == Decker::Objects::Type::GlimmerNode) {
            ppl7::PrintDebug("collision with GlimmerNode\n");

        }
    }
}

void Glimmer::updateAwaken()
{

}





static void emmitParticles(double time, const ppl7::grafix::PointF& p)
{
    std::list<Particle::ScaleGradientItem>scale_gradient;
    std::list<Particle::ColorGradientItem>color_gradient;
    scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.058));
    scale_gradient.push_back(Particle::ScaleGradientItem(0.604, 1.000));
    scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 0.010));
    color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(255, 255, 255, 255)));
    color_gradient.push_back(Particle::ColorGradientItem(0.595, ppl7::grafix::Color(255, 255, 255, 255)));
    color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(255, 255, 255, 0)));
    ParticleSystem* ps=GetParticleSystem();
    int new_particles=ppl7::rand(135, 166);
    for (int i=0;i < new_particles;i++) {
        Particle* particle=new Particle();
        particle->birth_time=time;
        particle->death_time=randf(0.448, 1.237) + time;
        particle->p=p;
        particle->layer=Particle::Layer::BeforePlayer;
        particle->weight=randf(0.105, 0.237);
        //particle->gravity.setPoint(0.000, 0.211);
        particle->velocity=calculateVelocity(randf(0.702, 1.754), 0.000 + randf(-180.000, 180.000));
        particle->scale=randf(0.289, 0.404);
        particle->color_mod.set(255, 255, 255, 255);
        particle->initAnimation(Particle::Type::StaticParticle);
        particle->initScaleGradient(scale_gradient, particle->scale);
        particle->initColorGradient(color_gradient);
        ps->addParticle(particle);
    }

}



void Glimmer::updateAppear()
{
    if (action_start_time == 0.0f) {
        action_start_time=time;
        emmitParticles(time, p);
    }
    if (scale < 1.0f) scale+=0.02f * frame_rate_compensation;
    if (scale >= 1.0f) {
        scale=1.0f;
        behavior=Behavior::Wait;
        if (next_node > 0) {
            Decker::Objects::ObjectSystem* objs=Decker::Objects::GetObjectSystem();
            Decker::Objects::Object* target=objs->getObject(next_node);
            if (target) target->trigger();
        }
    }
}

void Glimmer::updateDisappear()
{
    if (scale > 0.1f) scale-=0.02f * frame_rate_compensation;
    if (scale <= 0.1f) {
        scale=0.1f;
        behavior=Behavior::Invisible;
    }
}

void Glimmer::updateFlyTo()
{
    double dist=ppl7::grafix::Distance(target_coords, p);
    if (behavior == Behavior::FlyToAndStop) {
        maxspeed=dist / 40.0f;
    }
    //ppl7::PrintDebug("Glimmer::updateFlyTo, dist=%0.3f, target=%0.0f:%0.0f, maxspeed=%0.3f, speed=%0.3f, movestate=%d\n",
    //    dist, target_coords.x, target_coords.y, maxspeed, speed, (int)movestate);
    moveTo(target_coords);
    if (dist < 5.0f) wait(target_coords);
    if (dist > 5.0f && movestate == MoveState::Wait) movestate=MoveState::Move;
}

void Glimmer::updateWait()
{
    maxspeed=1.0f;
    moveTo(target_coords);
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
    maxspeed=dist / 40.0f;
    if (movestate == MoveState::Wait && dist > 100.0f) {
        movestate=MoveState::Move;
    } else if (movestate == MoveState::Move && dist < 20.0f) movestate=MoveState::Stop;
    moveTo(player_p);

}


void Glimmer::moveTo(const ppl7::grafix::PointF& target)
{

    float acceleration=0.1f;
    float c=ppl7::grafix::Distance(p, target);
    float a=target.x - p.x;
    float b=p.y - target.y;
    float target_angle=0.0f;
    if (a > 0.0f && b > 0.0f) {     // case 1 a+b are positive
        target_angle = asin(a / c) / rad_pi;
    } else if (a > 0.0f && b < 0.0f) {  // case 2
        target_angle =90.0f + asin(fabsf(b) / c) / rad_pi;
    } else if (a < 0.0f && b < 0.0f) {  // case 3, a+b are negative
        target_angle =180.0f + asin(fabsf(a) / c) / rad_pi;
    } else if (a < 0.0f && b>0.0f) {  // case 4
        target_angle =270.0f + asin(b / c) / rad_pi;
    }
    if (speed == 0.0f) direction=target_angle;


    if (maxspeed > 15.0f) maxspeed=15.0f;
    if (maxspeed < 0.1f) maxspeed=0.1;
    acceleration=speed / 10.0f;
    if (acceleration > 2.0f) acceleration=20.0f;
    if (acceleration < 0.1) acceleration=0.1f;

    if (movestate == MoveState::Start) {
        if (speed == 0.0f) {
            movestate=MoveState::Wait;
        } else {
            movestate=MoveState::Move;
        }
    }
    if (movestate == MoveState::Move) {
        if (speed < maxspeed) {
            speed+=acceleration * frame_rate_compensation;
            if (speed > maxspeed) speed=maxspeed;
        } else if (speed > maxspeed) {
            speed-=acceleration * frame_rate_compensation;
            if (speed < maxspeed) speed=maxspeed;
        }
    }
    if (movestate == MoveState::Stop) {
        if (speed > 0.0f) speed-=acceleration * frame_rate_compensation;
        if (speed <= 0.0f) {
            speed=0.0f;
            direction=0.0f;
            movestate=MoveState::Wait;
        }
    }




    //ppl7::PrintDebug("state: %d, speed: %0.3f, acceleration: %0.3f, a=%0.3f, b=%0.3f, target_angle=%0.3f\n", (int)movestate, speed, acceleration, a, b, target_angle);

    //ppl7::PrintDebug("my angle: %0.3f, target: %0.3f\n", direction, target_angle);

    if (movestate == MoveState::Move) {
        float g_angle=direction + 360;
        target_angle+=360;
        if (fabsf(target_angle - g_angle) < 90.0f) {
            if (target_angle < g_angle) direction-=2.0f * frame_rate_compensation;
            else direction+=2.0f * frame_rate_compensation;
        } else {
            movestate = MoveState::Stop;
        }
    }


    //direction=target_angle;

    updateVelocity();
    /*
    if (p.x < target.x) p.x+=speed;
    if (p.x > target.x) p.x-=speed;

    if (p.y < target.y) p.y+=speed;
    if (p.y > target.y) p.y-=speed;
    */
}


void Glimmer::updateVelocity()
{
    if (direction < 0.0f) direction+=360.0f;
    if (direction >= 360.0f) direction-=360.0f;
    if (speed == 0.0f) {
        velocity.x=0.0f;
        velocity.y=0.0f;
        return;
    }

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
            texture->drawScaled(renderer,
                tail_p[index].x + coords.x,
                tail_p[index].y + coords.y, 3, scale, color);
        }
        index--;
        if (index < 0) index=GLIMMER_TAIL_LENGTH - 1;
    }
    color.set(255, 255, 255, 255);


    // draw streaks
    texture->drawScaledWithAngle(renderer,
        p.x + coords.x,
        p.y + coords.y, 1, streak_size * scale, streak_size * scale, streak_rotation, color);
    // draw streaks
    texture->drawScaledWithAngle(renderer,
        p.x + coords.x,
        p.y + coords.y, 2, streak_size * scale, streak_size * scale, streak_rotation, color);
    texture->drawScaledWithAngle(renderer,
        p.x + coords.x,
        p.y + coords.y, 2, streak_size * scale, streak_size * scale, streak_rotation, color);


    // draw glimmer body
    texture->drawScaled(renderer,
        p.x + coords.x,
        p.y + coords.y, 3, scale, color);

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


void Glimmer::flyTo(const ppl7::grafix::PointF& target, float maxSpeed, bool stop_at_target)
{
    this->maxspeed=maxSpeed;
    target_coords=target;
    if (stop_at_target) behavior=Behavior::FlyToAndStop;
    else behavior=Behavior::FlyTo;
    action_start_time=0.0f;
    ppl7::PrintDebug("fly to triggered\n");
}

void Glimmer::followPlayer()
{
    movestate=MoveState::Start;
    behavior=Behavior::FollowPlayer;
    action_start_time=0.0f;
}

void Glimmer::appear()
{
    if (!enabled) enabled=true;
    behavior=Behavior::Appear;
    velocity.setPoint(0.0f, 0.0f);
    action_start_time=0.0f;
    scale=0.1f;
}

void Glimmer::awaken()
{
    if (!enabled) enabled=true;
    behavior=Behavior::Awaken;
    velocity.setPoint(0.0f, 0.0f);
    action_start_time=0.0f;
}

void Glimmer::disappear()
{
    behavior=Behavior::Disappear;
    action_start_time=0.0f;
}

void Glimmer::wait(const ppl7::grafix::PointF& target)
{
    behavior=Behavior::Wait;
    action_start_time=0.0f;
    target_coords=target;
}

void Glimmer::setNextNode(uint32_t id)
{
    next_node=id;
}
