#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {


Representation Stamper::representation()
{
	return Representation(Spriteset::StamperV2, 0);
}


Stamper::Stamper()
	:Trap(Type::ObjectType::Stamper)
{
	sprite_set=Spriteset::StamperV2;
	sprite_no=0;
	collisionDetection=true;
	visibleAtPlaytime=true;
	sprite_no_representation=0;
	pixelExactCollision=false;
	next_state=0.0f;
	state=State::Open;
	if (ppl7::rand(0, 1) == 1) {
		state=State::Closed;
		sprite_no=5;
	}
	initial_state=static_cast<int>(state);
	stamper_type=0;
	time_active=ppl7::randf(0.2f, 0.5f);
	time_inactive=ppl7::randf(0.2f, 0.5f);
	//printf("Stamper Initial: %0.3f, %0.3f\n", time_active, time_inactive);
	auto_intervall=true;
	next_animation=0.0f;
	color_stamper=7;
	color_teeth=9;
	orientation=Orientation::down;
	teeth_type=6;
	//texture=GetObjectSystem()->getTexture(Spriteset::StamperV2);

	init();
}

void Stamper::init()
{
	state=State::Open;
	position=0.0f;
	acceleration=0.0f;
	if (initial_state) {
		state=State::Closed;
		position=261.0f;
	}
	sprite_no=stamper_type * 20;
	if (state == State::Closed) sprite_no+=5;
	animation.setStaticFrame(sprite_no);
	//printf ("sprite_no=%d, state=%d\n",sprite_no, state);
	updateBoundary();
	updateStamperBoundary();
}

void Stamper::updateStamperBoundary()
{
	if (!texture) return;
	const SpriteTexture::SpriteIndexItem* stamper_item=texture->getSpriteIndex(stamper_type);
	if (!stamper_item) return;

	int s=teeth_type;
	if (stamper_type == 0) s+=7;
	const SpriteTexture::SpriteIndexItem* teeth_item=texture->getSpriteIndex(s);

	switch (orientation) {
	case Orientation::down:
		this->boundary.x1=p.x + (stamper_item->Offset.x - stamper_item->Pivot.x);
		this->boundary.y1=p.y + (stamper_item->Offset.y - stamper_item->Pivot.y);
		this->boundary.x2=this->boundary.x1 + stamper_item->r.w;
		this->boundary.y2=this->boundary.y1 + stamper_item->r.h - 261 + position;
		if (teeth_item) this->boundary.y2+=teeth_item->r.h - 7;
		break;
	default:
		this->boundary.setRect(0, 0, 0, 0);
	}

}


void Stamper::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	//Trap::draw(renderer, coords);

	const ColorPalette& palette=GetColorPalette();
	const SpriteTexture::SpriteIndexItem* spi_item=texture->getSpriteIndex(stamper_type);
	if (!spi_item) return;
	//ppl7::PrintDebug("ok\n");
	int x=p.x + coords.x;
	int y=p.y + coords.y;


	SDL_Rect tr;
	SDL_Rect sr=spi_item->r;
	sr.h=position;
	tr.x=x + (spi_item->Offset.x - spi_item->Pivot.x);
	tr.y=y + (spi_item->Offset.y - spi_item->Pivot.y);
	tr.w=spi_item->r.w;
	tr.h=position;
	SDL_SetTextureAlphaMod(spi_item->tex, 255);
	ppl7::grafix::Color c=palette.getColor(color_stamper);
	SDL_SetTextureColorMod(spi_item->tex, c.red(), c.green(), c.blue());
	SDL_RenderCopyEx(renderer, spi_item->tex, &sr, &tr, 180, NULL, SDL_FLIP_HORIZONTAL);

	/*
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderDrawRect(renderer, &tr);
	tr.x=x;
	tr.y=y;
	tr.w=1;
	tr.h=1;
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderDrawRect(renderer, &tr);
	*/
	int s=teeth_type;
	if (stamper_type == 0) s+=7;
	spi_item=texture->getSpriteIndex(s + 5);
	if (!spi_item) return;
	c=palette.getColor(color_teeth);
	SDL_SetTextureColorMod(spi_item->tex, c.red(), c.green(), c.blue());
	tr.x=x + (spi_item->Offset.x - spi_item->Pivot.x);
	tr.y=y + (spi_item->Offset.y - spi_item->Pivot.y) + position - 7 - 261;
	tr.w=spi_item->r.w;
	tr.h=spi_item->r.h;
	SDL_Point center;
	center.x=(spi_item->Pivot.x - spi_item->Offset.x);
	center.y=(spi_item->Pivot.y - spi_item->Offset.y);
	SDL_RenderCopyEx(renderer, spi_item->tex, &spi_item->r, &tr, 180, &center, SDL_FLIP_HORIZONTAL);


	// Collision-Box
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);
	SDL_Rect col;
	col.x=boundary.x1 + coords.x;
	col.y=boundary.y1 + coords.y;
	col.w=boundary.width();
	col.h=boundary.height();
	SDL_RenderDrawRect(renderer, &col);


}

