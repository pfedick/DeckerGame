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
    draw_enabled=true;
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
        case Behavior::FlyToPlayer:
            updateFlyToPlayer(player);
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
        float s=scale;
        if (scale < 1.0f) {
            s=scale * 2.0f;
            if (s > 1.0f) s=1.0f;
        }
        light.scale_x=light_size * s;
        light.scale_y=light_size * s;
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
    //ppl7::PrintDebug("Glimmer::checkCollisionWithOtherObjects: P: %d:%d, Rect: %d:%d - %d:%d\n", (int)p.x, (int)p.y, boundary.x1, boundary.y1, boundary.x2, boundary.y2);
    Decker::Objects::GetObjectSystem()->detectObjectCollision(boundary, object_list);
    std::list<Decker::Objects::Object*>::iterator it;
    for (it=object_list.begin();it != object_list.end();++it) {
        if ((*it)->type() == Decker::Objects::Type::GlimmerNode) {
            //ppl7::PrintDebug("collision with GlimmerNode\n");
            Decker::Objects::GlimmerNode* glimmernode=static_cast<Decker::Objects::GlimmerNode*>(*it);
            glimmernode->handleCollisionByGlimmer();
        } else {
            //ppl7::PrintDebug("collision with other object\n");
        }
    }
}


static void emmitParticles(double time, const ppl7::grafix::PointF& p)
{
    std::list<Particle::ScaleGradientItem>scale_gradient;
    std::list<Particle::ColorGradientItem>color_gradient;
    scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.058));
    scale_gradient.push_back(Particle::ScaleGradientItem(0.140, 1.400));
    scale_gradient.push_back(Particle::ScaleGradientItem(0.482, 0.184));
    scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 0.010));
    color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(255, 255, 255, 255)));
    color_gradient.push_back(Particle::ColorGradientItem(0.279, ppl7::grafix::Color(255, 255, 255, 255)));
    color_gradient.push_back(Particle::ColorGradientItem(0.766, ppl7::grafix::Color(255, 255, 255, 0)));
    ParticleSystem* ps=GetParticleSystem();
    int new_particles=ppl7::rand(135, 166);
    for (int i=0;i < new_particles;i++) {
        Particle* particle=new Particle();
        particle->birth_time=time;
        particle->death_time=randf(0.886, 1.149) + time;
        particle->p=p;
        particle->layer=Particle::Layer::BeforePlayer;
        particle->weight=randf(0.246, 0.000);
        particle->gravity.setPoint(0.000, 0.000);
        particle->velocity=calculateVelocity(randf(4.737, 6.316), 0.000 + randf(-180.000, 180.000));
        particle->scale=randf(0.237, 0.491);
        particle->color_mod.set(255, 255, 255, 255);
        particle->initAnimation(Particle::Type::StaticParticle);
        particle->initScaleGradient(scale_gradient, particle->scale);
        particle->initColorGradient(color_gradient);
        ps->addParticle(particle);
    }

}

void Glimmer::updateAwaken()
{
    if (movestate == MoveState::Start) {
        action_start_time=time;
        //emmitParticles(time, p);
        movestate=MoveState::Grow;
        scale=0.01f;
    }
    if (movestate == MoveState::Grow) {
        if (scale < 2.0f) scale+=0.01f * frame_rate_compensation;
        else {
            movestate=MoveState::Shrink;
        }
    } else if (movestate == MoveState::Shrink) {
        if (scale > 1.0f) scale-=0.01f * frame_rate_compensation;
        else {
            scale=1.0f;
            movestate=MoveState::Start;
            behavior=Behavior::Wait;
            if (next_node > 0) {
                Decker::Objects::ObjectSystem* objs=Decker::Objects::GetObjectSystem();
                Decker::Objects::Object* target=objs->getObject(next_node);
                if (target) target->trigger();
            }
        }
    }
}


void Glimmer::updateAppear()
{
    if (movestate == MoveState::Start) {
        action_start_time=time;
        emmitParticles(time, p);
        movestate=MoveState::Grow;
        scale=0.01f;
    }
    if (movestate == MoveState::Grow) {
        if (scale < 2.0f) scale+=0.08f * frame_rate_compensation;
        else {
            movestate=MoveState::Shrink;
        }
    } else if (movestate == MoveState::Shrink) {
        if (scale > 1.0f) scale-=0.08f * frame_rate_compensation;
        else {
            scale=1.0f;
            movestate=MoveState::Start;
            behavior=Behavior::Wait;
            if (next_node > 0) {
                Decker::Objects::ObjectSystem* objs=Decker::Objects::GetObjectSystem();
                Decker::Objects::Object* target=objs->getObject(next_node);
                if (target) target->trigger();
            }
        }
    }
}

