#include <ppl7.h>
#include <ppl7-grafix.h>
#include <SDL.h>
#include "objects.h"
#include "decker.h"
#include "player.h"

namespace Decker::Objects {

static int particle_transparent[]={ 216,217,218,219,220,221,222,223,224,225,226,
	227,228,229,230,231,232,233,234,235 };

static int particle_white[]={ 272,273,274,275,276,277,278,279,280,281,282,283,284,
285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301 };

static int snowflake_transparent[]={ 397,398,399,400,401,402,403,404,405,406,407,408,409,
410,411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426 };

static int snowflake_white[]={ 366,367,368,369,370,371,372,373,374,375,376,377,378,379,
380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395 };


/****************************************************************
 * Rain Particle
 ***************************************************************/

class RainParticle : public Object
{
	friend class RainEmitter;
public:
	class Velocity
	{
	public:
		Velocity() {
			x=0.0f;
			y=0.0f;
		}
		float x;
		float y;
	};
private:
	double next_animation;
	AnimationCycle animation;
	Velocity velocity;
	Velocity pf;
	ppl7::grafix::Point end;
public:
	RainParticle();
	static Representation representation();
	virtual void update(double time, TileTypePlane& ttplane, Player& player);
};


RainParticle::RainParticle()
	: Object(Type::ObjectType::Particle)
{
	sprite_set=Spriteset::GenericObjects;
	collisionDetection=false;
	visibleAtPlaytime=true;
	sprite_no_representation=216;
	sprite_no=216;
	next_animation=0.0f;
	spawned=true;
	myLayer=Layer::BeforePlayer;
}

void RainParticle::update(double time, TileTypePlane&, Player&)
{
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		sprite_no=animation.getFrame();
		sprite_no_representation=sprite_no;
	}
	pf.x+=velocity.x;
	pf.y+=velocity.y;
	p.x=(int)pf.x;
	p.y=(int)pf.y;
	updateBoundary();
	if (p.y > end.y) deleteDefered=true;
}

/****************************************************************
 * Rain Emitter
 ***************************************************************/


Representation RainEmitter::representation()
{
	return Representation(Spriteset::GenericObjects, 303);
}



RainEmitter::RainEmitter()
	:Object(Type::ObjectType::RainEmitter)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=303;
	collisionDetection=false;
	visibleAtPlaytime=false;
	sprite_no_representation=303;
	next_birth=0.0f;

	type=ParticleType::Rain;
	ParticleColor.set(255, 255, 255, 255);
	emitter_stud_width=16;
	max_particle_birth_per_cycle = 4;
	birth_time_min=0.020f;
	birth_time_max=0.300f;
	max_velocity_x=0.5f;
	min_velocity_y=4.0f;
	max_velocity_y=7.3f;
	scale_min=0.3f;
	scale_max=1.0f;

	save_size+=35;
}

static float randf(float min, float max)
{
	double range=max - min;
	double r=(((double)ppl7::rand(0, RAND_MAX)) / (double)RAND_MAX * range) + min;
	return r;
}

void RainEmitter::createParticle(const TileTypePlane& ttplane)
{
	RainParticle* particle=new RainParticle();
	particle->p.x=p.x + ppl7::rand(0, TILE_WIDTH * emitter_stud_width) - ((TILE_WIDTH * emitter_stud_width) / 2);
	particle->p.y=p.y;
	particle->initial_p.x=particle->p.x;
	particle->initial_p.y=particle->p.y;
	particle->pf.x=(float)particle->p.x;
	particle->pf.y=(float)particle->p.y;
	particle->velocity.x=randf(0, max_velocity_x) - (max_velocity_x / 2.0f);
	particle->velocity.y=randf(min_velocity_y, max_velocity_y);
	particle->end=particle->p;
	particle->scale=randf(scale_min, scale_max);
	particle->color_mod=ParticleColor;
	switch (type) {
	case ParticleType::Rain:
		particle->animation.startRandom(particle_transparent, sizeof(particle_transparent) / sizeof(int), true, 0);
		break;
	case ParticleType::ParticleWhite:
		particle->animation.startRandom(particle_white, sizeof(particle_white) / sizeof(int), true, 0);
		break;
	case ParticleType::SnowflakeWhite:
		particle->animation.startRandom(snowflake_white, sizeof(snowflake_white) / sizeof(int), true, 0);
		break;
	case ParticleType::SnowflakeTransparent:
		particle->animation.startRandom(snowflake_transparent, sizeof(snowflake_transparent) / sizeof(int), true, 0);
		break;
	};
	ppl7::grafix::Size levelsize=ttplane.size();
	levelsize.height*=TILE_HEIGHT;
	while (particle->end.y < levelsize.height && ttplane.getType(particle->end) != TileType::Blocking)
		particle->end.y+=TILE_HEIGHT;
	particle->end.y-=TILE_HEIGHT;
	GetObjectSystem()->addObject(particle);
}

