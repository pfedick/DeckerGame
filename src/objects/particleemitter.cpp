#include <ppl7.h>
#include <ppl7-grafix.h>
#include <SDL.h>
#include "objects.h"
#include "decker.h"
#include "player.h"

namespace Decker::Objects {


/****************************************************************
 * Particle Emitter
 ***************************************************************/

Representation ParticleEmitter::representation()
{
	return Representation(Spriteset::GenericObjects, 302);
}



ParticleEmitter::ParticleEmitter()
	:Object(Type::ObjectType::ParticleEmitter)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=302;
	collisionDetection=false;
	visibleAtPlaytime=false;
	sprite_no_representation=302;
	next_birth=0.0f;
	particle_type=Particle::Type::RotatingParticleWhite;
	emitter_type=EmitterType::Point;
	particle_layer=Particle::Layer::BehindPlayer;
	ParticleColor.set(255, 255, 255, 255);
	emitter_size.setSize(1, 1);
	flags=0;
	min_birth_per_cycle=1;
	max_birth_per_cycle = 4;
	birth_time_min=0.020f;
	birth_time_max=0.300f;
	direction=0.0f;
	variation=0.0f; // Abweichung in Grad
	min_velocity=4.0f;
	max_velocity=7.3f;
	scale_min=0.3f;
	scale_max=1.0f;
	age_min=5.0f;
	age_max=10.0f;
	weight_min=1.0f;
	weight_max=1.0f;
	gravity.x=0.0f;
	gravity.y=0.0f;
	weight_min=0.0f;
	weight_max=0.0f;
	current_state=true;
}


void ParticleEmitter::createParticle(ParticleSystem* ps, double time)
{
	Particle* particle=new Particle();
	particle->birth_time=time;
	particle->death_time=randf(age_min, age_max) + time;
	particle->p=getBirthPosition(p, emitter_type, emitter_size, direction);
	particle->layer=particle_layer;
	particle->weight=randf(weight_min, weight_max);
	particle->gravity=gravity;
	particle->velocity=calculateVelocity(randf(min_velocity, max_velocity), direction + randf(-variation, variation));
	particle->scale=randf(scale_min, scale_max);
	particle->color_mod=ParticleColor;
	particle->initAnimation(particle_type);
	if (flags & static_cast<int>(Flags::useScaleGradient))
		particle->initScaleGradient(scale_gradient, particle->scale);
	if (flags & static_cast<int>(Flags::useColorGradient)) {
		particle->initColorGradient(color_gradient);
	}
	ps->addParticle(particle);
}

void ParticleEmitter::update(double time, TileTypePlane& ttplane, Player& player, float)
{
	if (next_birth < time && current_state == true) {
		ParticleSystem* ps=GetParticleSystem();
		next_birth=time + randf(birth_time_min, birth_time_max);
		if (!emitterInPlayerRange(static_cast<int>(myPlane), p, player)) return;
		int new_particles=ppl7::rand(min_birth_per_cycle, max_birth_per_cycle);
		for (int i=0;i < new_particles;i++) {
			createParticle(ps, time);
		}
	}
}

size_t ParticleEmitter::saveSize() const
{
	return Object::saveSize() + 76 + color_gradient.size() * 8 + scale_gradient.size() * 8;
}

size_t ParticleEmitter::save(unsigned char* buffer, size_t size) const
{
	memset(buffer, 0, size);
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	ppl7::Poke8(buffer + bytes + 1, static_cast<int>(particle_type));
	ppl7::Poke8(buffer + bytes + 2, static_cast<int>(emitter_type));
	ppl7::Poke16(buffer + bytes + 3, emitter_size.width);
	ppl7::Poke16(buffer + bytes + 5, emitter_size.height);
	ppl7::Poke8(buffer + bytes + 7, min_birth_per_cycle);
	ppl7::Poke8(buffer + bytes + 8, max_birth_per_cycle);
	ppl7::Poke8(buffer + bytes + 9, ParticleColor.red());
	ppl7::Poke8(buffer + bytes + 10, ParticleColor.green());
	ppl7::Poke8(buffer + bytes + 11, ParticleColor.blue());
	ppl7::Poke8(buffer + bytes + 12, ParticleColor.alpha());
	ppl7::PokeFloat(buffer + bytes + 13, birth_time_min);
	ppl7::PokeFloat(buffer + bytes + 17, birth_time_max);
	ppl7::PokeFloat(buffer + bytes + 21, direction);
	ppl7::PokeFloat(buffer + bytes + 25, variation);
	ppl7::PokeFloat(buffer + bytes + 29, min_velocity);
	ppl7::PokeFloat(buffer + bytes + 33, max_velocity);
	ppl7::PokeFloat(buffer + bytes + 37, scale_min);
	ppl7::PokeFloat(buffer + bytes + 41, scale_max);
	ppl7::PokeFloat(buffer + bytes + 45, age_min);
	ppl7::PokeFloat(buffer + bytes + 49, age_max);
	ppl7::PokeFloat(buffer + bytes + 53, weight_min);
	ppl7::PokeFloat(buffer + bytes + 57, weight_max);
	ppl7::PokeFloat(buffer + bytes + 61, gravity.x);
	ppl7::PokeFloat(buffer + bytes + 65, gravity.y);
	ppl7::Poke8(buffer + bytes + 69, static_cast<int>(particle_layer));
	ppl7::Poke16(buffer + bytes + 70, flags);
	ppl7::Poke8(buffer + bytes + 72, 0);	// unused
	size_t p=73;
	{
		ppl7::Poke8(buffer + bytes + p, color_gradient.size());	// Number of Color Gradients, bei 0 wird ParticleColor verwendet
		p++;
		std::list<Particle::ColorGradientItem>::const_iterator it;
		for (it=color_gradient.begin();it != color_gradient.end();++it) {
			ppl7::PokeFloat(buffer + bytes + p, (*it).age);
			ppl7::Poke8(buffer + bytes + p + 4, (*it).color.red());
			ppl7::Poke8(buffer + bytes + p + 5, (*it).color.green());
			ppl7::Poke8(buffer + bytes + p + 6, (*it).color.blue());
			ppl7::Poke8(buffer + bytes + p + 7, (*it).color.alpha());
			p+=8;
		}
	}
	{
		ppl7::Poke8(buffer + bytes + p, scale_gradient.size());	// Number of Size Gradients
		p++;
		std::list<Particle::ScaleGradientItem>::const_iterator it;
		for (it=scale_gradient.begin();it != scale_gradient.end();++it) {
			ppl7::PokeFloat(buffer + bytes + p, (*it).age);
			ppl7::PokeFloat(buffer + bytes + p + 4, (*it).scale);
			p+=8;
		}
	}
	ppl7::Poke8(buffer + bytes + p, 0);	// 0
	return bytes + 76 + color_gradient.size() * 8 + scale_gradient.size() * 8;

}

