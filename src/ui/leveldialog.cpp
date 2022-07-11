#include <stdio.h>
#include <stdlib.h>

#include "ui.h"
#include "translate.h"

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

}

ppl7::String LevelDialog::widgetType() const
{
    return "LevelDialog";
}

LevelDialog::DialogState LevelDialog::state() const
{
    return my_state;
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

}   // Decker::ui
