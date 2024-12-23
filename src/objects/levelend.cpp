#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "decker.h"
#include "player.h"
#include "widgets.h"
#include "ui.h"

namespace Decker::Objects {

static int portal_animation[]={ 5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35 };


LevelEnd::LevelEnd()
    :Object(Type::ObjectType::LevelEnd)
{
    sprite_set=Spriteset::LevelEnd;
    sprite_no_representation=0;
    state=State::Inactive;
    flags=static_cast<Flags>(0);
    key_id=0;
    collisionDetection=false;
    pixelExactCollision=false;
    alwaysUpdate=false;
    next_animation=0.0;
    cooldown=0.0f;
    audio=NULL;
    color_doorframe=1;
    color_background=0;
    color_puddle=2;
    color_details_doorframe=1;
    color_details_arch=1;
    color_stairs=1;
    background_alpha=255;
    warp_to_id=0;
}

LevelEnd::~LevelEnd()
{
    if (audio) {
        getAudioPool().stopInstace(audio);
        delete audio;
        audio=NULL;
    }
}

Representation LevelEnd::representation()
{
    return Representation(Spriteset::LevelEnd, 0);
}

void LevelEnd::init()
{

}

void LevelEnd::update(double time, TileTypePlane& ttplane, Player& player, float)
{
    if (state == State::Active) {
        collisionDetection=true;
        if (time > next_animation) {
            next_animation=time + 0.056f;
            animation.update();
        }
    }
}

void LevelEnd::handleCollision(Player* player, const Collision& collision)
{
    if (state != State::Active) return;
    if (player->x > p.x - TILE_WIDTH * 2 && player->x < p.x + TILE_WIDTH * 2 &&
        player->y<p.y + TILE_HEIGHT && player->y>p.y - TILE_HEIGHT * 5) {
        double now=ppl7::GetMicrotime();
        Player::Keys keyboard=player->getKeyboardMatrix();
        if (((static_cast<int>(flags) & static_cast<int>(Flags::transferOnCollision)) || (keyboard.matrix & KeyboardKeys::Action)) && cooldown < now) {
            cooldown=now + 0.2;
            if (next_level.notEmpty()) {
                if (!GetGame().nextLevel(next_level)) {
                    toggle(false);
                    return;
                } else {
                    //printf("switch\n");
                    cooldown=now + 20000;
                }
            } else if (warp_to_id > 0) {
                LevelEnd* target=static_cast<LevelEnd*>(GetObjectSystem()->getObject(warp_to_id));
                if (target != NULL && target->type() == Type::ObjectType::LevelEnd) {
                    target->cooldown=now + 1.0f;
                    player->move(target->p.x, target->p.y);
                }
            }
        }


    }
}

size_t LevelEnd::saveSize() const
{
    return Object::saveSize() + 26 + next_level.size();
}

size_t LevelEnd::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 4);		// Object Version

    ppl7::Poke16(buffer + bytes + 1, static_cast<int>(flags));
    ppl7::Poke16(buffer + bytes + 3, color_doorframe);
    ppl7::Poke16(buffer + bytes + 5, color_background);
    ppl7::Poke16(buffer + bytes + 7, color_puddle);
    ppl7::Poke16(buffer + bytes + 9, color_details_doorframe);
    ppl7::Poke16(buffer + bytes + 11, color_details_arch);
    ppl7::Poke16(buffer + bytes + 13, color_stairs);
    ppl7::Poke32(buffer + bytes + 15, key_id);
    ppl7::Poke16(buffer + bytes + 19, next_level.size());
    memcpy(buffer + bytes + 21, next_level.c_str(), next_level.size());
    size_t p=21 + next_level.size();
    ppl7::Poke8(buffer + bytes + p, background_alpha);
    p++;
    ppl7::Poke32(buffer + bytes + p, warp_to_id);
    p+=4;

    return bytes + p;
}