size_t ParticleEmitter::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;

	particle_type=static_cast<Particle::Type>(ppl7::Peek8(buffer + bytes + 1));
	emitter_type=static_cast<EmitterType>(ppl7::Peek8(buffer + bytes + 2));
	emitter_size.width=ppl7::Peek16(buffer + bytes + 3);
	emitter_size.height=ppl7::Peek16(buffer + bytes + 5);
	min_birth_per_cycle=ppl7::Peek8(buffer + bytes + 7);
	max_birth_per_cycle=ppl7::Peek8(buffer + bytes + 8);
	ParticleColor.setRed(ppl7::Peek8(buffer + bytes + 9));
	ParticleColor.setGreen(ppl7::Peek8(buffer + bytes + 10));
	ParticleColor.setBlue(ppl7::Peek8(buffer + bytes + 11));
	ParticleColor.setAlpha(ppl7::Peek8(buffer + bytes + 12));
	birth_time_min=ppl7::PeekFloat(buffer + bytes + 13);
	birth_time_max=ppl7::PeekFloat(buffer + bytes + 17);
	direction=ppl7::PeekFloat(buffer + bytes + 21);
	variation=ppl7::PeekFloat(buffer + bytes + 25);
	min_velocity=ppl7::PeekFloat(buffer + bytes + 29);
	max_velocity=ppl7::PeekFloat(buffer + bytes + 33);
	scale_min=ppl7::PeekFloat(buffer + bytes + 37);
	scale_max=ppl7::PeekFloat(buffer + bytes + 41);
	age_min=ppl7::PeekFloat(buffer + bytes + 45);
	age_max=ppl7::PeekFloat(buffer + bytes + 49);
	weight_min=ppl7::PeekFloat(buffer + bytes + 53);
	weight_max=ppl7::PeekFloat(buffer + bytes + 57);
	gravity.x=ppl7::PeekFloat(buffer + bytes + 61);
	gravity.y=ppl7::PeekFloat(buffer + bytes + 65);
	particle_layer=static_cast<Particle::Layer>(ppl7::Peek8(buffer + bytes + 69));
	flags=ppl7::Peek16(buffer + bytes + 70);
	// 72 is unused
	size_t p=73;

	int num_color_gradients=ppl7::Peek8(buffer + bytes + p);
	p++;
	color_gradient.clear();
	for (int i=0;i < num_color_gradients;i++) {
		Particle::ColorGradientItem c;
		c.age=ppl7::PeekFloat(buffer + bytes + p);
		c.color.setRed(ppl7::Peek8(buffer + bytes + p + 4));
		c.color.setGreen(ppl7::Peek8(buffer + bytes + p + 5));
		c.color.setBlue(ppl7::Peek8(buffer + bytes + p + 6));
		c.color.setAlpha(ppl7::Peek8(buffer + bytes + p + 7));
		color_gradient.push_back(c);
		p+=8;
	}
	int num_scale_gradients=ppl7::Peek8(buffer + bytes + p);
	p++;
	scale_gradient.clear();
	for (int i=0;i < num_scale_gradients;i++) {
		Particle::ScaleGradientItem s;
		s.age=ppl7::PeekFloat(buffer + bytes + p);
		s.scale=ppl7::PeekFloat(buffer + bytes + p + 4);
		scale_gradient.push_back(s);
		p+=8;
	}

	if (flags & static_cast<int>(Flags::initialStateDisabled)) current_state=false;
	else current_state=true;

	return size;
}

void ParticleEmitter::toggle(bool enable, Object* source)
{
	current_state=enable;

}

void ParticleEmitter::trigger(Object* source)
{
	current_state=!current_state;

}


