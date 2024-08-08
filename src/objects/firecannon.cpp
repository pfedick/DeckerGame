#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"
#include "audiopool.h"
namespace Decker::Objects {


Representation FireCannon::representation()
{
	return Representation(Spriteset::GenericObjects, 299);
}

Fireball::Fireball()
	:Object(Type::ObjectType::Arrow)
{
	collisionDetection=true;
	next_birth=0.0f;
	scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.243));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.270, 0.620));
	scale_gradient.push_back(Particle::ScaleGradientItem(0.500, 0.421));
	scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
	color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(170, 11, 0, 18)));
	color_gradient.push_back(Particle::ColorGradientItem(0.036, ppl7::grafix::Color(216, 101, 0, 53)));
	color_gradient.push_back(Particle::ColorGradientItem(0.081, ppl7::grafix::Color(255, 255, 0, 78)));
	color_gradient.push_back(Particle::ColorGradientItem(0.176, ppl7::grafix::Color(255, 128, 0, 78)));
	color_gradient.push_back(Particle::ColorGradientItem(0.243, ppl7::grafix::Color(26, 22, 26, 112)));
	color_gradient.push_back(Particle::ColorGradientItem(0.486, ppl7::grafix::Color(120, 118, 116, 27)));
	color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(196, 196, 198, 0)));

	light_ball.color.set(255, 255, 255, 255);
	light_ball.sprite_no=2;
	light_ball.scale_x=0.2f;
	light_ball.scale_y=0.2f;
	light_ball.plane=static_cast<int>(LightPlaneId::Player);
	light_ball.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);

	light_shine.color.set(255, 200, 0, 255);
	light_shine.sprite_no=0;
	light_shine.scale_x=1.0f;
	light_shine.scale_y=1.0f;
	light_shine.plane=static_cast<int>(LightPlaneId::Player);
	light_shine.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);;

	light_shine.has_lensflare=true;
	light_shine.flarePlane=static_cast<int>(LightPlayerPlaneMatrix::Player);
	light_shine.flare_intensity=255;
	light_shine.flare_useLightColor=true;
	audio=NULL;
	gravity=0.0f;
	player_damage=20.0f;
	min_particles=79;
	max_particles=169;

}

Fireball::~Fireball()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

Representation Fireball::representation()
{
	return Representation(Spriteset::GenericObjects, 300);
}



/*
 * add the following function to your class:
 */

void Fireball::emmitParticles(double time, const Player& player)
{
	//if (time < next_birth) return;  // forget it, that looks awful! we need to emit particles every frame!
	//next_birth=time + randf(0.010, 0.111);
	ParticleSystem* ps=GetParticleSystem();
	if (!emitterInPlayerRange(p, player)) return;
	int new_particles=ppl7::rand(min_particles, max_particles);
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




void Fireball::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	AudioPool& pool=getAudioPool();
	if (!audio) {
		audio=pool.getInstance(AudioClip::fireball_fly);
		audio->setVolume(0.6f);
		audio->setPositional(p, 1200);
		audio->setLoop(true);
		pool.playInstance(audio);
	}
	p+=velocity * frame_rate_compensation;
	velocity.y+=(gravity * frame_rate_compensation);
	audio->setPositional(p, 1200);
	updateBoundary();

	if (isInViewport) {
		light_shine.x=p.x;
		light_shine.y=p.y;
		light_ball.x=p.x + 1;
		light_ball.y=p.y;
		LightSystem& lights=GetGame().getLightSystem();
		lights.addObjectLight(&light_shine);
		lights.addObjectLight(&light_ball);
		emmitParticles(time, player);
	}
	TileType::Type t1=ttplane.getType(ppl7::grafix::Point(p.x, p.y));
	if (t1 == TileType::Blocking || t1 == TileType::Water) {
		deleteDefered=true;
		pool.stopInstace(audio);
		pool.playOnce(AudioClip::fireball_impact, p, 1800, 1.0f);
	} else if (p.x < 0 || p.x>65535 || p.y < 0 || p.y>65535) {
		deleteDefered=true;
		pool.stopInstace(audio);
	}
}

void Fireball::handleCollision(Player* player, const Collision& collision)
{
	deleteDefered=true;
	player->dropHealth(player_damage, Physic::HealthDropReason::Burned);
}

void Fireball::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	draw(renderer, coords);
}

void Fireball::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
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

