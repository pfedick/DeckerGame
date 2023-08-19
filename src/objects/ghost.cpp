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
    next_state=ppl7::GetMicrotime() + (double)ppl7::rand(5, 20);
    state=State::Stand;
    orientation=Orientation::Front;
    next_animation=0.0f;
    collisionDetection=true;
    color_mod.setAlpha(128);
    glow=128.0f;
    glowtarget=64.0f;
    glowspeed=2.0f;
    myLayer=Layer::BeforePlayer;
    //glowtime=0.0f;
    //animation.start(turn_from_mid_to_left, sizeof(turn_from_mid_to_left) / sizeof(int), false, 0);
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

void Ghost::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
    //myLayer=Layer::BeforePlayer;
    update_glow(time, frame_rate_compensation);

}

void Ghost::handleCollision(Player* player, const Collision& collision)
{
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
}



} // EOF namespace
