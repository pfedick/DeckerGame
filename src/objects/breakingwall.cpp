#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {

Representation BreakingWall::representation()
{
	return Representation(Spriteset::BreakingWall, 0);
}

BreakingWall::BreakingWall()
	: Object(Type::ObjectType::BreakingWall)
{
	next_animation=0.0f;
	state=0;
	wall_color=2;
	sprite_set=Spriteset::BreakingWall;
	sprite_no=0;
	sprite_no_representation=0;
	collisionDetection=true;
	pixelExactCollision=false;
	myLayer=Layer::BeforePlayer;
	color_mod=GetColorPalette().getColor(wall_color);
}

void BreakingWall::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{

	if (state == 1) {
		if (time > next_animation) {
			next_animation=time + 0.04f;
			animation.update();
			sprite_no=animation.getFrame();
			if (animation.isFinished()) {
				state=2;
				collisionDetection=false;
			}

		}
	}
}

void BreakingWall::breakWall(Player* player)
{
	if (player->x < p.x) {
		animation.startSequence(30, 59, false, 59);
	} else {
		animation.startSequence(0, 29, false, 29);
	}
	collisionDetection=true;
	pixelExactCollision=false;
	state=1;
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::breakingwall, 0.7f);

}

void BreakingWall::handleCollision(Player* player, const Collision& collision)
{
	if (state == 0) {
		if (player->x < p.x)player->x=p.x - 64;
		if (player->x > p.x)player->x=p.x + 64;
		Player::Keys keyboard=player->getKeyboardMatrix();
		if (keyboard.matrix & KeyboardKeys::Action) {
			player->startHacking(this);
			if (player->x < p.x)player->x=p.x - 96;
			if (player->x > p.x)player->x=p.x + 96;
		}
		if (!player->hasSpoken(4)) {
			player->speak(4);
		}

	}

}

size_t BreakingWall::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version

	ppl7::Poke8(buffer + bytes + 1, wall_color);
	return bytes + 2;
}

size_t BreakingWall::saveSize() const
{
	return Object::saveSize() + 2;
}

size_t BreakingWall::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	myLayer=Layer::BeforePlayer;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	wall_color=ppl7::Peek8(buffer + bytes + 1);
	color_mod=GetColorPalette().getColor(wall_color);
	return size;
}


class BreakingWallDialog : public Decker::ui::Dialog
{
private:
	Decker::ui::ColorSelectionFrame* colorframe;
	BreakingWall* object;

	void setValuesToUi(const BreakingWall* object);

public:
	BreakingWallDialog(BreakingWall* object);
	virtual void valueChangedEvent(ppltk::Event* event, int value);
};


void BreakingWall::openUi()
{
	BreakingWallDialog* dialog=new BreakingWallDialog(this);
	GetGameWindow()->addChild(dialog);

}

BreakingWallDialog::BreakingWallDialog(BreakingWall* object)
	: Decker::ui::Dialog(600, 400, Dialog::Buttons::OK)
{
	this->object=object;
	setWindowTitle(ppl7::ToString("Breaking Wall"));

	//ppl7::grafix::Rect client=clientRect();
	int y=0;

	addChild(new ppltk::Label(0, y, 80, 30, "Color:"));
	ColorPalette& palette=GetColorPalette();
	colorframe=new Decker::ui::ColorSelectionFrame(150, y, 300, 300, palette);
	colorframe->setEventHandler(this);
	colorframe->setColorIndex(object->wall_color);
	this->addChild(colorframe);

}

void BreakingWallDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	ppltk::Widget* widget=event->widget();
	if (widget == colorframe) {
		object->wall_color=value;
		object->color_mod=GetColorPalette().getColor(object->wall_color);
	}
}


}	// EOF namespace Decker::Objects
