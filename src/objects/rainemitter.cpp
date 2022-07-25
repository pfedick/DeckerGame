#include <ppl7.h>
#include <ppl7-grafix.h>
#include <SDL.h>
#include "objects.h"
#include "decker.h"
#include "player.h"

namespace Decker::Objects {

/****************************************************************
 * Rain Particle
 ***************************************************************/

class RainParticle : public Particle
{
public:
	ppl7::grafix::Point end;
	void update(double time, TileTypePlane& ttplane, float frame_rate_compensation);
};


void RainParticle::update(double time, TileTypePlane& ttplane, float frame_rate_compensation)
{
	Particle::update(time, ttplane, frame_rate_compensation);
	if (p.y > end.y) death_time=0.0f;
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
	particle_type=Particle::Type::RotatingParticleTransparent;
	particle_layer=Particle::Layer::BehindPlayer;
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
	current_state=true;
	flags=static_cast<int>(RainEmitter::Flags::initialStateEnabled);


	save_size+=46;
}

static float randf(float min, float max)
{
	double range=max - min;
	double r=(((double)ppl7::rand(0, RAND_MAX)) / (double)RAND_MAX * range) + min;
	return r;
}

void RainEmitter::createParticle(ParticleSystem* ps, const TileTypePlane& ttplane, double time)
{
	RainParticle* particle=new RainParticle();
	particle->p.x=p.x + ppl7::rand(0, TILE_WIDTH * emitter_stud_width) - ((TILE_WIDTH * emitter_stud_width) / 2);
	particle->p.y=p.y - 40;
	particle->layer=particle_layer;
	particle->velocity.x=randf(0, max_velocity_x) - (max_velocity_x / 2.0f);
	particle->velocity.y=randf(min_velocity_y, max_velocity_y);
	particle->end=particle->p;
	particle->scale=randf(scale_min, scale_max);
	particle->color_mod=ParticleColor;
	particle->birth_time=time;
	particle->death_time=randf(age_min, age_max) + time;
	particle->initAnimation(particle_type);
	ppl7::grafix::Size levelsize=ttplane.size();
	levelsize.height*=TILE_HEIGHT;
	while (particle->end.y < levelsize.height && ttplane.getType(particle->end) != TileType::Blocking)
		particle->end.y+=TILE_HEIGHT;
	particle->end.y-=TILE_HEIGHT;
	ps->addParticle(particle);
}

void RainEmitter::update(double time, TileTypePlane& ttplane, Player& player)
{
	if (next_birth < time && current_state == true) {
		ParticleSystem* ps=GetParticleSystem();
		next_birth=time + randf(birth_time_min, birth_time_max);
		double d=ppl7::grafix::Distance(ppl7::grafix::PointF(player.WorldCoords.x + player.Viewport.width() / 2,
			player.WorldCoords.y + player.Viewport.height() / 2), p);
		if (d > 2 * player.Viewport.width()) return;
		int new_particles=ppl7::rand(1, max_particle_birth_per_cycle);
		for (int i=0;i < new_particles;i++) {
			createParticle(ps, ttplane, time);
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
	ppl7::Poke8(buffer + bytes + 43, static_cast<int>(particle_layer));
	ppl7::Poke16(buffer + bytes + 44, flags);
	return bytes + 46;
}

size_t RainEmitter::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 43) return 0;
	particle_type=static_cast<Particle::Type>(ppl7::Peek8(buffer + bytes));
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
	if (size > bytes + 44) {
		particle_layer=static_cast<Particle::Layer>(ppl7::Peek8(buffer + bytes + 43));
		flags=ppl7::Peek16(buffer + bytes + 44);
		if (flags & static_cast<int>(Flags::initialStateEnabled)) current_state=true;
		else current_state=false;
	}
	return size;
}

void RainEmitter::toggle(bool enable, Object* source)
{
	current_state=enable;

}



/****************************************************************
 * Rain Ui
 ***************************************************************/




class RainEmitterDialog : public Decker::ui::Dialog
{
private:
	Decker::ui::ColorSliderWidget* color;
	ppl7::tk::ComboBox* particle_type;
	ppl7::tk::ComboBox* particle_layer;
	ppl7::tk::HorizontalSlider* emitter_width;
	ppl7::tk::HorizontalSlider* max_birth;
	ppl7::tk::DoubleHorizontalSlider* birth_time_min, * birth_time_max;
	ppl7::tk::DoubleHorizontalSlider* min_velocity_y, * max_velocity_y, * max_velocity_x;
	ppl7::tk::DoubleHorizontalSlider* scale_min, * scale_max;
	ppl7::tk::DoubleHorizontalSlider* age_min, * age_max;
	ppl7::tk::CheckBox* initial_state_checkbox, * current_state_checkbox;
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
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked) override;
};

void RainEmitter::openUi()
{
	RainEmitterDialog* dialog=new RainEmitterDialog(this);
	GetGameWindow()->addChild(dialog);
}


