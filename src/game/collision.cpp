#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "player.h"


namespace Decker::Objects {

Collision::Collision()
{
    object=NULL;
}

Collision::Collision(const Collision& other)
{
    object=other.object;
    collision_points=other.collision_points;
    bounding_box_object=other.bounding_box_object;
    bounding_box_player=other.bounding_box_player;
    bounding_box_intersection=other.bounding_box_intersection;
    frame_rate_compensation=other.frame_rate_compensation;
}


Collision::Collision(const Player* player, const Object* object, float frame_rate_compensation)
{
    this->frame_rate_compensation=frame_rate_compensation;
    bounding_box_player=player->getBoundingBox();
    bounding_box_object=object->boundary;
    bounding_box_intersection=bounding_box_player.intersected(bounding_box_object);
}

void Collision::detect(Object* object, const std::list<ppl7::grafix::Point>& checkpoints, const Player& player)
{
    collision_points.clear();
    this->object=object;
    std::list<ppl7::grafix::Point>::const_iterator p_it;
    const ppl7::grafix::Drawable draw=object->texture->getDrawable(object->sprite_no);
    for (p_it=checkpoints.begin();p_it != checkpoints.end();++p_it) {
        if (draw.width()) {
            int x=(*p_it).x - object->boundary.x1;
            int y=(*p_it).y - object->boundary.y1;
            ppl7::grafix::Color c=draw.getPixel(x, y);
            if (c.alpha() > 92) {
                collision_points.push_back(ppl7::grafix::Point((*p_it).x - player.x, (*p_it).y - player.y));
            }
        }
    }
}

const std::list<ppl7::grafix::Point>& Collision::getCollisionPoints() const
{
    return collision_points;
}

Object* Collision::getObject() const
{
    return object;
}

bool Collision::onFoot() const
{
    int height=bounding_box_object.height();
    if (height > 2 * TILE_HEIGHT) height=height * 2 / 3;
    else height=height / 2;
    if (bounding_box_intersection.y2 <= bounding_box_object.y2 - height) {
        return true;
    }
    return false;
}

bool Collision::objectBottom() const
{
    if (bounding_box_player.y2 > bounding_box_object.y2 &&
        bounding_box_player.y1<bounding_box_object.y2 &&
        bounding_box_player.y1>bounding_box_object.y1) {
        return true;
    }
    return false;
}

bool Collision::objectTop() const
{
    if (bounding_box_player.y2<bounding_box_object.y2 &&
        bounding_box_player.y2>bounding_box_object.y1 &&
        bounding_box_player.y1 < bounding_box_object.y1) {
        return true;
    }
    return false;

}

bool Collision::objectLeft() const
{
    if (bounding_box_player.x2<bounding_box_object.x2 &&
        bounding_box_player.x2>bounding_box_object.x1 &&
        bounding_box_player.x1 < bounding_box_object.x1) {
        return true;
    }
    return false;
}

bool Collision::objectRight() const
{
    if (bounding_box_player.x1 > bounding_box_object.x1 &&
        bounding_box_player.x1<bounding_box_object.x2 &&
        bounding_box_player.x2>bounding_box_object.x2) {
        return true;
    }
    return false;
}


bool Collision::objectBottom(int t) const
{
    if (bounding_box_player.y1 > bounding_box_object.y2 - t &&
        bounding_box_player.y1 < bounding_box_object.y2) {
        return true;
    }
    return false;
}

bool Collision::objectTop(int t) const
{
    if (bounding_box_player.y2<bounding_box_object.y1 + t &&
        bounding_box_player.y2>bounding_box_object.y1) {
        return true;
    }
    return false;

}

bool Collision::objectLeft(int t) const
{
    if (bounding_box_player.x2<bounding_box_object.x1 + t &&
        bounding_box_player.x2>bounding_box_object.x1) {
        return true;
    }
    return false;
}

bool Collision::objectRight(int t) const
{
    if (bounding_box_player.x1 > bounding_box_object.x2 - t &&
        bounding_box_player.x1 < bounding_box_object.x2) {
        return true;
    }
    return false;
}

bool Collision::playerBottom(int t) const
{
    if (bounding_box_object.y2 > bounding_box_player.y2 &&
        bounding_box_object.y1<bounding_box_player.y2 &&
        bounding_box_object.y1>bounding_box_player.y2 - t) {
        return true;
    }
    return false;
}

bool Collision::playerTop(int t) const
{
    if (bounding_box_object.y2<bounding_box_player.y1 + t &&
        bounding_box_object.y2>bounding_box_player.y1 &&
        bounding_box_object.y1 < bounding_box_player.y1) {
        return true;
    }
    return false;

}

bool Collision::playerLeft(int t) const
{
    if (bounding_box_player.x1 > bounding_box_object.x2 - t &&
        bounding_box_player.x1 < bounding_box_object.x2) {
        return true;
    }
    return false;
}

bool Collision::playerRight(int t) const
{
    if (bounding_box_object.x1 > bounding_box_player.x1 &&
        bounding_box_object.x1<bounding_box_player.x2 - t &&
        bounding_box_object.x2>bounding_box_player.x2) {
        return true;
    }
    return false;
}


ObjectCollision::ObjectCollision(const Object* this_object, const Object* other_object)
{
    this->this_object=this_object;
    this->other_object=other_object;
    bounding_box_this_object=this_object->boundary;
    bounding_box_other_object=other_object->boundary;
    bounding_box_intersection=bounding_box_this_object.intersected(bounding_box_other_object);
}

void ObjectCollision::update()
{
    bounding_box_this_object=this_object->boundary;
    bounding_box_other_object=other_object->boundary;
    bounding_box_intersection=bounding_box_this_object.intersected(bounding_box_other_object);

}

bool ObjectCollision::objectTop(int tolerance) const
{
    if (bounding_box_this_object.y1 < bounding_box_other_object.y2 &&
        bounding_box_this_object.y1 > bounding_box_other_object.y1) {
        return true;
    }
    return false;
}

bool ObjectCollision::objectBottom(int tolerance) const
{
    if (bounding_box_this_object.y2 > bounding_box_other_object.y1 &&
        bounding_box_this_object.y2 < bounding_box_other_object.y2) {
        return true;
    }
    return false;
}

bool ObjectCollision::objectLeft(int tolerance) const
{
    if (bounding_box_this_object.x1 < bounding_box_other_object.x2 &&
        bounding_box_this_object.x1> bounding_box_other_object.x1) {
        return true;
    }
    return false;
}

bool ObjectCollision::objectRight(int tolerance) const
{
    if (bounding_box_this_object.x2 > bounding_box_other_object.x1 &&
        bounding_box_this_object.x2 < bounding_box_other_object.x2) {
        return true;
    }
    return false;
}


} // EOF namespace Decker::Objects
