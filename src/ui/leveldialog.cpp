#include <stdio.h>
#include <stdlib.h>

#include "ui.h"
#include "translate.h"
#include "decker.h"

namespace Decker::ui {



LevelDialog::LevelDialog(int width, int height)
    : Dialog(width, height, Dialog::None)
{
    my_state=DialogState::Open;
    setupUi();
}

void LevelDialog::setupUi()
{
    destroyChilds();
    ppl7::grafix::Size clientarea=this->clientSize();
    ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();

    ok_button=new ppl7::tk::Button(20, clientarea.height - 40, 200, 30, translate("OK"), gfx->Toolbar.getDrawable(24));
    ok_button->setEventHandler(this);
    cancel_button=new ppl7::tk::Button(clientarea.width - 220, clientarea.height - 40, 200, 30, translate("Cancel"), gfx->Toolbar.getDrawable(25));
    cancel_button->setEventHandler(this);
    this->addChild(ok_button);
    this->addChild(cancel_button);
    int y=0;
    int col1=150;

    this->addChild(new ppl7::tk::Label(0, 0, 200, 30, "Level Name:"));
    level_name=new ppl7::tk::LineInput(col1, 0, clientarea.width - 210, 30, "no name yet");
    level_name->setEventHandler(this);
    this->addChild(level_name);
    y+=35;

    this->addChild(new ppl7::tk::Label(0, y, 200, 30, "Level Size:"));
    level_pixel_size=new ppl7::tk::Label(col1 + 60 + 80 + 60 + 80 + 60, y, 200, 30, "= ? x ? pixel");
    this->addChild(level_pixel_size);
    this->addChild(new ppl7::tk::Label(col1, y, 60, 30, "width:"));
    level_width=new ppl7::tk::LineInput(col1 + 60, y, 80, 30, "512");
    level_width->setEventHandler(this);
    this->addChild(level_width);
    this->addChild(new ppl7::tk::Label(col1 + 60 + 80, y, 60, 30, ", height:"));
    level_height=new ppl7::tk::LineInput(col1 + 60 + 80 + 60, y, 80, 30, "256");
    level_height->setEventHandler(this);
    this->addChild(level_height);
    this->addChild(new ppl7::tk::Label(col1 + 60 + 80 + 60 + 80, y, 60, 30, "Studs"));
    y+=35;

    ppl7::tk::WindowManager* wm=ppl7::tk::GetWindowManager();
    wm->setKeyboardFocus(level_name);

}

ppl7::String LevelDialog::widgetType() const
{
    return "LevelDialog";
}

LevelDialog::DialogState LevelDialog::state() const
{
    return my_state;
}

void LevelDialog::setNewLevelFlag(bool newlevel)
{
    this->newlevel=newlevel;
    if (level_width != NULL && level_height != NULL) {
        level_width->setEnabled(newlevel);
        level_height->setEnabled(newlevel);
    }
}


void LevelDialog::mouseClickEvent(ppl7::tk::MouseEvent* event)
{
    if (event->widget() == ok_button) {
        my_state=DialogState::OK;
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        EventHandler::closeEvent(&event);
        return;
    } else if (event->widget() == cancel_button) {
        my_state=DialogState::Aborted;
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        EventHandler::closeEvent(&event);
        return;
    }
    Dialog::mouseClickEvent(event);
}

void LevelDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{

}

void LevelDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
    ppl7::tk::Widget* widget=event->widget();
    if (widget == level_width || widget == level_height) {
        level_pixel_size->setText(ppl7::ToString("= %d x %d pixel",
            level_width->text().toInt() * TILE_WIDTH,
            level_height->text().toInt() * TILE_HEIGHT
        ));
    }
}

void LevelDialog::keyDownEvent(ppl7::tk::KeyEvent* event)
{
    printf("keyDownEvent: %d, modifier: %04x\n", event->key, event->modifier);
    ppl7::tk::WindowManager* wm=ppl7::tk::GetWindowManager();
    ppl7::tk::Widget* widget=event->widget();
    if ((event->key == ppl7::tk::KeyEvent::KEY_TAB || event->key == ppl7::tk::KeyEvent::KEY_RETURN)
        && (event->modifier & ppl7::tk::KeyEvent::KEYMOD_SHIFT) == 0) {
           // Tab forward
        if (widget == level_name) wm->setKeyboardFocus(level_width);
        else if (widget == level_width) wm->setKeyboardFocus(level_height);

    } else if ((event->key == ppl7::tk::KeyEvent::KEY_TAB || event->key == ppl7::tk::KeyEvent::KEY_RETURN)
        && (event->modifier & ppl7::tk::KeyEvent::KEYMOD_SHIFT) != 0) {
           // Tab backward
        if (widget == level_height) wm->setKeyboardFocus(level_width);
        else if (widget == level_width) wm->setKeyboardFocus(level_name);

    }
}

}   // Decker::ui
