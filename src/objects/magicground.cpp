#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

Representation MagicGround::representation()
{
	return Representation(Spriteset::MagicGround, 27);
}

MagicGround::MagicGround()
	: Object(Type::ObjectType::MagicGround)
{
	sprite_set=Spriteset::MagicGround;
	sprite_no=0;
	sprite_no_representation=27;
	collisionDetection=true;
	pixelExactCollision=false;
	initial_state=State::active;
	current_state=initial_state;
	color=2;
	hasDebris=true;
	hasStuds=true;
	verticalMovement=true;
	roundEdges=true;
	canDissolve=false;
	isInvisible=false;
	movement_speed=1.0f;
	float_offset=0.0f;
	movement_range=2;
	floatstate=FloatState::GoingUp;
	current_speed=0;
	graficset=0;
	width=1;
	transparency=0.0f;
	max_debris_length=8;
	min_time_visible=2.0f;
	max_time_visible=4.0f;
	min_time_invisible=2.0f;
	max_time_invisible=4.0f;
	next_transparency_change=0.0f;
	linked_with_id=0;


	randomizeDebris();
	randomizeFloatState();
}


void MagicGround::randomizeDebris()
{
	for (int i=0;i < 16;i++) {
		debris_sprites[i]=ppl7::rand(22, 26);
	}
	num_debris=ppl7::rand(1, 4);
	for (int i=0;i < 4;i++) {
		debris_pos[i]=ppl7::rand(0, 6) * TILE_WIDTH / 2;
		debris_length[i]=ppl7::rand(1, max_debris_length);
	}
}

void MagicGround::updateBoundary()
{
	boundary.x1=p.x - 2 * TILE_WIDTH;
	boundary.y1=p.y;
	boundary.x2=boundary.x1 + width * 4 * TILE_WIDTH;
	boundary.y2=boundary.y1 + TILE_HEIGHT;
}

void MagicGround::randomizeFloatState()
{
	float_offset=ppl7::randf(0, (float)movement_range);
	if (ppl7::rand(0, 1) == 1) floatstate=FloatState::GoingUp;
	else floatstate=FloatState::GoingDown;
	current_speed=ppl7::randf(0, movement_speed);

	if (canDissolve) {
		if (ppl7::rand(0, 1) == 1) {
			transparency=0.0f;
			current_state = State::active;
		} else {
			transparency=1.0f;
			current_state = State::inactive;
		}
	}
}

void MagicGround::updateVerticalMovement(double time, float frame_rate_compensation)
{
	float factor=movement_speed / 50.0f;
	if (floatstate == FloatState::GoingUp) {
		if (current_speed < movement_speed) current_speed+=factor * frame_rate_compensation;
		if (current_speed > movement_speed) current_speed=movement_speed;
		float_offset+=current_speed * frame_rate_compensation;
		if (float_offset > (float)movement_range) floatstate=FloatState::BreakGoingUp;

	} else if (floatstate == FloatState::BreakGoingUp) {
		if (current_speed > 0.0f) current_speed-=factor * frame_rate_compensation;
		if (current_speed <= 0.0f) {
			current_speed=0.0f;
			floatstate=FloatState::GoingDown;
		}
		float_offset+=current_speed * frame_rate_compensation;
	} else if (floatstate == FloatState::GoingDown) {
		if (current_speed < movement_speed) current_speed+=factor * frame_rate_compensation;
		if (current_speed > movement_speed) current_speed=movement_speed;
		float_offset-=current_speed * frame_rate_compensation;
		if (float_offset < 0.0f) floatstate=FloatState::BreakGoingDown;
	} else if (floatstate == FloatState::BreakGoingDown) {
		if (current_speed > 0.0f) current_speed-=factor * frame_rate_compensation;
		if (current_speed <= 0.0f) {
			current_speed=0.0f;
			floatstate=FloatState::GoingUp;
		}
		float_offset-=current_speed * frame_rate_compensation;
	}
	p.y=initial_p.y - float_offset;
}

