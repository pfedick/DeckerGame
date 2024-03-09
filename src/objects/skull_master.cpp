#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {



Representation SkullMaster::representation()
{
	return Representation(Spriteset::Skull, 0);
}

SkullMaster::SkullMaster()
	:Enemy(Type::ObjectType::SkullMaster)
{
	next_state=0.0f;
	next_animation=0.0f;
	aState=ActionState::Wait;
	sprite_set=Spriteset::SkullMaster;
	sprite_no=0;
	sprite_no_representation=0;
	wait_state=0;
	next_birth=0.0f;
	health=100.0f;
	bounce_distance=0.0f;
	collision_cooldown=0.0f;
	bounce_speed=0.2f;
	max_bounce_velocity=1.0f;
	collisionDetection=true;
	updateBoundary();
	orientation=Orientation::Front;
	lightmap.color.set(255, 255, 255, 255);
	lightmap.sprite_no=36;
	lightmap.scale_x=1.0f;
	lightmap.scale_y=1.0f;
	lightmap.custom_texture=NULL;
	lightmap.intensity=255;
	lightmap.plane=static_cast<int>(LightPlaneId::Player);
	lightmap.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);
	shine.color.set(0, 255, 0, 255);
	shine.sprite_no=0;
	shine.scale_x=0.6f;
	shine.scale_y=0.6f;
	shine.intensity=128;
	shine.myType=LightType::Fire;
	shine.typeParameter=0.3;
	fire_cooldown=0.0f;
	voice_cooldown=0.0f;
	shine.plane=static_cast<int>(LightPlaneId::Player);
	shine.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);

	scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.269));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.509, 1.000));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 0.376));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(255, 210, 0, 255)));
	color_gradient.push_back(Particle::ColorGradientItem(0.216, ppl7::grafix::Color(255, 44, 0, 101)));
	color_gradient.push_back(Particle::ColorGradientItem(0.414, ppl7::grafix::Color(51, 0, 29, 101)));
	color_gradient.push_back(Particle::ColorGradientItem(0.541, ppl7::grafix::Color(0, 0, 0, 101)));
	color_gradient.push_back(Particle::ColorGradientItem(0.671, ppl7::grafix::Color(125, 123, 124, 65)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(125, 123, 124, 0)));

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


void SkullMaster::fire(double time, Player& player)
{
	Fireball* particle=new Fireball();
	particle->p=p;
	float direction=180.0f;
	if (orientation == Orientation::Left) {
		direction=270;
		animation.startSequence(11, 19, false, 10);
		particle->p.x-=64;
	} else if (orientation == Orientation::LeftDown) {
		direction=270 + 30;
		animation.startSequence(11, 19, false, 10);
		particle->p.x-=45;
	} else if (orientation == Orientation::Right) {
		direction=90;
		animation.startSequence(46, 54, false, 45);
		particle->p.x+=64;
	} else if (orientation == Orientation::RightDown) {
		direction=90 + 30;
		animation.startSequence(61, 69, false, 60);
		particle->p.x+=45;
	} else if (orientation == Orientation::FrontDown) {
		direction=180;
		animation.startSequence(85, 93, false, 84);
	} else {
		direction=180;
		animation.startSequence(71, 79, false, 0);
	}
	if (health > 30) fire_cooldown=time + ppl7::randf(0.5f, 1.5f);
	else fire_cooldown=time + ppl7::randf(0.2f, 0.8f);


	particle->p.y+=80;
	particle->initial_p=p;
	particle->spawned=true;
	particle->sprite_no=300;
	particle->player_damage=5.0f;
	particle->color_mod.set(64, 192, 0, 255);
	particle->sprite_set=Spriteset::GenericObjects;
	particle->sprite_no_representation=300;
	particle->velocity=calculateVelocity(ppl7::randf(9.0f, 12.0f), direction);
	particle->gravity=ppl7::randf(0.1f, 0.6f);
	particle->direction=180 + direction;
	GetObjectSystem()->addObject(particle);
	getAudioPool().playOnce(AudioClip::skull_shoot, p, 1600, 0.7f);
}