size_t LevelEnd::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0 || size < bytes + 1) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version < 2) return size;


    flags=static_cast<Flags>(ppl7::Peek16(buffer + bytes + 1));
    color_doorframe=ppl7::Peek16(buffer + bytes + 3);
    color_background=ppl7::Peek16(buffer + bytes + 5);
    color_puddle=ppl7::Peek16(buffer + bytes + 7);
    color_details_doorframe=ppl7::Peek16(buffer + bytes + 9);
    color_details_arch=ppl7::Peek16(buffer + bytes + 11);
    color_stairs=ppl7::Peek16(buffer + bytes + 13);

    key_id=ppl7::Peek32(buffer + bytes + 15);
    size_t next_level_size=ppl7::Peek16(buffer + bytes + 19);
    next_level.set((const char*)(buffer + bytes + 21), next_level_size);
    size_t p=21 + next_level_size;
    if (version > 2) {
        background_alpha=ppl7::Peek8(buffer + bytes + p);
        p++;
    }
    if (version >= 4) {
        warp_to_id=ppl7::Peek32(buffer + bytes + p);
    }
    state=State::Inactive;

    if (static_cast<int>(flags) & static_cast<int>(Flags::initialStateActive)) toggle(true, NULL);
    return size;
}


void LevelEnd::toggle(bool enable, Object* source)
{
    if (enable) {
        animation.startRandom(portal_animation, sizeof(portal_animation) / sizeof(int), true, 0);
        state=State::Active;


    } else {
        if (audio) {
            getAudioPool().stopInstace(audio);
            delete audio;
            audio=NULL;
        }
        sprite_no=0;
        state=State::Inactive;
    }
}

void LevelEnd::trigger(Object* source)
{
    if (state == State::Inactive) toggle(true, source);
    else toggle(false, source);
}

void LevelEnd::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
    LevelEnd::draw(renderer, coords);
}

void LevelEnd::draw(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
    const ColorPalette& palette=GetColorPalette();
    int sprite=animation.getFrame();
    // Puddle if active
    if (state == State::Active && (static_cast<int>(flags) & static_cast<int>(Flags::useBackgroundColorWhenActive))) {
        // Background
        ppl7::grafix::Color bg=palette.getColor(color_background);
        SDL_BlendMode currentBlendMode;
        SDL_Rect r;
        r.x=p.x + coords.x - 2 * TILE_WIDTH;
        r.y=p.y + coords.y - 7 * TILE_HEIGHT;
        r.w=5 * TILE_WIDTH;
        r.h=8 * TILE_HEIGHT;
        SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, bg.red(), bg.green(), bg.blue(), background_alpha);
        SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);

        // Puddle
        texture->draw(renderer,
            p.x + coords.x,
            p.y + coords.y,
            sprite, palette.getColor(color_puddle));

    }
    // Details Arch
    texture->draw(renderer,
        p.x + coords.x,
        p.y + coords.y,
        3, palette.getColor(color_details_arch));

    // Doorframe
    texture->draw(renderer,
        p.x + coords.x,
        p.y + coords.y,
        1, palette.getColor(color_doorframe));

    // Stairs
    texture->draw(renderer,
        p.x + coords.x,
        p.y + coords.y,
        4, palette.getColor(color_stairs));

    // Details Doorframe
    texture->draw(renderer,
        p.x + coords.x,
        p.y + coords.y,
        2, palette.getColor(color_details_doorframe));
        /*
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect r;
        r.x=p.x + coords.x - 2 * TILE_WIDTH;
        r.y=p.y + coords.y - 6 * TILE_HEIGHT;
        r.w=4 * TILE_WIDTH;
        r.h=6 * TILE_HEIGHT;
        SDL_RenderDrawRect(renderer, &r);
    }
    */

}

class LevelEndDialog : public Decker::ui::Dialog
{
private:
    LevelEnd* object;

