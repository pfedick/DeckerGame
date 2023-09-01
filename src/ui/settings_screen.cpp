#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"


SettingsScreen::SettingsScreen(Game& g, int x, int y, int width, int height, bool ingame)
    : game(g)
{
    create(x, y, width, height);
    select_menue=NULL;
    select_game_exit=NULL;
    this->ingame=ingame;
    this->setName("SettingsScreen");
    setupUi();
    selectSettingsPage(SettingsMenue::Audio);
}

void SettingsScreen::updateStyles()
{
    const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
    ppl7::grafix::Font font=style.buttonFont;
    font.setName("NotoSansBlack");
    font.setBold(false);
    font.setSize(12);
    font.setOrientation(ppl7::grafix::Font::TOP);
    font.setColor(ppl7::grafix::Color(255, 255, 255, 255));

    style_heading.font=font;
    style_heading.font.setSize(50);
    style_heading.size.setSize(300, 90);
    style_heading.total.setSize(300, 100);


    style_label.font=font;
    style_label.font.setSize(20);

    style_menue.font=font;
    style_menue.font.setSize(30);
    style_menue.total.setSize(300, 100);
    style_menue.size.setSize(300, 90);
    style_menue.border_width=10;

    settings_page.setPoint(340, 100);

    input_widget_x=260;
    input_widget.setSize(400, 40);

    if (height() < 600 || width() < 1280) {
        style_heading.font.setSize(40);
        style_heading.size.setSize(300, 70);
        style_heading.total.setSize(300, 80);

        style_label.font.setSize(16);

        style_menue.font.setSize(20);
        style_menue.total.setSize(240, 80);
        style_menue.size.setSize(240, 70);
        style_menue.border_width=7;

        settings_page.setPoint(260, 80);

        input_widget_x=200;
        input_widget.setSize(320, 32);
    }
    if (height() < 500 || width() < 1024) {
        style_heading.font.setSize(30);
        style_heading.size.setSize(300, 40);
        style_heading.total.setSize(300, 50);

        style_label.font.setName("NotoSans");
        style_label.font.setSize(12);
        style_label.font.setBold(true);

        style_menue.font.setSize(16);
        style_menue.total.setSize(200, 60);
        style_menue.size.setSize(200, 50);
        style_menue.border_width=5;

        settings_page.setPoint(210, 50);

        input_widget_x=150;
        input_widget.setSize(260, 26);


    }
}

void SettingsScreen::setupUi()
{
    //printf("size: %d x %d\n", width(), height());
    this->destroyChilds();
    updateStyles();
    ppl7::tk::Label* label;
    ppl7::String text=translate("Settings");
    label=new ppl7::tk::Label(10, 10, style_heading.size.width, style_heading.size.height, text);
    label->setFont(style_heading.font);
    ppl7::grafix::Size size=style_heading.font.measure(text);
    label->setPos((width() - size.width) / 2, 10);
    label->setWidth(size.width + 20);
    this->addChild(label);

    ppl7::grafix::Color background(20, 10, 0, 192);
    menue=new ppl7::tk::Frame(0, style_heading.total.height, style_menue.size.width + 20, height() - style_heading.total.height, ppl7::tk::Frame::BorderStyle::NoBorder);
    menue->setBackgroundColor(background);
    menue->setName("Settings Menue");
    menue->setEventHandler(this);
    this->addChild(menue);

    int y=0;
    select_audio=new GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Audio"));
    select_audio->setName("Settings Selection Audio");
    select_audio->setFontSize(style_menue.font.size());
    select_audio->setBorderWidth(style_menue.border_width);
    select_audio->setEventHandler(this);
    menue->addChild(select_audio);
    y+=style_menue.total.height;

    select_video=new GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Video"));
    select_video->setName("Settings Selection Video");
    select_video->setFontSize(style_menue.font.size());
    select_video->setBorderWidth(style_menue.border_width);
    select_video->setEventHandler(this);
    menue->addChild(select_video);
    y+=style_menue.total.height;

    select_misc=new GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Misc"));
    select_misc->setName("Settings Selection Misc");
    select_misc->setFontSize(style_menue.font.size());
    select_misc->setBorderWidth(style_menue.border_width);
    select_misc->setEventHandler(this);
    menue->addChild(select_misc);
    y+=style_menue.total.height;


    if (ingame) {
        select_menue=new GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Menue"));
        select_menue->setName("Settings Back to Menue");
        select_menue->setFontSize(style_menue.font.size());
        select_menue->setBorderWidth(style_menue.border_width);
        select_menue->setEventHandler(this);
        menue->addChild(select_menue);
        y+=style_menue.total.height;

        select_game_exit=new GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Exit Game"));
        select_game_exit->setName("Settings Exit Game");
        select_game_exit->setFontSize(style_menue.font.size());
        select_game_exit->setBorderWidth(style_menue.border_width);
        select_game_exit->setEventHandler(this);
        menue->addChild(select_game_exit);
        y+=style_menue.total.height;
    }

    select_back=new GameMenuArea(10, menue->height() - style_menue.total.height, style_menue.size.width, style_menue.size.height, translate("back (ESC)"));
    select_back->setName("Settings Selection Back");
    select_back->setFontSize(style_menue.font.size());
    select_back->setBorderWidth(style_menue.border_width);
    select_back->setEventHandler(this);
    menue->addChild(select_back);

    ppl7::tk::GetWindowManager()->setKeyboardFocus(menue);

    page_audio=NULL;
    page_video=NULL;
    page_misc=NULL;
    initPageAudio();
    initPageVideo();
    initPageMisc();


}