void MagicGround::updateTransparency(double time, float frame_rate_compensation)
{
	if (canDissolve) {
		if (next_transparency_change == 0.0f) {
			if (current_state == State::active || current_state == State::appears) next_transparency_change=time + ppl7::randf(min_time_visible, max_time_visible);
			else if (current_state == State::inactive || current_state == State::disappears) next_transparency_change=time + ppl7::randf(min_time_invisible, max_time_invisible);
		}
		if (next_transparency_change < time && current_state == State::active) {
			current_state=State::disappears;
		} else if (current_state == State::disappears) {
			transparency+=0.01 * frame_rate_compensation;
			if (transparency >= 1.0f) {
				transparency=1.0f;
				current_state=State::inactive;
				next_transparency_change=time + ppl7::randf(min_time_invisible, max_time_invisible);
			}
		} else if (next_transparency_change < time && current_state == State::inactive) {
			current_state=State::appears;
		} else if (current_state == State::appears) {
			transparency-=0.01 * frame_rate_compensation;
			if (transparency <= 0.0f) {
				transparency=0.0f;
				current_state=State::active;
				next_transparency_change=time + ppl7::randf(min_time_visible, max_time_visible);
			}
		}
	} else {
		if (current_state == State::appears && transparency > 0.0f) {
			transparency-=0.03 * frame_rate_compensation;
			if (transparency <= 0.0f) {
				transparency=0.0f;
				current_state=State::active;
				next_transparency_change=time + ppl7::randf(min_time_visible, max_time_visible);
			}
		} else if (current_state == State::disappears && transparency < 1.0f) {
			transparency+=0.03 * frame_rate_compensation;
			if (transparency >= 1.0f) {
				transparency=1.0f;
				current_state=State::inactive;
				next_transparency_change=time + ppl7::randf(min_time_visible, max_time_visible);
			}
		}
	}
}

void MagicGround::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	if (!copyFromOtherMagicGround()) {
		if (verticalMovement) updateVerticalMovement(time, frame_rate_compensation);
		updateTransparency(time, frame_rate_compensation);
	}
	if (roundEdges) sprite_no_representation=graficset * 4 + 3;
	else sprite_no_representation=graficset * 4 + 1;
	updateBoundary();
	if (current_state == State::active || transparency < 0.5f) enabled=true;
	else enabled=false;
}

bool MagicGround::copyFromOtherMagicGround()
{
	if (linked_with_id == 0) return false;
	MagicGround* other=static_cast<MagicGround*>(GetObjectSystem()->getObject(linked_with_id));
	if (!other || other->type() != Objects::Type::MagicGround) return false;
	current_speed=other->current_speed;
	floatstate=other->floatstate;
	transparency=other->transparency;
	float_offset=other->float_offset;
	current_state=other->current_state;
	p.y=initial_p.y - float_offset;
	return true;
}

void MagicGround::drawCommon(SDL_Renderer* renderer, const ppl7::grafix::Point& coords, const ppl7::grafix::Point& pp, float transp) const
{
	const ColorPalette& palette=GetColorPalette();
	ppl7::grafix::Color myColor=palette.getColor(color);
	myColor.setAlpha(255 - (transp * 255.0f));

	if (hasStuds) {
		for (int i=0;i < width;i++)
			texture->draw(renderer,
				pp.x + coords.x + i * 4 * TILE_WIDTH,
				pp.y + coords.y, 21, myColor);
	}
	if (hasDebris) {
		int d=0;
		for (int i=0;i < num_debris;i++) {
			for (int yy=debris_length[i] - 1;yy >= 0;yy--) {
				texture->draw(renderer,
					pp.x + coords.x + debris_pos[i] - 48,
					pp.y + TILE_HEIGHT + coords.y + yy * TILE_HEIGHT / 3, debris_sprites[d], myColor);
				d++;
				if (d > 15) d=0;
			}
		}
		// TODO
	}
	int base=4 * graficset;
	if (width == 1) {
		int g=base + 1;
		if (roundEdges) g=base + 3;
		texture->draw(renderer,
			pp.x + coords.x,
			pp.y + coords.y, g, myColor);
	} else if (width > 1) {
		int start=0;
		int end=width - 1;
		if (roundEdges) {
			start++;
			end--;
			texture->draw(renderer,
				pp.x + coords.x,
				pp.y + coords.y, base, myColor);
			texture->draw(renderer,
				pp.x + coords.x + (width - 1) * 4 * TILE_WIDTH,
				pp.y + coords.y, base + 2, myColor);
		}
		for (int i=start;i <= end;i++)
			texture->draw(renderer,
				pp.x + coords.x + i * 4 * TILE_WIDTH,
				pp.y + coords.y, base + 1, myColor);
	}
}

