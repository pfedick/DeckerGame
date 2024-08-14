#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "objects.h"
#include "decker.h"
#include "widgets.h"
#include "player.h"

namespace Decker::Objects {



Representation LevelModificator::representation()
{
    return Representation(Spriteset::GenericObjects, 467);
}

LevelModificator::LevelModificator()
    :Object(Type::ObjectType::LevelModificator)
{
    sprite_set=Spriteset::GenericObjects;
    sprite_no=467;
    collisionDetection=true;
    pixelExactCollision=false;
    visibleAtPlaytime=false;
    initialStateEnabled=true;
    sprite_no_representation=467;
    range.setPoint(TILE_WIDTH * 6, TILE_HEIGHT * 6);
    state=State::waiting_for_activation;
    loadLevelDefault=true;
    changeBackground=false;
    changeGlobalLighting=false;
    transitionTime=0.0f;

}

LevelModificator::~LevelModificator()
{

}

void LevelModificator::drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Point& coords) const
{
    SDL_Rect r;
    r.x=p.x + coords.x - range.x / 2;
    r.y=p.y + coords.y - range.y / 2;
    r.w=range.x;
    r.h=range.y;

    SDL_BlendMode currentBlendMode;
    SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 96, 192, 96);
    SDL_RenderFillRect(renderer, &r);
    SDL_SetRenderDrawColor(renderer, 0, 96, 192, 255);
    SDL_RenderDrawRect(renderer, &r);
    SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);

    Object::drawEditMode(renderer, coords);
}

void LevelModificator::handleCollision(Player* player, const Collision& collision)
{
    if (state != State::waiting_for_activation) return;
    state=State::in_transition;
    GetGame().startLevelModification(player->time, this);
}

void LevelModificator::test()
{
    state=State::in_transition;
    GetGame().startLevelModification(ppl7::GetMicrotime(), this);
}



void LevelModificator::update(double time, TileTypePlane& ttplane, Player& player, float)
{
    boundary.setRect(p.x - range.x / 2, p.y - range.y / 2, range.x, range.y);
    if (state == State::in_transition) {
        if (GetGame().getLevelModificationObject() != this) state=State::waiting_for_activation;
    }


}

size_t LevelModificator::saveSize() const
{
    size_t s=17 + 4 + BackgroundImage.size() + Song.size();
    return Object::saveSize() + s;

}

size_t LevelModificator::save(unsigned char* buffer, size_t size) const
{
    size_t bytes=Object::save(buffer, size);
    if (!bytes) return 0;
    ppl7::Poke8(buffer + bytes, 1);		// Object Version
    int flags=0;
    if (initialStateEnabled) flags|=1;
    if (loadLevelDefault) flags|=2;
    if (changeBackground) flags|=4;
    if (changeGlobalLighting) flags|=8;
    if (changeSong) flags|=16;

    ppl7::Poke8(buffer + bytes + 1, flags);
    ppl7::PokeFloat(buffer + bytes + 2, transitionTime);
    ppl7::Poke8(buffer + bytes + 6, static_cast<int>(backgroundType));
    ppl7::Poke8(buffer + bytes + 7, BackgroundColor.red());
    ppl7::Poke8(buffer + bytes + 8, BackgroundColor.green());
    ppl7::Poke8(buffer + bytes + 9, BackgroundColor.blue());
    ppl7::Poke8(buffer + bytes + 10, GlobalLighting.red());
    ppl7::Poke8(buffer + bytes + 11, GlobalLighting.green());
    ppl7::Poke8(buffer + bytes + 12, GlobalLighting.blue());
    ppl7::Poke16(buffer + bytes + 13, range.x);
    ppl7::Poke16(buffer + bytes + 15, range.y);
    int p=17;
    ppl7::Poke16(buffer + bytes + p, BackgroundImage.size());
    memcpy(buffer + bytes + p + 2, BackgroundImage.c_str(), BackgroundImage.size());
    p+=2 + BackgroundImage.size();
    ppl7::Poke16(buffer + bytes + p, Song.size());
    memcpy(buffer + bytes + p + 2, Song.c_str(), Song.size());
    p+=2 + Song.size();
    return bytes + p;
}