ppl7::String ParticleEmitter::generateCode() const
{
	ppl7::String Tmp;
	ppl7::String code="/*\n * add the following variables to your object class:\n */\n";
	code+="    double next_birth;\n";
	if (scale_gradient.size() > 0 && (flags & static_cast<int>(Flags::useScaleGradient))) code +="    std::list<Particle::ScaleGradientItem>scale_gradient;\n";
	if (color_gradient.size() > 0 && (flags & static_cast<int>(Flags::useColorGradient))) code +="    std::list<Particle::ColorGradientItem>color_gradient;\n";
	code +="/*\n * add the following code to your constructor:\n */\n";
	code +="    next_birth=0.0f;\n";
	if (flags & static_cast<int>(Flags::useScaleGradient)) {
		std::list<Particle::ScaleGradientItem>::const_iterator it;
		for (it=scale_gradient.begin();it != scale_gradient.end();++it) {
			code.appendf("    scale_gradient.push_back(Particle::ScaleGradientItem(%0.3f, %0.3f));\n",
				it->age, it->scale);
		}
	}
	if (flags & static_cast<int>(Flags::useColorGradient)) {
		std::list<Particle::ColorGradientItem>::const_iterator it;
		for (it=color_gradient.begin();it != color_gradient.end();++it) {
			code.appendf("    color_gradient.push_back(Particle::ColorGradientItem(%0.3f, ppl7::grafix::Color(%d, %d, %d, %d)));\n",
				it->age, it->color.red(), it->color.green(), it->color.blue(), it->color.alpha());
		}
	}
	code +="\n/*\n * add the following function to your class:\n */\n\n";
	code +="void MyObject::emmitParticles(double time, const Player& player)\n";
	code +="{\n";
	code +="    if (next_birth < time) {\n";
	code.appendf("        next_birth=time + randf(%0.3f, %0.3f);\n", birth_time_min, birth_time_max);
	code +="        ParticleSystem* ps=GetParticleSystem();\n";
	code +="        if (!emitterInPlayerRange(p, player)) return;\n";
	code.appendf("        int new_particles=ppl7::rand(%d, %d);\n", min_birth_per_cycle, max_birth_per_cycle);
	code +="        for (int i=0;i < new_particles;i++) {\n";
	code +="            Particle* particle=new Particle();\n";
	code +="            particle->birth_time=time;\n";
	code.appendf("            particle->death_time=randf(%0.3f, %0.3f) + time;\n", age_min, age_max);
	if (emitter_type == EmitterType::Point) {
		code +="            particle->p=p;\n";
	} else {
		Tmp="EmitterType::Ellipse";
		if (emitter_type == EmitterType::Rectangle) Tmp="EmitterType::Rectangle";
		code.appendf("            particle->p=getBirthPosition(p, %s, ppl7::grafix::Size(%d, %d), %0.3f);\n",
			(const char*)Tmp, emitter_size.width, emitter_size.height, direction);
	}
	switch (particle_layer) {
		case Particle::Layer::BehindBricks: Tmp="BehindBricks"; break;
		case Particle::Layer::BehindPlayer: Tmp="BehindPlayer"; break;
		case Particle::Layer::BeforePlayer: Tmp="BeforePlayer"; break;
		case Particle::Layer::BackplaneFront: Tmp="BackplaneFront"; break;
		case Particle::Layer::BackplaneBack: Tmp="BackplaneBack"; break;
		case Particle::Layer::FrontplaneFront: Tmp="FrontplaneFront"; break;
		case Particle::Layer::FrontplaneBack: Tmp="FrontplaneBack"; break;
		default: Tmp="BeforePlayer";
	}
	code.appendf("            particle->layer=Particle::Layer::%s;\n", (const char*)Tmp);
	code.appendf("            particle->weight=randf(%0.3f, %0.3f);\n", weight_min, weight_max);
	code.appendf("            particle->gravity.setPoint(%0.3f, %0.3f);\n", gravity.x, gravity.y);
	code.appendf("            particle->velocity=calculateVelocity(randf(%0.3f, %0.3f), %0.3f + randf(-%0.3f, %0.3f));\n",
		min_velocity, max_velocity, direction, variation, variation);
	code.appendf("            particle->scale=randf(%0.3f, %0.3f);\n", scale_min, scale_max);
	code.appendf("            particle->color_mod.set(%d, %d, %d, %d);\n", ParticleColor.red(), ParticleColor.green(),
		ParticleColor.blue(), ParticleColor.alpha());
	switch (particle_type) {
		case Particle::Type::RotatingParticleTransparent: Tmp="RotatingParticleTransparent"; break;
		case Particle::Type::RotatingParticleWhite: Tmp="RotatingParticleWhite"; break;
		case Particle::Type::RotatingSnowflakeTransparent: Tmp="RotatingSnowflakeTransparent"; break;
		case Particle::Type::RotatingSnowflakeWhite: Tmp="RotatingSnowflakeWhite"; break;
		case Particle::Type::RotatingCylinder: Tmp="RotatingCylinder"; break;
		case Particle::Type::StaticParticle: Tmp="StaticParticle"; break;
		case Particle::Type::StaticParticleBig: Tmp="StaticParticleBig"; break;
		case Particle::Type::StaticCircle: Tmp="StaticCircle"; break;
		case Particle::Type::StaticCircleBig: Tmp="StaticCircleBig"; break;
		case Particle::Type::StaticBulletSmall: Tmp="StaticBulletSmall"; break;
		case Particle::Type::StaticBulletBig: Tmp="StaticBulletBig"; break;
		case Particle::Type::StaticStudSmall: Tmp="StaticStudSmall"; break;
		case Particle::Type::StaticStudBig: Tmp="StaticStudBig"; break;
		case Particle::Type::SoftGradientSmall: Tmp="SoftGradientSmall"; break;
		case Particle::Type::SoftGradientMedium: Tmp="SoftGradientMedium"; break;
		case Particle::Type::SoftGradientBig: Tmp="SoftGradientBig"; break;
		default: Tmp="RotatingParticleWhite"; break;
	}
	code.appendf("            particle->initAnimation(Particle::Type::%s);\n", (const char*)Tmp);
	if (flags & static_cast<int>(Flags::useScaleGradient)) {
		code +="            particle->initScaleGradient(scale_gradient, particle->scale);\n";
	}
	if (flags & static_cast<int>(Flags::useColorGradient)) {
		code +="            particle->initColorGradient(color_gradient);\n";
	}
	code +="            ps->addParticle(particle);\n";
	code +="        }\n";
	code +="    }\n";
	code +="}\n\n";
	return code;
}


/****************************************************************
 * Rain Ui
 ***************************************************************/




class ParticleEmitterDialog : public Decker::ui::Dialog
{
private:
	ppltk::TabWidget* tabwidget;