void Stamper::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	Stamper::draw(renderer, coords);
}



void Stamper::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (state == State::Closing) {
		position+=acceleration * frame_rate_compensation;
		acceleration=acceleration * (1.7 * frame_rate_compensation);
	} else if (state == State::Opening) {
		position-=acceleration * frame_rate_compensation;
		acceleration+=(0.2 * frame_rate_compensation);
		if (acceleration > 5.0f) acceleration=5.0f;
	}
	//int start_sprite_no=stamper_type * 20;
	if (auto_intervall) {
		if (state == State::Open && time > next_state) {
			state=State::Closing;
			//animation.startSequence(start_sprite_no, start_sprite_no + 5, false, start_sprite_no + 5);
			position=0;
			acceleration=2.0f;
		} else if (state == State::Closing && position >= 261.0f) {
			position=261.0f;
			state=State::Closed;
			next_state=time + time_active;
			acceleration=0.0f;
		} else if (state == State::Closed && time > next_state) {
			state=State::Opening;
			acceleration=1.0f;
			//animation.startSequence(start_sprite_no + 6, start_sprite_no + 19, false, start_sprite_no + 19);
		} else if (state == State::Opening && position <= 0.0f) {
			state=State::Open;
			position=0.0f;
			next_state=time + time_inactive;
			if (GetGame().config.difficulty == Config::DifficultyLevel::easy) next_state+=time_inactive;
		}
	} else {

	}
	updateStamperBoundary();
}



void Stamper::handleCollision(Player* player, const Collision& collision)
{
	if (state == State::Closing) {
		player->dropHealth(100, Player::Smashed);
		return;
	}
	player->setZeroVelocity();
	if (player->x < p.x) player->x--;
	if (player->x > p.x) player->x++;
}

void Stamper::toggle(bool enabled, Object* source)
{
	//int start_sprite_no=stamper_type * 20;
	if (enabled) {
		//animation.startSequence(start_sprite_no, start_sprite_no + 5, false, start_sprite_no + 5);
		state=State::Closed;
		position=261.0f;
	} else {
		//animation.startSequence(start_sprite_no + 6, start_sprite_no + 19, false, start_sprite_no + 19);
		state=State::Open;
		position=0.0f;
	}
}

void Stamper::trigger(Object* source)
{
	toggle(!enabled, source);
}

size_t Stamper::saveSize() const
{
	return Object::saveSize() + 16;
}


size_t Stamper::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 2);		// Object Version

	int flags=0;
	if (auto_intervall) flags|=1;
	ppl7::Poke8(buffer + bytes + 1, flags);
	ppl7::Poke8(buffer + bytes + 2, initial_state);
	ppl7::Poke8(buffer + bytes + 3, stamper_type);
	ppl7::PokeFloat(buffer + bytes + 4, time_active);
	ppl7::PokeFloat(buffer + bytes + 8, time_inactive);
	ppl7::Poke8(buffer + bytes + 12, teeth_type);
	ppl7::Poke8(buffer + bytes + 13, color_stamper);
	ppl7::Poke8(buffer + bytes + 14, color_teeth);
	ppl7::Poke8(buffer + bytes + 15, static_cast<int>(orientation));


	return bytes + 16;
}

size_t Stamper::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version < 1) return 0;

	int flags=ppl7::Peek8(buffer + bytes + 1);
	auto_intervall=(bool)(flags & 1);
	initial_state=ppl7::Peek8(buffer + bytes + 2);
	stamper_type=ppl7::Peek8(buffer + bytes + 3);
	time_active=ppl7::PeekFloat(buffer + bytes + 4);
	time_inactive=ppl7::PeekFloat(buffer + bytes + 8);
	teeth_type=6;
	if (version > 1) {
		teeth_type=ppl7::Peek8(buffer + bytes + 12);
		color_stamper=ppl7::Peek8(buffer + bytes + 13);
		color_teeth=ppl7::Peek8(buffer + bytes + 14);
		orientation=static_cast<Orientation>(ppl7::Peek8(buffer + bytes + 15));
	}
	init();
	return size;
}

class StamperDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::ComboBox* stamper_type;
	ppl7::tk::ComboBox* teeth_type;
	ppl7::tk::ComboBox* orientation;
	ppl7::tk::CheckBox* initial_state;
	ppl7::tk::CheckBox* auto_intervall;
	ppl7::tk::DoubleHorizontalSlider* time_active;
	ppl7::tk::DoubleHorizontalSlider* time_inactive;

	Decker::ui::ColorSelectionFrame* colorframe;
	ppl7::tk::Button* button_color_stamper;
	ppl7::tk::Button* button_color_teeth;
	ppl7::tk::Frame* frame_color_stamper;
	ppl7::tk::Frame* frame_color_teeth;


	ppl7::tk::Label* current_element_label;
	ppl7::tk::Frame* current_element_color_frame;
	int* color_target;

	enum class Element {
		Stamper,
		Teeth,
	};
	Element current_element;

	Stamper* object;

	void setCurrentElement(Element element);

public:
	StamperDialog(Stamper* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
	virtual void valueChangedEvent(ppl7::tk::Event* event, double value);
	virtual void toggledEvent(ppl7::tk::Event* event, bool checked);
	virtual void mouseDownEvent(ppl7::tk::MouseEvent* event);
};

void Stamper::openUi()
{
	StamperDialog* dialog=new StamperDialog(this);
	GetGameWindow()->addChild(dialog);
}

StamperDialog::StamperDialog(Stamper* object)
	: Decker::ui::Dialog(700, 560)
{
	current_element=Element::Stamper;
	current_element_color_frame=NULL;
	color_target=NULL;

	this->object=object;
	setWindowTitle(ppl7::ToString("Stamper, Object-ID: %d", object->id));
	int y=0;
	ppl7::grafix::Rect client=clientRect();
	addChild(new ppl7::tk::Label(0, y, 120, 30, "Stamper-Type: "));
	stamper_type=new ppl7::tk::ComboBox(120, y, 400, 30);
	stamper_type->add("4 Tiles", "0");
	stamper_type->add("2 Tiles", "1");
	stamper_type->add("2 Tiles bricks", "2");
	stamper_type->add("2 Tiles lines vertical", "3");
	stamper_type->add("2 Tiles lines horizontal", "4");
	stamper_type->setCurrentIdentifier(ppl7::ToString("%d", object->stamper_type));
	stamper_type->setEventHandler(this);
	addChild(stamper_type);
	y+=35;
	addChild(new ppl7::tk::Label(0, y, 120, 30, "Teeth-Type: "));
	teeth_type=new ppl7::tk::ComboBox(120, y, 400, 30);
	teeth_type->add("plate with rounds", "0");
	teeth_type->add("plate with peaks", "1");
	teeth_type->add("peaks", "2");
	teeth_type->add("plate with spikes", "3");
	teeth_type->add("spikes", "4");
	teeth_type->add("plate", "5");
	teeth_type->add("tile", "6");
	teeth_type->setCurrentIdentifier(ppl7::ToString("%d", object->teeth_type));
	teeth_type->setEventHandler(this);
	addChild(teeth_type);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Orientation: "));
	orientation=new ppl7::tk::ComboBox(120, y, 400, 30);
	orientation->add("down", "0");
	orientation->add("up", "1");
	orientation->add("left", "2");
	orientation->add("right", "3");
	orientation->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->orientation)));
	orientation->setEventHandler(this);
	addChild(orientation);
	y+=35;



	addChild(new ppl7::tk::Label(0, y, 120, 30, "Colors: "));
	current_element_label=new ppl7::tk::Label(client.width() - 300, y, 300, 30, "Stamper");
	addChild(current_element_label);
	y+=35;
	ColorPalette& palette=GetColorPalette();
	colorframe=new Decker::ui::ColorSelectionFrame(client.width() - 300, y, 300, 300, palette);
	colorframe->setEventHandler(this);
	this->addChild(colorframe);

	button_color_stamper=new ppl7::tk::Button(30, y, 100, 30, "Stamper");
	button_color_stamper->setEventHandler(this);
	addChild(button_color_stamper);
	frame_color_stamper=new ppl7::tk::Frame(135, y, 60, 30, ppl7::tk::Frame::Inset);
	frame_color_stamper->setBackgroundColor(palette.getColor(object->color_stamper));
	addChild(frame_color_stamper);
	y+=35;

	button_color_teeth=new ppl7::tk::Button(30, y, 100, 30, "Teeth");
	button_color_teeth->setEventHandler(this);
	addChild(button_color_teeth);
	frame_color_teeth=new ppl7::tk::Frame(135, y, 60, 30, ppl7::tk::Frame::Inset);
	frame_color_teeth->setBackgroundColor(palette.getColor(object->color_teeth));
	addChild(frame_color_teeth);
	y+=35;


	addChild(new ppl7::tk::Label(0, y, 120, 30, "Flags: "));
	initial_state=new ppl7::tk::CheckBox(120, y, 200, 30, "Initial state", object->initial_state);
	initial_state->setEventHandler(this);
	addChild(initial_state);
	y+=30;
	auto_intervall=new ppl7::tk::CheckBox(120, y, 200, 30, "Auto Intervall", object->auto_intervall);
	auto_intervall->setEventHandler(this);
	addChild(auto_intervall);
	y+=35;


	addChild(new ppl7::tk::Label(0, y, 120, 30, "Time active:"));
	time_active=new ppl7::tk::DoubleHorizontalSlider(120, y, client.width() - 300 - 130, 30);
	time_active->setValue(object->time_active);
	time_active->setEventHandler(this);
	time_active->setLimits(0.0f, 10.0f);
	time_active->enableSpinBox(true, 0.01, 2, 80);
	this->addChild(time_active);
	y+=35;

	addChild(new ppl7::tk::Label(0, y, 120, 30, "Time inactive:"));
	time_inactive=new ppl7::tk::DoubleHorizontalSlider(120, y, client.width() - 300 - 130, 30);
	time_inactive->setValue(object->time_inactive);
	time_inactive->setEventHandler(this);
	time_inactive->setLimits(0.0f, 10.0f);
	time_inactive->enableSpinBox(true, 0.01, 2, 80);
	this->addChild(time_inactive);
	y+=35;



	setCurrentElement(Element::Stamper);

}