void RainEmitter::update(double time, TileTypePlane& ttplane, Player& player)
{
	if (next_birth < time) {
		next_birth=time + randf(birth_time_min, birth_time_max);
		double d=ppl7::grafix::Distance(ppl7::grafix::PointF(player.WorldCoords.x + player.Viewport.width() / 2,
			player.WorldCoords.y + player.Viewport.height() / 2), p);
		if (d > 2 * player.Viewport.width()) return;
		int new_particles=ppl7::rand(1, max_particle_birth_per_cycle);
		for (int i=0;i < new_particles;i++) {
			createParticle(ttplane);
		}
	}
}

size_t RainEmitter::save(unsigned char* buffer, size_t size)
{
	if (size < save_size) return 0;
	size_t bytes=Object::save(buffer, size);
	ppl7::Poke8(buffer + bytes, static_cast<int>(type));
	ppl7::Poke8(buffer + bytes + 1, emitter_stud_width);
	ppl7::Poke8(buffer + bytes + 2, max_particle_birth_per_cycle);
	ppl7::Poke8(buffer + bytes + 3, ParticleColor.red());
	ppl7::Poke8(buffer + bytes + 4, ParticleColor.green());
	ppl7::Poke8(buffer + bytes + 5, ParticleColor.blue());
	ppl7::Poke8(buffer + bytes + 6, ParticleColor.alpha());
	ppl7::PokeFloat(buffer + bytes + 7, birth_time_min);
	ppl7::PokeFloat(buffer + bytes + 11, birth_time_max);
	ppl7::PokeFloat(buffer + bytes + 15, max_velocity_x);
	ppl7::PokeFloat(buffer + bytes + 19, min_velocity_y);
	ppl7::PokeFloat(buffer + bytes + 23, max_velocity_y);
	ppl7::PokeFloat(buffer + bytes + 27, scale_min);
	ppl7::PokeFloat(buffer + bytes + 31, scale_max);
	return bytes + 35;
}

size_t RainEmitter::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < save_size) return 0;
	type=static_cast<ParticleType>(ppl7::Peek8(buffer + bytes));
	emitter_stud_width=ppl7::Peek8(buffer + bytes + 1);
	max_particle_birth_per_cycle=ppl7::Peek8(buffer + bytes + 2);
	ParticleColor.setRed(ppl7::Peek8(buffer + bytes + 3));
	ParticleColor.setGreen(ppl7::Peek8(buffer + bytes + 4));
	ParticleColor.setBlue(ppl7::Peek8(buffer + bytes + 5));
	ParticleColor.setAlpha(ppl7::Peek8(buffer + bytes + 6));
	birth_time_min=ppl7::PeekFloat(buffer + bytes + 7);
	birth_time_max=ppl7::PeekFloat(buffer + bytes + 11);
	max_velocity_x=ppl7::PeekFloat(buffer + bytes + 15);
	min_velocity_y=ppl7::PeekFloat(buffer + bytes + 19);
	max_velocity_y=ppl7::PeekFloat(buffer + bytes + 23);
	scale_min=ppl7::PeekFloat(buffer + bytes + 27);
	scale_max=ppl7::PeekFloat(buffer + bytes + 31);
	return size;
}



/****************************************************************
 * Rain Ui
 ***************************************************************/




class RainEmitterDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::ComboBox* color_scheme;
	ppl7::tk::ComboBox* on_start_state;
	ppl7::tk::CheckBox* initial_state;
	ppl7::tk::CheckBox* always_on;
	ppl7::tk::CheckBox* block_player;
	ppl7::tk::LineInput* time_on_min, * time_on_max;
	ppl7::tk::LineInput* time_off_min, * time_off_max;

	RainEmitter* object;

public:
	RainEmitterDialog(RainEmitter* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
	//virtual void mouseDownEvent(ppl7::tk::MouseEvent *event);
};

void RainEmitter::openUi()
{
	RainEmitterDialog* dialog=new RainEmitterDialog(this);
	GetGameWindow()->addChild(dialog);
}


RainEmitterDialog::RainEmitterDialog(RainEmitter* object)
	: Decker::ui::Dialog(600, 400)
{

}


}	// EOF namespace Decker::Objects
