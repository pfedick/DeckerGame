#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "player.h"
#include "widgets.h"

namespace Decker::Objects {

static int portal_animation[]={ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,22,23,24,25,26,27,28,29,30 };


LevelEnd::LevelEnd()
    :Object(Type::ObjectType::LevelEnd)
{
    sprite_set=Spriteset::LevelEnd;
    sprite_no_representation=1;
    state=State::Inactive;
    flags=static_cast<Flags>(0);
    key_id=0;
    collisionDetection=false;
    next_animation=0.0;
    audio=NULL;
}

LevelEnd::~LevelEnd()
{
    if (audio) {
        getAudioPool().stopInstace(audio);
        delete audio;
        audio=NULL;
    }
}

Representation LevelEnd::representation()
{
    return Representation(Spriteset::LevelEnd, 1);
}

void LevelEnd::init()
{

}

void LevelEnd::update(double time, TileTypePlane& ttplane, Player& player)
{
    if (state == State::Active) {
        collisionDetection=true;
        if (time > next_animation) {
            next_animation=time + 0.056f;
            animation.update();
            int new_sprite=animation.getFrame();
            if (new_sprite != sprite_no) {
                sprite_no=new_sprite;
                updateBoundary();
            }
        }


    } else {

    }
}

void LevelEnd::handleCollision(Player* player, const Collision& collision)
{

}

size_t LevelEnd::save(unsigned char* buffer, size_t size)
{
    return 0;
}

size_t LevelEnd::load(const unsigned char* buffer, size_t size)
{
    return 0;
}

void LevelEnd::reset()
{

}

void LevelEnd::openUi()
{

}

void LevelEnd::toggle(bool enable, Object* source)
{

}




}   // EOF namespace