	// Particle Tab
	ppltk::ComboBox* particle_layer;
	ppltk::ComboBox* emitter_type;
	ppltk::ComboBox* particle_type;
	ppltk::HorizontalSlider* emitter_pixel_width, * emitter_pixel_height;
	ppltk::HorizontalSlider* min_birth_per_cycle, * max_birth_per_cycle;
	ppltk::DoubleHorizontalSlider* birth_time_min, * birth_time_max;
	ppltk::DoubleHorizontalSlider* min_velocity, * max_velocity;
	ppltk::DoubleHorizontalSlider* scale_min, * scale_max;
	ppltk::DoubleHorizontalSlider* age_min, * age_max;
	ppltk::DoubleHorizontalSlider* weight_min, * weight_max;
	ppltk::DoubleHorizontalSlider* direction, * variation, * gravity_x, * gravity_y;
	ppltk::CheckBox* initial_state_checkbox, * current_state_checkbox;

	// Color Tab
	Decker::ui::ColorSliderWidget* color;
	ppltk::RadioButton* radio_solid_color;
	ppltk::RadioButton* radio_color_gradient;
	Decker::ui::ColorSliderWidget* color_gradient;
	ppltk::DoubleHorizontalSlider* color_gradient_age;
	Decker::ui::GradientWidget* gradient_widget;

	// Scale Gradient
	ppltk::CheckBox* checkbox_scale_gradient;
	Decker::ui::GradientWidget* scale_gradient_widget;
	ppltk::DoubleHorizontalSlider* scale_gradient_age;
	ppltk::DoubleHorizontalSlider* scale_gradient_scale;


	void setValuesToUi(const ParticleEmitter* object);
	void setupParticleTab();
	void setupColorTab();
	void setupSizeTab();

	void copyColorGradientToObject();
	void copyScaleGradientToObject();
	ParticleEmitter* object;

public:
	ParticleEmitterDialog(ParticleEmitter* object);
	virtual void selectionChangedEvent(ppltk::Event* event) override;
	virtual void valueChangedEvent(ppltk::Event* event, int value) override;
	virtual void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	virtual void valueChangedEvent(ppltk::Event* event, double value) override;
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
	virtual void toggledEvent(ppltk::Event* event, bool checked) override;
};

void ParticleEmitter::openUi()
{
	ParticleEmitterDialog* dialog=new ParticleEmitterDialog(this);
	GetGameWindow()->addChild(dialog);
}


ParticleEmitterDialog::ParticleEmitterDialog(ParticleEmitter* object)
	: Decker::ui::Dialog(750, 620, Dialog::Buttons::OK | Dialog::Buttons::CopyAndPaste)
{
	this->object=object;
	this->setWindowTitle("Particle Emitter");
	ppl7::grafix::Rect client=clientRect();

	tabwidget=new ppltk::TabWidget(client.x1, client.y1, client.width(), client.height());
	tabwidget->addTab("Particle generation");
	tabwidget->addTab("Colors");
	tabwidget->addTab("Size gradient");
	//tabwidget->setCurrentTab(2);
	addChild(tabwidget);
	setupParticleTab();
	setupColorTab();
	setupSizeTab();
	tabwidget->setCurrentIndex(0);
	setValuesToUi(object);
}