void MagicGround::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	if (isInvisible) return;
	if (current_state == State::inactive) return;
	drawCommon(renderer, coords, p, transparency);
}

void MagicGround::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
	if (!isInvisible) drawCommon(renderer, coords, p, 0.5f);
	drawCommon(renderer, coords, initial_p, 0.0f);
}


void MagicGround::handleCollision(Player* player, const Collision& collision)
{
	if (current_state != State::inactive && transparency < 0.5f && collision.onFoot()) {
		player->y=p.y;
		player->setStandingOnObject(this);
	}
}


void MagicGround::toggle(bool enable, Object* source)
{
	//ppl7::PrintDebug("MagicGround::toggle: %d\n", (int)enable);
	if (enable) current_state = State::appears;
	else current_state = State::disappears;
}

void MagicGround::trigger(Object* source)
{
	if (current_state == State::inactive || current_state == State::disappears) toggle(true, source);
	else toggle(false, source);
}


size_t MagicGround::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 3);		// Object Version

	int flags=0;
	if (initial_state == State::active) flags|=1;
	if (hasDebris) flags|=2;
	if (hasStuds) flags|=4;
	if (verticalMovement) flags|=8;
	if (roundEdges) flags|=16;
	if (canDissolve) flags|=32;
	if (isInvisible) flags|=64;
	ppl7::Poke8(buffer + bytes + 1, flags);
	ppl7::Poke8(buffer + bytes + 2, graficset);
	ppl7::Poke8(buffer + bytes + 3, width);
	ppl7::Poke8(buffer + bytes + 4, movement_range);
	ppl7::Poke8(buffer + bytes + 5, color);
	ppl7::Poke8(buffer + bytes + 6, max_debris_length);
	ppl7::PokeFloat(buffer + bytes + 7, movement_speed);
	ppl7::PokeFloat(buffer + bytes + 11, min_time_visible);
	ppl7::PokeFloat(buffer + bytes + 15, max_time_visible);
	ppl7::PokeFloat(buffer + bytes + 19, min_time_invisible);
	ppl7::PokeFloat(buffer + bytes + 23, max_time_invisible);
	ppl7::Poke32(buffer + bytes + 27, linked_with_id);
	ppl7::Poke8(buffer + bytes + 31, num_debris);
	for (int i=0;i < 16;i++) {
		ppl7::Poke8(buffer + bytes + 32 + i, debris_sprites[i]);
	}
	for (int i=0;i < 4;i++) {
		ppl7::Poke8(buffer + bytes + 48 + i, debris_pos[i]);
	}
	for (int i=0;i < 4;i++) {
		ppl7::Poke8(buffer + bytes + 52 + i, debris_length[i]);
	}
	return bytes + 56;
}

size_t MagicGround::saveSize() const
{
	int debris_size=16 + 1 + 4 + 4;
	return Object::saveSize() + debris_size + 31;
}

size_t MagicGround::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version < 1 || version > 3) return 0;
	int flags=ppl7::Peek8(buffer + bytes + 1);
	if (flags & 1) {
		initial_state=State::active;
		transparency=0.0f;
	} else {
		initial_state=State::inactive;
		transparency=1.0f;
	}
	current_state=initial_state;
	hasDebris=(bool)(flags & 2);
	hasStuds=(bool)(flags & 4);
	verticalMovement=(bool)(flags & 8);
	roundEdges=(bool)(flags & 16);
	canDissolve=(bool)(flags & 32);
	isInvisible=(bool)(flags & 64);

	graficset=ppl7::Peek8(buffer + bytes + 2);
	width=ppl7::Peek8(buffer + bytes + 3);
	movement_range=ppl7::Peek8(buffer + bytes + 4);
	color=ppl7::Peek8(buffer + bytes + 5);
	max_debris_length=ppl7::Peek8(buffer + bytes + 6);
	movement_speed=ppl7::PeekFloat(buffer + bytes + 7);
	if (version >= 2) {
		min_time_visible=ppl7::PeekFloat(buffer + bytes + 11);
		max_time_visible=ppl7::PeekFloat(buffer + bytes + 15);
		min_time_invisible=ppl7::PeekFloat(buffer + bytes + 19);
		max_time_invisible=ppl7::PeekFloat(buffer + bytes + 23);
		linked_with_id=ppl7::Peek32(buffer + bytes + 27);
	}
	if (version < 3) {
		randomizeDebris();
	} else {
		for (int i=0;i < 16;i++) {
			debris_sprites[i]=ppl7::Peek8(buffer + bytes + 32 + i);
		}
		for (int i=0;i < 4;i++) {
			debris_pos[i]=ppl7::Peek8(buffer + bytes + 48 + i);
		}
		for (int i=0;i < 4;i++) {
			debris_length[i]=ppl7::Peek8(buffer + bytes + 52 + i);
		}
	}
	randomizeFloatState();

	return size;
}