FireCannon::FireCannon()
	:Trap(Type::ObjectType::FireCannon)
{
	sprite_set=0;
	sprite_no=299;
	sprite_no_representation=299;
	collisionDetection=false;
	visibleAtPlaytime=false;
	direction=90.0f;
	variation=0.0f;
	speed=12.0f;
	initial_state_on=true;
	current_state_on=true;
	min_cooldown_time=2.5f;
	max_cooldown_time=3.5f;
	min_particles=79;
	max_particles=169;

	next_state=ppl7::GetMicrotime() + ppl7::randf(0.0f, max_cooldown_time);
}


void FireCannon::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (current_state_on && next_state < time) {
		next_state=time + ppl7::randf(min_cooldown_time, max_cooldown_time);
		// Emit a new Fireball
		fire();
	}
}

void FireCannon::fire()
{
	Fireball* particle=new Fireball();
	float angle=direction + ppl7::randf(-variation, +variation);
	particle->p=p;
	particle->initial_p=p;
	particle->spawned=true;
	particle->sprite_no=300;
	particle->sprite_set=sprite_set;
	particle->sprite_no_representation=300;
	particle->velocity=calculateVelocity(speed, angle);
	particle->direction=180 + angle;
	particle->min_particles=min_particles;
	particle->max_particles=max_particles;
	GetObjectSystem()->addObject(particle);
	getAudioPool().playOnce(AudioClip::skull_shoot, p, 1200, 0.7f);
}

void FireCannon::toggle(bool enable, Object* source)
{
	current_state_on=enable;
}

void FireCannon::trigger(Object* source)
{
	current_state_on=!current_state_on;
}


size_t FireCannon::saveSize() const
{
	return Object::saveSize() + 24;
}

size_t FireCannon::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 3);		// Object Version

	uint8_t flags=0;
	if (initial_state_on) flags|=1;

	ppl7::Poke8(buffer + bytes + 1, flags);	// reserved
	ppl7::PokeFloat(buffer + bytes + 2, speed);
	ppl7::PokeFloat(buffer + bytes + 6, direction);
	ppl7::PokeFloat(buffer + bytes + 10, min_cooldown_time);
	ppl7::PokeFloat(buffer + bytes + 14, max_cooldown_time);
	ppl7::Poke8(buffer + bytes + 18, min_particles);
	ppl7::Poke8(buffer + bytes + 19, max_particles);
	ppl7::PokeFloat(buffer + bytes + 20, variation);
	return bytes + 24;
}

size_t FireCannon::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version < 1 || version>3) return 0;
	uint8_t flags=ppl7::Peek8(buffer + bytes + 1);
	initial_state_on=(flags & 1);
	current_state_on=initial_state_on;

	speed=ppl7::PeekFloat(buffer + bytes + 2);
	direction=ppl7::PeekFloat(buffer + bytes + 6);
	min_cooldown_time=ppl7::PeekFloat(buffer + bytes + 10);
	max_cooldown_time=ppl7::PeekFloat(buffer + bytes + 14);
	next_state=ppl7::GetMicrotime() + ppl7::randf(0.0f, max_cooldown_time);
	if (version >= 2) {
		min_particles=ppl7::Peek8(buffer + bytes + 18);
		max_particles=ppl7::Peek8(buffer + bytes + 19);
	}
	if (version >= 3) {
		variation=ppl7::PeekFloat(buffer + bytes + 20);
	}
	return size;
}


class FireCannonDialog : public Decker::ui::Dialog
{
private:
	ppltk::DoubleHorizontalSlider* direction;
	ppltk::DoubleHorizontalSlider* min_cooldown;
	ppltk::DoubleHorizontalSlider* max_cooldown;
	ppltk::DoubleHorizontalSlider* variation;
	ppltk::DoubleHorizontalSlider* speed;
	ppltk::HorizontalSlider* min_particles;
	ppltk::HorizontalSlider* max_particles;
	ppltk::CheckBox* current_state;
	ppltk::CheckBox* initial_state;

	FireCannon* object;

public:
	FireCannonDialog(FireCannon* object);
	virtual void valueChangedEvent(ppltk::Event* event, double value) override;
	virtual void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	virtual void toggledEvent(ppltk::Event* event, bool checked) override;
};


void FireCannon::openUi()
{
	FireCannonDialog* dialog=new FireCannonDialog(this);
	GetGameWindow()->addChild(dialog);
}