void ParticleEmitterDialog::setupParticleTab()
{
	Widget* tab=tabwidget->getWidget(0);
	if (!tab) return;

	int col1=100;
	int y=0;
	ppl7::grafix::Rect client=tab->clientRect();

	tab->addChild(new ppltk::Label(0, 0, 60, 30, "Layer:"));
	particle_layer=new ppltk::ComboBox(60, y, 150, 30);
	particle_layer->add("Before Player", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BeforePlayer)));
	particle_layer->add("Behind Player", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BehindPlayer)));
	particle_layer->add("Behind Bricks", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BehindBricks)));
	particle_layer->add("Backplane Front", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BackplaneFront)));
	particle_layer->add("Backplane Back", ppl7::ToString("%d", static_cast<int>(Particle::Layer::BackplaneBack)));
	particle_layer->add("Frontplane Front", ppl7::ToString("%d", static_cast<int>(Particle::Layer::FrontplaneFront)));
	particle_layer->add("Frontplane Back", ppl7::ToString("%d", static_cast<int>(Particle::Layer::FrontplaneBack)));
	particle_layer->add("Nearplane Front", ppl7::ToString("%d", static_cast<int>(Particle::Layer::NearPlaneFront)));
	particle_layer->add("Nearplane Back", ppl7::ToString("%d", static_cast<int>(Particle::Layer::NearPlaneBack)));
	particle_layer->add("Middleplane Front", ppl7::ToString("%d", static_cast<int>(Particle::Layer::MiddlePlaneFront)));
	particle_layer->add("Middleplane Back", ppl7::ToString("%d", static_cast<int>(Particle::Layer::MiddlePlaneBack)));
	particle_layer->add("Farplane Front", ppl7::ToString("%d", static_cast<int>(Particle::Layer::FarPlaneFront)));
	particle_layer->add("Farplane Back", ppl7::ToString("%d", static_cast<int>(Particle::Layer::FarPlaneBack)));
	particle_layer->add("Horizonplane Front", ppl7::ToString("%d", static_cast<int>(Particle::Layer::HorizonPlaneFront)));
	particle_layer->add("Horizonplane Back", ppl7::ToString("%d", static_cast<int>(Particle::Layer::HorizonPlaneBack)));



	particle_layer->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(Particle::Layer::BehindPlayer)));
	particle_layer->setEventHandler(this);
	tab->addChild(particle_layer);

	tab->addChild(new ppltk::Label(220, y, 100, 30, "Emitter Type:"));
	emitter_type=new ppltk::ComboBox(320, y, client.width() - 330, 30);
	emitter_type->setEventHandler(this);
	emitter_type->add("Point", ppl7::ToString("%d", static_cast<int>(EmitterType::Point)));
	emitter_type->add("Rectangle", ppl7::ToString("%d", static_cast<int>(EmitterType::Rectangle)));
	emitter_type->add("Ellipse", ppl7::ToString("%d", static_cast<int>(EmitterType::Ellipse)));

	tab->addChild(emitter_type);
	y+=35;

	tab->addChild(new ppltk::Label(220, y, 100, 30, "Particle Type:"));
	particle_type=new ppltk::ComboBox(320, y, client.width() - 330, 30);
	particle_type->setEventHandler(this);
	particle_type->add("rotating transparent particle", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingParticleTransparent)));
	particle_type->add("rotating particle", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingParticleWhite)));
	particle_type->add("rotating snowflake transparent", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingSnowflakeTransparent)));
	particle_type->add("rotating snowflake", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingSnowflakeWhite)));
	particle_type->add("rotating cylinder", ppl7::ToString("%d", static_cast<int>(Particle::Type::RotatingCylinder)));
	particle_type->add("static particle", ppl7::ToString("%d", static_cast<int>(Particle::Type::StaticParticle)));
	particle_type->add("static particle big", ppl7::ToString("%d", static_cast<int>(Particle::Type::StaticParticleBig)));
	particle_type->add("static circle", ppl7::ToString("%d", static_cast<int>(Particle::Type::StaticCircle)));
	particle_type->add("static circle big", ppl7::ToString("%d", static_cast<int>(Particle::Type::StaticCircleBig)));
	particle_type->add("static bullet small", ppl7::ToString("%d", static_cast<int>(Particle::Type::StaticBulletSmall)));
	particle_type->add("static bullet big", ppl7::ToString("%d", static_cast<int>(Particle::Type::StaticBulletBig)));
	particle_type->add("static stud small", ppl7::ToString("%d", static_cast<int>(Particle::Type::StaticStudSmall)));
	particle_type->add("static stud big", ppl7::ToString("%d", static_cast<int>(Particle::Type::StaticStudBig)));
	particle_type->add("soft gradient small", ppl7::ToString("%d", static_cast<int>(Particle::Type::SoftGradientSmall)));
	particle_type->add("soft gradient medium", ppl7::ToString("%d", static_cast<int>(Particle::Type::SoftGradientMedium)));
	particle_type->add("soft gradient big", ppl7::ToString("%d", static_cast<int>(Particle::Type::SoftGradientBig)));


	tab->addChild(particle_type);
	y+=35;

	int col0=0;
	int w0=150;
	col1=150;
	int w1=50;
	int w3=70;
	int col2=col1 + w1;
	int sw=(client.width() - col2 - w3) / 2;
	int w2=sw;
	int w4=sw;
	int col3=col2 + w2 + 10;
	int col4=col3 + w3;



	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Emitter size (pixel):"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "width:"));
	emitter_pixel_width=new ppltk::HorizontalSlider(col2, y, w2, 30);
	emitter_pixel_width->setEventHandler(this);
	emitter_pixel_width->setLimits(1, 1024);
	emitter_pixel_width->enableSpinBox(true, 1, 80);
	tab->addChild(emitter_pixel_width);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "height:"));
	emitter_pixel_height=new ppltk::HorizontalSlider(col4, y, w4, 30);
	emitter_pixel_height->setEventHandler(this);
	emitter_pixel_height->setLimits(1, 1024);
	emitter_pixel_height->enableSpinBox(true, 1, 80);
	tab->addChild(emitter_pixel_height);

	y+=35;


	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Next birth time (sec):"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "min:"));
	birth_time_min=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	birth_time_min->setEventHandler(this);
	birth_time_min->setLimits(0.010, 4.0f);
	birth_time_min->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(birth_time_min);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "max:"));
	birth_time_max=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	birth_time_max->setEventHandler(this);
	birth_time_max->setLimits(0.010, 4.0f);
	birth_time_max->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(birth_time_max);
	y+=35;


	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Birth per cycle:"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "min:"));
	min_birth_per_cycle=new ppltk::HorizontalSlider(col2, y, w2, 30);
	min_birth_per_cycle->setEventHandler(this);
	min_birth_per_cycle->setLimits(0, 200);
	min_birth_per_cycle->enableSpinBox(true, 1, 80);
	tab->addChild(min_birth_per_cycle);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "max:"));
	max_birth_per_cycle=new ppltk::HorizontalSlider(col4, y, w4, 30);
	max_birth_per_cycle->setEventHandler(this);
	max_birth_per_cycle->setLimits(0, 200);
	max_birth_per_cycle->enableSpinBox(true, 1, 80);
	tab->addChild(max_birth_per_cycle);
	y+=35;


	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Velocity:"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "min:"));
	min_velocity=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	min_velocity->setEventHandler(this);
	min_velocity->setLimits(0.0f, 20.0f);
	min_velocity->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(min_velocity);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "max:"));
	max_velocity=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	max_velocity->setEventHandler(this);
	max_velocity->setLimits(0.0f, 20.0f);
	max_velocity->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(max_velocity);
	y+=35;

	// Direction + variation
	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Direction (degrees):"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "base:"));
	direction=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	direction->setEventHandler(this);
	direction->setLimits(0.0f, 360.0f);
	direction->enableSpinBox(true, 15.0f, 1, 80);
	tab->addChild(direction);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "variation:"));
	variation=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	variation->setEventHandler(this);
	variation->setLimits(0.0f, 180.0f);
	variation->enableSpinBox(true, 1.0f, 1, 80);
	tab->addChild(variation);
	y+=35;

	// Scale
	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Scale:"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "min:"));
	scale_min=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	scale_min->setEventHandler(this);
	scale_min->setLimits(0.010, 2.0f);
	scale_min->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(scale_min);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "max:"));
	scale_max=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	scale_max->setEventHandler(this);
	scale_max->setLimits(0.010, 5.0f);
	scale_max->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(scale_max);
	y+=35;

	// Age
	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Age (sec):"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "min:"));
	age_min=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	age_min->setEventHandler(this);
	age_min->setLimits(0.010f, 10.0f);
	age_min->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(age_min);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "max:"));
	age_max=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	age_max->setEventHandler(this);
	age_max->setLimits(0.010f, 10.0f);
	age_max->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(age_max);
	y+=35;

	// Weight
	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Weight:"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "min:"));
	weight_min=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	weight_min->setEventHandler(this);
	weight_min->setLimits(0.0f, 1.0f);
	weight_min->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(weight_min);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "max:"));
	weight_max=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	weight_max->setEventHandler(this);
	weight_max->setLimits(0.0f, 1.0f);
	weight_max->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(weight_max);
	y+=35;

	// Gravity
	tab->addChild(new ppltk::Label(col0, y, w0, 30, "Gravity:"));
	tab->addChild(new ppltk::Label(col1, y, w1, 30, "x:"));
	gravity_x=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	gravity_x->setEventHandler(this);
	gravity_x->setLimits(-1.0f, 1.0f);
	gravity_x->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(gravity_x);
	tab->addChild(new ppltk::Label(col3, y, w3, 30, "y:"));
	gravity_y=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	gravity_y->setEventHandler(this);
	gravity_y->setLimits(-1.0f, 1.0f);
	gravity_y->enableSpinBox(true, 0.01f, 3, 80);
	tab->addChild(gravity_y);
	y+=35;

	// State
	initial_state_checkbox=new ppltk::CheckBox(0, y, w0 + w1, 30, "initial State");
	initial_state_checkbox->setEventHandler(this);
	tab->addChild(initial_state_checkbox);
	current_state_checkbox=new ppltk::CheckBox(col2, y, w2, 30, "current State");
	current_state_checkbox->setEventHandler(this);
	tab->addChild(current_state_checkbox);

}

