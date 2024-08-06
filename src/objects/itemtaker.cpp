#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"
#include "widgets.h"

namespace Decker::Objects {


Representation ItemTaker::representation()
{
    return Representation(Spriteset::GenericObjects, 298);
}


ItemTaker::ItemTaker()
    :Object(Type::ObjectType::ItemTaker)
{
    sprite_set=Spriteset::GenericObjects;
    sprite_no=298;
    sprite_no_representation=298;
    collisionDetection=false;
    visibleAtPlaytime=false;

    takeFlashlight=false;
    takeHammer=false;
    takeCheese=false;
    takeEnergyCells=false;
    takeExtralife=false;
    takeEnergy=false;
}

ItemTaker::~ItemTaker()
{

}

void ItemTaker::trigger(Object* source)
{
    //ppl7::PrintDebug("ItemTaker::trigger\n");
    Player* player=GetGame().getPlayer();
    if (takeFlashlight) player->takeAllItems(Objects::Type::Flashlight);
    if (takeHammer) player->takeAllItems(Objects::Type::Hammer);
    if (takeCheese) player->takeAllItems(Objects::Type::Cheese);
    if (takeEnergyCells) player->takeAllItems(Objects::Type::PowerCell);
    if (takeExtralife) player->takeAllItems(Objects::Type::ExtraLife);
    if (takeEnergy) player->drainBatteryCompletely();
}

void ItemTaker::test()
{
    trigger(NULL);
}

void ItemTaker::reset()
{

}


size_t ItemTaker::saveSize() const
{
    return Object::saveSize() + 3;
}

size_t ItemTaker::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 1);		// Object Version
    uint16_t flags=0;
    if (takeFlashlight) flags|=1;
    if (takeHammer) flags|=2;
    if (takeCheese) flags|=4;
    if (takeEnergyCells) flags|=8;
    if (takeExtralife) flags|=16;
    if (takeEnergy) flags|=32;

    ppl7::Poke16(buffer + bytes + 1, flags);
    return bytes + 3;

}

size_t ItemTaker::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version != 1) return 0;
    uint16_t flags=ppl7::Peek16(buffer + bytes + 1);
    takeFlashlight=flags & 1;
    takeHammer=flags & 2;
    takeCheese=flags & 4;
    takeEnergyCells=flags & 8;
    takeExtralife=flags & 16;
    takeEnergy=flags & 32;
    return size;

}

class ItemTakerDialog : public Decker::ui::Dialog
{
private:
    ppltk::CheckBox* takeFlashlight, * takeHammer;
    ppltk::CheckBox* takeCheese, * takeEnergyCells, * takeExtralife;
    ppltk::CheckBox* takeEnergy;

    ItemTaker* object;

public:
    ItemTakerDialog(ItemTaker* object);
    void toggledEvent(ppltk::Event* event, bool checked) override;
    virtual void dialogButtonEvent(Dialog::Buttons button) override;

};


void ItemTaker::openUi()
{
    ItemTakerDialog* dialog=new ItemTakerDialog(this);
    GetGameWindow()->addChild(dialog);
}

ItemTakerDialog::ItemTakerDialog(ItemTaker* object)
    : Decker::ui::Dialog(400, 400, Dialog::Buttons::OK | Dialog::Buttons::Test | Dialog::Buttons::Reset)
{
    this->object=object;
    this->setWindowTitle(ppl7::ToString("ItemTaker, ID: %d", object->id));
    ppl7::grafix::Rect client=clientRect();
    int y=0;
    int sw=(client.width()) / 2;
    // State
    takeFlashlight=new ppltk::CheckBox(0, y, sw, 30, "Flashlight", object->takeFlashlight);
    takeFlashlight->setEventHandler(this);
    addChild(takeFlashlight);
    y+=30;

    takeHammer=new ppltk::CheckBox(0, y, sw, 30, "Hammer", object->takeHammer);
    takeHammer->setEventHandler(this);
    addChild(takeHammer);
    y+=30;

    takeCheese=new ppltk::CheckBox(0, y, sw, 30, "Cheese", object->takeCheese);
    takeCheese->setEventHandler(this);
    addChild(takeCheese);
    y+=30;

    takeEnergyCells=new ppltk::CheckBox(0, y, sw, 30, "Power Cells", object->takeEnergyCells);
    takeEnergyCells->setEventHandler(this);
    addChild(takeEnergyCells);
    y+=30;

    takeExtralife=new ppltk::CheckBox(0, y, sw, 30, "Extra Life", object->takeExtralife);
    takeExtralife->setEventHandler(this);
    addChild(takeExtralife);
    y+=30;

    takeEnergy=new ppltk::CheckBox(0, y, sw, 30, "Energy", object->takeEnergy);
    takeEnergy->setEventHandler(this);
    addChild(takeEnergy);
    y+=30;


}

void ItemTakerDialog::toggledEvent(ppltk::Event* event, bool checked)
{
    if (event->widget() == takeFlashlight) {
        object->takeFlashlight=checked;
    } else if (event->widget() == takeHammer) {
        object->takeHammer=checked;
    } else if (event->widget() == takeCheese) {
        object->takeCheese=checked;
    } else if (event->widget() == takeEnergyCells) {
        object->takeEnergyCells=checked;
    } else if (event->widget() == takeExtralife) {
        object->takeExtralife=checked;
    } else if (event->widget() == takeEnergy) {
        object->takeEnergy=checked;
    }
}


void ItemTakerDialog::dialogButtonEvent(Dialog::Buttons button)
{
    if (button & Buttons::Reset) {
        object->reset();
    } else if (button & Buttons::Test) {
        object->test();
    }

}

}	// EOF namespace Decker::Objects