    ppltk::CheckBox* initial_state_checkbox, * current_state_checkbox;
    ppltk::CheckBox* use_background_color_when_active;
    ppltk::CheckBox* transfer_on_collision;
    ppltk::LineInput* next_level;
    ppltk::SpinBox* key_id;
    ppltk::SpinBox* warp_to_id;
    Decker::ui::ColorSelectionFrame* colorframe;
    ppltk::Button* button_color_background;
    ppltk::Button* button_color_doorframe;
    ppltk::Button* button_color_doorframe_details;
    ppltk::Button* button_color_stairs;
    ppltk::Button* button_color_arch_details;
    ppltk::Button* button_color_puddle;

    ppltk::Frame* frame_color_background;
    ppltk::Frame* frame_color_doorframe;
    ppltk::Frame* frame_color_doorframe_details;
    ppltk::Frame* frame_color_stairs;
    ppltk::Frame* frame_color_arch_details;
    ppltk::Frame* frame_color_puddle;
    ppltk::Label* current_element_label;
    ppltk::Frame* current_element_color_frame;
    ppltk::HorizontalSlider* background_alpha;
    int* color_target;

    enum class Element {
        Background,
        Puddle,
        Doorframe,
        Details,
        Arch,
        Stairs
    };

    Element current_element;

    void setCurrentElement(Element element);

public:
    LevelEndDialog(LevelEnd* object);

    void toggledEvent(ppltk::Event* event, bool checked) override;
    void textChangedEvent(ppltk::Event* event, const ppl7::String& text) override;
    void valueChangedEvent(ppltk::Event* event, int value) override;
    void valueChangedEvent(ppltk::Event* event, int64_t value) override;
    void mouseDownEvent(ppltk::MouseEvent* event) override;

};




void LevelEnd::openUi()
{
    LevelEndDialog* dialog=new LevelEndDialog(this);
    GetGameWindow()->addChild(dialog);
}

