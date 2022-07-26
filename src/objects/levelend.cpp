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
    color_doorframe.set(255, 255, 255, 255);
    color_background.set(255, 255, 255, 255);
    color_puddle.set(255, 255, 255, 255);
    //save_size+=18;
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
    }
}

void LevelEnd::handleCollision(Player* player, const Collision& collision)
{

}

size_t LevelEnd::save(unsigned char* buffer, size_t size)
{
    //if (size < save_size) return 0;
    size_t bytes=Object::save(buffer, size);
    return bytes;
    ppl7::Poke8(buffer + bytes + 0, 1); // Version 1
    ppl7::Poke16(buffer + bytes + 1, static_cast<int>(flags));
    ppl7::Poke8(buffer + bytes + 3, color_doorframe.red());
    ppl7::Poke8(buffer + bytes + 4, color_doorframe.green());
    ppl7::Poke8(buffer + bytes + 5, color_doorframe.blue());
    ppl7::Poke8(buffer + bytes + 6, color_doorframe.alpha());
    ppl7::Poke8(buffer + bytes + 7, color_background.red());
    ppl7::Poke8(buffer + bytes + 8, color_background.green());
    ppl7::Poke8(buffer + bytes + 9, color_background.blue());
    ppl7::Poke8(buffer + bytes + 10, color_background.alpha());
    ppl7::Poke8(buffer + bytes + 11, color_puddle.red());
    ppl7::Poke8(buffer + bytes + 12, color_puddle.green());
    ppl7::Poke8(buffer + bytes + 13, color_puddle.blue());
    ppl7::Poke8(buffer + bytes + 14, color_puddle.alpha());
    ppl7::Poke32(buffer + bytes + 15, color_puddle.alpha());
    ppl7::Poke8(buffer + bytes + 19, next_level.size());
    // 20

    return bytes + 20 + next_level.size();
}

size_t LevelEnd::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    return bytes;
}

void LevelEnd::reset()
{

}

void LevelEnd::openUi()
{

}

void LevelEnd::toggle(bool enable, Object* source)
{
    if (enable) {
        animation.startRandom(portal_animation, sizeof(portal_animation) / sizeof(int), true, 0);
        state=State::Active;


    } else {
        if (audio) {
            getAudioPool().stopInstace(audio);
            delete audio;
            audio=NULL;
        }
        sprite_no=0;
        state=State::Inactive;
    }
}




}   // EOF namespace