void SkullMaster::turn(Orientation target)
{
	if (orientation == target) return;
	if (animation.isFinished()) {
		if (orientation == Orientation::LeftDown) {
			orientation=Orientation::Left;
			animation.startSequence(25, 20, false, 10);
		} else if (orientation == Orientation::RightDown) {
			orientation=Orientation::Right;
			animation.startSequence(60, 55, false, 45);
		} else if (orientation == Orientation::FrontDown) {
			orientation=Orientation::Front;
			animation.startSequence(25, 20, false, 20);
		} else if (orientation == Orientation::Front && target == Orientation::FrontDown) {
			orientation=Orientation::FrontDown;
			animation.startSequence(79, 84, false, 84);
		} else if (orientation == Orientation::Front && (target == Orientation::Left || target == Orientation::LeftDown)) {
			orientation=Orientation::Left;
			animation.startSequence(0, 10, false, 10);
		} else if (orientation == Orientation::Front && (target == Orientation::Right || target == Orientation::RightDown)) {
			orientation=Orientation::Right;
			animation.startSequence(35, 45, false, 45);
		} else if (orientation == Orientation::Left && target == Orientation::LeftDown) {
			orientation=Orientation::LeftDown;
			animation.startSequence(20, 25, false, 25);
		} else if (orientation == Orientation::Right && target == Orientation::RightDown) {
			orientation=Orientation::RightDown;
			animation.startSequence(55, 60, false, 60);
		} else if (orientation == Orientation::Left && (target == Orientation::Front || target == Orientation::FrontDown || target == Orientation::Right || target == Orientation::RightDown)) {
			orientation=Orientation::Front;
			animation.startSequence(10, 0, false, 0);
		} else if (orientation == Orientation::Right && (target == Orientation::Front || target == Orientation::FrontDown || target == Orientation::Left || target == Orientation::LeftDown)) {
			orientation=Orientation::Front;
			animation.startSequence(45, 35, false, 0);
		}

	}
}

void SkullMaster::update_bounce(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	switch (wait_state) {
		case 0:	// increase speed going up
			if (velocity.y > -max_bounce_velocity) velocity.y-=bounce_speed * frame_rate_compensation;
			if (p.y < initial_p.y - bounce_distance) wait_state=1;
			break;
		case 1: // decrease speed going up
			if (velocity.y < 0.0f) velocity.y+=(bounce_speed / 2.0f) * frame_rate_compensation;
			if (velocity.y >= 0) {
				velocity.y=0.0f;
				wait_state=2;
			}
			break;
		case 2: // increase speed going down
			if (velocity.y < max_bounce_velocity) velocity.y+=bounce_speed * frame_rate_compensation;
			if (p.y > initial_p.y + bounce_distance) wait_state=3;
			break;
		case 3: // decrease speed going down
			if (velocity.y > 0.0f) velocity.y-=(bounce_speed / 2.0f) * frame_rate_compensation;
			if (velocity.y <= 0) {
				velocity.y=0.0f;
				wait_state=0;
			}
			break;
	}
}
void SkullMaster::update_wait(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (p.x > initial_p.x) {
		p.x-=0.5f * frame_rate_compensation;
		if (p.x < initial_p.x) p.x=initial_p.x;
	} else if (p.x < initial_p.x) {
		p.x+=0.5f * frame_rate_compensation;
		if (p.x > initial_p.x) p.x=initial_p.x;
	}
	if (health < 100.0f) health+=0.1f * frame_rate_compensation;

	bounce_distance=32;
	shine.color.set(0, 255, 0, 255);
	shine.intensity=128;
	bounce_speed=0.2f;
	max_bounce_velocity=1.0f;

	if (animation.isFinished()) {
		if (time > next_state) {
			next_state=time + ppl7::randf(1.0f, 5.0f);
			if (orientation == Orientation::Left) {
				orientation=Orientation::Front;
				animation.startSequence(10, 0, false, 0);
			} else if (orientation == Orientation::Right) {
				orientation=Orientation::Front;
				animation.startSequence(45, 35, false, 0);
			} else if (orientation == Orientation::Front) {
				int r=ppl7::rand(0, 1);
				if (r == 0) {
					orientation=Orientation::Left;
					animation.startSequence(0, 10, false, 10);
				} else {
					orientation=Orientation::Right;
					animation.startSequence(35, 45, false, 45);
				}
			}
		}
		if (orientation == Orientation::LeftDown) {
			orientation=Orientation::Left;
			animation.startSequence(25, 20, false, 10);
		} else if (orientation == Orientation::RightDown) {
			orientation=Orientation::Right;
			animation.startSequence(60, 55, false, 45);
		} else if (orientation == Orientation::FrontDown) {
			orientation=Orientation::Front;
			animation.startSequence(25, 20, false, 20);
		}
	}
	float dist=ppl7::grafix::Distance(p, ppl7::grafix::PointF(player.x, player.y));
	if (dist < 700) {
		aState=ActionState::Attack;
		fire_cooldown=time + ppl7::randf(0.5f, 3.0f);
	}

}