void StamperDialog::setCurrentElement(Element element)
{
	current_element=element;
	switch (element) {
	case Element::Stamper:
		current_element_label->setText("Stamper");
		current_element_color_frame=frame_color_stamper;
		color_target=&object->color_stamper;
		break;
	case Element::Teeth:
		current_element_label->setText("Teeth");
		current_element_color_frame=frame_color_teeth;
		color_target=&object->color_teeth;
		break;
	}

	colorframe->setColorIndex(*color_target);
}


void StamperDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	if (event->widget() == stamper_type) {
		object->stamper_type=stamper_type->currentIdentifier().toInt();
		object->init();
	} else if (event->widget() == teeth_type) {
		object->teeth_type=teeth_type->currentIdentifier().toInt();
		object->init();
	} else if (event->widget() == orientation) {
		object->orientation=static_cast<Stamper::Orientation>(orientation->currentIdentifier().toInt());
		object->init();
	} else if (event->widget() == colorframe && color_target != NULL) {
		*color_target=value;
		if (current_element_color_frame)
			current_element_color_frame->setBackgroundColor(GetColorPalette().getColor(value));
	}
}

void StamperDialog::valueChangedEvent(ppl7::tk::Event* event, double value)
{
	if (event->widget() == time_active) object->time_active=value;
	else if (event->widget() == time_inactive) object->time_inactive=value;
	//printf("Stamper valueChangedEvent: %0.3f, %0.3f\n", object->time_active, object->time_inactive);
}

void StamperDialog::toggledEvent(ppl7::tk::Event* event, bool checked)
{
	if (event->widget() == initial_state) object->initial_state=(int)checked * 2;
	else if (event->widget() == auto_intervall) object->auto_intervall=checked;
	//printf("object->initial_state=%d\n", object->initial_state);
	object->init();

}

void StamperDialog::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	ppl7::tk::Widget* widget=event->widget();
	if (widget == button_color_stamper || widget == frame_color_stamper) setCurrentElement(Element::Stamper);
	else if (widget == button_color_teeth || widget == frame_color_teeth) setCurrentElement(Element::Teeth);
	else Dialog::mouseDownEvent(event);
}


}	// EOF namespace Decker::Objects