FireCannonDialog::FireCannonDialog(FireCannon* object)
	: Decker::ui::Dialog(500, 370)
{
	this->object=object;
	setWindowTitle("Fire cannon Trap");
	ppl7::grafix::Rect client=clientRect();
	int y=0;
	int col1=100;
	int w=client.width() - col1 - 10;
	int col2=client.width() / 2;
	int col3=col2 + 100;

	addChild(new ppltk::Label(0, y, 100, 30, "initial State:"));
	initial_state=new ppltk::CheckBox(col1, y, 120, 30, "enabled", object->initial_state_on);
	initial_state->setEventHandler(this);
	addChild(initial_state);
	addChild(new ppltk::Label(col2, y, 100, 30, "current State:"));
	current_state=new ppltk::CheckBox(col3, y, 120, 30, "enabled", object->current_state_on);
	current_state->setEventHandler(this);
	addChild(current_state);
	y+=35;


	addChild(new ppltk::Label(0, y, 100, 30, "Direction:"));
	direction=new ppltk::DoubleHorizontalSlider(col1, y, w, 30);
	direction->setLimits(0.0f, 360.0f);
	direction->enableSpinBox(true, 1.0f, 0, 80);
	direction->setValue(object->direction);
	direction->setEventHandler(this);
	addChild(direction);
	y+=35;

	addChild(new ppltk::Label(0, y, 100, 30, "Variation:"));
	variation=new ppltk::DoubleHorizontalSlider(col1, y, w, 30);
	variation->setLimits(0.0f, 180.0f);
	variation->enableSpinBox(true, 1.0f, 0, 80);
	variation->setValue(object->variation);
	variation->setEventHandler(this);
	addChild(variation);
	y+=35;

	addChild(new ppltk::Label(0, y, 100, 30, "Speed:"));
	speed=new ppltk::DoubleHorizontalSlider(col1, y, w, 30);
	speed->setLimits(1.0f, 30.0f);
	speed->enableSpinBox(true, 0.1f, 1, 80);
	speed->setValue(object->speed);
	speed->setEventHandler(this);
	addChild(speed);
	y+=35;

	addChild(new ppltk::Label(0, y, 100, 30, "Min Cooldown:"));
	min_cooldown=new ppltk::DoubleHorizontalSlider(col1, y, w, 30);
	min_cooldown->setLimits(0.1f, 30.0f);
	min_cooldown->enableSpinBox(true, 0.1f, 1, 80);
	min_cooldown->setValue(object->min_cooldown_time);
	min_cooldown->setEventHandler(this);
	addChild(min_cooldown);
	y+=35;

	addChild(new ppltk::Label(0, y, 100, 30, "Max Cooldown:"));
	max_cooldown=new ppltk::DoubleHorizontalSlider(col1, y, w, 30);
	max_cooldown->setLimits(0.1f, 30.0f);
	max_cooldown->enableSpinBox(true, 0.1f, 1, 80);
	max_cooldown->setValue(object->max_cooldown_time);
	max_cooldown->setEventHandler(this);
	addChild(max_cooldown);
	y+=35;

	addChild(new ppltk::Label(0, y, 100, 30, "Min Particles:"));
	min_particles=new ppltk::HorizontalSlider(col1, y, w, 30);
	min_particles->setLimits(0, 255);
	min_particles->enableSpinBox(true, 1, 80);
	min_particles->setValue(object->min_particles);
	min_particles->setEventHandler(this);
	addChild(min_particles);
	y+=35;

	addChild(new ppltk::Label(0, y, 100, 30, "Max Particles:"));
	max_particles=new ppltk::HorizontalSlider(col1, y, w, 30);
	max_particles->setLimits(0, 255);
	max_particles->enableSpinBox(true, 1, 80);
	max_particles->setValue(object->max_particles);
	max_particles->setEventHandler(this);
	addChild(max_particles);

}


void FireCannonDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	if (event->widget() == direction) {
		object->direction=value;
	} else 	if (event->widget() == speed) {
		object->speed=value;
	} else 	if (event->widget() == min_cooldown) {
		object->min_cooldown_time=value;
		if (value > object->max_cooldown_time) max_cooldown->setValue(value);
	} else 	if (event->widget() == max_cooldown) {
		object->max_cooldown_time=value;
		if (value < object->min_cooldown_time) min_cooldown->setValue(value);
	} else 	if (event->widget() == variation) {
		object->variation=value;
	}

}

void FireCannonDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	if (event->widget() == min_particles) {
		object->min_particles=value;
		if (value > object->max_particles) max_particles->setValue(value);
	} else 	if (event->widget() == max_particles) {
		object->max_particles=value;
		if (value < object->min_particles) min_particles->setValue(value);
	}
}

void FireCannonDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (event->widget() == initial_state) {
		object->initial_state_on=checked;
	} else if (event->widget() == current_state) {
		object->current_state_on=checked;
	}
}


}	// EOF namespace Decker::Objects
