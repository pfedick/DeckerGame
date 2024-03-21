#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation LightSignal::representation()
{
    return Representation(Spriteset::GenericObjects, 2);
}

LightSignal::LightSignal()
    :Object(Type::ObjectType::LightSignal)
{
    color_on.set(0, 192, 0, 255);
    color_off.set(192, 0, 0, 255);
    dualColor=false;
    initialState=true;
    currentState=true;
    sprite_set=Spriteset::GenericObjects;
    sprite_no=2;
    sprite_no_representation=2;

    led.sprite_no=0;
    led.scale_x=0.2f;
    led.scale_y=0.2f;
    led.custom_texture=NULL;
    led.intensity=255;
    led.plane=static_cast<int>(LightPlaneId::Player);
    led.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);
    led.has_lensflare=true;
    led.flare_intensity=128;
    led.flare_useLightColor=true;
    led.flarePlane=static_cast<int>(LightPlayerPlaneMatrix::Back);
}

LightSignal::~LightSignal()
{

}

void LightSignal::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
    if (currentState) {
        led.color=color_on;
        led.x=p.x;
        led.y=p.y;
        LightSystem& lights=GetGame().getLightSystem();
        lights.addObjectLight(&led);
        sprite_no=2;
        color_mod=color_on;
    } else if (dualColor) {
        led.color=color_off;
        led.x=p.x;
        led.y=p.y;
        LightSystem& lights=GetGame().getLightSystem();
        lights.addObjectLight(&led);
        sprite_no=2;
        color_mod=color_off;
    } else {
        sprite_no=1;
        color_mod=color_on;
    }

}

void LightSignal::toggle(bool enable, Object* source)
{
    currentState=enable;
}

void LightSignal::trigger(Object* source)
{
    toggle(!currentState, source);
}

size_t LightSignal::saveSize() const
{
    return Object::saveSize() + 10;
}

size_t LightSignal::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 1);		// Object Version
    uint8_t flags=0;
    if (initialState) flags|=1;
    if (dualColor) flags|=2;
    ppl7::Poke8(buffer + bytes + 1, flags);
    ppl7::Poke32(buffer + bytes + 2, color_on.color());
    ppl7::Poke32(buffer + bytes + 6, color_off.color());
    return bytes + 10;

}

size_t LightSignal::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0) return 0;
    if (size < bytes + 10) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version != 1) return 0;
    uint8_t flags=ppl7::Peek8(buffer + bytes + 1);
    initialState=flags & 1;
    currentState=initialState;
    dualColor=flags & 2;
    color_on.setColor(ppl7::Peek32(buffer + bytes + 2));
    color_off.setColor(ppl7::Peek32(buffer + bytes + 6));
    return size;

}

void LightSignal::openUi()
{

}


}	// EOF namespace Decker::Objects
