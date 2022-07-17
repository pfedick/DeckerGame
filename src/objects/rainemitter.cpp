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
	double death_time;
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
	death_time=1.0f;
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
	if (p.y > end.y || time > death_time) deleteDefered=true;
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

	particle_type=ParticleType::Rain;
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
	age_min=5.0f;
	age_max=10.0f;


	save_size+=43;
}

static float randf(float min, float max)
{
	double range=max - min;
	double r=(((double)ppl7::rand(0, RAND_MAX)) / (double)RAND_MAX * range) + min;
	return r;
}

void RainEmitter::createParticle(const TileTypePlane& ttplane, double time)
{
	RainParticle* particle=new RainParticle();
	particle->p.x=p.x + ppl7::rand(0, TILE_WIDTH * emitter_stud_width) - ((TILE_WIDTH * emitter_stud_width) / 2);
	particle->p.y=p.y;
	particle->myLayer=myLayer;
	particle->initial_p.x=particle->p.x;
	particle->initial_p.y=particle->p.y;
	particle->pf.x=(float)particle->p.x;
	particle->pf.y=(float)particle->p.y;
	particle->velocity.x=randf(0, max_velocity_x) - (max_velocity_x / 2.0f);
	particle->velocity.y=randf(min_velocity_y, max_velocity_y);
	particle->end=particle->p;
	particle->scale=randf(scale_min, scale_max);
	particle->color_mod=ParticleColor;
	particle->death_time=randf(age_min, age_max) + time;
	switch (particle_type) {
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
			createParticle(ttplane, time);
		}
	}
}

size_t RainEmitter::save(unsigned char* buffer, size_t size)
{
	if (size < save_size) return 0;
	size_t bytes=Object::save(buffer, size);
	ppl7::Poke8(buffer + bytes, static_cast<int>(particle_type));
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
	ppl7::PokeFloat(buffer + bytes + 35, age_min);
	ppl7::PokeFloat(buffer + bytes + 39, age_max);
	return bytes + 43;
}

size_t RainEmitter::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < save_size) return 0;
	particle_type=static_cast<ParticleType>(ppl7::Peek8(buffer + bytes));
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
	age_min=ppl7::PeekFloat(buffer + bytes + 35);
	age_max=ppl7::PeekFloat(buffer + bytes + 39);
	return size;
}



/****************************************************************
 * Rain Ui
 ***************************************************************/




class RainEmitterDialog : public Decker::ui::Dialog
{
private:
	Decker::ui::ColorSliderWidget* color;
	ppl7::tk::ComboBox* particle_type;
	ppl7::tk::HorizontalSlider* emitter_width;
	ppl7::tk::HorizontalSlider* max_birth;
	ppl7::tk::DoubleHorizontalSlider* birth_time_min, * birth_time_max;
	ppl7::tk::DoubleHorizontalSlider* min_velocity_y, * max_velocity_y, * max_velocity_x;
	ppl7::tk::DoubleHorizontalSlider* scale_min, * scale_max;
	ppl7::tk::DoubleHorizontalSlider* age_min, * age_max;
	/*
	ppl7::tk::ComboBox* color_scheme;
	ppl7::tk::ComboBox* on_start_state;
	ppl7::tk::CheckBox* initial_state;
	ppl7::tk::CheckBox* always_on;
	ppl7::tk::CheckBox* block_player;
	ppl7::tk::LineInput* time_on_min, * time_on_max;
	ppl7::tk::LineInput* time_off_min, * time_off_max;
	*/
	void setValuesToUi(const RainEmitter* object);
	RainEmitter* object;

public:
	RainEmitterDialog(RainEmitter* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value) override;
	virtual void valueChangedEvent(ppl7::tk::Event* event, int64_t value) override;
	virtual void valueChangedEvent(ppl7::tk::Event* event, double value) override;
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
};

void RainEmitter::openUi()
{
	RainEmitterDialog* dialog=new RainEmitterDialog(this);
	GetGameWindow()->addChild(dialog);
}


