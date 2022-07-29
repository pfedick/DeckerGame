#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

// front=1-10
// left=12-21
// right=23-32
// back=34-43
// attack left=45
// attack right=46

namespace Decker::Objects {


Representation Bird::representation()
{
	return Representation(Spriteset::Bird, 0);
}

Bird::Bird()
	:Enemy(Type::ObjectType::Bird)
{
	sprite_set=Spriteset::Bird;
	sprite_no=0;
	sprite_no_representation=0;
	if (ppl7::rand(0, 1)) changeState(BirdState::FlyRight);
	else changeState(BirdState::FlyLeft);
	collisionDetection=true;
	next_animation=0.0f;
	velocity=5;
}

void Bird::changeState(BirdState state)
{
	switch (state) {
	case BirdState::FlyLeft:
		animation.startSequence(12, 21, true, 0);
		break;
	case BirdState::FlyRight:
		animation.startSequence(23, 32, true, 0);
		break;
	case BirdState::FlyFront:
		animation.startSequence(1, 10, true, 0);
		break;
	case BirdState::FlyBack:
		animation.startSequence(34, 43, true, 0);
		break;
	case BirdState::AttackLeft:
		animation.setStaticFrame(45);
		break;
	case BirdState::AttackRight:
		animation.setStaticFrame(46);
		break;

	}
	this->state=state;
}

void Bird::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(10);
}

void Bird::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (time > next_animation) {
		next_animation=time + 0.03f;
		animation.update();
		if (animation.getFrame() != sprite_no) {
			sprite_no=animation.getFrame();
		}
	}
	if (state == BirdState::FlyLeft) {
		p.x-=velocity * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y));
		if (t1 != TileType::NonBlocking) {
			changeState(BirdState::FlyRight);
		}
	} else 	if (state == BirdState::FlyRight) {
		p.x+=velocity * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y));
		if (t1 != TileType::NonBlocking) {
			changeState(BirdState::FlyLeft);
		}
	}
	updateBoundary();
}




}	// EOF namespace Decker::Objects
