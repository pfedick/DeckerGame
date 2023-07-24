#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

Representation KeyReward::representation()
{
	return Representation(Spriteset::GenericObjects, 272);
}

KeyReward::KeyReward()
	: Object(Type::ObjectType::Key)
{
	sprite_set=Spriteset::GenericObjects;
	sprite_no=272;
	collisionDetection=true;
	sprite_no_representation=272;
	key_type=KeyType::silver;
	color_modification=2;
	next_animation=0.0f;
	init();
}

void KeyReward::init()
{
	switch (key_type) {
	case KeyType::silver:
		animation.startRandomSequence(272, 292, true, 272);
		break;
	case KeyType::golden:
		animation.startRandomSequence(216, 236, true, 216);
		break;
	case KeyType::colored:
		animation.startRandomSequence(366, 386, true, 366);
		break;
	}
	updateColor();
}

void KeyReward::updateColor()
{
	const ColorPalette& palette=GetColorPalette();
	color_mod=palette.getColor(color_modification);

}

void KeyReward::update(double time, TileTypePlane&, Player&, float)
{
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
}


void KeyReward::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->countObject(type());
	player->addInventory(id, Representation(Spriteset::GenericObjects, sprite_no));
	// TODO: audio sound
	//AudioPool &audio=getAudioPool();
	//audio.playOnce(AudioClip::crystal, 1.0f);
}


size_t KeyReward::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 2);		// Object Version
	ppl7::Poke8(buffer + bytes + 1, static_cast<int>(key_type));
	ppl7::Poke8(buffer + bytes + 2, color_modification);
	return bytes + 3;
}

size_t KeyReward::saveSize() const
{
	return Object::saveSize() + 3;
}

size_t KeyReward::load(const unsigned char* buffer, size_t size)
{
	sprite_no_representation=272;
	key_type=KeyType::silver;
	color_modification=2;
	size_t bytes=Object::load(buffer, size);
	ppl7::PrintDebugTime("bytes=%zd, size=%zd\n", bytes, size);
	if (bytes > 0 || size > bytes + 1) {
		int version=ppl7::Peek8(buffer + bytes);
		if (version == 2) {
			key_type=static_cast<KeyType>(ppl7::Peek8(buffer + bytes + 1));
			color_modification=ppl7::Peek8(buffer + bytes + 2);
		}
	}
	init();
	return size;
}


class KeyRewardDialog : public Decker::ui::Dialog
{
private:
	ppl7::tk::ComboBox* key_type;
	Decker::ui::ColorSelectionFrame* colorframe;


	KeyReward* object;

	void setValuesToUi(const Scorpion* object);

public:
	KeyRewardDialog(KeyReward* object);
	virtual void valueChangedEvent(ppl7::tk::Event* event, int value);
};


void KeyReward::openUi()
{
	KeyRewardDialog* dialog=new KeyRewardDialog(this);
	GetGameWindow()->addChild(dialog);
}

KeyRewardDialog::KeyRewardDialog(KeyReward* object)
	: Decker::ui::Dialog(600, 400, Dialog::Buttons::OK)
{
	this->object=object;
	setWindowTitle(ppl7::ToString("Key (ID: %d)", object->id));

	//ppl7::grafix::Rect client=clientRect();
	int y=0;


	addChild(new ppl7::tk::Label(0, y, 150, 30, "Key Type:"));
	key_type=new ppl7::tk::ComboBox(150, y, 300, 30);
	key_type->add("silver", "0");
	key_type->add("golden", "1");
	key_type->add("colored", "2");
	key_type->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->key_type)));
	key_type->setEventHandler(this);
	addChild(key_type);
	y+=40;
	addChild(new ppl7::tk::Label(0, y, 80, 30, "Color:"));
	ColorPalette& palette=GetColorPalette();
	colorframe=new Decker::ui::ColorSelectionFrame(150, y, 300, 300, palette);
	colorframe->setEventHandler(this);
	colorframe->setColorIndex(object->color_modification);
	this->addChild(colorframe);

}


void KeyRewardDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
	ppl7::tk::Widget* widget=event->widget();
	if (widget == key_type) {
		object->key_type=static_cast<KeyReward::KeyType>(key_type->currentIdentifier().toInt());
		object->init();
	} else if (widget == colorframe) {
		object->color_modification=value;
		object->updateColor();
	}
}



}	// EOF namespace Decker::Objects