void ParticleEmitterDialog::setupColorTab()
{
	Widget* tab=tabwidget->getWidget(1);
	if (!tab) return;

	int col1=30;
	int y=0;
	ppl7::grafix::Rect client=tab->clientRect();

	radio_solid_color=new ppltk::RadioButton(0, y, 200, 30, "solid color:", false);
	radio_solid_color->setEventHandler(this);
	tab->addChild(radio_solid_color);
	y+=35;

	color=new Decker::ui::ColorSliderWidget(col1, y, client.width() - col1, 4 * 35, true);
	color->setEventHandler(this);
	tab->addChild(color);
	y+=4 * 35;

	radio_color_gradient=new ppltk::RadioButton(0, y, 200, 30, "color gradient:", false);
	radio_color_gradient->setEventHandler(this);
	tab->addChild(radio_color_gradient);
	y+=35;

	int col3=tab->width() - 500;
	gradient_widget=new Decker::ui::GradientWidget(0, y, col3 - 10, 7 * 35);
	gradient_widget->setEventHandler(this);
	tab->addChild(gradient_widget);

	tab->addChild(new ppltk::Label(col3, y, 40, 30, "age:"));
	color_gradient_age=new ppltk::DoubleHorizontalSlider(col3 + 40, y, 460, 30);
	color_gradient_age->setEventHandler(this);
	color_gradient_age->setLimits(0.0f, 1.0f);
	color_gradient_age->enableSpinBox(true, 0.001f, 3, 80);
	color_gradient_age->setEnabled(false);
	color_gradient_age->setVisible(false);
	tab->addChild(color_gradient_age);
	y+=35;
	color_gradient=new Decker::ui::ColorSliderWidget(col3, y, client.width() - col3, 4 * 35, true);
	color_gradient->setEventHandler(this);
	color_gradient->setEnabled(false);
	color_gradient->setVisible(false);
	color_gradient->setColorPreviewSize(2 * 35, 4 * 35);
	tab->addChild(color_gradient);

}

void ParticleEmitterDialog::setupSizeTab()
{
	Widget* tab=tabwidget->getWidget(2);
	if (!tab) return;
	int y=0;
	//ppl7::grafix::Rect client=tab->clientRect();

	checkbox_scale_gradient=new ppltk::CheckBox(0, y, 200, 30, "use scale gradient", false);
	checkbox_scale_gradient->setEventHandler(this);
	tab->addChild(checkbox_scale_gradient);
	y+=35;
	int col3=tab->width() - 500;
	scale_gradient_widget=new Decker::ui::GradientWidget(0, y, col3 - 10, 7 * 35);
	scale_gradient_widget->setEventHandler(this);
	tab->addChild(scale_gradient_widget);

	tab->addChild(new ppltk::Label(col3, y, 40, 30, "age:"));
	scale_gradient_age=new ppltk::DoubleHorizontalSlider(col3 + 40, y, 460, 30);
	scale_gradient_age->setEventHandler(this);
	scale_gradient_age->setLimits(0.0f, 1.0f);
	scale_gradient_age->enableSpinBox(true, 0.001f, 3, 80);
	tab->addChild(scale_gradient_age);
	y+=35;

	tab->addChild(new ppltk::Label(col3, y, 40, 30, "scale:"));
	scale_gradient_scale=new ppltk::DoubleHorizontalSlider(col3 + 40, y, 460, 30);
	scale_gradient_scale->setEventHandler(this);
	scale_gradient_scale->setLimits(0.01f, 1.0f);
	scale_gradient_scale->enableSpinBox(true, 0.010f, 3, 80);
	tab->addChild(scale_gradient_scale);
	y+=35;

}