void SkullMaster::update_attack(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	bounce_distance=64;
	shine.color.set(255, 64, 128, 255);
	shine.intensity=255;
	bounce_speed=0.2f;
	max_bounce_velocity=5.0f;
	int px_dist=abs(player.x - p.x);
	if (px_dist < 60 && abs(initial_p.x - p.x) < 500) {
		if (player.x > p.x) {
			velocity.x-=0.4f * frame_rate_compensation;
			if (velocity.x < -3.0f) velocity.x=-5.0f;
		} else if (player.x < p.x) {
			velocity.x+=0.4f * frame_rate_compensation;
			if (velocity.x > 3.0f) velocity.x=5.0f;
		}
	} else if (px_dist > 700 && abs(initial_p.x - p.x) < 500) {
		if (player.x > p.x) {
			velocity.x+=0.2f * frame_rate_compensation;
			if (velocity.x > 3.0f) velocity.x=3.0f;
		} else if (player.x < p.x) {
			velocity.x-=0.2f * frame_rate_compensation;
			if (velocity.x < -3.0f) velocity.x=-3.0f;
		}
	} else if (velocity.x < 0.0f) {
		velocity.x+=0.1f * frame_rate_compensation;
		if (velocity.x > 0.0f)velocity.x=0.0f;
	} else if (velocity.x > 0.0f) {
		velocity.x-=0.1f * frame_rate_compensation;
		if (velocity.x < 0.0f)velocity.x=0.0f;
	}

	if (animation.isFinished()) {
		if (fire_cooldown < time) {
			fire(time, player);
			fire_cooldown=time + ppl7::randf(0.5f, 2.0f);
		}

		if (player.x > p.x) {
			if (px_dist < 100) turn(Orientation::FrontDown);
			else if (px_dist > 400) turn(Orientation::Right);
			else turn(Orientation::RightDown);
		} else {
			if (px_dist < 100) turn(Orientation::FrontDown);
			else if (px_dist > 400) turn(Orientation::Left);
			else turn(Orientation::LeftDown);
		}
	}

	float dist=ppl7::grafix::Distance(p, ppl7::grafix::PointF(player.x, player.y));
	if (dist > 1500) aState=ActionState::Wait;
}

void SkullMaster::die(double time) {
	getAudioPool().playOnce(AudioClip::skull_death, p, 1600, 0.6f);
	getAudioPool().playOnce(AudioClip::skull_impact, p, 1600, 0.6f);

	animation.startSequence(43, 54, false, 54);
	collisionDetection=false;
	//velocity.y=1.0f;
	emmitParticles(time, p);
	myLayer = Layer::BeforeBricks;
	aState = ActionState::Die;
	next_state=time + 1.0f;
}