LevelEndDialog::LevelEndDialog(LevelEnd* object)
    : Decker::ui::Dialog(570, 640, Dialog::Buttons::OK)
{
    current_element=Element::Background;
    current_element_color_frame=NULL;
    color_target=NULL;
    this->object=object;
    this->setWindowTitle("Level End");
    ppl7::grafix::Rect client=clientRect();
    //int col1=100;
    int y=0;
    int sw=(client.width()) / 2;
    // State
    initial_state_checkbox=new ppltk::CheckBox(0, y, sw, 30, "initial State");
    initial_state_checkbox->setEventHandler(this);
    addChild(initial_state_checkbox);
    current_state_checkbox=new ppltk::CheckBox(sw, y, sw, 30, "current State");
    current_state_checkbox->setEventHandler(this);
    addChild(current_state_checkbox);
    y+=35;
    use_background_color_when_active=new ppltk::CheckBox(0, y, client.width(), 30, "use Background color when active");
    use_background_color_when_active->setEventHandler(this);
    addChild(use_background_color_when_active);
    y+=35;
    transfer_on_collision=new ppltk::CheckBox(0, y, client.width(), 30, "transfer on collision");
    transfer_on_collision->setEventHandler(this);
    addChild(transfer_on_collision);
    y+=35;

    addChild(new ppltk::Label(0, y, 80, 30, "Level file:"));
    next_level=new ppltk::LineInput(80, y, client.width() - 100, 30, "");
    next_level->setEventHandler(this);
    addChild(next_level);
    y+=35;

    addChild(new ppltk::Label(0, y, 150, 30, "unlock with Object ID:"));
    key_id=new ppltk::SpinBox(150, y, 100, 30);
    key_id->setLimits(0, 65535);
    key_id->setEventHandler(this);
    addChild(key_id);

    addChild(new ppltk::Label(280, y, 150, 30, "transfer to object ID:"));
    warp_to_id=new ppltk::SpinBox(430, y, 100, 30);
    warp_to_id->setLimits(0, 65535);
    warp_to_id->setValue(object->warp_to_id);
    warp_to_id->setEventHandler(this);
    addChild(warp_to_id);
    y+=40;

    addChild(new ppltk::Label(0, y, 80, 30, "Colors:"));
    current_element_label=new ppltk::Label(client.width() - 300, y, 300, 30, "Background");
    addChild(current_element_label);
    y+=35;

    ColorPalette& palette=GetColorPalette();
    colorframe=new Decker::ui::ColorSelectionFrame(client.width() - 300, y, 300, 300, palette);
    colorframe->setEventHandler(this);
    this->addChild(colorframe);

    button_color_background=new ppltk::Button(30, y, 100, 30, "Background");
    button_color_background->setEventHandler(this);
    addChild(button_color_background);
    frame_color_background=new ppltk::Frame(135, y, 60, 30, ppltk::Frame::Inset);
    frame_color_background->setBackgroundColor(palette.getColor(object->color_background));
    addChild(frame_color_background);

    y+=35;

    button_color_puddle=new ppltk::Button(30, y, 100, 30, "Puddle");
    button_color_puddle->setEventHandler(this);
    addChild(button_color_puddle);
    frame_color_puddle=new ppltk::Frame(135, y, 60, 30, ppltk::Frame::Inset);
    frame_color_puddle->setBackgroundColor(palette.getColor(object->color_puddle));
    addChild(frame_color_puddle);
    y+=35;

    button_color_doorframe=new ppltk::Button(30, y, 100, 30, "Doorframe");
    button_color_doorframe->setEventHandler(this);
    addChild(button_color_doorframe);
    frame_color_doorframe=new ppltk::Frame(135, y, 60, 30, ppltk::Frame::Inset);
    frame_color_doorframe->setBackgroundColor(palette.getColor(object->color_doorframe));
    addChild(frame_color_doorframe);
    y+=35;

    button_color_doorframe_details=new ppltk::Button(30, y, 100, 30, "Details");
    button_color_doorframe_details->setEventHandler(this);
    addChild(button_color_doorframe_details);
    frame_color_doorframe_details=new ppltk::Frame(135, y, 60, 30, ppltk::Frame::Inset);
    frame_color_doorframe_details->setBackgroundColor(palette.getColor(object->color_details_doorframe));
    addChild(frame_color_doorframe_details);
    y+=35;

    button_color_arch_details=new ppltk::Button(30, y, 100, 30, "Arch");
    button_color_arch_details->setEventHandler(this);
    addChild(button_color_arch_details);
    frame_color_arch_details=new ppltk::Frame(135, y, 60, 30, ppltk::Frame::Inset);
    frame_color_arch_details->setBackgroundColor(palette.getColor(object->color_details_arch));
    addChild(frame_color_arch_details);
    y+=35;

    button_color_stairs=new ppltk::Button(30, y, 100, 30, "Stairs");
    button_color_stairs->setEventHandler(this);
    addChild(button_color_stairs);
    frame_color_stairs=new ppltk::Frame(135, y, 60, 30, ppltk::Frame::Inset);
    frame_color_stairs->setBackgroundColor(palette.getColor(object->color_stairs));
    addChild(frame_color_stairs);
    y+=35;

    addChild(new ppltk::Label(0, y, 80, 30, "Background-Alpha:"));
    y+=35;
    background_alpha=new ppltk::HorizontalSlider(0, y, client.width() - 300, 30);
    background_alpha->setEventHandler(this);
    background_alpha->setLimits(0, 255);
    addChild(background_alpha);

    next_level->setText(object->next_level);
    initial_state_checkbox->setChecked(static_cast<int>(object->flags) & static_cast<int>(LevelEnd::Flags::initialStateActive));
    use_background_color_when_active->setChecked(static_cast<int>(object->flags) & static_cast<int>(LevelEnd::Flags::useBackgroundColorWhenActive));
    transfer_on_collision->setChecked(static_cast<int>(object->flags) & static_cast<int>(LevelEnd::Flags::transferOnCollision));
    current_state_checkbox->setChecked(object->state == LevelEnd::State::Active);
    key_id->setValue(object->key_id);
    background_alpha->setValue(object->background_alpha);

    setCurrentElement(Element::Background);
}