RainEmitterDialog::RainEmitterDialog(RainEmitter* object)
	: Decker::ui::Dialog(700, 520, Dialog::Buttons::OK | Dialog::Buttons::CopyAndPaste)
{
	this->object=object;
	this->setWindowTitle("Rain Emitter");
	int col1=100;
	int y=0;
	ppl7::grafix::Rect client=clientRect();
	addChild(new ppl7::tk::Label(0, y, col1, 30, "Particle Type:"));
	particle_type=new ppl7::tk::ComboBox(col1, y, client.width() - col1, 30);
	particle_type->setEventHandler(this);
	particle_type->add("Transparent particle", ppl7::ToString("%d", static_cast<int>(RainEmitter::ParticleType::Rain)));
	particle_type->add("White particle", ppl7::ToString("%d", static_cast<int>(RainEmitter::ParticleType::ParticleWhite)));
	particle_type->add("Transparent Snowflake", ppl7::ToString("%d", static_cast<int>(RainEmitter::ParticleType::SnowflakeTransparent)));
	particle_type->add("White Snowflake", ppl7::ToString("%d", static_cast<int>(RainEmitter::ParticleType::SnowflakeWhite)));
	addChild(particle_type);
	y+=35;
	addChild(new ppl7::tk::Label(0, y, col1, 30, "Particle Color:"));
	color=new Decker::ui::ColorSliderWidget(col1, y, client.width() - col1, 4 * 35, true);
	color->setEventHandler(this);
	addChild(color);
	y+=4 * 35;

	col1=150;

	addChild(new ppl7::tk::Label(0, y, col1, 30, "Emitter size:"));
	emitter_width=new ppl7::tk::HorizontalSlider(col1, y, client.width() - col1 - 20, 30);
	emitter_width->setEventHandler(this);
	emitter_width->setLimits(1, 32);
	emitter_width->enableSpinBox(true, 1, 60);
	addChild(emitter_width);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, col1, 30, "Max birth per cycle:"));
	max_birth=new ppl7::tk::HorizontalSlider(col1, y, client.width() - col1 - 20, 30);
	max_birth->setEventHandler(this);
	max_birth->setLimits(1, 100);
	max_birth->enableSpinBox(true, 1, 60);
	addChild(max_birth);
	y+=35;

	int sw=(client.width() - col1 - 40 - 40) / 2;
	addChild(new ppl7::tk::Label(0, y, col1, 30, "Next birth time (sec):"));
	addChild(new ppl7::tk::Label(col1, y, 40, 30, "min:"));
	birth_time_min=new ppl7::tk::DoubleHorizontalSlider(col1 + 40, y, sw, 30);
	birth_time_min->setEventHandler(this);
	birth_time_min->setLimits(0.010, 4.0f);
	birth_time_min->enableSpinBox(true, 0.01f, 3, 80);
	addChild(birth_time_min);
	addChild(new ppl7::tk::Label(col1 + 40 + sw, y, 40, 30, "max:"));
	birth_time_max=new ppl7::tk::DoubleHorizontalSlider(col1 + 80 + sw, y, sw, 30);
	birth_time_max->setEventHandler(this);
	birth_time_max->setLimits(0.010, 4.0f);
	birth_time_max->enableSpinBox(true, 0.01f, 3, 80);
	addChild(birth_time_max);
	y+=35;



	addChild(new ppl7::tk::Label(0, y, col1, 30, "Velocity:"));
	addChild(new ppl7::tk::Label(col1, y, 40, 30, "min:"));
	min_velocity_y=new ppl7::tk::DoubleHorizontalSlider(col1 + 40, y, sw, 30);
	min_velocity_y->setEventHandler(this);
	min_velocity_y->setLimits(0.010, 20.0f);
	min_velocity_y->enableSpinBox(true, 0.01f, 3, 80);
	addChild(min_velocity_y);
	addChild(new ppl7::tk::Label(col1 + 40 + sw, y, 40, 30, "max:"));
	max_velocity_y=new ppl7::tk::DoubleHorizontalSlider(col1 + 80 + sw, y, sw, 30);
	max_velocity_y->setEventHandler(this);
	max_velocity_y->setLimits(0.010, 20.0f);
	max_velocity_y->enableSpinBox(true, 0.01f, 3, 80);
	addChild(max_velocity_y);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, col1, 30, "Horizontal Velocity:"));
	addChild(new ppl7::tk::Label(col1, y, 40, 30, "max:"));
	max_velocity_x=new ppl7::tk::DoubleHorizontalSlider(col1 + 40, y, sw, 30);
	max_velocity_x->setEventHandler(this);
	max_velocity_x->setLimits(0.010, 5.0f);
	max_velocity_x->enableSpinBox(true, 0.01f, 3, 80);
	addChild(max_velocity_x);
	y+=35;

	// Scale
	addChild(new ppl7::tk::Label(0, y, col1, 30, "Scale:"));
	addChild(new ppl7::tk::Label(col1, y, 40, 30, "min:"));
	sw=(client.width() - col1 - 40 - 40) / 2;
	scale_min=new ppl7::tk::DoubleHorizontalSlider(col1 + 40, y, sw, 30);
	scale_min->setEventHandler(this);
	scale_min->setLimits(0.010, 2.0f);
	scale_min->enableSpinBox(true, 0.01f, 3, 80);
	addChild(scale_min);
	addChild(new ppl7::tk::Label(col1 + 40 + sw, y, 40, 30, "max:"));
	scale_max=new ppl7::tk::DoubleHorizontalSlider(col1 + 80 + sw, y, sw, 30);
	scale_max->setEventHandler(this);
	scale_max->setLimits(0.010, 2.0f);
	scale_max->enableSpinBox(true, 0.01f, 3, 80);
	addChild(scale_max);
	y+=35;

	// Age
	addChild(new ppl7::tk::Label(0, y, col1, 30, "Age (sec):"));
	addChild(new ppl7::tk::Label(col1, y, 40, 30, "min:"));
	sw=(client.width() - col1 - 40 - 40) / 2;
	age_min=new ppl7::tk::DoubleHorizontalSlider(col1 + 40, y, sw, 30);
	age_min->setEventHandler(this);
	age_min->setLimits(0.010f, 10.0f);
	age_min->enableSpinBox(true, 0.01f, 3, 80);
	addChild(age_min);
	addChild(new ppl7::tk::Label(col1 + 40 + sw, y, 40, 30, "max:"));
	age_max=new ppl7::tk::DoubleHorizontalSlider(col1 + 80 + sw, y, sw, 30);
	age_max->setEventHandler(this);
	age_max->setLimits(0.010f, 10.0f);
	age_max->enableSpinBox(true, 0.01f, 3, 80);
	addChild(age_max);
	y+=35;

	setValuesToUi(object);
}