RainEmitterDialog::RainEmitterDialog(RainEmitter* object)
	: Decker::ui::Dialog(700, 560, Dialog::Buttons::OK | Dialog::Buttons::CopyAndPaste)
{
	this->object=object;
	this->setWindowTitle("Rain Emitter");
	int col1=100;
	int y=0;
	ppl7::grafix::Rect client=clientRect();
	int sw=(client.width() - col1 - 40 - 40) / 2;

	addChild(new ppl7::tk::Label(0, y, col1, 30, "Particle Type:"));
	particle_type=new ppl7::tk::ComboBox(col1, y, sw, 30);
	particle_type->setEventHandler(this);
	particle_type->add("Transparent particle", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingParticleTransparent)));
	particle_type->add("White particle", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingParticleWhite)));
	particle_type->add("Transparent Snowflake", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingSnowflakeTransparent)));
	particle_type->add("White Snowflake", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingSnowflakeWhite)));
	addChild(particle_type);
	addChild(new ppl7::tk::Label(col1 + 40 + sw, y, col1, 40, "Layer:"));
	particle_layer=new ppl7::tk::ComboBox(col1 + 90 + sw, y, sw - 10, 30);
	particle_layer->add("Before Player", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BeforePlayer)));
	particle_layer->add("Behind Player", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BehindPlayer)));
	particle_layer->add("Behind Bricks", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BehindBricks)));
	particle_layer->add("Backplane Front", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BackplaneFront)));
	particle_layer->add("Backplane Back", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BackplaneBack)));
	particle_layer->add("Frontplane Front", ppl7::ToString("%d", static_cast<int>(Particle::Layer::FrontplaneFront)));
	particle_layer->add("Frontplane Back", ppl7::ToString("%d", static_cast<int>(Particle::Layer::FrontplaneBack)));
	particle_layer->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(Particle::Layer::BehindPlayer)));
	particle_layer->setEventHandler(this);
	addChild(particle_layer);
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

	// State
	initial_state_checkbox=new ppl7::tk::CheckBox(0, y, col1 + 40 + sw, 30, "initial State");
	initial_state_checkbox->setEventHandler(this);
	addChild(initial_state_checkbox);
	current_state_checkbox=new ppl7::tk::CheckBox(col1 + 40 + sw, y, sw, 30, "current State");
	current_state_checkbox->setEventHandler(this);
	addChild(current_state_checkbox);

	setValuesToUi(object);
}

void RainEmitterDialog::setValuesToUi(const RainEmitter* object)
{
	particle_type->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->particle_type)));
	particle_layer->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->particle_layer)));
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
	current_state_checkbox->setChecked(object->current_state);
	initial_state_checkbox->setChecked(object->flags & static_cast<int>(RainEmitter::Flags::initialStateEnabled));

}


void RainEmitterDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with int value\n");
	ppl7::tk::Widget* widget=event->widget();
	if (widget == particle_type) {
		object->particle_type=static_cast<Particle::Type>(particle_type->currentIdentifier().toInt());
	} else if (widget == color) {
		object->ParticleColor=color->color();
	} else if (widget == particle_layer) {
		object->particle_layer=static_cast<Particle::Layer>(particle_layer->currentIdentifier().toInt());
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
		if (value > birth_time_max->value()) birth_time_max->setValue(value);
	} else if (widget == birth_time_max) {
		object->birth_time_max=value;
		if (value < birth_time_min->value()) birth_time_min->setValue(value);
	} else if (widget == min_velocity_y) {
		object->min_velocity_y=(float)value;
		if (value > max_velocity_y->value()) max_velocity_y->setValue(value);
	} else if (widget == max_velocity_y) {
		object->max_velocity_y=(float)value;
		if (value < min_velocity_y->value()) min_velocity_y->setValue(value);
	} else if (widget == max_velocity_x) {
		object->max_velocity_x=(float)value;
	} else if (widget == scale_min) {
		object->scale_min=(float)value;
		if (value > scale_max->value()) scale_max->setValue(value);
	} else if (widget == scale_max) {
		object->scale_max=(float)value;
		if (value < scale_min->value()) scale_min->setValue(value);
	} else if (widget == age_min) {
		object->age_min=(float)value;
		if (value > age_max->value()) age_max->setValue(value);
	} else if (widget == age_max) {
		object->age_max=(float)value;
		if (value < age_min->value()) age_min->setValue(value);
	}
}

void RainEmitterDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	if (event->widget() == initial_state_checkbox) {
		int flags=object->flags & (0xffff - static_cast<int>(RainEmitter::Flags::initialStateEnabled));
		if (initial_state_checkbox->checked()) flags|=static_cast<int>(RainEmitter::Flags::initialStateEnabled);
		object->flags=flags;
		needsRedraw();
	} else if (event->widget() == current_state_checkbox) {
		object->current_state=current_state_checkbox->checked();
		needsRedraw();
	}
	//printf("flags=%d\n", object->flags);

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
