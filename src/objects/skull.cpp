#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Skull::representation()
{
	return Representation(Spriteset::Skull, 0);
}

Skull::Skull()
	:Enemy(Type::ObjectType::Skull)
{
	next_state=0.0f;
	next_animation=0.0f;
	aState=ActionState::Wait;
	sprite_set=Spriteset::Skull;
	sprite_no=0;
	sprite_no_representation=0;
	state=0;
	collisionDetection=true;
	updateBoundary();
	orientation=Orientation::Front;
	light.color.set(255, 255, 255, 255);
	light.sprite_no=36;
	light.scale_x=1.0f;
	light.scale_y=1.0f;
	light.custom_texture=NULL;
	light.intensity=255;
	light.plane=static_cast<int>(LightPlaneId::Player);
	light.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

	shine.color.set(0, 255, 0, 255);
	shine.sprite_no=0;
	shine.scale_x=0.15f;
	shine.scale_y=0.15f;
	shine.intensity=128;
	shine.plane=static_cast<int>(LightPlaneId::Player);
	shine.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

}

static void emmitParticles(double time, const ppl7::grafix::PointF& p)
{
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.286));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(107, 145, 104, 29)));
	color_gradient.push_back(Particle::ColorGradientItem(0.279, ppl7::grafix::Color(155, 255, 160, 29)));
	color_gradient.push_back(Particle::ColorGradientItem(0.653, ppl7::grafix::Color(125, 237, 216, 21)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(84, 100, 69, 0)));

	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(200, 300);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.104, 1.235) + time;
		particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(49, 1), 0.000);
		particle->layer=Particle::Layer::BeforePlayer;
		particle->weight=randf(0.000, 0.000);
		particle->gravity.setPoint(0.000, 0.000);
		particle->velocity=calculateVelocity(randf(1.396, 3.981), 0.000 + randf(-10.189, 10.189));
		particle->scale=randf(0.723, 1.799);
		particle->color_mod.set(210, 255, 203, 29);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		particle->initScaleGradient(scale_gradient, particle->scale);
		particle->initColorGradient(color_gradient);
		ps->addParticle(particle);
	}
}


void Skull::updateBouncing(float frame_rate_compensation)
{
	//ppl7::PrintDebugTime("Skull::updateBouncing\n");
	if (state == 0) {
		bounce_start=p;
		state=1;
		velocity.y=-ppl7::randf(0.2f, 2.0f);
	}
	if (state == 1) {
		velocity.y-=0.05f * frame_rate_compensation;
		if (p.y < bounce_start.y - 5.0f) state=2;
	} else if (state == 2) {
		velocity.y+=0.05f * frame_rate_compensation;
		if (velocity.y > 0) {
			state=3;
		}
	} else if (state == 3) {
		velocity.y+=0.05f * frame_rate_compensation;
		if (p.y >= bounce_start.y) state=4;
	} else if (state == 4) {
		velocity.y-=0.05f * frame_rate_compensation;
		if (velocity.y < 0) {
			state=1;
		}
	}
}

void Skull::newState(double time, TileTypePlane& ttplane, Player& player)
{
	next_state=time + ppl7::randf(0.2f, 2.0f);
	int r=ppl7::rand(0, 6);
	velocity.setPoint(0, 0);
	state=0;
	float distance=ppl7::grafix::Distance(ppl7::grafix::PointF(player.x, player.y), p);
	if (distance < 300) {
		aState=ActionState::Attack;
		return;
	}
	//ppl7::PrintDebugTime("Skull::newState: %d\n", r);
	switch (r) {
	case 0: aState=ActionState::GoBackToOrigin;
		break;
	case 1: if (distance < 600) aState=ActionState::Attack;
		  else aState=ActionState::Bouncing;
		break;

	default: aState=ActionState::Bouncing;

	}
}

void Skull::goToTarget(double time, float frame_rate_compensation, const ppl7::grafix::PointF& target)
{
	float dx=abs(p.x - target.x);
	float dy=abs(p.y - target.y);
	if (p.x < target.x && velocity.x < 5.0f) {
		if (dx > 20) velocity.x+=0.1 * frame_rate_compensation;
		else {
			velocity.x-=0.1 * frame_rate_compensation;
			if (velocity.x < 1.0f) velocity.x=1.0f;
		}
	} else if (p.x > target.x && velocity.x > -5.0f) {
		if (dx > 20) velocity.x-=0.1 * frame_rate_compensation;
		else {
			velocity.x+=0.1 * frame_rate_compensation;
			if (velocity.x > -1.0f) velocity.x=-1.0f;
		}
	}
	if (p.y < target.y && velocity.y < 5.0f) {
		if (dy > 20) velocity.y+=0.1 * frame_rate_compensation;
		else {
			velocity.y-=0.1 * frame_rate_compensation;
			if (velocity.y < 1.0f) velocity.y=1.0f;
		}
	} else if (p.y > target.y && velocity.y > -5.0f) {
		if (dy > 20) velocity.y-=0.1 * frame_rate_compensation;
		else {
			velocity.y+=0.1 * frame_rate_compensation;
			if (velocity.y > -1.0f) velocity.y=-1.0f;
		}
	}

}

void Skull::updateGoBackToOrigin(double time, float frame_rate_compensation)
{
	float distance=ppl7::grafix::Distance(p, initial_p);
	//ppl7::PrintDebugTime("Skull::updateGoBackToOrigin:  distance: %0.3f\n", distance);
	if (distance <= 3.0f) {
		p=initial_p;
		aState=ActionState::Stop;
		next_state=time;
		return;
	}
	goToTarget(time, frame_rate_compensation, initial_p);

}