void MagicGround::reset()
{
	//ppl7::PrintDebug("MagicGround::reset()\n");
	p=initial_p;
	current_state=initial_state;

	randomizeFloatState();
	randomizeDebris();
}

class MagicGroundDialog : public Decker::ui::Dialog
{
private:
	ppltk::HorizontalSlider* graficset;
	ppltk::HorizontalSlider* width;
	ppltk::CheckBox* initial_state_checkbox, * current_state_checkbox;


	ppltk::CheckBox* hasDebris;
	ppltk::CheckBox* hasStuds;
	ppltk::CheckBox* verticalMovement;
	ppltk::CheckBox* roundEdges;
	ppltk::CheckBox* canDissolve;
	ppltk::CheckBox* isInvisible;

	Decker::ui::ColorSelectionFrame* color;

	ppltk::HorizontalSlider* movement_range;
	ppltk::HorizontalSlider* max_debris_length;
	ppltk::SpinBox* linked_with_id;

	ppltk::DoubleHorizontalSlider* movement_speed;
	ppltk::DoubleHorizontalSlider* min_time_visible;
	ppltk::DoubleHorizontalSlider* max_time_visible;
	ppltk::DoubleHorizontalSlider* min_time_invisible;
	ppltk::DoubleHorizontalSlider* max_time_invisible;


	MagicGround* object;

public:
	MagicGroundDialog(MagicGround* object);
	~MagicGroundDialog();

	void valueChangedEvent(ppltk::Event* event, int value) override;
	void valueChangedEvent(ppltk::Event* event, int64_t value) override;
	void valueChangedEvent(ppltk::Event* event, double value) override;
	void toggledEvent(ppltk::Event* event, bool checked) override;
	virtual void dialogButtonEvent(Dialog::Buttons button) override;
	//void mouseDownEvent(ppltk::MouseEvent* event) override;
};


void MagicGround::openUi()
{
	MagicGroundDialog* dialog=new MagicGroundDialog(this);
	GetGameWindow()->addChild(dialog);
}