void LevelEndDialog::setCurrentElement(Element element)
{
    current_element=element;
    switch (element) {
        case Element::Background:
            current_element_label->setText("Background");
            current_element_color_frame=frame_color_background;
            color_target=&object->color_background;
            break;
        case Element::Puddle:
            current_element_label->setText("Puddle");
            current_element_color_frame=frame_color_puddle;
            color_target=&object->color_puddle;
            break;
        case Element::Doorframe:
            current_element_label->setText("Doorframe");
            current_element_color_frame=frame_color_doorframe;
            color_target=&object->color_doorframe;
            break;
        case Element::Details:
            current_element_label->setText("Doorframe Details");
            current_element_color_frame=frame_color_doorframe_details;
            color_target=&object->color_details_doorframe;
            break;
        case Element::Arch:
            current_element_label->setText("Arch");
            current_element_color_frame=frame_color_arch_details;
            color_target=&object->color_details_arch;
            break;
        case Element::Stairs:
            current_element_label->setText("Stairs");
            current_element_color_frame=frame_color_stairs;
            color_target=&object->color_stairs;
            break;
    }

    colorframe->setColorIndex(*color_target);
}

void LevelEndDialog::toggledEvent(ppltk::Event* event, bool checked)
{
    ppltk::Widget* widget=event->widget();
    int flags=static_cast<int>(object->flags);
    if (widget == initial_state_checkbox) {
        flags=flags & (0xffff - static_cast<int>(LevelEnd::Flags::initialStateActive));
        if (checked) flags|=static_cast<int>(LevelEnd::Flags::initialStateActive);
    } else if (widget == use_background_color_when_active) {
        flags=flags & (0xffff - static_cast<int>(LevelEnd::Flags::useBackgroundColorWhenActive));
        if (checked) flags|=static_cast<int>(LevelEnd::Flags::useBackgroundColorWhenActive);
    } else if (widget == transfer_on_collision) {
        flags=flags & (0xffff - static_cast<int>(LevelEnd::Flags::transferOnCollision));
        if (checked) flags|=static_cast<int>(LevelEnd::Flags::transferOnCollision);
    } else if (widget == current_state_checkbox) {
        object->toggle(checked);
    }

    object->flags=static_cast<LevelEnd::Flags>(flags);
}


void LevelEndDialog::textChangedEvent(ppltk::Event* event, const ppl7::String& text)
{
    if (event->widget() == next_level) object->next_level=text;
}

void LevelEndDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    ppltk::Widget* widget=event->widget();
    if (widget == key_id) object->key_id=value;
    else if (widget == warp_to_id) object->warp_to_id=value;
    else if (widget == background_alpha) object->background_alpha=value;
}

void LevelEndDialog::valueChangedEvent(ppltk::Event* event, int value)
{
    ppltk::Widget* widget=event->widget();
    if (widget == colorframe && color_target != NULL) {
        *color_target=value;
        if (current_element_color_frame)
            current_element_color_frame->setBackgroundColor(GetColorPalette().getColor(value));

    }
}

void LevelEndDialog::mouseDownEvent(ppltk::MouseEvent* event)
{
    ppltk::Widget* widget=event->widget();
    if (widget == button_color_background || widget == frame_color_background) setCurrentElement(Element::Background);
    else if (widget == button_color_doorframe) setCurrentElement(Element::Doorframe);
    else if (widget == button_color_doorframe_details) setCurrentElement(Element::Details);
    else if (widget == button_color_stairs) setCurrentElement(Element::Stairs);
    else if (widget == button_color_arch_details) setCurrentElement(Element::Arch);
    else if (widget == button_color_puddle) setCurrentElement(Element::Puddle);
    else Dialog::mouseDownEvent(event);


}


}   // EOF namespace