void Glimmer::updateDisappear()
{
    //ppl7::PrintDebug("Glimmer::updateDisappear: movestate=%d, scale=%0.3f\n", (int)movestate, scale);
    if (movestate == MoveState::Start) {
        movestate=MoveState::Shrink;
        action_start_time=time;

    }
    if (movestate == MoveState::Shrink) {
        if (scale > 0.01f) scale-=(0.08f * frame_rate_compensation);
        if (scale <= 0.01f) {
            scale=0.1f;
            velocity.setPoint(0.0f, 0.0f);
            movestate=MoveState::Stop;
            emmitParticles(time, p);
            draw_enabled=false;
        }
    } else if (movestate == MoveState::Stop) {
        if (scale < 1.0f) scale+=(0.08f * frame_rate_compensation);
        if (scale > 1.0f) {
            scale=1.0f;
            enabled=false;
            draw_enabled=true;
            behavior=Behavior::Invisible;
        }

    }
}

void Glimmer::updateFlyTo()
{
    double dist=ppl7::grafix::Distance(target_coords, p);
    if (behavior == Behavior::FlyToAndStop) {
        maxspeed=dist / 40.0f;
    }
    moveTo(target_coords);
    if (dist < 5.0f) wait(target_coords);
    if (dist > 5.0f && movestate == MoveState::Wait) movestate=MoveState::Move;
}

void Glimmer::updateWait()
{
    if (movestate == MoveState::Start) {
        double dist=ppl7::grafix::Distance(target_coords, p);
        if (dist < 10.0f) {
            movestate=MoveState::Wait;
            velocity.setPoint(0.0f, 0.0f);
            speed=0.0f;
        }
        maxspeed=1.0f;
        moveTo(target_coords);
    }
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

void Glimmer::updateFlyToPlayer(Player& player)
{
    ppl7::grafix::PointF player_p(player.x, player.getBoundingBox().y1 + 1 * TILE_HEIGHT);
    double dist=ppl7::grafix::Distance(player_p, p);
    maxspeed=dist / 40.0f;
    if (movestate == MoveState::Wait && dist > 100.0f) {
        movestate=MoveState::Move;
    } else if (movestate == MoveState::Move && dist < 20.0f) movestate=MoveState::Stop;
    if (movestate == MoveState::Stop) {
        if (next_node > 0) {
            Decker::Objects::ObjectSystem* objs=Decker::Objects::GetObjectSystem();
            Decker::Objects::Object* target=objs->getObject(next_node);
            if (target) target->trigger();
        } else {
            behavior=Behavior::Wait;
            movestate=MoveState::Start;
        }
    }
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


    float diff_links=0.0f - (direction - target_angle);
    if (diff_links >= 0.0f) diff_links=-360.0f + diff_links;
    float diff_rechts=360.0f - fabsf(diff_links);
    float diff_angle=0.0f;;
    if (diff_rechts < fabsf(diff_links)) diff_angle=diff_rechts;
    else diff_angle=diff_links;

    //ppl7::PrintDebug("Glimmer: movestate=%d, speed=%5.2f, my angle: %6.2f, target: %6.2f, diff=%7.2f, links=%7.2f, rechts=%7.2f\n",
    //    (int)movestate, speed, direction, target_angle, diff_angle, diff_links, diff_rechts);

    if (movestate == MoveState::Move) {
        if (fabsf(diff_angle) < 120.0f) {
            float factor=fabsf(diff_angle) / 10.0f;
            if (factor > 5.0f) factor=5.0f;
            if (factor < 0.1f) factor=0.1f;

            if (diff_angle < 0.0f) direction-=factor * frame_rate_compensation;
            else direction+=factor * frame_rate_compensation;
        } else {
            movestate = MoveState::Stop;
        }
    }

    updateVelocity();

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
    if (!draw_enabled) return;
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
    //ppl7::PrintDebug("fly to triggered\n");
}

void Glimmer::flyToPlayer(float maxSpeed)
{
    this->maxspeed=maxSpeed;
    behavior=Behavior::FlyToPlayer;
    movestate=MoveState::Start;
    action_start_time=0.0f;
    //ppl7::PrintDebug("fly to triggered\n");
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
    movestate=MoveState::Start;
    velocity.setPoint(0.0f, 0.0f);
    action_start_time=0.0f;
    scale=0.1f;
}

void Glimmer::awaken()
{
    if (!enabled) enabled=true;
    behavior=Behavior::Awaken;
    velocity.setPoint(0.0f, 0.0f);
    movestate=MoveState::Start;
    action_start_time=0.0f;
    scale=0.01f;
}

void Glimmer::disappear()
{
    behavior=Behavior::Disappear;
    movestate=MoveState::Start;
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
