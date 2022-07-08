#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"


SettingsScreen::SettingsScreen(Game& g, SDL& s, int x, int y, int width, int height)
    : game(g), sdl(s)
{
    create(x, y, width, height);
    this->setName("SettingsScreen");
    const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
    ppl7::grafix::Font font=style.buttonFont;
    font.setName("NotoSansBlack");
    font.setBold(false);
    font.setSize(50);
    font.setOrientation(ppl7::grafix::Font::TOP);
    font.setColor(ppl7::grafix::Color(255, 255, 255, 255));
    labelFont=font;
    labelFont.setSize(20);

    ppl7::tk::Label* label;
    ppl7::String text=translate("Settings");
    label=new ppl7::tk::Label(10, 10, 300, 90, text);
    label->setFont(font);
    ppl7::grafix::Size size=font.measure(text);
    label->setPos((width - size.width) / 2, 10);
    this->addChild(label);

    ppl7::grafix::Color background(20, 10, 0, 192);
    menue=new ppl7::tk::Frame(0, 100, 320, height - 100, ppl7::tk::Frame::BorderStyle::NoBorder);
    menue->setBackgroundColor(background);
    menue->setName("Settings Menue");
    menue->setEventHandler(this);
    this->addChild(menue);


    select_audio=new GameMenuArea(10, 0, 300, 90, translate("Audio"));
    select_audio->setName("Settings Selection Audio");
    select_audio->setEventHandler(this);
    menue->addChild(select_audio);

    select_video=new GameMenuArea(10, 100, 300, 90, translate("Video"));
    select_video->setName("Settings Selection Video");
    select_video->setEventHandler(this);
    menue->addChild(select_video);

    select_misc=new GameMenuArea(10, 200, 300, 90, translate("Misc"));
    select_misc->setName("Settings Selection Misc");
    select_misc->setEventHandler(this);
    menue->addChild(select_misc);

    select_back=new GameMenuArea(10, menue->height() - 100, 300, 90, translate("back (ESC)"));
    select_back->setName("Settings Selection Back");
    select_back->setEventHandler(this);
    menue->addChild(select_back);

    ppl7::tk::GetWindowManager()->setKeyboardFocus(menue);

    page_audio=NULL;
    page_video=NULL;
    page_misc=NULL;
    initPageAudio();
    initPageVideo();
    initPageMisc();

    selectSettingsPage(SettingsMenue::Audio);
}

void SettingsScreen::initPageAudio()
{
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_audio=new ppl7::tk::Frame(340, 100, this->width() - 350, this->height() - 110, ppl7::tk::Frame::BorderStyle::NoBorder);
    page_audio->setName("SettingsPageAudio");
    page_audio->setBackgroundColor(background);

    ppl7::tk::Label* label;
    label=new ppl7::tk::Label(0, 0, 200, 40, translate("Audio device:"));
    label->setFont(labelFont);
    page_audio->addChild(label);

    audio_device_combobox=new Decker::ui::ComboBox(210, 0, 400, 40);
    std::list<ppl7::String> device_names;
    game.audiosystem.enumerateDevices(device_names);
    std::list<ppl7::String>::const_iterator it;
    for (it=device_names.begin();it != device_names.end();++it)
        audio_device_combobox->add((*it));
    page_audio->addChild(audio_device_combobox);

    label=new ppl7::tk::Label(0, 50, 200, 40, translate("Volume:"));
    label->setFont(labelFont);
    page_audio->addChild(label);


    label=new ppl7::tk::Label(100, 100, 200, 40, translate("all:"));
    label->setFont(labelFont);
    page_audio->addChild(label);

    label=new ppl7::tk::Label(100, 150, 200, 40, translate("music:"));
    label->setFont(labelFont);
    page_audio->addChild(label);
    label=new ppl7::tk::Label(100, 200, 200, 40, translate("effects:"));
    label->setFont(labelFont);
    page_audio->addChild(label);

}