void RainEmitterDialog::setValuesToUi(const RainEmitter* object)
{
	particle_type->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->particle_type)));
	color->setColor(object->ParticleColor);
	emitter_width->setValue(object->emitter_stud_width);
	max_birth->setValue(object->max_particle_birth_per_cycle);
	birth_time_min->setValue(object->birth_time_min);
	birth_time_max->setValue(object->birth_time_max);
	min_velocity_y->setValue(object->min_velocity_y);
	max_velocity_y->setValue(object->max_velocity_y);
	max_velocity_x->setValue(object->max_velocity_x);
	scale_min->setValue(object->scale_min);
	scale_max->setValue(object->scale_max);
	age_min->setValue(object->age_min);
	age_max->setValue(object->age_max);

}


void RainEmitterDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with int value\n");
	ppl7::tk::Widget* widget=event->widget();
	if (widget == particle_type) {
		object->particle_type=static_cast<RainEmitter::ParticleType>(particle_type->currentIdentifier().toInt());
	} else if (widget == color) {
		object->ParticleColor=color->color();
	}
}

void RainEmitterDialog::valueChangedEvent(ppl7::tk::Event* event, int64_t value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with int64_t value\n");
	ppl7::tk::Widget* widget=event->widget();
	if (widget == emitter_width) {
		object->emitter_stud_width=value;
	} else if (widget == max_birth) {
		object->max_particle_birth_per_cycle=value;
	}

}

void RainEmitterDialog::valueChangedEvent(ppl7::tk::Event* event, double value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with double value\n");
	ppl7::tk::Widget* widget=event->widget();
	if (widget == birth_time_min) {
		object->birth_time_min=value;
	} else if (widget == birth_time_max) {
		object->birth_time_max=value;
	} else if (widget == min_velocity_y) {
		object->min_velocity_y=(float)value;
	} else if (widget == max_velocity_y) {
		object->max_velocity_y=(float)value;
	} else if (widget == max_velocity_x) {
		object->max_velocity_x=(float)value;
	} else if (widget == scale_min) {
		object->scale_min=(float)value;
	} else if (widget == scale_max) {
		object->scale_max=(float)value;
	} else if (widget == age_min) {
		object->age_min=(float)value;
	} else if (widget == age_max) {
		object->age_max=(float)value;
	}
}


static RainEmitter clipboard;

void RainEmitterDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button == Dialog::Buttons::Copy) {
		clipboard.particle_type=object->particle_type;
		clipboard.ParticleColor=object->ParticleColor;
		clipboard.emitter_stud_width=object->emitter_stud_width;
		clipboard.max_particle_birth_per_cycle=object->max_particle_birth_per_cycle;
		clipboard.birth_time_min=object->birth_time_min;
		clipboard.birth_time_max=object->birth_time_max;
		clipboard.max_velocity_x=object->max_velocity_x;
		clipboard.min_velocity_y=object->min_velocity_y;
		clipboard.max_velocity_y=object->max_velocity_y;
		clipboard.scale_min=object->scale_min;
		clipboard.scale_max=object->scale_max;
		clipboard.age_min=object->age_min;
		clipboard.age_max=object->age_max;

	} else if (button == Dialog::Buttons::Paste) {
		object->particle_type=clipboard.particle_type;
		object->ParticleColor=clipboard.ParticleColor;
		object->emitter_stud_width=clipboard.emitter_stud_width;
		object->max_particle_birth_per_cycle=clipboard.max_particle_birth_per_cycle;
		object->birth_time_min=clipboard.birth_time_min;
		object->birth_time_max=clipboard.birth_time_max;
		object->max_velocity_x=clipboard.max_velocity_x;
		object->min_velocity_y=clipboard.min_velocity_y;
		object->max_velocity_y=clipboard.max_velocity_y;
		object->scale_min=clipboard.scale_min;
		object->scale_max=clipboard.scale_max;
		object->age_min=clipboard.age_min;
		object->age_max=clipboard.age_max;

		setValuesToUi(object);
	}
}

}	// EOF namespace Decker::Objects
