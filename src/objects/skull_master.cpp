#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

class SkullMasterFireball : public Object
{
	friend class SkullMaster;
private:
	ppl7::grafix::PointF velocity;
	double next_birth;
	float direction;
	std::list<Particle::ScaleGradientItem>scale_gradient;
	std::list<Particle::ColorGradientItem>color_gradient;
	void emmitParticles(double time, const Player& player);

	LightObject light_ball;
	LightObject light_shine;
	AudioInstance* audio;

public:
	SkullMasterFireball();
	~SkullMasterFireball();
	static Representation representation();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;
	void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const override;

	virtual void update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation);
	virtual void handleCollision(Player* player, const Collision& collision);
};



SkullMasterFireball::SkullMasterFireball()
	:Object(Type::ObjectType::Arrow)
{
	collisionDetection=true;
	next_birth=0.0f;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.243));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.270, 0.620));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.500, 0.421));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(11, 170, 0, 18)));
	color_gradient.push_back(Particle::ColorGradientItem(0.036, ppl7::grafix::Color(30, 216, 0, 53)));
	color_gradient.push_back(Particle::ColorGradientItem(0.081, ppl7::grafix::Color(40, 255, 0, 78)));
	color_gradient.push_back(Particle::ColorGradientItem(0.176, ppl7::grafix::Color(70, 255, 0, 78)));
	color_gradient.push_back(Particle::ColorGradientItem(0.243, ppl7::grafix::Color(26, 22, 26, 112)));
	color_gradient.push_back(Particle::ColorGradientItem(0.486, ppl7::grafix::Color(120, 118, 116, 27)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(196, 196, 198, 0)));

	light_ball.color.set(255, 255, 255, 255);
	light_ball.sprite_no=2;
	light_ball.scale_x=0.2f;
	light_ball.scale_y=0.2f;
	light_ball.plane=static_cast<int>(LightPlaneId::Player);
	light_ball.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

	light_shine.color.set(100, 255, 0, 255);
	light_shine.sprite_no=0;
	light_shine.scale_x=0.4f;
	light_shine.scale_y=0.4f;
	light_shine.plane=static_cast<int>(LightPlaneId::Player);
	light_shine.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);;

	light_shine.has_lensflare=true;
	light_shine.flarePlane=static_cast<int>(LightPlayerPlaneMatrix::Player);
	light_shine.flare_intensity=60;
	light_shine.flare_useLightColor=true;
	audio=NULL;

}

SkullMasterFireball::~SkullMasterFireball()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

Representation SkullMasterFireball::representation()
{
	return Representation(Spriteset::GenericObjects, 300);
}



/*
 * add the following function to your class:
 */

void SkullMasterFireball::emmitParticles(double time, const Player& player)
{
	//if (next_birth < time) {
		//next_birth=time + randf(0.010, 0.111);
	ParticleSystem* ps=GetParticleSystem();
	if (!emitterInPlayerRange(p, player)) return;
	int new_particles=ppl7::rand(10, 30);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.552, 1.189) + time;
		particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(32, 32), 0.000);
		particle->layer=Particle::Layer::BeforePlayer;
		particle->weight=randf(0.132, 0.660);
		particle->gravity.setPoint(0.000, 0.000);
		particle->velocity=calculateVelocity(randf(0.566, 1.321), direction + randf(-15.283, 15.283));
		particle->scale=randf(0.300, 1.375);
		particle->color_mod.set(255, 255, 255, 255);
		particle->initAnimation(Particle::Type::RotatingParticleWhite);
		particle->initScaleGradient(scale_gradient, particle->scale);
		particle->initColorGradient(color_gradient);
		ps->addParticle(particle);
	}
//}
}




void SkullMasterFireball::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	AudioPool& pool=getAudioPool();
	if (!audio) {
		audio=pool.getInstance(AudioClip::skull_fireball);
		audio->setVolume(0.7f);
		audio->setPositional(p, 1200);
		audio->setLoop(true);
		pool.playInstance(audio);
	}
	p+=velocity * frame_rate_compensation;
	audio->setPositional(p, 1200);
	updateBoundary();
	light_shine.x=p.x;
	light_shine.y=p.y;
	light_ball.x=p.x + 1;
	light_ball.y=p.y;
	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&light_shine);
	lights.addObjectLight(&light_ball);


	emmitParticles(time, player);
	TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y));
	if (t1 == TileType::Blocking) {
		deleteDefered=true;
		pool.stopInstace(audio);
		pool.playOnce(AudioClip::skull_impact, p, 1800, 1.0f);
	} else if (p.x < 0 || p.x>65535 || p.y < 0 || p.y>65535) {
		deleteDefered=true;
		pool.stopInstace(audio);
	}
}

void SkullMasterFireball::handleCollision(Player* player, const Collision& collision)
{
	deleteDefered=true;
	player->dropHealth(10, Physic::HealthDropReason::Burned);
}

void SkullMasterFireball::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	draw(renderer, coords);
}

void SkullMasterFireball::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	texture->draw(renderer,
		p.x + coords.x,
		p.y + coords.y,
		sprite_no, color_mod);
	return;

	SpriteTexture& lightmap=getResources().Lightmaps;
	Game& game=GetGame();
	SDL_Texture* lighttarget=game.getLightRenderTarget();
	if (!lighttarget) return;
	SDL_Texture* rendertarget=SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, lighttarget);

	ppl7::grafix::Color rc[7];
	rc[0].setColor(255, 255, 255, 255);
	rc[2].setColor(255, 255, 0, 255);
	rc[3].setColor(255, 128, 0, 255);
	rc[4].setColor(255, 192, 0, 255);
	rc[5].setColor(255, 0, 0, 255);
	rc[6].setColor(255, 255, 255, 255);

	ppl7::grafix::Color c(255, 200, 60, 255);
	lightmap.drawScaled(renderer, p.x + coords.x,
		p.y + coords.y, 0, 1.0f, c);
	c=rc[ppl7::rand(0, 6)];
	c.setAlpha(ppl7::rand(128, 255));

	lightmap.drawScaled(renderer, p.x + coords.x,
		p.y + coords.y, 1, 1.0f, c);
	c.setColor(255, 255, 255, 255);
	lightmap.drawScaled(renderer, p.x + coords.x,
		p.y + coords.y, 1, 0.1f, c);

	SDL_SetRenderTarget(renderer, rendertarget);

}



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
	shine.scale_x=0.4f;
	shine.scale_y=0.4f;
	shine.intensity=128;
	shine.myType=LightType::Fire;
	shine.typeParameter=0.9;
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
	SkullMasterFireball* particle=new SkullMasterFireball();
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



	if (player.isFlashlightOn() && aState != ActionState::Dead) {
		float y_dist=abs((player.y - 40) - p.y);
		float x_dist=abs(player.x - p.x);
		if (y_dist < 2 * TILE_HEIGHT && x_dist < 6 * TILE_WIDTH) {
			if ((player.orientation == Player::PlayerOrientation::Left && p.x < player.x) ||
				(player.orientation == Player::PlayerOrientation::Right && p.x > player.x)) {
					// In light cone
				//health-=0.5f * frame_rate_compensation;
				if (health < 0) {
					die(time);
					player.addPoints(20);
					player.addHealth(10);


				}
			}
		}
	}
	lightmap.custom_texture=this->texture;
	lightmap.sprite_no=70 + sprite_no;
	lightmap.x=p.x;
	lightmap.y=p.y;
	shine.update(time, frame_rate_compensation);
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
