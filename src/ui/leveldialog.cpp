#include <stdio.h>
#include <stdlib.h>

#include "ui.h"
#include "translate.h"
#include "decker.h"
#include "audiopool.h"

namespace Decker::ui {



LevelDialog::LevelDialog(int width, int height)
    : Dialog(width, height, Dialog::None)
{
    my_state=DialogState::Open;
    this->setWindowTitle("edit level parameters");

    color_red=NULL;
    color_green=NULL;
    color_blue=NULL;
    slider_red=NULL;
    slider_green=NULL;
    slider_blue=NULL;
    color_preview=NULL;
    game=NULL;
    setupUi();
}

void LevelDialog::setupUi()
{
    destroyChilds();
    ppl7::grafix::Size clientarea=this->clientSize();
    ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();

    ok_button=new ppl7::tk::Button(20, clientarea.height - 40, 70, 30, translate("OK"), gfx->Toolbar.getDrawable(24));
    ok_button->setEventHandler(this);
    cancel_button=new ppl7::tk::Button(clientarea.width - 110, clientarea.height - 40, 90, 30, translate("Cancel"), gfx->Toolbar.getDrawable(25));
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
    level_width=new ppl7::tk::SpinBox(col1 + 60, y, 80, 30, 512);
    level_width->setEventHandler(this);
    level_width->setLimits(64, 8192);
    level_width->setStepSize(16);
    this->addChild(level_width);
    this->addChild(new ppl7::tk::Label(col1 + 60 + 80, y, 60, 30, ", height:"));
    level_height=new ppl7::tk::SpinBox(col1 + 60 + 80 + 60, y, 80, 30, 256);
    level_height->setLimits(64, 8192);
    level_height->setStepSize(16);
    level_height->setEventHandler(this);
    this->addChild(level_height);
    this->addChild(new ppl7::tk::Label(col1 + 60 + 80 + 60 + 80, y, 60, 30, "Studs"));
    level_width->setValue(512);
    level_height->setValue(384);
    y+=35;

    {   // Background
        int y1=0;
        col1=90;
        this->addChild(new ppl7::tk::Label(0, y, 200, 30, "Background:"));
        level_background_frame=new ppl7::tk::Frame(col1, y, width() - col1 - 20, 35 * 5 + 20);
        this->addChild(level_background_frame);

        // Image
        radio_image=new ppl7::tk::RadioButton(0, y1, 80, 30, "Image:");
        radio_image->setEventHandler(this);
        level_background_frame->addChild(radio_image);
        background_image=new ppl7::tk::ComboBox(col1, y1, level_background_frame->width() - col1 - 30, 30);
        level_background_frame->addChild(background_image);
        background_image->clear();
        Resources& res=getResources();
        std::list<ppl7::String>::const_iterator it;
        background_image->add("no image", "");
        for (it=res.background_images.begin();it != res.background_images.end();++it) {
            background_image->add((*it), (*it));
        }
        y1+=35;

        // Color
        radio_color=new ppl7::tk::RadioButton(0, y1, 80, 30, "Color:");
        radio_color->setEventHandler(this);
        level_background_frame->addChild(radio_color);
        int col2=col1 + 50;
        int col3=col2 + 80;
        int col4=col3 + 320;
        color_preview=new ppl7::tk::Frame(col4, y1, 3 * 35, 3 * 35);
        color_preview->setBackgroundColor(ppl7::grafix::Color(0, 0, 0, 255));
        level_background_frame->addChild(color_preview);

        // red
        level_background_frame->addChild(new ppl7::tk::Label(col1, y1, 50, 30, "red"));
        color_red=new ppl7::tk::SpinBox(col2, y1, 70, 30, 0);
        color_red->setLimits(0, 255);
        color_red->setEventHandler(this);
        level_background_frame->addChild(color_red);
        slider_red=new ppl7::tk::HorizontalSlider(col3, y1, 300, 30);
        slider_red->setDimension(0, 255);
        slider_red->setEventHandler(this);
        level_background_frame->addChild(slider_red);
        y1+=35;

        // green
        level_background_frame->addChild(new ppl7::tk::Label(col1, y1, 50, 30, "green"));
        color_green=new ppl7::tk::SpinBox(col2, y1, 70, 30, 0);
        color_green->setLimits(0, 255);
        color_green->setEventHandler(this);
        level_background_frame->addChild(color_green);
        slider_green=new ppl7::tk::HorizontalSlider(col3, y1, 300, 30);
        slider_green->setDimension(0, 255);
        slider_green->setEventHandler(this);
        level_background_frame->addChild(slider_green);
        y1+=35;

        // blue
        level_background_frame->addChild(new ppl7::tk::Label(col1, y1, 50, 30, "blue"));
        color_blue=new ppl7::tk::SpinBox(col2, y1, 70, 30, 0);
        color_blue->setLimits(0, 255);
        color_blue->setEventHandler(this);
        level_background_frame->addChild(color_blue);
        slider_blue=new ppl7::tk::HorizontalSlider(col3, y1, 300, 30);
        slider_blue->setDimension(0, 255);
        slider_blue->setEventHandler(this);
        level_background_frame->addChild(slider_blue);
        y1+=35;
    }

    y+=35 * 5 + 25;
    {   // Soundtrack
        this->addChild(new ppl7::tk::Label(0, y, 200, 30, "Soundtrack:"));
        level_soundtrack_frame=new ppl7::tk::Frame(col1, y, width() - col1 - 20, 35 * 7 + 10);
        this->addChild(level_soundtrack_frame);
        int y2=0;
        int c2=120;
        int w=level_soundtrack_frame->clientRect().width() - c2 - 35;
        int x3=level_soundtrack_frame->clientRect().width() - 35;
        level_soundtrack_frame->addChild(new ppl7::tk::Label(0, y2, c2, 30, "primary:"));
        base_soundtrack=new ppl7::tk::ComboBox(c2, y2, w, 30);
        level_soundtrack_frame->addChild(base_soundtrack);
        y2+=35;
        level_soundtrack_frame->addChild(new ppl7::tk::Label(0, y2, c2, 30, "additional:"));
        additional_soundtrack=new ppl7::tk::ComboBox(c2, y2, w, 30);
        level_soundtrack_frame->addChild(additional_soundtrack);
        add_soundtrack_button=new ppl7::tk::Button(x3, y2, 30, 30, "", gfx->Toolbar.getDrawable(43));
        add_soundtrack_button->setEventHandler(this);
        level_soundtrack_frame->addChild(add_soundtrack_button);
        delete_soundtrack_button=new ppl7::tk::Button(x3, y2 + 35 * 4, 30, 30, "", gfx->Toolbar.getDrawable(44));
        delete_soundtrack_button->setEventHandler(this);
        level_soundtrack_frame->addChild(delete_soundtrack_button);
        soundtrack_list=new ppl7::tk::ListWidget(c2, y2 + 35, w, 35 * 4);
        level_soundtrack_frame->addChild(soundtrack_list);

        soundtrack_random=new ppl7::tk::CheckBox(c2, y2 + 35 * 5, 300, 30, "play random");
        level_soundtrack_frame->addChild(soundtrack_random);

        AudioPool& pool=getAudioPool();
        std::list<MusicTrack>::const_iterator it;
        base_soundtrack->add("no song", "");
        song_map_identifier.clear();
        for (it=pool.musictracks.begin();it != pool.musictracks.end();++it) {
            base_soundtrack->add((*it).Name, (*it).Filename);
            additional_soundtrack->add((*it).Name, (*it).Filename);
            song_map_identifier.insert(std::pair<ppl7::String, ppl7::String>((*it).Filename, (*it).Name));
        }
    }
    y+=35 * 7 + 15;

    ppl7::tk::WindowManager* wm=ppl7::tk::GetWindowManager();
    wm->setKeyboardFocus(level_name);

}

void LevelDialog::setGame(Game* game)
{
    this->game=game;
    previous_background=game->getLevel().params.BackgroundColor;
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
    if (newlevel) {
        setWindowTitle("create new level");
        LevelParameter default_params;
        loadValues(default_params);

    }
}

bool LevelDialog::isNewLevel() const
{
    return newlevel;
}

void LevelDialog::loadValues(const LevelParameter& params)
{
    level_width->setValue(params.width);
    level_height->setValue(params.height);
    level_name->setText(params.Name);

    // Background
    if (params.backgroundType == Background::Type::Color) radio_color->setChecked(true);
    else if (params.backgroundType == Background::Type::Image) radio_image->setChecked(true);

    ppl7::grafix::Color bg=params.BackgroundColor;
    color_red->setValue(bg.red());
    color_green->setValue(bg.green());
    color_blue->setValue(bg.blue());
    background_image->setCurrentIdentifier(params.BackgroundImage);

    // Soundtrack
    if (params.InitialSong.notEmpty())base_soundtrack->setCurrentIdentifier(params.InitialSong);
    soundtrack_random->setChecked(params.randomSong);
    std::vector<ppl7::String>::const_iterator it;
    for (it=params.SongPlaylist.begin();it != params.SongPlaylist.end();++it) {
        std::map<ppl7::String, ppl7::String>::const_iterator sit;
        sit=song_map_identifier.find((*it));
        if (sit != song_map_identifier.end()) {
            soundtrack_list->add(sit->second, sit->first);
        }
    }

}

void LevelDialog::saveValues(LevelParameter& params) const
{
    params.width=level_width->value();
    params.height=level_height->value();
    params.Name=level_name->text();

    // Background
    if (radio_color->checked()) params.backgroundType=Background::Type::Color;
    if (radio_image->checked()) params.backgroundType=Background::Type::Image;
    params.BackgroundImage=background_image->currentIdentifier();
    params.BackgroundColor=ppl7::grafix::Color(color_red->value(), color_green->value(), color_blue->value(), 255);

    // Soundtrack
    params.InitialSong=base_soundtrack->currentIdentifier();
    params.randomSong=soundtrack_random->checked();
    params.SongPlaylist.clear();
    const std::list<ppl7::tk::ListWidget::ListWidgetItem>& songs=soundtrack_list->getItems();
    std::list<ppl7::tk::ListWidget::ListWidgetItem>::const_iterator it;
    for (it=songs.begin();it != songs.end();++it) {
        params.SongPlaylist.push_back((*it).identifier);
    }

}

void LevelDialog::updateColorPreview()
{
    if (color_preview && color_red && color_green && color_blue) {
        color_preview->setBackgroundColor(ppl7::grafix::Color(color_red->value(), color_green->value(), color_blue->value(), 255));
        if (game) game->getLevel().params.BackgroundColor=color_preview->backgroundColor();
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
        if (game) {
            game->getLevel().params.BackgroundColor=previous_background;
        }
        my_state=DialogState::Aborted;
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        EventHandler::closeEvent(&event);
        return;
    }
    Dialog::mouseClickEvent(event);
}

void LevelDialog::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
    ppl7::tk::Widget* w=event->widget();
    if (w == add_soundtrack_button) {
        soundtrack_list->add(additional_soundtrack->currentText(),
            additional_soundtrack->currentIdentifier());
    } else if (w == delete_soundtrack_button) {
        soundtrack_list->remove(soundtrack_list->currentIdentifier());
    }
}