void ParticleEmitterDialog::setValuesToUi(const ParticleEmitter* object)
{
	emitter_type->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->emitter_type)));
	particle_type->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->particle_type)));
	particle_layer->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->particle_layer)));
	color->setColor(object->ParticleColor);
	emitter_pixel_width->setValue(object->emitter_size.width);
	emitter_pixel_height->setValue(object->emitter_size.height);
	min_birth_per_cycle->setValue(object->min_birth_per_cycle);
	max_birth_per_cycle->setValue(object->max_birth_per_cycle);
	birth_time_min->setValue(object->birth_time_min);
	birth_time_max->setValue(object->birth_time_max);
	min_velocity->setValue(object->min_velocity);
	max_velocity->setValue(object->max_velocity);
	direction->setValue(object->direction);
	variation->setValue(object->variation);
	scale_min->setValue(object->scale_min);
	scale_max->setValue(object->scale_max);
	age_min->setValue(object->age_min);
	age_max->setValue(object->age_max);
	weight_min->setValue(object->weight_min);
	weight_max->setValue(object->weight_max);
	gravity_x->setValue(object->gravity.x);
	gravity_y->setValue(object->gravity.y);
	current_state_checkbox->setChecked(object->current_state);
	initial_state_checkbox->setChecked(!(object->flags & static_cast<int>(ParticleEmitter::Flags::initialStateDisabled)));

	{
		gradient_widget->clear();
		std::list<Particle::ColorGradientItem>::const_iterator it;
		for (it=object->color_gradient.begin();it != object->color_gradient.end();++it) {
			gradient_widget->addItem(it->age, it->color);
		}
		if (!object->color_gradient.empty()) {
			color_gradient->setEnabled(true);
			color_gradient->setVisible(true);
			color_gradient_age->setEnabled(true);
			color_gradient_age->setVisible(true);
		}
		if (object->flags & static_cast<int>(ParticleEmitter::Flags::useColorGradient)) radio_color_gradient->setChecked(true);
		else radio_solid_color->setChecked(true);
		color_gradient->setColor(gradient_widget->currentColor());
		color_gradient_age->setValue(gradient_widget->currentAge());
	}
	{
		scale_gradient_widget->clear();
		std::list<Particle::ScaleGradientItem>::const_iterator it;
		for (it=object->scale_gradient.begin();it != object->scale_gradient.end();++it) {
			int c=it->scale * 255;
			scale_gradient_widget->addItem(it->age, ppl7::grafix::Color(c, c, c, 255), it->scale);
		}
		scale_gradient_age->setValue(scale_gradient_widget->currentAge());
		scale_gradient_scale->setValue(scale_gradient_widget->currentValue());

		if (object->flags & static_cast<int>(ParticleEmitter::Flags::useScaleGradient)) checkbox_scale_gradient->setChecked(true);
	}

}


void ParticleEmitterDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with int value\n");
	ppltk::Widget* widget=event->widget();
	if (widget == particle_type) {
		object->particle_type=static_cast<Particle::Type>(particle_type->currentIdentifier().toInt());
	} else 	if (widget == emitter_type) {
		object->emitter_type=static_cast<EmitterType>(emitter_type->currentIdentifier().toInt());
	} else if (widget == particle_layer) {
		object->particle_layer=static_cast<Particle::Layer>(particle_layer->currentIdentifier().toInt());
	} else if (widget == color) {
		object->ParticleColor=color->color();
	} else if (widget == color_gradient) {
		gradient_widget->setCurrentColor(color_gradient->color());
		copyColorGradientToObject();
	} else if (widget == gradient_widget) {
		color_gradient_age->setValue(gradient_widget->currentAge());
		copyColorGradientToObject();
	} else if (widget == scale_gradient_widget) {
		scale_gradient_age->setValue(scale_gradient_widget->currentAge());
		scale_gradient_scale->setValue(scale_gradient_widget->currentValue());
		copyColorGradientToObject();
	}
}

void ParticleEmitterDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with int64_t value\n");
	ppltk::Widget* widget=event->widget();
	if (widget == emitter_pixel_width) {
		object->emitter_size.width=value;
	} else if (widget == emitter_pixel_height) {
		object->emitter_size.height=value;
	} else if (widget == min_birth_per_cycle) {
		object->min_birth_per_cycle=value;
		if (value > object->max_birth_per_cycle) max_birth_per_cycle->setValue(value);
	} else if (widget == max_birth_per_cycle) {
		object->max_birth_per_cycle=value;
		if (value < min_birth_per_cycle->value()) min_birth_per_cycle->setValue(value);
	}

}

void ParticleEmitterDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	//printf("got a RainEmitterDialog::valueChangedEvent with double value\n");
	ppltk::Widget* widget=event->widget();
	if (widget == birth_time_min) {
		object->birth_time_min=value;
		if (value > birth_time_max->value()) birth_time_max->setValue(value);
	} else if (widget == birth_time_max) {
		object->birth_time_max=value;
		if (value < birth_time_min->value()) birth_time_min->setValue(value);
	} else if (widget == min_velocity) {
		object->min_velocity=(float)value;
		if (value > max_velocity->value()) max_velocity->setValue(value);
	} else if (widget == max_velocity) {
		object->max_velocity=(float)value;
		if (value < min_velocity->value()) min_velocity->setValue(value);
	} else if (widget == direction) {
		object->direction=(float)value;
	} else if (widget == variation) {
		object->variation=(float)value;
	} else if (widget == gravity_x) {
		object->gravity.x=(float)value;
	} else if (widget == gravity_y) {
		object->gravity.y=(float)value;
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
	} else if (widget == weight_min) {
		object->weight_min=(float)value;
		if (value > weight_max->value()) weight_max->setValue(value);
	} else if (widget == weight_max) {
		object->weight_max=(float)value;
		if (value < weight_min->value()) weight_min->setValue(value);
	} else if (widget == color_gradient_age) {
		gradient_widget->setCurrentAge(color_gradient_age->value());
		copyColorGradientToObject();
	} else if (widget == scale_gradient_age) {
		scale_gradient_widget->setCurrentAge(scale_gradient_age->value());
		copyScaleGradientToObject();
	} else if (widget == scale_gradient_scale) {
		scale_gradient_widget->setCurrentValue(scale_gradient_scale->value());
		int c=scale_gradient_scale->value() / scale_gradient_scale->maximum() * 255;
		scale_gradient_widget->setCurrentColor(ppl7::grafix::Color(c, c, c, 255));
		copyScaleGradientToObject();
	}
}