size_t LevelModificator::load(const unsigned char* buffer, size_t size)
{
    size_t bytes=Object::load(buffer, size);
    if (bytes == 0 || size < bytes + 1) return 0;
    int version=ppl7::Peek8(buffer + bytes);
    if (version != 1) return 0;

    int flags=ppl7::Peek8(buffer + bytes + 1);
    initialStateEnabled=false;
    loadLevelDefault=false;
    changeBackground=false;
    changeGlobalLighting=false;
    changeSong=false;
    if (flags & 1) initialStateEnabled=true;
    if (flags & 2) loadLevelDefault=true;
    if (flags & 4) changeBackground=true;
    if (flags & 8) changeGlobalLighting=true;
    if (flags & 16) changeSong=true;
    if (!initialStateEnabled) enabled=false;
    state=State::waiting_for_activation;
    transitionTime=ppl7::PeekFloat(buffer + bytes + 2);
    backgroundType=static_cast<BackgroundType>(ppl7::Peek8(buffer + bytes + 6));
    BackgroundColor.setRed(ppl7::Peek8(buffer + bytes + 7));
    BackgroundColor.setGreen(ppl7::Peek8(buffer + bytes + 8));
    BackgroundColor.setBlue(ppl7::Peek8(buffer + bytes + 9));
    BackgroundColor.setAlpha(255);

    GlobalLighting.setRed(ppl7::Peek8(buffer + bytes + 10));
    GlobalLighting.setGreen(ppl7::Peek8(buffer + bytes + 11));
    GlobalLighting.setBlue(ppl7::Peek8(buffer + bytes + 12));
    GlobalLighting.setAlpha(255);

    range.x=ppl7::Peek16(buffer + bytes + 13);
    range.y=ppl7::Peek16(buffer + bytes + 15);
    int p=17;
    size_t s=ppl7::Peek16(buffer + bytes + p);
    BackgroundImage.set((const char*)(buffer + bytes + p + 2), s);
    p+=2;
    s=ppl7::Peek16(buffer + bytes + p);
    Song.set((const char*)(buffer + bytes + p + 2), s);
    GetGame().texture_cache.get(BackgroundImage);
    return size;
}

void LevelModificator::reset()
{
    if (initialStateEnabled) enabled=true;
    else enabled=false;
    state=State::waiting_for_activation;
}

void LevelModificator::toggle(bool enable, Object* source)
{
    enabled=enable;
    if (!enable) state=State::disabled;
    else state=State::waiting_for_activation;
}

void LevelModificator::trigger(Object* source)
{
    if (!enabled) {
        enabled=true;
        state=State::waiting_for_activation;
        return;
    }
    if (state == State::disabled) return;
    if (state == State::waiting_for_activation) {
        state=State::in_transition;
    }

}



class LevelModificatorDialog : public Decker::ui::Dialog
{
private:
    ppltk::HorizontalSlider* range_x;
    ppltk::HorizontalSlider* range_y;
    ppltk::DoubleHorizontalSlider* transitionTime;
    ppltk::CheckBox* initialStateEnabled, * currentState;
    LevelModificator* object;

    ppltk::CheckBox* loadLevelDefault;
    ppltk::CheckBox* changeBackground;
    ppltk::CheckBox* changeGlobalLighting;
    ppltk::CheckBox* changeSong;
    ppltk::Frame* globalLightingFrame;
    ppltk::Frame* backgroundFrame;
    ppltk::Frame* songFrame;
    ppltk::RadioButton* backgroundTypeImage;
    ppltk::RadioButton* backgroundTypeColor;
    Decker::ui::ColorSliderWidget* GlobalLighting;
    Decker::ui::ColorSliderWidget* BackgroundColor;
    ppltk::ComboBox* backgroundImage;
    ppltk::ComboBox* songComboBox;
    ppltk::Button* CopyGlobalLightingFromLevel;
    ppltk::Button* CopyBackgroundFromLevel;



public:
    LevelModificatorDialog(LevelModificator* object);
    ~LevelModificatorDialog();