void LevelDialog::valueChangedEvent(ppl7::tk::Event* event, int value)
{
    ppl7::tk::Widget* widget=event->widget();

    if (widget == slider_red) {
        if (color_red && color_red->value() != slider_red->value()) color_red->setValue(slider_red->value());
        updateColorPreview();
    } else if (widget == slider_green) {
        if (color_green && color_green->value() != slider_green->value()) color_green->setValue(slider_green->value());
        updateColorPreview();
    } else if (widget == slider_blue) {
        if (color_blue && color_blue->value() != slider_blue->value()) color_blue->setValue(slider_blue->value());
        updateColorPreview();
    }

}

void LevelDialog::textChangedEvent(ppl7::tk::Event* event, const ppl7::String& text)
{
    ppl7::tk::Widget* widget=event->widget();
    if (level_width != NULL && level_width != NULL) {
        if (widget == level_width || widget == level_height) {
            level_pixel_size->setText(ppl7::ToString("= %d x %d pixel",
                level_width->value() * TILE_WIDTH,
                level_height->value() * TILE_HEIGHT
            ));
        }
    }
    if (widget == color_red) {
        if (slider_red && slider_red->value() != color_red->value()) slider_red->setValue(color_red->value());
        updateColorPreview();
    } else if (widget == color_green) {
        if (slider_green && slider_green->value() != color_green->value()) slider_green->setValue(color_green->value());
        updateColorPreview();
    } else if (widget == color_blue) {
        if (slider_blue && slider_blue->value() != color_blue->value()) slider_blue->setValue(color_blue->value());
        updateColorPreview();
    }

}

