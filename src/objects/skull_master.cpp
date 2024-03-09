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
	state=0;
	health=100.0f;
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
	float direction=0.0f;
	if (player.x < p.x) {
		direction=270;
		animation.startSequence(22, 31, false, 22);
	} else {
		direction=90;
		animation.startSequence(33, 42, false, 32);
	}
	fire_cooldown=time + ppl7::randf(0.5f, 1.5f);
	Fireball* particle=new Fireball();
	particle->p=p;
	particle->initial_p=p;
	particle->spawned=true;
	particle->sprite_no=152;
	particle->color_mod.set(64, 192, 0, 255);
	particle->sprite_set=sprite_set;
	particle->sprite_no_representation=152;
	particle->velocity=calculateVelocity(10.0f, direction);
	particle->direction=180 + direction;
	GetObjectSystem()->addObject(particle);
	getAudioPool().playOnce(AudioClip::skull_shoot, p, 1600, 0.7f);
}

void SkullMaster::die(double time) {
	getAudioPool().playOnce(AudioClip::skull_death, p, 1600, 0.6f);
	getAudioPool().playOnce(AudioClip::skull_impact, p, 1600, 0.6f);
	state=5;
	animation.startSequence(43, 54, false, 54);
	collisionDetection=false;
	//velocity.y=1.0f;
	emmitParticles(time, p);
	myLayer = Layer::BeforeBricks;
	aState = ActionState::Dead;
	next_state=time + 1.0f;
}

void SkullMaster::turn()
{
	if (animation.isFinished()) {


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
	if (aState == ActionState::Dead && time >= next_state) {
		enabled=false;
		return;
	}


	if (player.isFlashlightOn() && aState != ActionState::Dead) {
		float y_dist=abs((player.y - 40) - p.y);
		float x_dist=abs(player.x - p.x);
		if (y_dist < 2 * TILE_HEIGHT && x_dist < 6 * TILE_WIDTH) {
			if ((player.orientation == Player::PlayerOrientation::Left && p.x < player.x) ||
				(player.orientation == Player::PlayerOrientation::Right && p.x > player.x)) {
					// In light cone
				health-=0.5f * frame_rate_compensation;
				if (health < 0) {
					die(time);
					player.addPoints(20);
					player.addHealth(10);


				}
			}
		}
	}
	lightmap.custom_texture=this->texture;
	lightmap.sprite_no=94 + sprite_no;
	lightmap.x=p.x;
	lightmap.y=p.y;
	//shine.update(time, frame_rate_compensation);
	shine.x=p.x;
	shine.y=p.y - 1;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&shine);
	lights.addObjectLight(&lightmap);
}

void SkullMaster::handleCollision(Player* player, const Collision& collision)
{
	player->dropHealth(1);
}



}	// EOF namespace Decker::Objects
