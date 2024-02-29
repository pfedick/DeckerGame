#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "audiopool.h"

namespace Decker::Objects {


Representation Ghost::representation()
{
    return Representation(Spriteset::Ghost, 0);
}

Ghost::Ghost()
    :Enemy(Type::ObjectType::Ghost)
{
    sprite_set=Spriteset::Ghost;
    sprite_no=0;
    animation.setStaticFrame(0);
    sprite_no_representation=0;
    next_state=ppl7::GetMicrotime() + (double)ppl7::rand(1, 5);
    state=State::Stand;
    next_animation=0.0f;
    collisionDetection=true;
    color_mod.setAlpha(128);
    glow=128.0f;
    glowtarget=64.0f;
    glowspeed=2.0f;
    height=0.0f;
    myLayer=Layer::BeforePlayer;
    land=false;
    audio=NULL;
    change_height=0.0f;
    target_height=0.0f;
    audio=getAudioPool().getInstance(AudioClip::ghost_chains);
    if (audio) {
        audio->setVolume(1.0f);
        audio->setAutoDelete(false);
        audio->setLoop(true);
        audio->setPositional(p, 960);
        //audiopool.playInstance(audio);
    }


//glowtime=0.0f;
//animation.start(turn_from_mid_to_left, sizeof(turn_from_mid_to_left) / sizeof(int), false, 0);
}

Ghost::~Ghost()
{
    if (audio) {
        getAudioPool().stopInstace(audio);
        delete audio;
        audio=NULL;
    }
}


void Ghost::update_glow(double time, float frame_rate_compensation)
{
    if (!enabled) return;
    if (glow > glowtarget) {
        glow-=glowspeed * frame_rate_compensation;
        if (glow < 0.0f) glow=0.0f;
        if (glow <= glowtarget) {
            glowtarget=ppl7::randf(128, 230);
            glowspeed=ppl7::randf(1.0f, 4.0f);
        }
    } else if (glow < glowtarget) {
        glow+=glowspeed * frame_rate_compensation;
        if (glow >= 255) glow=255.0f;
        if (glow >= glowtarget) {
            glowtarget=ppl7::randf(30, 128);
            glowspeed=ppl7::randf(1.0f, 4.0f);
        }
    }
    color_mod.setAlpha(glow);

}

void Ghost::update_animation(double time)
{
    if (time > next_animation) {
        next_animation=time + 0.056f;
        animation.update();
        int new_sprite=animation.getFrame();
        if (new_sprite != sprite_no) {
            sprite_no=new_sprite;
            updateBoundary();
        }
    }
}

void Ghost::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
    update_glow(time, frame_rate_compensation);
    update_animation(time);
    AudioPool& audiopool=getAudioPool();