void SettingsScreen::initPageVideo()
{
    ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_video=new ppl7::tk::Frame(340, 100, this->width() - 350, this->height() - 110, ppl7::tk::Frame::BorderStyle::NoBorder);
    page_video->setName("SettingsPageVideo");
    page_video->setBackgroundColor(background);

    ppl7::tk::Label* label;
    label=new ppl7::tk::Label(0, 0, 250, 40, translate("Video device:"));
    label->setFont(labelFont);
    page_video->addChild(label);

    video_device_combobox=new Decker::ui::ComboBox(260, 0, 400, 40);
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
    label->setFont(labelFont);
    page_video->addChild(label);

    screen_resolution_combobox=new Decker::ui::ComboBox(260, 50, 400, 40);
    screen_resolution_combobox->setEventHandler(this);
    updateVideoModes();
    page_video->addChild(screen_resolution_combobox);

    label=new ppl7::tk::Label(0, 100, 250, 40, translate("Window mode:"));
    label->setFont(labelFont);
    page_video->addChild(label);

    windowmode_combobox=new Decker::ui::ComboBox(260, 100, 400, 40);
    windowmode_combobox->setEventHandler(this);
    windowmode_combobox->add(translate("Window"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::Window)));
    windowmode_combobox->add(translate("Fullscreen"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::Fullscreen)));
    windowmode_combobox->add(translate("Fullscreen Desktop"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::FullscreenDesktop)));
    windowmode_combobox->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(game.config.windowMode)));
    page_video->addChild(windowmode_combobox);

    save_video_settings_button=new ppl7::tk::Button(260, 200, 250, 50,
        translate("use video settings"),
        gfx->Toolbar.getDrawable(24));
    save_video_settings_button->setFont(labelFont);
    save_video_settings_button->setEventHandler(this);
    page_video->addChild(save_video_settings_button);

}

void SettingsScreen::initPageMisc()
{
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_misc=new ppl7::tk::Frame(340, 100, this->width() - 350, this->height() - 110, ppl7::tk::Frame::BorderStyle::NoBorder);
    page_misc->setName("SettingsPageMisc");
    page_misc->setBackgroundColor(background);

    ppl7::tk::Label* label;
    label=new ppl7::tk::Label(0, 0, 200, 40, translate("Text language:"));
    label->setFont(labelFont);
    page_misc->addChild(label);

    language_combobox=new Decker::ui::ComboBox(210, 0, 300, 40);
    language_combobox->add(translate("english"), "en");
    language_combobox->add(translate("german"), "de");
    language_combobox->setCurrentIdentifier(game.config.Language);
    language_combobox->setEventHandler(this);
    page_misc->addChild(language_combobox);



}


SettingsScreen::~SettingsScreen()
{

}

void SettingsScreen::selectSettingsPage(SettingsMenue page)
{
    select_audio->setSelected(false);
    select_video->setSelected(false);
    select_misc->setSelected(false);
    select_back->setSelected(false);
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
        if ((*mit).format == ppl7::grafix::RGBFormat::X8R8G8B8 && (*mit).width >= 1024 && (*mit).height >= 768) {
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
    //printf("SettingsScreen::keyDownEvent\n");
    if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE) {
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
        this->getParent()->closeEvent(&event);
    }
    //printf("%s\n", (const  char*)event->widget()->name());
    if (event->widget() == menue) {
        if (event->key == ppl7::tk::KeyEvent::KEY_DOWN && currentMenueSelection < SettingsMenue::Back) {
            selectSettingsPage(static_cast<SettingsMenue>(static_cast<int>(currentMenueSelection) + 1));
        } else if (event->key == ppl7::tk::KeyEvent::KEY_UP && currentMenueSelection > SettingsMenue::Audio) {
            selectSettingsPage(static_cast<SettingsMenue>(static_cast<int>(currentMenueSelection) - 1));
        }

    }
}

void SettingsScreen::mouseEnterEvent(ppl7::tk::MouseEvent* event)
{
    if (event->widget() == select_audio) selectSettingsPage(SettingsMenue::Audio);
    else if (event->widget() == select_video) selectSettingsPage(SettingsMenue::Video);
    else if (event->widget() == select_misc) selectSettingsPage(SettingsMenue::Misc);
    else if (event->widget() == select_back) selectSettingsPage(SettingsMenue::Back);
}


void SettingsScreen::mouseClickEvent(ppl7::tk::MouseEvent* event)
{
    if (event->widget() == select_back) {
        ppl7::tk::Event event(ppl7::tk::Event::Close);
        event.setWidget(this);
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
            game.config.ScreenResolution.setSize(dmode.width, dmode.height);
            game.config.ScreenRefreshRate=dmode.refresh_rate;
            game.config.windowMode=mode;
            game.config.save();
        }
        catch (const ppl7::Exception& exp) {
            exp.print();
        }
    }
}


void SettingsScreen::valueChangedEvent(ppl7::tk::Event* event, int value)
{
    if (event->widget() == video_device_combobox) updateVideoModes();
}