    //virtual void valueChangedEvent(ppltk::Event* event, int value);
    virtual void valueChangedEvent(ppltk::Event* event, int value);
    virtual void valueChangedEvent(ppltk::Event* event, int64_t value);
    virtual void valueChangedEvent(ppltk::Event* event, double value);
    virtual void toggledEvent(ppltk::Event* event, bool checked) override;
    virtual void dialogButtonEvent(Dialog::Buttons button) override;
    virtual void mouseDownEvent(ppltk::MouseEvent* event) override;
};


void LevelModificator::openUi()
{
    LevelModificatorDialog* dialog=new LevelModificatorDialog(this);
    GetGameWindow()->addChild(dialog);
}


LevelModificatorDialog::LevelModificatorDialog(LevelModificator* object)
    : Decker::ui::Dialog(800, 700, Buttons::OK | Buttons::Test | Buttons::Reset)
{
    ppl7::grafix::Rect client=clientRect();
    this->object=object;
    setWindowTitle(ppl7::ToString("LevelModificator, Object ID: %u", object->id));
    int y=0;

    int sw=client.width() / 2;
    initialStateEnabled=new ppltk::CheckBox(0, y, sw, 30, "initial state");
    initialStateEnabled->setEventHandler(this);
    initialStateEnabled->setChecked(object->initialStateEnabled);
    addChild(initialStateEnabled);
    currentState=new ppltk::CheckBox(sw, y, sw, 30, "current state");
    currentState->setEventHandler(this);
    currentState->setChecked(object->enabled);
    addChild(currentState);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Collision Range x:"));
    range_x=new ppltk::HorizontalSlider(120, y, sw - 120, 30);
    range_x->setLimits(0, 1600);
    range_x->setValue(object->range.x);
    range_x->enableSpinBox(true, 1, 80);
    range_x->setEventHandler(this);
    addChild(range_x);
    addChild(new ppltk::Label(sw, y, 60, 30, "Range y:"));
    range_y=new ppltk::HorizontalSlider(sw + 60, y, sw - 60, 30);
    range_y->setLimits(0, 1600);
    range_y->setValue(object->range.y);
    range_y->enableSpinBox(true, 1, 80);
    range_y->setEventHandler(this);
    addChild(range_y);
    y+=35;

    loadLevelDefault=new ppltk::CheckBox(0, y, client.width(), 30, "load level default", object->loadLevelDefault);
    loadLevelDefault->setEventHandler(this);
    addChild(loadLevelDefault);
    y+=35;

    changeBackground=new ppltk::CheckBox(0, y, client.width(), 30, "change Background", object->changeBackground);
    changeBackground->setEventHandler(this);
    addChild(changeBackground);
    y+=35;
    backgroundFrame=new ppltk::Frame(40, y, client.width() - 40, 175);

    int y1=0;
    CopyBackgroundFromLevel=new ppltk::Button(backgroundFrame->width() - 195, 0, 195, 30, "copy from level");
    CopyBackgroundFromLevel->setEventHandler(this);
    backgroundFrame->addChild(CopyBackgroundFromLevel);


    backgroundTypeImage=new ppltk::RadioButton(0, y1, 80, 30, "Image:");
    backgroundTypeImage->setEventHandler(this);
    backgroundFrame->addChild(backgroundTypeImage);
    backgroundImage=new ppltk::ComboBox(100, y1, backgroundFrame->width() - 310, 30);
    backgroundImage->setEventHandler(this);
    Resources& res=getResources();
    std::list<ppl7::String>::const_iterator it;
    backgroundImage->add("no image", "");
    for (it=res.background_images.begin();it != res.background_images.end();++it) {
        backgroundImage->add((*it), (*it));
    }
    if (object->BackgroundImage.notEmpty()) backgroundImage->setCurrentText(object->BackgroundImage);
    backgroundFrame->addChild(backgroundImage);
    y1+=35;

    backgroundTypeColor=new ppltk::RadioButton(0, y1, 80, 30, "Color:");
    backgroundTypeColor->setEventHandler(this);
    backgroundFrame->addChild(backgroundTypeColor);
    y1+=35;
    BackgroundColor=new Decker::ui::ColorSliderWidget(0, y1, backgroundFrame->width() - 200, 100);
    BackgroundColor->setEventHandler(this);
    BackgroundColor->setColor(object->BackgroundColor);
    backgroundFrame->addChild(BackgroundColor);

    if (object->backgroundType == LevelModificator::BackgroundType::Image) backgroundTypeImage->setChecked(true);
    else backgroundTypeColor->setChecked(true);

    addChild(backgroundFrame);
    backgroundFrame->setEnabled(changeBackground->checked());
    y+=180;


    changeGlobalLighting=new ppltk::CheckBox(0, y, client.width(), 30, "change global lighting", object->changeGlobalLighting);
    changeGlobalLighting->setEventHandler(this);
    addChild(changeGlobalLighting);
    y+=35;
    globalLightingFrame=new ppltk::Frame(40, y, client.width() - 40, 110);
    GlobalLighting=new Decker::ui::ColorSliderWidget(0, 0, globalLightingFrame->width() - 200, 100);
    GlobalLighting->setEventHandler(this);
    GlobalLighting->setColor(object->GlobalLighting);
    globalLightingFrame->addChild(GlobalLighting);
    CopyGlobalLightingFromLevel=new ppltk::Button(globalLightingFrame->width() - 195, 0, 195, 30, "copy from level");
    CopyGlobalLightingFromLevel->setEventHandler(this);
    globalLightingFrame->addChild(CopyGlobalLightingFromLevel);
    addChild(globalLightingFrame);
    globalLightingFrame->setEnabled(changeGlobalLighting->checked());
    y+=115;



    changeSong=new ppltk::CheckBox(0, y, client.width(), 30, "change song", object->changeSong);
    changeSong->setEventHandler(this);
    addChild(changeSong);
    y+=35;
    songFrame=new ppltk::Frame(40, y, client.width() - 40, 50);
    songFrame->addChild(new ppltk::Label(0, 0, 100, 30, "Song:"));
    songComboBox=new ppltk::ComboBox(100, 0, 300, 30);
    songComboBox->add("no song", "");
    AudioPool& pool=getAudioPool();
    {
        std::list<MusicTrack>::const_iterator it;
        for (it=pool.musictracks.begin();it != pool.musictracks.end();++it) {
            songComboBox->add((*it).Name, (*it).Filename);
        }
    }
    songComboBox->setEventHandler(this);
    if (object->Song.notEmpty()) songComboBox->setCurrentText(object->Song);
    songFrame->addChild(songComboBox);
    songFrame->setEnabled(changeSong->checked());
    addChild(songFrame);
    y+=55;

    /*

    addChild(new ppltk::Label(0, y, 120, 30, "max trigger count: "));
    maxTriggerCount=new ppltk::HorizontalSlider(120, y, sw - 120, 30);
    maxTriggerCount->setLimits(0, 1024);
    maxTriggerCount->setValue(object->maxTriggerCount);
    maxTriggerCount->enableSpinBox(true, 1, 80);
    maxTriggerCount->setEventHandler(this);
    addChild(maxTriggerCount);
    y+=35;

    addChild(new ppltk::Label(0, y, 120, 30, "Cooldown: "));
    cooldownUntilNextTrigger=new ppltk::DoubleHorizontalSlider(120, y, 300, 30);
    cooldownUntilNextTrigger->setLimits(0.0f, 120.0f);
    cooldownUntilNextTrigger->setValue(object->cooldownUntilNextTrigger);
    cooldownUntilNextTrigger->enableSpinBox(true, 0.2f, 3, 80);
    cooldownUntilNextTrigger->setEventHandler(this);
    addChild(cooldownUntilNextTrigger);
    addChild(new ppltk::Label(420, y, 120, 30, "seconds"));
    y+=35;
    */

    addChild(new ppltk::Label(0, y, 120, 30, "Transition time: "));
    transitionTime=new ppltk::DoubleHorizontalSlider(120, y, client.width() - 120 - 120, 30);
    transitionTime->setLimits(0.0f, 60.0f);
    transitionTime->setValue(object->transitionTime);
    transitionTime->enableSpinBox(true, 0.1f, 3, 80);
    transitionTime->setEventHandler(this);
    addChild(transitionTime);
    addChild(new ppltk::Label(client.width() - 120, y, 120, 30, "seconds"));
    y+=35;




    //current_state_checkbox->setChecked(object->current_state);
    //initial_state_checkbox->setChecked(object->flags & static_cast<int>(Speaker::Flags::initialStateEnabled));


}