void Skull::updateAttack(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	ppl7::grafix::PointF playerp=ppl7::grafix::PointF(player.x, player.y - 64);
	float distance=ppl7::grafix::Distance(playerp, p);
	if (distance > 1000) {
		aState=ActionState::GoBackToOrigin;
		return;
	} else if (distance < 10.0f) {
		die(time);
		return;

	}
	// check, if direct way to player is free
	ppl7::grafix::PointF checkp=p;
	while (ttplane.getType(checkp) != TileType::Blocking) {
		distance=ppl7::grafix::Distance(playerp, checkp);
		if (distance < 20) {
			goToTarget(time, frame_rate_compensation, playerp);
			return;
		}
		if (playerp.x < checkp.x) checkp.x-=10.0f;
		if (playerp.x > checkp.x) checkp.x+=10.0f;
		if (playerp.y < checkp.y) checkp.y-=10.0f;
		if (playerp.y > checkp.y) checkp.y+=10.0f;
	}
	aState=ActionState::GoBackToOrigin;
}

void Skull::die(double time) {
	state=5;
	animation.startSequence(24, 35, false, 35);
	collisionDetection=false;
	//velocity.y=1.0f;
	emmitParticles(time, p);
	myLayer = Layer::BeforeBricks;
	aState = ActionState::Dead;
}

void Skull::turn()
{
	if (animation.isFinished()) {
		if (velocity.x < 0 && orientation == Orientation::Right) {
			animation.startSequence(21, 11, false, 0);
			orientation=Orientation::Front;
		} else if (velocity.x < 0 && orientation == Orientation::Front) {
			animation.startSequence(0, 10, false, 22);
			orientation=Orientation::Left;
		} else if (velocity.x > 0 && orientation == Orientation::Left) {
			animation.startSequence(10, 0, false, 0);
			orientation=Orientation::Front;
		} else if (velocity.x > 0 && orientation == Orientation::Front) {
			animation.startSequence(11, 21, false, 23);
			orientation=Orientation::Right;
		} else if (velocity.x == 0.0f && orientation == Orientation::Right) {
			animation.startSequence(21, 11, false, 0);
			orientation=Orientation::Front;
		} else if (velocity.x == 0.0f && orientation == Orientation::Left) {
			animation.startSequence(10, 0, false, 0);
			orientation=Orientation::Front;
		}

	}
}

void Skull::updateStop(double time, float frame_rate_compensation)
{
	if (velocity.x == 0.0f && velocity.y == 0.0f) {
		aState = ActionState::Wait;
		next_state=time;
	}
	stopHorizontalVelocity(frame_rate_compensation);
	if (velocity.y > 0.0f) {
		velocity.y -=0.2f * frame_rate_compensation;
		if (velocity.y < 0.0f) velocity.y=0.0f;
	} else 	if (velocity.y < 0.0f) {
		velocity.y +=0.2f * frame_rate_compensation;
		if (velocity.y > 0.0f) velocity.y=0.0f;
	}
}

void Skull::stopHorizontalVelocity(float frame_rate_compensation)
{
	if (velocity.x > 0.0f) {
		velocity.x -=0.2f * frame_rate_compensation;
		if (velocity.x < 0.0f) velocity.x=0.0f;
	} else 	if (velocity.x < 0.0f) {
		velocity.x +=0.2f * frame_rate_compensation;
		if (velocity.x > 0.0f) velocity.x=0.0f;
	}
}

void Skull::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (!enabled) return;
	if (time > next_animation) {
		next_animation=time + 0.033f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	//ppl7::PrintDebugTime("velocity=%0.3f : %0.3f, aState=%d\n", velocity.x, velocity.y, (int)aState);
	if (next_state == 0.0f) next_state=time + ppl7::randf(0.2f, 4.0f);

	float distance=ppl7::grafix::Distance(ppl7::grafix::PointF(player.x, player.y), p);
	if (distance < 300 && aState != ActionState::Dead) aState=ActionState::Attack;



	if ((aState == ActionState::Wait || aState == ActionState::Bouncing) && next_state < time) newState(time, ttplane, player);
	else if (aState == ActionState::Bouncing) updateBouncing(frame_rate_compensation);
	else if (aState == ActionState::GoBackToOrigin) updateGoBackToOrigin(time, frame_rate_compensation);
	else if (aState == ActionState::Attack) updateAttack(time, ttplane, player, frame_rate_compensation);
	else if (aState == ActionState::Stop) updateStop(time, frame_rate_compensation);
	else if (aState == ActionState::Dead) {
		velocity.y+=0.2f * frame_rate_compensation;
		if (velocity.x != 0.0f) stopHorizontalVelocity(frame_rate_compensation);
		if (p.y > initial_p.y + 1080) {
			enabled=false;
		}
	}
	turn();
	p+=velocity;

	updateBoundary();


	if (player.isFlashlightOn() && aState != ActionState::Dead) {
		float y_dist=abs((player.y - 40) - p.y);
		float x_dist=abs(player.x - p.x);
		if (y_dist < 2 * TILE_HEIGHT && x_dist < 4 * TILE_WIDTH) {
			if ((player.orientation == Player::PlayerOrientation::Left && p.x < player.x) ||
				(player.orientation == Player::PlayerOrientation::Right && p.x > player.x)) {
					// In light cone
				die(time);
				player.addPoints(20);
			}
		}
	}
	light.custom_texture=this->texture;
	light.sprite_no=36 + sprite_no;
	light.x=p.x;
	light.y=p.y;
	shine.x=p.x;
	shine.y=p.y - 1;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&shine);
	lights.addObjectLight(&light);
}

void Skull::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(2);
}



}	// EOF namespace Decker::Objects