void SettingsScreen::initPageAudio()
{
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_audio=new ppl7::tk::Frame(settings_page.x, settings_page.y, this->width() - settings_page.x - 10, this->height() - settings_page.y - 10, ppl7::tk::Frame::BorderStyle::NoBorder);
    page_audio->setName("SettingsPageAudio");
    page_audio->setBackgroundColor(background);

    int y=0;
    ppl7::tk::Label* label;
    label=new ppl7::tk::Label(0, y, input_widget_x, 40, translate("Audio device:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_device_combobox=new ppl7::tk::ComboBox(input_widget_x, y, input_widget.width, input_widget.height);
    std::list<ppl7::String> device_names;
    game.audiosystem.enumerateDevices(device_names);
    std::list<ppl7::String>::const_iterator it;
    for (it=device_names.begin();it != device_names.end();++it)
        audio_device_combobox->add((*it));
    audio_device_combobox->setEventHandler(this);
    page_audio->addChild(audio_device_combobox);
    y+=50;

    label=new ppl7::tk::Label(0, y, 200, 40, translate("Volume:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);
    y+=50;

    label=new ppl7::tk::Label(100, y, 200, 40, translate("all:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_total_slider=new ppl7::tk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_total_slider->setEventHandler(this);
    audio_total_slider->setLimits(0, 255);
    audio_total_slider->setValue(game.config.volumeTotal * 255.0f);
    page_audio->addChild(audio_total_slider);
    y+=50;

    label=new ppl7::tk::Label(100, y, 200, 40, translate("music:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_music_slider=new ppl7::tk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_music_slider->setEventHandler(this);
    audio_music_slider->setLimits(0, 255);
    audio_music_slider->setValue(game.config.volumeMusic * 255.0f * 2.0f);
    page_audio->addChild(audio_music_slider);
    y+=50;

    label=new ppl7::tk::Label(100, y, 200, 40, translate("effects:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_effects_slider=new ppl7::tk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_effects_slider->setEventHandler(this);
    audio_effects_slider->setLimits(0, 255);
    audio_effects_slider->setValue(game.config.volumeEffects * 255.0f);
    page_audio->addChild(audio_effects_slider);
    y+=50;

    label=new ppl7::tk::Label(100, y, 200, 40, translate("ambience:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_ambience_slider=new ppl7::tk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_ambience_slider->setEventHandler(this);
    audio_ambience_slider->setLimits(0, 255);
    audio_ambience_slider->setValue(game.config.volumeAmbience * 255.0f);
    page_audio->addChild(audio_ambience_slider);
    y+=50;

    label=new ppl7::tk::Label(100, y, 200, 40, translate("speech:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_speech_slider=new ppl7::tk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_speech_slider->setEventHandler(this);
    audio_speech_slider->setLimits(0, 255);
    audio_speech_slider->setValue(game.config.volumeSpeech * 255.0f);
    page_audio->addChild(audio_speech_slider);

}

void SettingsScreen::initPageVideo()
{
    ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_video=new ppl7::tk::Frame(settings_page.x, settings_page.y, this->width() - settings_page.x - 10, this->height() - settings_page.y - 10, ppl7::tk::Frame::BorderStyle::NoBorder);
    page_video->setName("SettingsPageVideo");
    page_video->setBackgroundColor(background);

    ppl7::tk::Label* label;
    label=new ppl7::tk::Label(0, 0, 250, 40, translate("Video device:"));
    label->setFont(style_label.font);
    page_video->addChild(label);

    video_device_combobox=new ppl7::tk::ComboBox(input_widget_x, 0, input_widget.width, input_widget.height);
    video_device_combobox->setEventHandler(this);
    std::list<SDL::VideoDisplay> display_list;
    SDL::getVideoDisplays(display_list);
    std::list<SDL::VideoDisplay>::const_iterator dit;
    for (dit=display_list.begin();dit != display_list.end();++dit) {
        video_device_combobox->add((*dit).name, ppl7::ToString("%d", (*dit).id));
    }
    video_device_combobox->setCurrentIdentifier(ppl7::ToString("%d", game.config.videoDevice));
    page_video->addChild(video_device_combobox);

    label=new ppl7::tk::Label(0, 50, 250, 40, translate("Screen resolution:"));
    label->setFont(style_label.font);
    page_video->addChild(label);

    screen_resolution_combobox=new ppl7::tk::ComboBox(input_widget_x, 50, input_widget.width, input_widget.height);
    screen_resolution_combobox->setEventHandler(this);
    updateVideoModes();
    page_video->addChild(screen_resolution_combobox);

    label=new ppl7::tk::Label(0, 100, 250, 40, translate("Window mode:"));
    label->setFont(style_label.font);
    page_video->addChild(label);

    windowmode_combobox=new ppl7::tk::ComboBox(input_widget_x, 100, input_widget.width, input_widget.height);
    windowmode_combobox->setEventHandler(this);
    windowmode_combobox->add(translate("Window"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::Window)));
    windowmode_combobox->add(translate("Fullscreen"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::Fullscreen)));
    windowmode_combobox->add(translate("Fullscreen Desktop"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::FullscreenDesktop)));
    windowmode_combobox->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(game.config.windowMode)));
    page_video->addChild(windowmode_combobox);

    save_video_settings_button=new ppl7::tk::Button(input_widget_x, 200, 450, 50,
        translate("use video settings"),
        gfx->Toolbar.getDrawable(24));
    save_video_settings_button->setFont(style_label.font);
    save_video_settings_button->setEventHandler(this);
    page_video->addChild(save_video_settings_button);

}

void SettingsScreen::initPageMisc()
{
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_misc=new ppl7::tk::Frame(settings_page.x, settings_page.y, this->width() - settings_page.x - 10, this->height() - settings_page.y - 10, ppl7::tk::Frame::BorderStyle::NoBorder);
    page_misc->setName("SettingsPageMisc");
    page_misc->setBackgroundColor(background);

    ppl7::tk::Label* label;
    int y=0;
    label=new ppl7::tk::Label(0, 0, 300, 40, translate("Language Settings:"));
    label->setFont(style_label.font);
    page_misc->addChild(label);
    y+=50;

    label=new ppl7::tk::Label(50, y, 150, 40, translate("Text:"));
    label->setFont(style_label.font);
    page_misc->addChild(label);

    text_language_combobox=new ppl7::tk::ComboBox(input_widget_x, y, input_widget.width, input_widget.height);
    text_language_combobox->add(translate("english"), "en");
    text_language_combobox->add(translate("german"), "de");
    text_language_combobox->setCurrentIdentifier(game.config.TextLanguage);
    text_language_combobox->setEventHandler(this);
    page_misc->addChild(text_language_combobox);
    y+=50;

    label=new ppl7::tk::Label(50, y, 150, 40, translate("Speech:"));
    label->setFont(style_label.font);
    page_misc->addChild(label);

    speech_language_combobox=new ppl7::tk::ComboBox(input_widget_x, y, input_widget.width, input_widget.height);
    speech_language_combobox->add(translate("english"), "en");
    speech_language_combobox->add(translate("german"), "de");
    speech_language_combobox->setCurrentIdentifier(game.config.SpeechLanguage);
    speech_language_combobox->setEventHandler(this);
    page_misc->addChild(speech_language_combobox);

    y+=50;
    skipIntro_checkbox=new ppl7::tk::CheckBox(0, y, 400, 40, translate("skip intro"), game.config.skipIntro);
    skipIntro_checkbox->setFont(style_label.font);
    skipIntro_checkbox->setEventHandler(this);
    page_misc->addChild(skipIntro_checkbox);


}


SettingsScreen::~SettingsScreen()
{

}

void SettingsScreen::retranslateUi()
{
    setupUi();
}

void SettingsScreen::selectSettingsPage(SettingsMenue page)
{
    select_audio->setSelected(false);
    select_video->setSelected(false);
    select_misc->setSelected(false);
    select_back->setSelected(false);
    if (select_menue) select_menue->setSelected(false);
    if (select_game_exit) select_game_exit->setSelected(false);
    this->removeChild(page_audio);
    this->removeChild(page_video);
    this->removeChild(page_misc);
    switch (page) {
    case SettingsMenue::Audio:
        select_audio->setSelected(true);
        this->addChild(page_audio);
        break;
    case SettingsMenue::Video:
        select_video->setSelected(true);
        this->addChild(page_video);
        break;
    case SettingsMenue::Misc:
        select_misc->setSelected(true);
        this->addChild(page_misc);
        break;
    case SettingsMenue::Back:
        select_back->setSelected(true);
        break;
    case SettingsMenue::Menue:
        if (select_menue) select_menue->setSelected(true);
        break;
    case SettingsMenue::Exit:
        if (select_game_exit) select_game_exit->setSelected(true);
        break;
    }
    currentMenueSelection=page;
}

void SettingsScreen::updateVideoModes()
{
    int display_id=video_device_combobox->currentIdentifier().toInt();
    screen_resolution_combobox->clear();
    std::list<SDL::DisplayMode> mode_list;
    SDL::getDisplayModes(display_id, mode_list);
    std::list<SDL::DisplayMode>::const_iterator mit;
    for (mit=mode_list.begin();mit != mode_list.end();++mit) {
        if ((*mit).format == ppl7::grafix::RGBFormat::X8R8G8B8 && (*mit).width >= 640 && (*mit).height >= 480) {
            screen_resolution_combobox->add(
                ppl7::ToString("%d x %d, %d Hz", (*mit).width, (*mit).height, (*mit).refresh_rate),
                ppl7::ToString("%d,%d,%d", (*mit).width, (*mit).height, (*mit).refresh_rate));
        }
    }
    ppl7::String id;
    id.setf("%d,%d,%d", game.config.ScreenResolution.width,
        game.config.ScreenResolution.height,
        game.config.ScreenRefreshRate);
    screen_resolution_combobox->setCurrentIdentifier(id);
}

void SettingsScreen::paint(ppl7::grafix::Drawable& draw)
{
    ppl7::grafix::Color bg(20, 10, 0, 192);
    ppl7::grafix::Color border(255, 200, 0, 255);
    ppl7::grafix::Color fg(128, 128, 128, 255);
    draw.cls(bg);
    return;
    for (int i=0;i < 6;i++) {
        draw.drawRect(i, i, draw.width() - i, draw.height() - i, border);
    }
}


void SettingsScreen::keyDownEvent(ppl7::tk::KeyEvent* event)
{
    //ppl7::PrintDebugTime("SettingsScreen::keyDownEvent: key=%d, %s\n", event->key, (const  char*)event->widget()->name());
    if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE) {
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        this->getParent()->closeEvent(&event);
    }

    if (event->widget() == menue) {
        int m=static_cast<int>(currentMenueSelection);
        if (event->key == ppl7::tk::KeyEvent::KEY_DOWN && currentMenueSelection < SettingsMenue::Back) {
            if (!ingame && m == 2) m=4;
            selectSettingsPage(static_cast<SettingsMenue>(m + 1));
        } else if (event->key == ppl7::tk::KeyEvent::KEY_UP && currentMenueSelection > SettingsMenue::Audio) {
            if (!ingame && m == 5) m = 3;
            selectSettingsPage(static_cast<SettingsMenue>(m - 1));
        } else if (event->key == ppl7::tk::KeyEvent::KEY_RETURN || event->key == ppl7::tk::KeyEvent::KEY_ENTER) {
            if (select_back->isSelected()) {
                ppl7::tk::Event event(ppl7::tk::Event::Close);
                event.setWidget(this);
                event.setCustomId(0);
                this->getParent()->closeEvent(&event);
            } else if (select_menue != NULL && select_menue->isSelected()) {
                ppl7::tk::Event event(ppl7::tk::Event::Close);
                event.setWidget(this);
                event.setCustomId(1);
                this->getParent()->closeEvent(&event);
            } else if (select_game_exit != NULL && select_game_exit->isSelected()) {
                ppl7::tk::Event event(ppl7::tk::Event::Close);
                event.setWidget(this);
                event.setCustomId(2);
                this->getParent()->closeEvent(&event);
            }
        }
    }
}

void SettingsScreen::mouseEnterEvent(ppl7::tk::MouseEvent* event)
{
    if (event->widget() == select_audio) selectSettingsPage(SettingsMenue::Audio);
    else if (event->widget() == select_video) selectSettingsPage(SettingsMenue::Video);
    else if (event->widget() == select_misc) selectSettingsPage(SettingsMenue::Misc);
    else if (event->widget() == select_back) selectSettingsPage(SettingsMenue::Back);
    else if (select_menue != NULL && event->widget() == select_menue) selectSettingsPage(SettingsMenue::Menue);
    else if (select_game_exit != NULL && event->widget() == select_game_exit) selectSettingsPage(SettingsMenue::Exit);
}


void SettingsScreen::mouseClickEvent(ppl7::tk::MouseEvent* event)
{
    if (event->widget() == select_back) {
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        event.setCustomId(0);
        this->getParent()->closeEvent(&event);
    } else if (select_menue != NULL && event->widget() == select_menue) {
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        event.setCustomId(1);
        this->getParent()->closeEvent(&event);
    } else if (select_game_exit != NULL && event->widget() == select_game_exit) {
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        event.setCustomId(2);
        this->getParent()->closeEvent(&event);
    } else if (event->widget() == save_video_settings_button) {
        Config::WindowMode mode=static_cast<Config::WindowMode>(windowmode_combobox->currentIdentifier().toInt());

        ppl7::tk::WindowManager_SDL2* wm=(ppl7::tk::WindowManager_SDL2*)ppl7::tk::GetWindowManager();
        wm->changeWindowMode(game.window(), mode);
        ppl7::tk::Window::DisplayMode dmode;
        dmode.format=game.window().rgbFormat();
        ppl7::String id=screen_resolution_combobox->currentIdentifier();
        ppl7::Array Tok(id, ",");
        dmode.width=Tok[0].toInt();
        dmode.height=Tok[1].toInt();
        dmode.refresh_rate=Tok[2].toInt();
        try {
            game.window().setWindowDisplayMode(dmode);
            game.resizeEvent(NULL);
            game.config.ScreenResolution.setSize(dmode.width, dmode.height);
            game.config.ScreenRefreshRate=dmode.refresh_rate;
            game.config.windowMode=mode;
            game.config.save();
        } catch (const ppl7::Exception& exp) {
            exp.print();
        }
    }
}


void SettingsScreen::valueChangedEvent(ppl7::tk::Event* event, int64_t value)
{
    if (event->widget() == video_device_combobox) updateVideoModes();
    else if (event->widget() == audio_total_slider) {
        game.config.volumeTotal=(float)audio_total_slider->value() / 255.0f;
        game.audiosystem.setGlobalVolume(game.config.volumeTotal);
        game.config.save();
    } else if (event->widget() == audio_music_slider) {
        game.config.volumeMusic=(float)audio_music_slider->value() / 512.0f;
        game.audiosystem.setVolume(AudioClass::Music, game.config.volumeMusic);
        game.config.save();
    } else if (event->widget() == audio_effects_slider) {
        game.config.volumeEffects=(float)audio_effects_slider->value() / 255.0f;
        game.audiosystem.setVolume(AudioClass::Effect, game.config.volumeEffects);
        game.config.save();
    } else if (event->widget() == audio_ambience_slider) {
        game.config.volumeAmbience=(float)audio_ambience_slider->value() / 255.0f;
        game.audiosystem.setVolume(AudioClass::Ambience, game.config.volumeAmbience);
        game.config.save();
    } else if (event->widget() == audio_speech_slider) {
        game.config.volumeSpeech=(float)audio_speech_slider->value() / 255.0f;
        game.audiosystem.setVolume(AudioClass::Speech, game.config.volumeSpeech);
        game.config.save();
    }
}

void SettingsScreen::valueChangedEvent(ppl7::tk::Event* event, int value)
{
    if (event->widget() == text_language_combobox) {
        game.config.TextLanguage=text_language_combobox->currentIdentifier();
        game.config.save();
        GetTranslator().setLanguage(game.config.TextLanguage);
        retranslateUi();
    }
    if (event->widget() == speech_language_combobox) {
        game.config.SpeechLanguage=speech_language_combobox->currentIdentifier();
        game.config.save();
    }

}

void SettingsScreen::resizeEvent(ppl7::tk::ResizeEvent* event)
{
    //printf("SettingsScreen: we got a resize event: %d x %d\n", width(), height());
    setupUi();
    selectSettingsPage(currentMenueSelection);

    needsRedraw();

}

void SettingsScreen::toggledEvent(ppl7::tk::Event* event, bool checked)
{
    if (event->widget() == skipIntro_checkbox) {
        ppl7::PrintDebugTime("toggle\n");
        game.config.skipIntro=checked;
        game.config.save();
    }
}