LevelModificatorDialog::~LevelModificatorDialog()
{

}

void LevelModificatorDialog::toggledEvent(ppltk::Event* event, bool checked)
{
    //ppl7::PrintDebugTime("LevelModificatorDialog::toggledEvent\n");
    if (event->widget() == initialStateEnabled) {
        object->initialStateEnabled=checked;
    } else if (event->widget() == currentState) {
        object->enabled=checked;
    } else if (event->widget() == loadLevelDefault) {
        object->loadLevelDefault=checked;
    } else if (event->widget() == changeBackground) {
        object->changeBackground=checked;
        backgroundFrame->setEnabled(checked);
    } else if (event->widget() == changeGlobalLighting) {
        object->changeGlobalLighting=checked;
        globalLightingFrame->setEnabled(checked);
    } else if (event->widget() == changeSong) {
        object->changeSong=checked;
        songFrame->setEnabled(checked);
    } else if (event->widget() == backgroundTypeColor) {
        object->backgroundType = LevelModificator::BackgroundType::Color;
    } else if (event->widget() == backgroundTypeImage) {
        object->backgroundType = LevelModificator::BackgroundType::Image;

    }
}



void LevelModificatorDialog::valueChangedEvent(ppltk::Event* event, double value)
{
    //ppl7::PrintDebugTime("SpeakerDialog::valueChangedEvent (volume): >>%0.3f<<", value);
    if (event->widget() == transitionTime) {
        object->transitionTime=value;
    }
}

void LevelModificatorDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    //ppl7::PrintDebugTime("LevelModificatorDialog::valueChangedEvent int64_t\n");
    if (event->widget() == range_x) {
        object->range.x=(int)value;
    } else if (event->widget() == range_y) {
        object->range.y=(int)value;
    }
}

void LevelModificatorDialog::valueChangedEvent(ppltk::Event* event, int value)
{
    //ppl7::PrintDebugTime("LevelModificatorDialog::valueChangedEvent int\n");
    if (event->widget() == GlobalLighting) {
        object->GlobalLighting=GlobalLighting->color();
    } else if (event->widget() == BackgroundColor) {
        object->BackgroundColor=BackgroundColor->color();
    } else if (event->widget() == backgroundImage) {
        object->BackgroundImage=backgroundImage->currentText();
        if (backgroundImage->currentIndex() == 0) object->BackgroundImage.clear();
    } else if (event->widget() == songComboBox) {
        object->Song=songComboBox->currentText();
        if (songComboBox->currentIndex() == 0) object->Song.clear();
    } else Decker::ui::Dialog::valueChangedEvent(event, value);

}

void LevelModificatorDialog::mouseDownEvent(ppltk::MouseEvent* event)
{
    //ppl7::PrintDebugTime("LevelModificatorDialog::mouseDownEvent\n");
    if (event->widget() == CopyGlobalLightingFromLevel) {
        ppl7::grafix::Color c=GetGame().getLevel().params.GlobalLighting;
        GlobalLighting->setColor(c);
        object->GlobalLighting.setColor(c);
    } else if (event->widget() == CopyBackgroundFromLevel) {
        ppl7::grafix::Color c=GetGame().getLevel().params.BackgroundColor;
        BackgroundColor->setColor(c);
        object->BackgroundColor.setColor(c);
        object->BackgroundImage=GetGame().getLevel().params.BackgroundImage;
        if (GetGame().getLevel().params.backgroundType == Background::Type::Color) {
            object->backgroundType=LevelModificator::BackgroundType::Color;
            backgroundTypeColor->setChecked(true);
            backgroundTypeImage->setChecked(false);
        } else {
            object->backgroundType=LevelModificator::BackgroundType::Image;
            backgroundTypeImage->setChecked(true);
            backgroundTypeColor->setChecked(false);
        }


    } else Decker::ui::Dialog::mouseDownEvent(event);
}

void LevelModificatorDialog::dialogButtonEvent(Dialog::Buttons button)
{
    if (button & Buttons::Reset) {
        object->reset();
    } else if (button & Buttons::Test) {
        object->test();
    }

}


} // namespace Decker::Objects
