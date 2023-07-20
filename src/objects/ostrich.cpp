#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "audiopool.h"

namespace Decker::Objects {

static int walk_cycle_left[]={ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
static int walk_cycle_right[]={ 20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35 };
static int turn_from_left_to_right[]={ 0,17,19 };
static int turn_from_right_to_left[]={ 19,18,0 };
static int death_animation_left[]={ 45,46,47,48,49,50,51 };
static int death_animation_right[]={ 37,38,39,40,41,42,43 };

/*
 * states:
 * 0: stand, looking to front
 * 1: walk left
 */

Representation Ostrich::representation()
{
    return Representation(Spriteset::Ostrich, 0);
}

Ostrich::Ostrich()
    :Enemy(Type::ObjectType::Ostrich)
{
    sprite_set=Spriteset::Ostrich;
    sprite_no=17;
    animation.setStaticFrame(17);
    sprite_no_representation=0;
    next_state=ppl7::GetMicrotime() + (double)ppl7::rand(5, 20);
    state=0;
    next_animation=0.0f;
    collisionDetection=true;
    updateBoundary();
    audio=NULL;
}

Ostrich::~Ostrich()
{
    if (audio) {
        getAudioPool().stopInstace(audio);
        delete audio;
        audio=NULL;
    }
}

void Ostrich::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
    if (state > 6) return;

    if (time > next_animation) {
        next_animation=time + 0.05f;
        animation.update();
        int new_sprite=animation.getFrame();
        if (new_sprite != sprite_no) {
            sprite_no=new_sprite;
            updateBoundary();
        }
    }

    if (state == 0 && animation.isFinished()) {
        state=1;
        animation.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
    } else if (state == 1) {	// walk left
        p.x-=3 * frame_rate_compensation;
        updateBoundary();
        TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 20, p.y - 6));
        TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x - 20, p.y + 6));
        if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || t2 != TileType::Blocking) {
            state=2;
            animation.setStaticFrame(0);
            next_state=time + (double)ppl7::rand(1, 5);
        }
    } else if (state == 2 && time > next_state) {
        getAudioPool().playOnce(AudioClip::skeleton_turn, p, 1600, 1.0f);
        getAudioPool().playOnce(AudioClip::turkey_gobble, p, 1600, 0.4f);
        animation.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 20);

        state=3;
    } else if (state == 3 && animation.isFinished()) {
        state=4;
        animation.start(walk_cycle_right, sizeof(walk_cycle_right) / sizeof(int), true, 20);

    } else if (state == 4) {
        p.x+=3 * frame_rate_compensation;
        updateBoundary();
        TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 20, p.y - 6));
        TileType::Type t2=ttplane.getType(ppl7::grafix::Point(p.x + 20, p.y + 6));
        if (t1 == TileType::Blocking || t1 == TileType::EnemyBlocker || t2 != TileType::Blocking) {
            state=5;
            animation.setStaticFrame(20);
            next_state=time + (double)ppl7::rand(1, 5);
        }
    } else if (state == 5 && time > next_state) {
        getAudioPool().playOnce(AudioClip::skeleton_turn, p, 1600, 1.0f);
        getAudioPool().playOnce(AudioClip::turkey_gobble, p, 1600, 0.4f);
        animation.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 0);
        state=0;
    } else if (state == 6) {
        if (animation.isFinished()) {
            getAudioPool().playOnce(AudioClip::digging, p, 1600, 0.4f);
            state=7;
            next_state=time + 5.0f;
        }
    }



    if (!audio && state < 6) {
        AudioPool& audiopool=getAudioPool();
        audio=audiopool.getInstance(AudioClip::turkey_sound);
        if (audio) {
            audio->setVolume(0.4f);
            audio->setAutoDelete(false);
            audio->setLoop(true);
            audio->setPositional(p, 960);
            audiopool.playInstance(audio);
        }
    } else if (audio) {
        audio->setPositional(p, 960);
    }
}

void Ostrich::handleCollision(Player* player, const Collision& collision)
{
    Player::PlayerMovement movement=player->getMovement();
    if (collision.onFoot() == true && movement == Player::Falling) {
        if (state == 0 || state == 1 || state == 2) {
            animation.start(death_animation_left, sizeof(death_animation_left) / sizeof(int), false, 51);
        } else if (state == 3 || state == 4 || state == 5) {
            animation.start(death_animation_right, sizeof(death_animation_right) / sizeof(int), false, 43);
        }
        AudioPool& audiopool=getAudioPool();

        if (audio) {
            audiopool.stopInstace(audio);
            delete audio;
            audio=NULL;
        }

        state=6;
        collisionDetection=false;
        //enabled=false;
        player->addPoints(200);
        audiopool.playOnce(AudioClip::turkey_gobble, 0.4);
    } else {
        player->dropHealth(4);
    }
}

}	// EOF namespace Decker::Objects