MagicGroundDialog::MagicGroundDialog(MagicGround* object)
	: Decker::ui::Dialog(640, 650, Buttons::OK | Buttons::Reset)
{
	ppl7::grafix::Rect client=clientRect();
	this->object=object;
	setWindowTitle(ppl7::ToString("MagicGround, ID: %d", object->id));
	int y=0;
	int sw=(client.width()) / 2;
	int col1=150;
	// State
	initial_state_checkbox=new ppltk::CheckBox(0, y, sw, 30, "initial State", (bool)(object->initial_state == MagicGround::State::active));
	initial_state_checkbox->setEventHandler(this);
	addChild(initial_state_checkbox);
	current_state_checkbox=new ppltk::CheckBox(sw, y, sw, 30, "current State", (bool)(object->current_state == MagicGround::State::active));
	current_state_checkbox->setEventHandler(this);
	addChild(current_state_checkbox);
	y+=35;

	addChild(new ppltk::Label(0, y, 120, 30, "Type: "));
	graficset=new ppltk::HorizontalSlider(120, y, sw - 120, 30);
	graficset->setLimits(0, 4);
	graficset->setValue(object->graficset);
	graficset->enableSpinBox(true, 1, 80);
	graficset->setEventHandler(this);
	addChild(graficset);

	addChild(new ppltk::Label(sw, y, 120, 30, "Width: "));
	width=new ppltk::HorizontalSlider(sw + 120, y, client.width() - sw - 120, 30);
	width->setLimits(1, 16);
	width->setValue(object->width);
	width->enableSpinBox(true, 1, 80);
	width->setEventHandler(this);
	addChild(width);
	y+=35;

	addChild(new ppltk::Label(0, y, 80, 30, "Color:"));
	ColorPalette& palette=GetColorPalette();
	color=new Decker::ui::ColorSelectionFrame(120, y, 300, 300, palette);
	color->setEventHandler(this);
	color->setColorIndex(object->color);
	this->addChild(color);

	int y1=y;
	int x1=120 + 305;
	int w=client.width() - x1 - 30;

	addChild(new ppltk::Label(x1, y1, 120, 30, "Flags: "));
	y1+=30;
	hasDebris=new ppltk::CheckBox(x1 + 30, y1, w, 30, "has debris", object->hasDebris);
	hasDebris->setEventHandler(this);
	addChild(hasDebris);
	y1+=30;
	hasStuds=new ppltk::CheckBox(x1 + 30, y1, w, 30, "has studs", object->hasStuds);
	hasStuds->setEventHandler(this);
	addChild(hasStuds);
	y1+=30;
	canDissolve=new ppltk::CheckBox(x1 + 30, y1, w, 30, "can dissolve", object->canDissolve);
	canDissolve->setEventHandler(this);
	addChild(canDissolve);
	y1+=30;
	roundEdges=new ppltk::CheckBox(x1 + 30, y1, w, 30, "round edges", object->roundEdges);
	roundEdges->setEventHandler(this);
	addChild(roundEdges);
	y1+=30;
	verticalMovement=new ppltk::CheckBox(x1 + 30, y1, w, 30, "moves vertical", object->verticalMovement);
	verticalMovement->setEventHandler(this);
	addChild(verticalMovement);
	y1+=30;
	isInvisible=new ppltk::CheckBox(x1 + 30, y1, w, 30, "is invisible", object->isInvisible);
	isInvisible->setEventHandler(this);
	addChild(isInvisible);

	y1+=35;
	addChild(new ppltk::Label(x1, y1, 120, 30, "Debris: "));
	y1+=30;
	max_debris_length=new ppltk::HorizontalSlider(x1 + 30, y1, w, 30);
	max_debris_length->setLimits(1, 16);
	max_debris_length->setValue(object->max_debris_length);
	max_debris_length->enableSpinBox(true, 1, 80);
	max_debris_length->setEventHandler(this);
	addChild(max_debris_length);
	y1+=35;


	y+=305;

	addChild(new ppltk::Label(0, y, 120, 30, "Movement:"));
	movement_range=new ppltk::HorizontalSlider(120, y, sw - 120, 30);
	movement_range->setLimits(0, 255);
	movement_range->setValue(object->movement_range);
	movement_range->enableSpinBox(true, 1, 100);
	movement_range->setEventHandler(this);
	addChild(movement_range);

	addChild(new ppltk::Label(sw, y, 60, 30, "Speed:"));
	movement_speed=new ppltk::DoubleHorizontalSlider(sw + 60, y, client.width() - sw - 60, 30);
	movement_speed->setLimits(0.1f, 10.0f);
	movement_speed->setValue(object->movement_speed);
	movement_speed->enableSpinBox(true, 0.1, 1, 100);
	movement_speed->setEventHandler(this);
	addChild(movement_speed);

	y+=35;



	int col0=0;
	int w0=150;
	int w1=50;
	int w3=70;
	int col2=col1 + w1;
	sw=(client.width() - col2 - w3) / 2;
	int w2=sw;
	int w4=sw;
	int col3=col2 + w2 + 10;
	int col4=col3 + w3;


	addChild(new ppltk::Label(col0, y, w0, 30, "Time visible:"));
	addChild(new ppltk::Label(col1, y, w1, 30, "min:"));
	min_time_visible=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	min_time_visible->setEventHandler(this);
	min_time_visible->setLimits(1.0f, 20.0f);
	min_time_visible->setValue(object->min_time_visible);
	min_time_visible->enableSpinBox(true, 0.1f, 1, 80);
	addChild(min_time_visible);
	addChild(new ppltk::Label(col3, y, w3, 30, "max:"));
	max_time_visible=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	max_time_visible->setEventHandler(this);
	max_time_visible->setLimits(1.0f, 20.0f);
	max_time_visible->setValue(object->max_time_visible);
	max_time_visible->enableSpinBox(true, 0.1f, 1, 80);
	addChild(max_time_visible);
	y+=35;

	addChild(new ppltk::Label(col0, y, w0, 30, "Time invisible:"));
	addChild(new ppltk::Label(col1, y, w1, 30, "min:"));
	min_time_invisible=new ppltk::DoubleHorizontalSlider(col2, y, w2, 30);
	min_time_invisible->setEventHandler(this);
	min_time_invisible->setLimits(1.0f, 20.0f);
	min_time_invisible->setValue(object->min_time_invisible);
	min_time_invisible->enableSpinBox(true, 0.1f, 1, 80);
	addChild(min_time_invisible);
	addChild(new ppltk::Label(col3, y, w3, 30, "max:"));
	max_time_invisible=new ppltk::DoubleHorizontalSlider(col4, y, w4, 30);
	max_time_invisible->setEventHandler(this);
	max_time_invisible->setLimits(1.0f, 20.0f);
	max_time_invisible->setValue(object->max_time_invisible);
	max_time_invisible->enableSpinBox(true, 0.1f, 1, 80);
	addChild(max_time_invisible);
	y+=35;

	addChild(new ppltk::Label(0, y, 160, 30, "linked with Object ID:"));
	linked_with_id=new ppltk::SpinBox(col2, y, 100, 30);
	linked_with_id->setLimits(0, 65535);
	linked_with_id->setValue(object->linked_with_id);
	linked_with_id->setEventHandler(this);
	addChild(linked_with_id);
	y+=35;


}


