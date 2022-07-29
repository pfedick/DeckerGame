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
	next_state_change=0.0f;
	velocity=3;
}

void Bird::changeState(BirdState state)
{
	switch (state) {
	case BirdState::FlyLeft:
		animation.startSequence(12, 21, true, 0);
		velocity=randf(3.0, 5.0);
		break;
	case BirdState::FlyRight:
		animation.startSequence(23, 32, true, 0);
		velocity=randf(3.0, 5.0);
		break;
	case BirdState::FlyFront:
		animation.startSequence(1, 10, true, 0);
		break;
	case BirdState::FlyBack:
		animation.startSequence(34, 43, true, 0);
		break;
	case BirdState::AttackLeft:
		animation.setStaticFrame(45);
		velocity=10;
		break;
	case BirdState::AttackRight:
		animation.setStaticFrame(46);
		velocity=10;
		break;
	case BirdState::FlyOnPlace:
		if (ppl7::rand(0, 1)) animation.startSequence(1, 10, true, 0);
		else animation.startSequence(34, 43, true, 0);
		break;
	case BirdState::WaitThenFlyUp:
		if (ppl7::rand(0, 1)) animation.setStaticFrame(4);
		else animation.setStaticFrame(37);
		break;
	case BirdState::FlyUp:
		animation.startSequence(1, 10, true, 0);
		velocity=randf(3.0, 5.0);
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
			changeState(BirdState::FlyOnPlace);
			next_state_change=time + randf(1.0, 5.0f);
		}
	} else 	if (state == BirdState::FlyRight) {
		p.x+=velocity * frame_rate_compensation;
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x + 32, p.y));
		if (t1 != TileType::NonBlocking) {
			changeState(BirdState::FlyOnPlace);
			next_state_change=time + randf(1.0, 5.0f);
		}
	} else 	if (state == BirdState::FlyUp) {
		p.y-=velocity * frame_rate_compensation;
		if (p.y < initial_p.y) {
			p.y=initial_p.y;
			changeState(BirdState::FlyOnPlace);
			next_state_change=time + randf(0.2, 1.0f);
		}
	} else if (state == BirdState::FlyOnPlace && time > next_state_change) {
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x - 32, p.y));
		if (t1 != TileType::NonBlocking) {
			changeState(BirdState::FlyRight);
		} else {
			changeState(BirdState::FlyLeft);
		}
	} else if (state == BirdState::AttackRight) {
		p.x+=velocity * frame_rate_compensation;
		p.y+=velocity * frame_rate_compensation;
	} else if (state == BirdState::AttackLeft) {
		p.x-=velocity * frame_rate_compensation;
		p.y+=velocity * frame_rate_compensation;
	} else if (state == BirdState::WaitThenFlyUp && time > next_state_change) {
		changeState(BirdState::FlyUp);
	}
	if (state == BirdState::AttackRight || state == BirdState::AttackLeft) {
		TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y));
		if (t1 != TileType::NonBlocking && p.y > player.position().y - 10) {
			changeState(BirdState::WaitThenFlyUp);
			next_state_change=time + randf(1.0, 5.0f);
		}
	}
	if ((state == BirdState::FlyLeft || state == BirdState::FlyRight) && time > next_state_change) {
		double dist=ppl7::grafix::Distance(p, player.position());
		if (dist < 800) {
			ppl7::grafix::PointF d=player.position() - p;
			if (abs(d.x) > abs(d.y) - 16 && abs(d.x) < abs(d.y) + 16) {
				if (p.x < player.position().x && state == BirdState::FlyRight) {
					if (ppl7::rand(1, 3) == 1) {	// Decide to attack?
						changeState(BirdState::AttackRight);
					}
					next_state_change=time + 2.0;
				} else if (p.x > player.position().x && state == BirdState::FlyLeft) {
					if (ppl7::rand(1, 3) == 1) {	// Decide to attack?
						changeState(BirdState::AttackLeft);
					}
					next_state_change=time + 2.0;
				}
			}
			//printf("Bird: Player in Range, x=%d, y=%d\n", (int)d.x, (int)d.y);
		}

	}
	updateBoundary();
}




}	// EOF namespace Decker::Objects