void LevelDialog::keyDownEvent(ppl7::tk::KeyEvent* event)
{
    //printf("keyDownEvent: %d, modifier: %04x\n", event->key, event->modifier);
    ppl7::tk::WindowManager* wm=ppl7::tk::GetWindowManager();
    ppl7::tk::Widget* widget=event->widget();
    if ((event->key == ppl7::tk::KeyEvent::KEY_TAB || event->key == ppl7::tk::KeyEvent::KEY_RETURN)
        && (event->modifier & ppl7::tk::KeyEvent::KEYMOD_SHIFT) == 0) {
           // Tab forward
        if (widget == level_name) wm->setKeyboardFocus(level_width);
        else if (widget == level_width) wm->setKeyboardFocus(level_height);
        else if (widget == level_height) wm->setKeyboardFocus(color_red);
        else if (widget == color_red) wm->setKeyboardFocus(color_green);
        else if (widget == color_green) wm->setKeyboardFocus(color_blue);

    } else if ((event->key == ppl7::tk::KeyEvent::KEY_TAB || event->key == ppl7::tk::KeyEvent::KEY_RETURN)
        && (event->modifier & ppl7::tk::KeyEvent::KEYMOD_SHIFT) != 0) {
           // Tab backward
        if (widget == level_height) wm->setKeyboardFocus(level_width);
        else if (widget == level_width) wm->setKeyboardFocus(level_name);
        else if (widget == color_blue) wm->setKeyboardFocus(color_green);
        else if (widget == color_green) wm->setKeyboardFocus(color_red);
        else if (widget == color_red) wm->setKeyboardFocus(level_height);

    }
}

}   // Decker::ui
