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
    this->setWindowIcon(gfx->Toolbar.getDrawable(73));

    ok_button=new ppltk::Button(20, clientarea.height - 30, 70, 30, translate("OK"), gfx->Toolbar.getDrawable(24));
    ok_button->setEventHandler(this);
    cancel_button=new ppltk::Button(clientarea.width - 130, clientarea.height - 30, 120, 30, translate("Cancel"), gfx->Toolbar.getDrawable(25));
    cancel_button->setEventHandler(this);
    this->addChild(ok_button);
    this->addChild(cancel_button);

    tabwidget=new ppltk::TabWidget(0, 0, clientarea.width, clientarea.height - 50);
    this->addChild(tabwidget);
    setupLevelTab();
    setupBackgroundTab();
    setupSoundtrackTab();
    tabwidget->setCurrentIndex(0);
    //ppltk::WindowManager* wm=ppltk::GetWindowManager();
    //wm->setKeyboardFocus(level_name);

}


void LevelDialog::setupLevelTab()
{
    ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
    ppltk::Widget* tab=new ppltk::Widget();
    tabwidget->addTab(tab, "Level", gfx->Toolbar.getDrawable(58));
    ppl7::grafix::Size clientarea=tab->clientSize();
    int y=0;
    int col1=150;

    /*
    tab->addChild(new ppltk::Label(0, 0, 200, 30, "Level Name:"));
    level_name=new ppltk::LineInput(col1, 0, clientarea.width - 210, 30, "no name yet");
    level_name->setEventHandler(this);
    tab->addChild(level_name);
    y+=35;
    */

    tab->addChild(new ppltk::Label(0, y, 200, 30, "Level Size:"));
    level_pixel_size=new ppltk::Label(col1 + 60 + 80 + 60 + 80 + 60, y, 200, 30, "= ? x ? pixel");
    tab->addChild(level_pixel_size);
    tab->addChild(new ppltk::Label(col1, y, 60, 30, "width:"));
    level_width=new ppltk::SpinBox(col1 + 60, y, 80, 30, 512);
    level_width->setEventHandler(this);
    level_width->setLimits(64, 8192);
    level_width->setStepSize(16);
    tab->addChild(level_width);
    tab->addChild(new ppltk::Label(col1 + 60 + 80, y, 60, 30, ", height:"));
    level_height=new ppltk::SpinBox(col1 + 60 + 80 + 60, y, 80, 30, 256);
    level_height->setLimits(64, 8192);
    level_height->setStepSize(16);
    level_height->setEventHandler(this);
    tab->addChild(level_height);
    tab->addChild(new ppltk::Label(col1 + 60 + 80 + 60 + 80, y, 60, 30, "Studs"));
    level_width->setValue(512);
    level_height->setValue(384);
    y+=35;
    // TODO: Flags + Description
    level_is_listed=new ppltk::CheckBox(0, y, 400, 30, "level is visible in level selection");
    tab->addChild(level_is_listed);
    y+=35;

    part_of_story=new ppltk::CheckBox(0, y, 200, 30, "level is part of story");
    tab->addChild(part_of_story);
    tab->addChild(new ppltk::Label(200, y, col1, 30, "Level sort:"));
    LevelSort=new ppltk::SpinBox(300, y, 100, 30, 0);
    LevelSort->setLimits(0, 65535);
    LevelSort->setEventHandler(this);
    tab->addChild(LevelSort);
    y+=35;

    tab->addChild(new ppltk::Label(0, y, 100, 30, "Thumbnail:"));
    thumbnail=new ppltk::Label(100, y, 320, 180, "", ppltk::Label::BorderStyle::Inset);
    thumbnail->setEventHandler(this);
    tab->addChild(thumbnail);

    thumb_take_screenshot=new ppltk::Button(430, y + 0, 200, 30, "take screenshot", gfx->Toolbar.getDrawable(2));
    thumb_to_clipboard=new ppltk::Button(430, y + 30, 200, 30, "copy to clipboard", gfx->Toolbar.getDrawable(37));
    thumb_from_clipboard=new ppltk::Button(430, y + 60, 200, 30, "copy from clipboard", gfx->Toolbar.getDrawable(38));
    thumb_load=new ppltk::Button(430, y + 90, 200, 30, "load", gfx->Toolbar.getDrawable(32));
    thumb_save=new ppltk::Button(430, y + 120, 200, 30, "save", gfx->Toolbar.getDrawable(33));

    thumb_take_screenshot->setEventHandler(this);
    thumb_to_clipboard->setEventHandler(this);
    thumb_from_clipboard->setEventHandler(this);
    thumb_load->setEventHandler(this);
    thumb_save->setEventHandler(this);
    tab->addChild(thumb_take_screenshot);
    tab->addChild(thumb_to_clipboard);
    tab->addChild(thumb_from_clipboard);
    tab->addChild(thumb_load);
    tab->addChild(thumb_save);
    y+=190;

    tstrings=new ppltk::TabWidget(0, y, clientarea.width, clientarea.height - y);
    tab->addChild(tstrings);
    Translator translator=GetTranslator();
    for (auto it=translator.languages.begin();it != translator.languages.end();++it) {
        ppltk::Widget* langtab=new ppltk::Widget();
        tstrings->addTab(langtab, it->first);
        ppl7::grafix::Size client=langtab->clientSize();
        int yy=0;
        langtab->addChild(new ppltk::Label(0, yy, 100, 30, "Level Name:"));
        ppltk::LineInput* name=new ppltk::LineInput(100, 0, client.width - 100, 30, "no name yet");
        name->setEventHandler(this);
        langtab->addChild(name);
        yy+=35;

        langtab->addChild(new ppltk::Label(0, yy, 100, 30, "Description:"));
        ppltk::TextEdit* descr=new ppltk::TextEdit(100, yy, client.width - 100, client.height - yy, "");
        descr->setEventHandler(this);
        langtab->addChild(descr);
        yy+=35;

    }

}