MagicGroundDialog::~MagicGroundDialog()
{

}


void MagicGroundDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	if (event->widget() == color) {
		object->color=value;

	}
}

void MagicGroundDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
	if (event->widget() == movement_range) object->movement_range=value;
	else if (event->widget() == width) object->width=value;
	else if (event->widget() == graficset) object->graficset=value;
	else if (event->widget() == max_debris_length) {
		object->max_debris_length=value;
		object->randomizeDebris();
	} else if (event->widget() == linked_with_id) object->linked_with_id=value;
}

void MagicGroundDialog::valueChangedEvent(ppltk::Event* event, double value)
{
	if (event->widget() == movement_speed) {
		object->movement_speed=value;
	} else if (event->widget() == min_time_visible) {
		object->min_time_visible=value;
		if (object->min_time_visible > object->max_time_visible) max_time_visible->setValue(object->min_time_visible);
	} else if (event->widget() == max_time_visible) {
		object->max_time_visible=value;
		if (object->max_time_visible < object->min_time_visible) min_time_visible->setValue(object->max_time_visible);
	} else if (event->widget() == min_time_invisible) {
		object->min_time_invisible=value;
		if (object->min_time_invisible > object->max_time_invisible) max_time_invisible->setValue(object->min_time_invisible);
	} else if (event->widget() == max_time_invisible) {
		object->max_time_invisible=value;
		if (object->max_time_invisible < object->min_time_invisible) min_time_invisible->setValue(object->max_time_invisible);
	}
}

void MagicGroundDialog::toggledEvent(ppltk::Event* event, bool checked)
{
	if (event->widget() == initial_state_checkbox) {
		if (checked) object->initial_state=MagicGround::State::active;
		else object->initial_state=MagicGround::State::inactive;
		object->reset();
	} else if (event->widget() == current_state_checkbox) {
		if (checked) object->current_state=MagicGround::State::active;
		else object->current_state=MagicGround::State::inactive;
		object->reset();
	} else if (event->widget() == hasDebris) {
		object->hasDebris=checked;
		if (checked) object->randomizeDebris();
	} else if (event->widget() == hasStuds) {
		object->hasStuds=checked;
	} else if (event->widget() == verticalMovement) {
		object->verticalMovement=checked;
	} else if (event->widget() == roundEdges) {
		object->roundEdges=checked;
	} else if (event->widget() == canDissolve) {
		object->canDissolve=checked;
	} else if (event->widget() == isInvisible) {
		object->isInvisible=checked;

	}
}

void MagicGroundDialog::dialogButtonEvent(Dialog::Buttons button)
{
	if (button == Dialog::Buttons::Reset) object->reset();
}


}	// EOF namespace Decker::Objects