void ParticleEmitterDialog::selectionChangedEvent(ppltk::Event* event)
{
	if (event->widget() == gradient_widget) {
		color_gradient->setColor(gradient_widget->currentColor());
		color_gradient->setEnabled(true);
		color_gradient->setVisible(true);
		color_gradient_age->setValue(gradient_widget->currentAge());
		color_gradient_age->setEnabled(true);
		color_gradient_age->setVisible(true);
	} else if (event->widget() == scale_gradient_widget) {
		scale_gradient_age->setValue(scale_gradient_widget->currentAge());
		scale_gradient_scale->setValue(scale_gradient_widget->currentValue());
	}
}


void ParticleEmitterDialog::copyColorGradientToObject()
{
	std::map<float, ppl7::grafix::Color> items=gradient_widget->getColorItems();
	object->color_gradient.clear();
	std::map<float, ppl7::grafix::Color>::const_iterator it;
	for (it=items.begin();it != items.end();++it) {
		object->color_gradient.push_back(Particle::ColorGradientItem(it->first, it->second));
	}
}

void ParticleEmitterDialog::copyScaleGradientToObject()
{
	std::map<float, float> items=scale_gradient_widget->getValueItems();
	object->scale_gradient.clear();
	std::map<float, float>::const_iterator it;
	for (it=items.begin();it != items.end();++it) {
		//printf("copy scale item %0.3f: %0.3f\n", it->first, it->second);
		object->scale_gradient.push_back(Particle::ScaleGradientItem(it->first, it->second));
	}
}

void ParticleEmitterDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (event->widget() == radio_color_gradient || event->widget() == radio_solid_color) {
		int flags=object->flags & (0xffff - static_cast<int>(ParticleEmitter::Flags::useColorGradient));
		if (radio_color_gradient->checked()) flags|=static_cast<int>(ParticleEmitter::Flags::useColorGradient);
		object->flags=flags;
	} else if (event->widget() == checkbox_scale_gradient) {
		int flags=object->flags & (0xffff - static_cast<int>(ParticleEmitter::Flags::useScaleGradient));
		if (checkbox_scale_gradient->checked()) flags|=static_cast<int>(ParticleEmitter::Flags::useScaleGradient);
		object->flags=flags;
		needsRedraw();
	} else if (event->widget() == initial_state_checkbox) {
		int flags=object->flags & (0xffff - static_cast<int>(ParticleEmitter::Flags::initialStateDisabled));
		if (!initial_state_checkbox->checked()) flags|=static_cast<int>(ParticleEmitter::Flags::initialStateDisabled);
		object->flags=flags;
		needsRedraw();
	} else if (event->widget() == current_state_checkbox) {
		object->current_state=current_state_checkbox->checked();
		needsRedraw();
	}
	//printf("flags=%d\n", object->flags);

}

static ParticleEmitter clipboard;

void ParticleEmitterDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button == Dialog::Buttons::Copy) {
		clipboard.particle_type=object->particle_type;
		clipboard.emitter_type=object->emitter_type;
		clipboard.particle_layer=object->particle_layer;
		clipboard.ParticleColor=object->ParticleColor;
		clipboard.emitter_size=object->emitter_size;
		clipboard.flags=object->flags;
		clipboard.min_birth_per_cycle=object->min_birth_per_cycle;
		clipboard.max_birth_per_cycle=object->max_birth_per_cycle;
		clipboard.birth_time_min=object->birth_time_min;
		clipboard.birth_time_max=object->birth_time_max;
		clipboard.direction=object->direction;
		clipboard.variation=object->variation;
		clipboard.min_velocity=object->min_velocity;
		clipboard.max_velocity=object->max_velocity;
		clipboard.scale_min=object->scale_min;
		clipboard.scale_max=object->scale_max;
		clipboard.age_min=object->age_min;
		clipboard.age_max=object->age_max;
		clipboard.weight_min=object->weight_min;
		clipboard.weight_max=object->weight_max;
		clipboard.gravity=object->gravity;
		clipboard.scale_gradient=object->scale_gradient;
		clipboard.color_gradient=object->color_gradient;

		SDL_SetClipboardText(object->generateCode());

	} else if (button == Dialog::Buttons::Paste) {
		object->particle_type=clipboard.particle_type;
		object->emitter_type=clipboard.emitter_type;
		object->particle_layer=clipboard.particle_layer;
		object->ParticleColor=clipboard.ParticleColor;
		object->emitter_size=clipboard.emitter_size;
		object->flags=clipboard.flags;
		object->min_birth_per_cycle=clipboard.min_birth_per_cycle;
		object->max_birth_per_cycle=clipboard.max_birth_per_cycle;
		object->birth_time_min=clipboard.birth_time_min;
		object->birth_time_max=clipboard.birth_time_max;
		object->direction=clipboard.direction;
		object->variation=clipboard.variation;
		object->min_velocity=clipboard.min_velocity;
		object->max_velocity=clipboard.max_velocity;
		object->scale_min=clipboard.scale_min;
		object->scale_max=clipboard.scale_max;
		object->age_min=clipboard.age_min;
		object->age_max=clipboard.age_max;
		object->weight_min=clipboard.weight_min;
		object->weight_max=clipboard.weight_max;
		object->gravity=clipboard.gravity;
		object->scale_gradient=clipboard.scale_gradient;
		object->color_gradient=clipboard.color_gradient;
		setValuesToUi(object);
	}
}

}	// EOF namespace Decker::Objects