void LevelDialog::setupBackgroundTab()
{
    ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
    ppltk::Widget* tab=new ppltk::Widget();

    tabwidget->addTab(tab, "Background", gfx->Toolbar.getDrawable(57));
    ppl7::grafix::Size clientarea=tab->clientSize();
    int y1=0;
    int col1=90;
    //this->addChild(new ppltk::Label(0, y, 200, 30, "Background:"));
    //level_background_frame=new ppltk::Frame(col1, y, width() - col1 - 20, 35 * 5 + 20);
    //this->addChild(level_background_frame);

    // Image
    radio_image=new ppltk::RadioButton(0, y1, 80, 30, "Image:");
    radio_image->setEventHandler(this);
    tab->addChild(radio_image);
    background_image=new ppltk::ComboBox(col1, y1, clientarea.width - col1 - 30, 30);
    tab->addChild(background_image);
    background_image->clear();
    Resources& res=getResources();
    std::list<ppl7::String>::const_iterator it;
    background_image->add("no image", "");
    for (it=res.background_images.begin();it != res.background_images.end();++it) {
        background_image->add((*it), (*it));
    }
    y1+=35;

    // Color
    radio_color=new ppltk::RadioButton(0, y1, 80, 30, "Color:");
    radio_color->setEventHandler(this);
    tab->addChild(radio_color);
    int col2=col1 + 50;
    int col3=col2 + 80;
    int col4=col3 + 320;
    color_preview=new ppltk::Frame(col4, y1, 3 * 35, 3 * 35);
    color_preview->setBackgroundColor(ppl7::grafix::Color(0, 0, 0, 255));
    tab->addChild(color_preview);

    // red
    tab->addChild(new ppltk::Label(col1, y1, 50, 30, "red"));
    color_red=new ppltk::SpinBox(col2, y1, 70, 30, 0);
    color_red->setLimits(0, 255);
    color_red->setEventHandler(this);
    tab->addChild(color_red);
    slider_red=new ppltk::HorizontalSlider(col3, y1, 300, 30);
    slider_red->setLimits(0, 255);
    slider_red->setEventHandler(this);
    tab->addChild(slider_red);
    y1+=35;

    // green
    tab->addChild(new ppltk::Label(col1, y1, 50, 30, "green"));
    color_green=new ppltk::SpinBox(col2, y1, 70, 30, 0);
    color_green->setLimits(0, 255);
    color_green->setEventHandler(this);
    tab->addChild(color_green);
    slider_green=new ppltk::HorizontalSlider(col3, y1, 300, 30);
    slider_green->setLimits(0, 255);
    slider_green->setEventHandler(this);
    tab->addChild(slider_green);
    y1+=35;

    // blue
    tab->addChild(new ppltk::Label(col1, y1, 50, 30, "blue"));
    color_blue=new ppltk::SpinBox(col2, y1, 70, 30, 0);
    color_blue->setLimits(0, 255);
    color_blue->setEventHandler(this);
    tab->addChild(color_blue);
    slider_blue=new ppltk::HorizontalSlider(col3, y1, 300, 30);
    slider_blue->setLimits(0, 255);
    slider_blue->setEventHandler(this);
    tab->addChild(slider_blue);
    y1+=35;

}