void SkullMaster::update_die(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	shine.scale_x=2.0f;
	shine.scale_y=2.0f;
	shine.color.set(255, 255, 255, 255);
	shine.intensity=255;
	shine.plane=static_cast<int>(LightPlaneId::Player);
	shine.playerPlane=static_cast<int>(LightPlayerPlaneMatrix::All);




	if (next_birth < time) {
		next_birth=time + randf(0.010, 0.123);
		ParticleSystem* ps=GetParticleSystem();
		int new_particles=ppl7::rand(158, 200);
		for (int i=0;i < new_particles;i++) {
			Particle* particle=new Particle();
			particle->birth_time=time;
			particle->death_time=randf(0.952, 1.047) + time;
			particle->p=getBirthPosition(p, EmitterType::Ellipse, ppl7::grafix::Size(58, 58), 0.000);
			particle->layer=Particle::Layer::FrontplaneBack;
			particle->weight=randf(0.264, 0.642);
			particle->gravity.setPoint(0.000, 0.000);
			particle->velocity=calculateVelocity(randf(3.396, 7.300), 0.000 + randf(-180.000, 180.000));
			particle->scale=randf(0.300, 1.940);
			particle->color_mod.set(255, 255, 255, 255);
			particle->initAnimation(Particle::Type::RotatingParticleWhite);
			particle->initScaleGradient(scale_gradient, particle->scale);
			particle->initColorGradient(color_gradient);
			ps->addParticle(particle);
		}
	}
	if (time >= next_state) {
		aState=ActionState::Dead;
		next_state=time + 0.7f;
	}
}




void SkullMaster::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
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
	update_bounce(time, ttplane, player, frame_rate_compensation);
	switch (aState) {
		case ActionState::Wait:
			update_wait(time, ttplane, player, frame_rate_compensation);
			break;
		case ActionState::Turn:
			break;
		case ActionState::Attack:
			update_attack(time, ttplane, player, frame_rate_compensation);
			break;
		case ActionState::Die:
			update_die(time, ttplane, player, frame_rate_compensation);
			break;
		case ActionState::Dead:
			if (shine.scale_x > 0.2f) shine.scale_x-=0.1f * frame_rate_compensation;
			else {
				shine.scale_x=0.6f;
				shine.scale_y=0.6f;
				shine.plane=static_cast<int>(LightPlaneId::Player);
				shine.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);
				myLayer=Layer::BehindPlayer;
				enabled=false;
				aState=ActionState::Wait;
				health=100.0f;
				p=initial_p;
				return;
			}
			break;
	}
	p+=velocity;



	if (player.isFlashlightOn() && aState != ActionState::Dead && aState != ActionState::Die) {
		float y_dist=abs((player.y - 40) - p.y);
		float x_dist=abs(player.x - p.x);
		if (y_dist < 2 * TILE_HEIGHT && x_dist < 6 * TILE_WIDTH) {
			if ((player.orientation == Player::PlayerOrientation::Left && p.x < player.x) ||
				(player.orientation == Player::PlayerOrientation::Right && p.x > player.x)) {
					// In light cone
				health-=0.5f * frame_rate_compensation;
				if (health < 0) {
					die(time);
					player.addPoints(1000);
					player.addHealth(30);


				}
			}
		}
	}
	lightmap.custom_texture=this->texture;
	lightmap.sprite_no=94 + sprite_no;
	lightmap.x=p.x;
	lightmap.y=p.y;
	shine.x=p.x;
	shine.y=p.y - 1;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&shine);
	lights.addObjectLight(&lightmap);
}

void SkullMaster::handleCollision(Player* player, const Collision& collision)
{
	if (collision_cooldown < player->time) {
		collision_cooldown=player->time + 1.0f;
		player->dropHealth(5);
	}
}

void SkullMaster::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	if (aState == ActionState::Dead) return;
	Object::draw(renderer, coords);
	if (health <= 0.0f) return;
	SDL_Rect r;
	r.x=p.x + coords.x - 52;
	r.y=p.y + coords.y - 130;
	r.w=104;
	r.h=12;
	// To see the healthbar, we have to draw its rectangle into the lightmap
	SDL_Texture* lightmap=GetGame().getLightRenderTarget();
	if (lightmap) {
		SDL_Texture* renderer_backup=SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, lightmap);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &r);
		SDL_SetRenderTarget(renderer, renderer_backup);
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &r);
	r.x++;
	r.y++;
	r.w-=2;
	r.h-=2;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &r);
	r.x++;
	r.y++;
	r.h-=2;
	r.w=int(health);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &r);
}



}	// EOF namespace Decker::Objects