    if (state == State::Stand && time > next_state) {
        if (ppl7::rand(0, 1) == 0) {
            state=State::TurnToLeft;
            animation.startSequence(11, 20, false, 20);
            next_state=time + ppl7::randf(4.0f, 40.0f);
        } else {
            state=State::TurnToRight;
            animation.startSequence(1, 10, false, 10);
            next_state=time + ppl7::randf(4.0f, 40.0f);
        }
    } else if (state == State::Stand && time < next_state) {
        TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y + 1));
        if (t1 == TileType::NonBlocking) height-=(10 * frame_rate_compensation);
        else {
            while (ttplane.getType(ppl7::grafix::Point(p.x, p.y - 1)) == TileType::Blocking) {
                height+=1;
                p.y=initial_p.y - height;
            }
        }

    } else if (state == State::TurnToLeft && animation.isFinished()) {
        state=State::FlyLeft;
        if (audio) audiopool.playInstance(audio);
        animation.startSequence(55, 55, false, 55);
        //height=0.0f;
        land=false;
        target_height=2 * TILE_HEIGHT;
        change_height=time + ppl7::randf(1.0f, 5.0f);
    } else if (state == State::TurnToRight && animation.isFinished()) {
        state=State::FlyRight;
        if (audio) audiopool.playInstance(audio);
        animation.startSequence(60, 65, false, 65);
        //height=0.0f;
        target_height=2 * TILE_HEIGHT;
        land=false;
        change_height=time + ppl7::randf(1.0f, 5.0f);
    } else if (state == State::FlyLeft) {
        p.x-=(6 * frame_rate_compensation);
        if (p.x < initial_p.x) {
            for (int i=0;i < 4;i++) {
                TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 96, p.y - i * TILE_HEIGHT));
                if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || p.x < initial_p.x - 1000) {
                    state=State::FlyLeftToRight;
                    audiopool.playOnce(AudioClip::ghost_turn, p, 1600, 1.0f);
                    animation.startSequence(86, 100, false, 65);
                    break;
                }
            }
        }
    } else if (state == State::FlyRight) {
        p.x+=(6 * frame_rate_compensation);
        if (p.x > initial_p.x) {
            for (int i=0;i < 4;i++) {
                TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 96, p.y - i * TILE_HEIGHT));
                if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || p.x > initial_p.x + 1000) {
                    state=State::FlyRightToLeft;
                    audiopool.playOnce(AudioClip::ghost_turn, p, 1600, 1.0f);
                    animation.startSequence(71, 85, false, 55);
                    break;
                }
            }
        }
    } else if (state == State::FlyLeftToRight && animation.isFinished()) {
        state=State::FlyRight;
    } else if (state == State::FlyLeftToRight) {
        p.x-=(3 * frame_rate_compensation);
    } else if (state == State::FlyRightToLeft && animation.isFinished()) {
        state=State::FlyLeft;
    } else if (state == State::FlyRightToLeft) {
        p.x+=(3 * frame_rate_compensation);
    } else if (state == State::LandLeft && animation.isFinished()) {
        state=State::TurnToMid;
        audiopool.playOnce(AudioClip::ghost_land, p, 1600, 1.0f);
        if (audio) audiopool.stopInstace(audio);
        animation.startSequence(20, 11, false, 0);
    } else if (state == State::LandRight && animation.isFinished()) {
        state=State::TurnToMid;
        audiopool.playOnce(AudioClip::ghost_land, p, 1600, 1.0f);
        if (audio) audiopool.stopInstace(audio);
        animation.startSequence(10, 0, false, 0);
    } else if (state == State::TurnToMid && animation.isFinished()) {
        next_state=time + ppl7::randf(1.0f, 5.0f);
        state=State::Stand;
    }

    if (state == State::FlyLeft || state == State::FlyRight) {
        if (next_state < time) land=true;
        if (land == false) {
            if (height < target_height) {
                height+=2 * frame_rate_compensation;
                if (height > target_height) {
                    target_height=ppl7::randf(TILE_HEIGHT, 6 * TILE_HEIGHT);
                    //change_height=time + ppl7::randf(0.0f, 2.0f);
                }
            } else if (height > target_height) {
                height-=2 * frame_rate_compensation;
                if (height < target_height) {
                    target_height=ppl7::randf(TILE_HEIGHT, 6 * TILE_HEIGHT);
                    //change_height=time + ppl7::randf(0.0f, 2.0f);
                }
            }
            /*
            if (change_height < time) {
                change_height=time + ppl7::randf(0.0f, 1.0f);
                target_height=ppl7::randf(10, 3 * TILE_HEIGHT);
            }
            */
        } else {
            TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y));
            if (t1 != TileType::Blocking) {
                height-=(10 * frame_rate_compensation);
            } else {
                while (ttplane.getType(ppl7::grafix::Point(p.x, p.y)) == TileType::Blocking) {
                    height+=1;
                    p.y=initial_p.y - height;
                }


            //if (height > 0) height-=4 * frame_rate_compensation;
            //if (height < 0) {
                //height=0;
                if (state == State::FlyRight) {
                    state = State::LandRight;
                    animation.startSequence(65, 70, false, 10);
                } else {
                    state = State::LandLeft;
                    animation.startSequence(55, 60, false, 20);
                }
            }
        }
    }
    p.y=initial_p.y - height;
    if (audio)  audio->setPositional(p, 1200);
    if (abs(initial_p.x - p.x) > 2000 || abs(initial_p.y - p.y) > 2000) p=initial_p;
    updateBoundary();
}

void Ghost::handleCollision(Player* player, const Collision& collision)
{
    /*
    Player::PlayerMovement movement=player->getMovement();
    if (collision.onFoot() == true && movement == Player::Falling) {
        //animation.start(death_animation, sizeof(death_animation) / sizeof(int), false, 100);
        state=State::Dead;
        collisionDetection=false;
        //enabled=false;
        player->addPoints(100);
        //AudioPool& audio=getAudioPool();
        //audio.playOnce(AudioClip::skeleton_death);

    } else {
        player->dropHealth(4);
    }
    */
    player->dropHealth(4);
}

void Ghost::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
    ppl7::grafix::Color c(255, 255, 255, 255);
    c.setAlpha(glow / 2);
    SpriteTexture& lightmap=getResources().Lightmaps;
    lightmap.setTextureBlendMode(SDL_BLENDMODE_BLEND);
    lightmap.drawScaled(renderer, p.x + coords.x,
        p.y + coords.y - 2 * TILE_HEIGHT, 1, glow / 200.0f, c);
    lightmap.setTextureBlendMode(SDL_BLENDMODE_ADD);
    Enemy::draw(renderer, coords);
    Game& game=GetGame();
    SDL_Texture* lighttarget=game.getLightRenderTarget();
    if (!lighttarget) return;
    SDL_Texture* rendertarget=SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, lighttarget);
    texture->setTextureBlendMode(SDL_BLENDMODE_ADD);
    c.setAlpha(glow * 3);
    texture->draw(renderer,
        p.x + coords.x,
        p.y + coords.y,
        sprite_no, c);
    lightmap.drawScaled(renderer, p.x + coords.x,
        p.y + coords.y - 2 * TILE_HEIGHT, 1, glow / 255.0f, c);
    SDL_SetRenderTarget(renderer, rendertarget);
    texture->setTextureBlendMode(SDL_BLENDMODE_BLEND);
}



} // EOF namespace