void LevelDialog::setupSoundtrackTab()
{
    ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
    ppltk::Widget* tab=new ppltk::Widget();
    tabwidget->addTab(tab, "Soundtrack", gfx->Toolbar.getDrawable(55));
    ppl7::grafix::Size clientarea=tab->clientSize();

    //this->addChild(new ppltk::Label(0, y, 200, 30, "Soundtrack:"));
    //level_soundtrack_frame=new ppltk::Frame(col1, y, width() - col1 - 20, 35 * 7 + 10);
    //this->addChild(level_soundtrack_frame);
    int y2=0;
    int c2=120;
    int w=clientarea.width - c2 - 35;
    int x3=clientarea.width - 35;
    tab->addChild(new ppltk::Label(0, y2, c2, 30, "primary:"));
    base_soundtrack=new ppltk::ComboBox(c2, y2, w, 30);
    tab->addChild(base_soundtrack);
    y2+=35;
    tab->addChild(new ppltk::Label(0, y2, c2, 30, "additional:"));
    additional_soundtrack=new ppltk::ComboBox(c2, y2, w, 30);
    tab->addChild(additional_soundtrack);
    add_soundtrack_button=new ppltk::Button(x3, y2, 30, 30, "", gfx->Toolbar.getDrawable(43));
    add_soundtrack_button->setEventHandler(this);
    tab->addChild(add_soundtrack_button);
    delete_soundtrack_button=new ppltk::Button(x3, y2 + 35 * 4, 30, 30, "", gfx->Toolbar.getDrawable(44));
    delete_soundtrack_button->setEventHandler(this);
    tab->addChild(delete_soundtrack_button);
    soundtrack_list=new ppltk::ListWidget(c2, y2 + 35, w, 35 * 4);
    tab->addChild(soundtrack_list);

    soundtrack_random=new ppltk::CheckBox(c2, y2 + 35 * 5, 300, 30, "play random");
    tab->addChild(soundtrack_random);

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
    //level_name->setText(params.Name);

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
    //params.Name=level_name->text();

    // Background
    if (radio_color->checked()) params.backgroundType=Background::Type::Color;
    if (radio_image->checked()) params.backgroundType=Background::Type::Image;
    params.BackgroundImage=background_image->currentIdentifier();
    params.BackgroundColor=ppl7::grafix::Color(color_red->value(), color_green->value(), color_blue->value(), 255);

    // Soundtrack
    params.InitialSong=base_soundtrack->currentIdentifier();
    params.randomSong=soundtrack_random->checked();
    params.SongPlaylist.clear();
    const std::list<ppltk::ListWidget::ListWidgetItem>& songs=soundtrack_list->getItems();
    std::list<ppltk::ListWidget::ListWidgetItem>::const_iterator it;
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


void LevelDialog::mouseClickEvent(ppltk::MouseEvent* event)
{
    if (event->widget() == ok_button) {
        my_state=DialogState::OK;
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        EventHandler::closeEvent(&event);
        return;
    } else if (event->widget() == cancel_button) {
        if (game) {
            game->getLevel().params.BackgroundColor=previous_background;
        }
        my_state=DialogState::Aborted;
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        EventHandler::closeEvent(&event);
        return;
    }
    Dialog::mouseClickEvent(event);
}

void LevelDialog::mouseDownEvent(ppltk::MouseEvent* event)
{
    ppltk::Widget* w=event->widget();
    if (w == add_soundtrack_button) {
        soundtrack_list->add(additional_soundtrack->currentText(),
            additional_soundtrack->currentIdentifier());
    } else if (w == delete_soundtrack_button) {
        soundtrack_list->remove(soundtrack_list->currentIdentifier());
    }
    Dialog::mouseDownEvent(event);
}

void LevelDialog::closeEvent(ppltk::Event* event)
{
    if (game) {
        game->getLevel().params.BackgroundColor=previous_background;
    }
    my_state=DialogState::Aborted;
    ppltk::Event ev(ppltk::Event::Close);
    ev.setWidget(this);
    EventHandler::closeEvent(&ev);
}



void LevelDialog::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    ppltk::Widget* widget=event->widget();

    if (widget == slider_red) {
        if (color_red && color_red->value() != slider_red->value()) color_red->setValue(slider_red->value());
        updateColorPreview();
    } else if (widget == slider_green) {
        if (color_green && color_green->value() != slider_green->value()) color_green->setValue(slider_green->value());
        updateColorPreview();
    } else if (widget == slider_blue) {
        if (color_blue && color_blue->value() != slider_blue->value()) color_blue->setValue(slider_blue->value());
        updateColorPreview();
    } else if (widget == color_red) {
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

void LevelDialog::textChangedEvent(ppltk::Event* event, const ppl7::String& text)
{
    ppltk::Widget* widget=event->widget();
    if (level_width != NULL && level_width != NULL) {
        if (widget == level_width || widget == level_height) {
            level_pixel_size->setText(ppl7::ToString("= %d x %d pixel",
                level_width->value() * TILE_WIDTH,
                level_height->value() * TILE_HEIGHT
            ));
        }
    }


}

void LevelDialog::keyDownEvent(ppltk::KeyEvent* event)
{
    //printf("keyDownEvent: %d, modifier: %04x\n", event->key, event->modifier);
#ifdef TODO1
    ppltk::WindowManager* wm=ppltk::GetWindowManager();
    ppltk::Widget* widget=event->widget();

    if ((event->key == ppltk::KeyEvent::KEY_TAB || event->key == ppltk::KeyEvent::KEY_RETURN)
        && (event->modifier & ppltk::KeyEvent::KEYMOD_SHIFT) == 0) {
           // Tab forward
        //if (widget == level_name) wm->setKeyboardFocus(level_width);
        if (widget == level_width) wm->setKeyboardFocus(level_height);
        else if (widget == level_height) wm->setKeyboardFocus(color_red);
        else if (widget == color_red) wm->setKeyboardFocus(color_green);
        else if (widget == color_green) wm->setKeyboardFocus(color_blue);

    } else if ((event->key == ppltk::KeyEvent::KEY_TAB || event->key == ppltk::KeyEvent::KEY_RETURN)
        && (event->modifier & ppltk::KeyEvent::KEYMOD_SHIFT) != 0) {
           // Tab backward
        if (widget == level_height) wm->setKeyboardFocus(level_width);
        //else if (widget == level_width) wm->setKeyboardFocus(level_name);
        else if (widget == color_blue) wm->setKeyboardFocus(color_green);
        else if (widget == color_green) wm->setKeyboardFocus(color_red);
        else if (widget == color_red) wm->setKeyboardFocus(level_height);

    }
#endif
}

}   // Decker::ui
