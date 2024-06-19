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
    ppltk::GetWindowManager()->setGameControllerFocus(this);
    keyfocus=KeyFocusArea::Menu;
}

void SettingsScreen::updateStyles()
{
    const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
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
    ppltk::Label* label;
    ppl7::String text=translate("Settings");
    label=new ppltk::Label(10, 10, style_heading.size.width, style_heading.size.height, text);
    label->setFont(style_heading.font);
    ppl7::grafix::Size size=style_heading.font.measure(text);
    label->setPos((width() - size.width) / 2, 10);
    label->setWidth(size.width + 20);
    this->addChild(label);

    ppl7::grafix::Color background(20, 10, 0, 192);
    menue=new ppltk::Frame(0, style_heading.total.height, style_menue.size.width + 20, height() - style_heading.total.height, ppltk::Frame::BorderStyle::NoBorder);
    menue->setBackgroundColor(background);
    menue->setName("Settings Menue");
    menue->setEventHandler(this);
    this->addChild(menue);

    int y=0;
    select_audio=new Decker::ui::GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Audio"));
    select_audio->setName("Settings Selection Audio");
    select_audio->setFontSize(style_menue.font.size());
    select_audio->setBorderWidth(style_menue.border_width);
    select_audio->setEventHandler(this);
    menue->addChild(select_audio);
    y+=style_menue.total.height;

    select_video=new Decker::ui::GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Video"));
    select_video->setName("Settings Selection Video");
    select_video->setFontSize(style_menue.font.size());
    select_video->setBorderWidth(style_menue.border_width);
    select_video->setEventHandler(this);
    menue->addChild(select_video);
    y+=style_menue.total.height;

    select_controller=new Decker::ui::GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Controller"));
    select_controller->setName("Settings Selection Controller");
    select_controller->setFontSize(style_menue.font.size());
    select_controller->setBorderWidth(style_menue.border_width);
    select_controller->setEventHandler(this);
    menue->addChild(select_controller);
    y+=style_menue.total.height;

    select_misc=new Decker::ui::GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Misc"));
    select_misc->setName("Settings Selection Misc");
    select_misc->setFontSize(style_menue.font.size());
    select_misc->setBorderWidth(style_menue.border_width);
    select_misc->setEventHandler(this);
    menue->addChild(select_misc);
    y+=style_menue.total.height;


    if (ingame) {
        select_menue=new Decker::ui::GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Menue"));
        select_menue->setName("Settings Back to Menue");
        select_menue->setFontSize(style_menue.font.size());
        select_menue->setBorderWidth(style_menue.border_width);
        select_menue->setEventHandler(this);
        menue->addChild(select_menue);
        y+=style_menue.total.height;

        select_game_exit=new Decker::ui::GameMenuArea(10, y, style_menue.size.width, style_menue.size.height, translate("Exit Game"));
        select_game_exit->setName("Settings Exit Game");
        select_game_exit->setFontSize(style_menue.font.size());
        select_game_exit->setBorderWidth(style_menue.border_width);
        select_game_exit->setEventHandler(this);
        menue->addChild(select_game_exit);
        y+=style_menue.total.height;
    }

    select_back=new Decker::ui::GameMenuArea(10, menue->height() - style_menue.total.height, style_menue.size.width, style_menue.size.height, translate("back (ESC)"));
    select_back->setName("Settings Selection Back");
    select_back->setFontSize(style_menue.font.size());
    select_back->setBorderWidth(style_menue.border_width);
    select_back->setEventHandler(this);
    menue->addChild(select_back);

    ppltk::GetWindowManager()->setKeyboardFocus(menue);

    page_audio=NULL;
    page_video=NULL;
    page_misc=NULL;
    page_controller=NULL;
    initPageAudio();
    initPageVideo();
    initPageController();
    initPageMisc();


}

void SettingsScreen::initPageAudio()
{
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_audio=new ppltk::Frame(settings_page.x, settings_page.y, this->width() - settings_page.x - 10, this->height() - settings_page.y - 10, ppltk::Frame::BorderStyle::NoBorder);
    page_audio->setName("SettingsPageAudio");
    page_audio->setBackgroundColor(background);

    int y=0;
    ppltk::Label* label;
    /*
    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Audio device:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);
    audio_device_combobox=new ppltk::ComboBox(input_widget_x, y, input_widget.width, input_widget.height);
    std::list<ppl7::String> device_names;
    game.audiosystem.enumerateDevices(device_names);
    std::list<ppl7::String>::const_iterator it;
    for (it=device_names.begin();it != device_names.end();++it)
        audio_device_combobox->add((*it));
    audio_device_combobox->setEventHandler(this);
    page_audio->addChild(audio_device_combobox);
    y+=50;
    */

    label=new ppltk::Label(0, y, 200, 40, translate("Volume:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);
    y+=50;

    label=new ppltk::Label(100, y, 200, 40, translate("all:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_total_slider=new ppltk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_total_slider->setEventHandler(this);
    audio_total_slider->setLimits(0, 255);
    audio_total_slider->setValue(game.config.volumeTotal * 255.0f);
    page_audio->addChild(audio_total_slider);
    y+=50;

    label=new ppltk::Label(100, y, 200, 40, translate("music:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_music_slider=new ppltk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_music_slider->setEventHandler(this);
    audio_music_slider->setLimits(0, 255);
    audio_music_slider->setValue(game.config.volumeMusic * 255.0f * 2.0f);
    page_audio->addChild(audio_music_slider);
    y+=50;

    label=new ppltk::Label(100, y, 200, 40, translate("effects:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_effects_slider=new ppltk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_effects_slider->setEventHandler(this);
    audio_effects_slider->setLimits(0, 255);
    audio_effects_slider->setValue(game.config.volumeEffects * 255.0f);
    page_audio->addChild(audio_effects_slider);
    y+=50;

    label=new ppltk::Label(100, y, 200, 40, translate("ambience:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_ambience_slider=new ppltk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_ambience_slider->setEventHandler(this);
    audio_ambience_slider->setLimits(0, 255);
    audio_ambience_slider->setValue(game.config.volumeAmbience * 255.0f);
    page_audio->addChild(audio_ambience_slider);
    y+=50;

    label=new ppltk::Label(100, y, 200, 40, translate("speech:"));
    label->setFont(style_label.font);
    page_audio->addChild(label);

    audio_speech_slider=new ppltk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    audio_speech_slider->setEventHandler(this);
    audio_speech_slider->setLimits(0, 255);
    audio_speech_slider->setValue(game.config.volumeSpeech * 255.0f);
    page_audio->addChild(audio_speech_slider);

}

void SettingsScreen::initPageVideo()
{
    //ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
    ppltk::WindowManager* wm=ppltk::GetWindowManager();
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_video=new ppltk::Frame(settings_page.x, settings_page.y, this->width() - settings_page.x - 10, this->height() - settings_page.y - 10, ppltk::Frame::BorderStyle::NoBorder);
    page_video->setName("SettingsPageVideo");
    page_video->setBackgroundColor(background);

    ppltk::Label* label;
    label=new ppltk::Label(0, 0, 250, 40, translate("Video device:"));
    label->setFont(style_label.font);
    page_video->addChild(label);

    video_device_combobox=new ppltk::ComboBox(input_widget_x, 0, input_widget.width, input_widget.height);
    video_device_combobox->setEventHandler(this);
    std::list<SDL::VideoDisplay> display_list;
    SDL::getVideoDisplays(display_list);
    std::list<SDL::VideoDisplay>::const_iterator dit;
    for (dit=display_list.begin();dit != display_list.end();++dit) {
        video_device_combobox->add((*dit).name, ppl7::ToString("%d", (*dit).id));
    }
    video_device_combobox->setCurrentIdentifier(ppl7::ToString("%d", game.config.videoDevice));
    page_video->addChild(video_device_combobox);

    label=new ppltk::Label(0, 50, 250, 40, translate("Screen resolution:"));
    label->setFont(style_label.font);
    page_video->addChild(label);

    screen_resolution_combobox=new ppltk::ComboBox(input_widget_x, 50, input_widget.width, input_widget.height);
    screen_resolution_combobox->setEventHandler(this);
    updateVideoModes();
    page_video->addChild(screen_resolution_combobox);

    label=new ppltk::Label(0, 100, 250, 40, translate("Window mode:"));
    label->setFont(style_label.font);
    page_video->addChild(label);

    windowmode_combobox=new ppltk::ComboBox(input_widget_x, 100, input_widget.width, input_widget.height);
    windowmode_combobox->setEventHandler(this);
    windowmode_combobox->add(translate("Window"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::Window)));
    windowmode_combobox->add(translate("Fullscreen"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::Fullscreen)));
    windowmode_combobox->add(translate("Fullscreen Desktop"), ppl7::ToString("%d", static_cast<int>(Config::WindowMode::FullscreenDesktop)));
    windowmode_combobox->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(game.config.windowMode)));
    page_video->addChild(windowmode_combobox);

    save_video_settings_button=new ppltk::Button(input_widget_x, 200, 450, 50,
        translate("use video settings"),
        wm->Toolbar.getDrawable(24));
    save_video_settings_button->setFont(style_label.font);
    save_video_settings_button->setEventHandler(this);
    page_video->addChild(save_video_settings_button);

}

void SettingsScreen::initPageMisc()
{
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_misc=new ppltk::Frame(settings_page.x, settings_page.y, this->width() - settings_page.x - 10, this->height() - settings_page.y - 10, ppltk::Frame::BorderStyle::NoBorder);
    page_misc->setName("SettingsPageMisc");
    page_misc->setBackgroundColor(background);

    ppltk::Label* label;
    int y=0;
    label=new ppltk::Label(0, 0, 300, 40, translate("Language Settings:"));
    label->setFont(style_label.font);
    page_misc->addChild(label);
    y+=50;

    label=new ppltk::Label(50, y, 150, 40, translate("Text:"));
    label->setFont(style_label.font);
    page_misc->addChild(label);

    text_language_combobox=new ppltk::ComboBox(input_widget_x, y, input_widget.width, input_widget.height);
    text_language_combobox->add(translate("english"), "en");
    text_language_combobox->add(translate("german"), "de");
    text_language_combobox->setCurrentIdentifier(game.config.TextLanguage);
    text_language_combobox->setEventHandler(this);
    page_misc->addChild(text_language_combobox);
    y+=50;

    label=new ppltk::Label(50, y, 150, 40, translate("Speech:"));
    label->setFont(style_label.font);
    page_misc->addChild(label);

    speech_language_combobox=new ppltk::ComboBox(input_widget_x, y, input_widget.width, input_widget.height);
    speech_language_combobox->add(translate("english"), "en");
    speech_language_combobox->add(translate("german"), "de");
    speech_language_combobox->setCurrentIdentifier(game.config.SpeechLanguage);
    speech_language_combobox->setEventHandler(this);
    page_misc->addChild(speech_language_combobox);

    y+=50;
    skipIntro_checkbox=new ppltk::CheckBox(0, y, 400, 40, translate("skip intro"), game.config.skipIntro);
    skipIntro_checkbox->setFont(style_label.font);
    skipIntro_checkbox->setEventHandler(this);
    page_misc->addChild(skipIntro_checkbox);

    y+=50;
    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Difficulty:"));
    label->setFont(style_label.font);
    page_misc->addChild(label);

    difficulty_slider=new ppltk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    difficulty_slider->setEventHandler(this);
    difficulty_slider->setLimits(1, 3);
    difficulty_slider->setValue(static_cast<int>(game.config.difficulty));
    page_misc->addChild(difficulty_slider);

    difficulty_name=new ppltk::Label(input_widget_x + input_widget.width, y, 200, 40, translate("normal"));
    difficulty_name->setFont(style_label.font);
    page_misc->addChild(difficulty_name);
    y+=50;
    difficulty_description=new ppltk::Label(input_widget_x, y, input_widget.width + 200, 40, translate("normal"));
    difficulty_description->setFont(style_label.font);
    page_misc->addChild(difficulty_description);
    updateDifficultyDescription();
    y+=50;

    noBlood_checkbox=new ppltk::CheckBox(0, y, 400, 40, translate("no blood"), game.config.noBlood);
    noBlood_checkbox->setFont(style_label.font);
    noBlood_checkbox->setEventHandler(this);
    page_misc->addChild(noBlood_checkbox);
    y+=50;

}

void SettingsScreen::updateDifficultyDescription()
{
    Config::DifficultyLevel level=static_cast<Config::DifficultyLevel>(difficulty_slider->value());
    switch (level) {
        case Config::DifficultyLevel::easy:
            difficulty_name->setText(translate("easy"));
            difficulty_description->setText(translate("no falling damage, less enemies, less damage"));
            break;
        case Config::DifficultyLevel::normal:
            difficulty_name->setText(translate("normal"));
            difficulty_description->setText(translate("all the fun, play how it's meant to be played"));
            break;
        case Config::DifficultyLevel::hard:
            difficulty_name->setText(translate("hard"));
            difficulty_description->setText(translate("more enemies, more damage"));
            break;
    }
}

void SettingsScreen::initPageController()
{
    ppl7::grafix::Color background(20, 10, 0, 192);
    page_controller=new ppltk::Frame(settings_page.x, settings_page.y, this->width() - settings_page.x - 10, this->height() - settings_page.y - 10, ppltk::Frame::BorderStyle::NoBorder);
    page_controller->setName("SettingsPageController");
    page_controller->setBackgroundColor(background);

    ppltk::Label* label;
    int y=0;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Deadzone:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);

    controller_deadzone=new ppltk::HorizontalSlider(input_widget_x, y, input_widget.width, input_widget.height);
    controller_deadzone->setEventHandler(this);
    controller_deadzone->setLimits(0, 30000);
    controller_deadzone->setValue(static_cast<int>(game.config.controller.deadzone));
    page_controller->addChild(controller_deadzone);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Stick walk:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_axis_walk=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_axis_walk->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Axis);
    controller_axis_walk->setId(game.controller.mapping.getSDLAxis(GameControllerMapping::Axis::Walk));
    controller_axis_walk->setEventHandler(this);
    page_controller->addChild(controller_axis_walk);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Stick jump:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_axis_jump=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_axis_jump->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Axis);
    controller_axis_jump->setId(game.controller.mapping.getSDLAxis(GameControllerMapping::Axis::Jump));
    controller_axis_jump->setEventHandler(this);
    page_controller->addChild(controller_axis_jump);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Button up:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_up=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_up->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_up->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::MenuUp));
    controller_button_up->setEventHandler(this);
    page_controller->addChild(controller_button_up);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Button down:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_down=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_down->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_down->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::MenuDown));
    controller_button_down->setEventHandler(this);
    page_controller->addChild(controller_button_down);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Button left:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_left=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_left->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_left->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::MenuLeft));
    controller_button_left->setEventHandler(this);
    page_controller->addChild(controller_button_left);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Button right:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_right=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_right->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_right->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::MenuRight));
    controller_button_right->setEventHandler(this);
    page_controller->addChild(controller_button_right);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Button menu:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_menu=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_menu->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_menu->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::Menu));
    controller_button_menu->setEventHandler(this);
    page_controller->addChild(controller_button_menu);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Button action:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_action=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_action->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_action->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::Action));
    controller_button_action->setEventHandler(this);
    page_controller->addChild(controller_button_action);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Button back:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_back=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_back->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_back->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::Back));
    controller_button_back->setEventHandler(this);
    page_controller->addChild(controller_button_back);
    y+=50;

    label=new ppltk::Label(0, y, input_widget_x, 40, translate("Button jump:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_jump=new Decker::ui::ControllerButtonSelector(input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_jump->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_jump->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::Jump));
    controller_button_jump->setEventHandler(this);
    page_controller->addChild(controller_button_jump);
    y+=50;

    y=0;
    int xbase=input_widget_x + input_widget.width + 10;

    label=new ppltk::Label(xbase, y, input_widget_x, 40, translate("Button crouch:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_crouch=new Decker::ui::ControllerButtonSelector(xbase + input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_crouch->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_crouch->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::Crouch));
    controller_button_crouch->setEventHandler(this);
    page_controller->addChild(controller_button_crouch);
    y+=50;

    label=new ppltk::Label(xbase, y, input_widget_x, 40, translate("Button flashlight:"));
    label->setFont(style_label.font);
    page_controller->addChild(label);
    controller_button_flashlight=new Decker::ui::ControllerButtonSelector(xbase + input_widget_x, y, input_widget.width, input_widget.height);
    controller_button_flashlight->setControllerType(Decker::ui::ControllerButtonSelector::ControllerType::Button);
    controller_button_flashlight->setId(game.controller.mapping.getSDLButton(GameControllerMapping::Button::Flashlight));
    controller_button_flashlight->setEventHandler(this);
    page_controller->addChild(controller_button_flashlight);
    y+=50;

    controller_use_rumble=new ppltk::CheckBox(xbase, y, 700, 40, translate("rumble when player gets hurt"), game.config.controller.use_rumble);
    controller_use_rumble->setFont(style_label.font);
    controller_use_rumble->setEventHandler(this);
    page_controller->addChild(controller_use_rumble);
    y+=50;

}



SettingsScreen::~SettingsScreen()
{
    if (page_audio) delete (page_audio);
    if (page_video) delete (page_video);
    if (page_misc) delete (page_misc);
    if (page_controller) delete (page_controller);
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
    select_controller->setSelected(false);
    select_back->setSelected(false);
    if (select_menue) select_menue->setSelected(false);
    if (select_game_exit) select_game_exit->setSelected(false);
    this->removeChild(page_audio);
    this->removeChild(page_video);
    this->removeChild(page_misc);
    this->removeChild(page_controller);
    switch (page) {
        case SettingsMenue::Audio:
            select_audio->setSelected(true);
            this->addChild(page_audio);
            break;
        case SettingsMenue::Video:
            select_video->setSelected(true);
            this->addChild(page_video);
            break;
        case SettingsMenue::Controller:
            select_controller->setSelected(true);
            this->addChild(page_controller);
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
        if ((*mit).format == ppl7::grafix::RGBFormat::X8R8G8B8 && (*mit).width >= 640 && (*mit).height >= 480 &&
            (*mit).width <= 1920 && (*mit).height <= 1200) {
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

void SettingsScreen::handleMenuKeyDownEvent(int key)
{
    if (key == ppltk::KeyEvent::KEY_ESCAPE) {
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        this->getParent()->closeEvent(&event);
        return;
    }

    int m=static_cast<int>(currentMenueSelection);
    if (key == ppltk::KeyEvent::KEY_DOWN && currentMenueSelection < SettingsMenue::Back) {
        if (!ingame && m == 3) m=5;
        selectSettingsPage(static_cast<SettingsMenue>(m + 1));
    } else if (key == ppltk::KeyEvent::KEY_UP && currentMenueSelection > SettingsMenue::Audio) {
        if (!ingame && m == 6) m = 4;
        selectSettingsPage(static_cast<SettingsMenue>(m - 1));
    } else if (key == ppltk::KeyEvent::KEY_RETURN || key == ppltk::KeyEvent::KEY_ENTER || key == ppltk::KeyEvent::KEY_RIGHT) {
        if (select_back->isSelected()) {
            ppltk::Event event(ppltk::Event::Close);
            event.setWidget(this);
            event.setCustomId(0);
            this->getParent()->closeEvent(&event);
        } else if (select_menue != NULL && select_menue->isSelected()) {
            ppltk::Event event(ppltk::Event::Close);
            event.setWidget(this);
            event.setCustomId(1);
            this->getParent()->closeEvent(&event);
        } else if (select_game_exit != NULL && select_game_exit->isSelected()) {
            ppltk::Event event(ppltk::Event::Close);
            event.setWidget(this);
            event.setCustomId(2);
            this->getParent()->closeEvent(&event);
        } else if (select_audio != NULL && select_audio->isSelected()) {
            keyfocus=KeyFocusArea::Audio;
            //ppltk::GetWindowManager()->setKeyboardFocus(page_audio);
            //select_audio->setSelected(false);
            currentAudioSelection=SettingsAudio::Total;
            audio_total_slider->setFocus();
        } else if (select_controller != NULL && select_controller->isSelected()) {
            keyfocus=KeyFocusArea::Controller;
            currentControllerSelection=SettingsController::Deadzone;
            controller_deadzone->setFocus();
        }
    }
}

ppltk::HorizontalSlider* SettingsScreen::getCurrentAudioSlider()
{
    switch (currentAudioSelection) {
        case SettingsAudio::Total: return audio_total_slider;
        case SettingsAudio::Music: return audio_music_slider;
        case SettingsAudio::Effects: return audio_effects_slider;
        case SettingsAudio::Ambience: return audio_ambience_slider;
        case SettingsAudio::Speech: return audio_speech_slider;
    }
    return NULL;
}

Decker::ui::ControllerButtonSelector* SettingsScreen::getCurrentControllerInput()
{
    switch (currentControllerSelection) {
        case SettingsController::AxisWalk: return controller_axis_walk;
        case SettingsController::AxisJump: return controller_axis_jump;
        case SettingsController::ButtonUp: return controller_button_up;
        case SettingsController::ButtonDown: return controller_button_down;
        case SettingsController::ButtonLeft: return controller_button_left;
        case SettingsController::ButtonRight: return controller_button_right;
        case SettingsController::ButtonMenu: return controller_button_menu;
        case SettingsController::ButtonAction: return controller_button_action;
        case SettingsController::ButtonBack: return controller_button_back;
        case SettingsController::ButtonJump: return controller_button_jump;
        case SettingsController::ButtonCrouch: return controller_button_crouch;
        case SettingsController::ButtonFlashlight: return controller_button_flashlight;
        default: return NULL;
    }
    return NULL;
}


void SettingsScreen::handleAudioKeyDownEvent(int key)
{
    if (key == ppltk::KeyEvent::KEY_ESCAPE) {
        keyfocus=KeyFocusArea::Menu;
        ppltk::GetWindowManager()->setKeyboardFocus(menue);
        //select_audio->setSelected(true);
    }
    if (key == ppltk::KeyEvent::KEY_DOWN) {
        if (currentAudioSelection == SettingsAudio::Total) {
            currentAudioSelection = SettingsAudio::Music;
            audio_music_slider->setFocus();
        } else if (currentAudioSelection == SettingsAudio::Music) {
            currentAudioSelection = SettingsAudio::Effects;
            audio_effects_slider->setFocus();
        } else if (currentAudioSelection == SettingsAudio::Effects) {
            currentAudioSelection = SettingsAudio::Ambience;
            audio_ambience_slider->setFocus();
        } else if (currentAudioSelection == SettingsAudio::Ambience) {
            currentAudioSelection = SettingsAudio::Speech;
            audio_speech_slider->setFocus();
        }
    } else  if (key == ppltk::KeyEvent::KEY_UP) {
        if (currentAudioSelection == SettingsAudio::Speech) {
            currentAudioSelection = SettingsAudio::Ambience;
            audio_ambience_slider->setFocus();
        } else if (currentAudioSelection == SettingsAudio::Ambience) {
            currentAudioSelection = SettingsAudio::Effects;
            audio_effects_slider->setFocus();
        } else if (currentAudioSelection == SettingsAudio::Effects) {
            currentAudioSelection = SettingsAudio::Music;
            audio_music_slider->setFocus();
        } else if (currentAudioSelection == SettingsAudio::Music) {
            currentAudioSelection = SettingsAudio::Total;
            audio_total_slider->setFocus();
        }

    } else  if (key == ppltk::KeyEvent::KEY_LEFT) {
        ppltk::HorizontalSlider* slider=getCurrentAudioSlider();
        if (slider) {
            slider->setValue(slider->value() - 10);
            ppltk::Event ev(ppltk::Event::ValueChanged);
            ev.setWidget(slider);
            valueChangedEvent(&ev, slider->value());

        }
    } else  if (key == ppltk::KeyEvent::KEY_RIGHT) {
        ppltk::HorizontalSlider* slider=getCurrentAudioSlider();
        if (slider) {
            slider->setValue(slider->value() + 10);
            ppltk::Event ev(ppltk::Event::ValueChanged);
            ev.setWidget(slider);
            valueChangedEvent(&ev, slider->value());
        }

    }
}
void SettingsScreen::handleControllerKeyDownEvent(int key)
{
    if (key == ppltk::KeyEvent::KEY_ESCAPE) {
        keyfocus=KeyFocusArea::Menu;
        ppltk::GetWindowManager()->setKeyboardFocus(menue);
    }
    if (key == ppltk::KeyEvent::KEY_DOWN && static_cast<int>(currentControllerSelection) < static_cast<int>(SettingsController::ButtonFlashlight)) {
        currentControllerSelection=static_cast<SettingsController>(1 + static_cast<int>(currentControllerSelection));
        setFocusToControllerWidget();
    } else  if (key == ppltk::KeyEvent::KEY_UP && static_cast<int>(currentControllerSelection) > static_cast<int>(SettingsController::Deadzone)) {
        currentControllerSelection=static_cast<SettingsController>(static_cast<int>(currentControllerSelection) - 1);
        setFocusToControllerWidget();
    } else if (key == ppltk::KeyEvent::KEY_RETURN) {
        Decker::ui::ControllerButtonSelector* bs=getCurrentControllerInput();
        if (bs) bs->setInputmode();
    } else if (currentControllerSelection == SettingsController::Deadzone) {
        if (key == ppltk::KeyEvent::KEY_LEFT) controller_deadzone->setValue(controller_deadzone->value() - 1000);
        if (key == ppltk::KeyEvent::KEY_RIGHT)  controller_deadzone->setValue(controller_deadzone->value() + 1000);
        ppltk::Event ev(ppltk::Event::ValueChanged);
        ev.setWidget(controller_deadzone);
        valueChangedEvent(&ev, controller_deadzone->value());
    }
}

void SettingsScreen::setFocusToControllerWidget()
{
    switch (currentControllerSelection) {
        case SettingsController::Deadzone: controller_deadzone->setFocus(); break;
        case SettingsController::AxisWalk: controller_axis_walk->setFocus(); break;
        case SettingsController::AxisJump: controller_axis_jump->setFocus(); break;
        case SettingsController::ButtonUp: controller_button_up->setFocus(); break;
        case SettingsController::ButtonDown: controller_button_down->setFocus(); break;
        case SettingsController::ButtonLeft: controller_button_left->setFocus(); break;
        case SettingsController::ButtonRight: controller_button_right->setFocus(); break;
        case SettingsController::ButtonMenu: controller_button_menu->setFocus(); break;
        case SettingsController::ButtonAction: controller_button_action->setFocus(); break;
        case SettingsController::ButtonBack: controller_button_back->setFocus(); break;
        case SettingsController::ButtonJump: controller_button_jump->setFocus(); break;
        case SettingsController::ButtonCrouch: controller_button_crouch->setFocus(); break;
        case SettingsController::ButtonFlashlight: controller_button_flashlight->setFocus(); break;
    }
}

void SettingsScreen::keyDownEvent(ppltk::KeyEvent* event)
{
    handleKeyDownEvent(event->key);

}

void SettingsScreen::handleKeyDownEvent(int key)
{
    //ppl7::PrintDebugTime("SettingsScreen::keyDownEvent: key=%d, %s\n", event->key, (const  char*)event->widget()->name());

    if (keyfocus == KeyFocusArea::Menu) {
        handleMenuKeyDownEvent(key);
    } else if (keyfocus == KeyFocusArea::Audio) {
        handleAudioKeyDownEvent(key);
    } else if (keyfocus == KeyFocusArea::Controller) {
        handleControllerKeyDownEvent(key);
    }
}

void SettingsScreen::gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event)
{
    GameControllerMapping::Button b=game.controller.mapping.getButton(event);
    if (b == GameControllerMapping::Button::Menu) {
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        this->getParent()->closeEvent(&event);
    }

    if (b == GameControllerMapping::Button::MenuDown) handleKeyDownEvent(ppltk::KeyEvent::KEY_DOWN);
    else if (b == GameControllerMapping::Button::MenuUp) handleKeyDownEvent(ppltk::KeyEvent::KEY_UP);
    else if (b == GameControllerMapping::Button::MenuLeft) handleKeyDownEvent(ppltk::KeyEvent::KEY_LEFT);
    else if (b == GameControllerMapping::Button::MenuRight) handleKeyDownEvent(ppltk::KeyEvent::KEY_RIGHT);
    else if (b == GameControllerMapping::Button::Action) handleKeyDownEvent(ppltk::KeyEvent::KEY_RETURN);
    else if (b == GameControllerMapping::Button::Back) handleKeyDownEvent(ppltk::KeyEvent::KEY_ESCAPE);


}

void SettingsScreen::gameControllerAxisMotionEvent(ppltk::GameControllerAxisEvent* event)
{
    GameControllerMapping::Axis axis=game.controller.mapping.getAxis(event);
    if (axis == GameControllerMapping::Axis::Walk && currentMenueSelection == SettingsMenue::Audio) {
        ppltk::HorizontalSlider* slider=getCurrentAudioSlider();
        if (slider) {
            if (event->value < -10000) slider->setValue(slider->value() - 10);
            if (event->value > 10000) slider->setValue(slider->value() + 10);
            ppltk::Event ev(ppltk::Event::ValueChanged);
            ev.setWidget(slider);
            valueChangedEvent(&ev, slider->value());
        }

    }
    if (axis == GameControllerMapping::Axis::Walk && currentMenueSelection == SettingsMenue::Controller) {
        if (currentControllerSelection == SettingsController::Deadzone) {
            if (event->value < -10000) controller_deadzone->setValue(controller_deadzone->value() - 1000);
            if (event->value > 10000) controller_deadzone->setValue(controller_deadzone->value() + 1000);
            ppltk::Event ev(ppltk::Event::ValueChanged);
            ev.setWidget(controller_deadzone);
            valueChangedEvent(&ev, controller_deadzone->value());
        }

    }

}

void SettingsScreen::gameControllerDeviceAdded(ppltk::GameControllerEvent* event)
{
    GetGame().gameControllerDeviceAdded(event);
}

void SettingsScreen::gameControllerDeviceRemoved(ppltk::GameControllerEvent* event)
{
    GetGame().gameControllerDeviceRemoved(event);
}



void SettingsScreen::mouseEnterEvent(ppltk::MouseEvent* event)
{
    if (event->widget() == select_audio) selectSettingsPage(SettingsMenue::Audio);
    else if (event->widget() == select_video) selectSettingsPage(SettingsMenue::Video);
    else if (event->widget() == select_controller) selectSettingsPage(SettingsMenue::Controller);
    else if (event->widget() == select_misc) selectSettingsPage(SettingsMenue::Misc);
    else if (event->widget() == select_back) selectSettingsPage(SettingsMenue::Back);
    else if (select_menue != NULL && event->widget() == select_menue) selectSettingsPage(SettingsMenue::Menue);
    else if (select_game_exit != NULL && event->widget() == select_game_exit) selectSettingsPage(SettingsMenue::Exit);
}


void SettingsScreen::mouseClickEvent(ppltk::MouseEvent* event)
{
    if (event->widget() == select_back) {
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        event.setCustomId(0);
        this->getParent()->closeEvent(&event);
    } else if (select_menue != NULL && event->widget() == select_menue) {
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        event.setCustomId(1);
        this->getParent()->closeEvent(&event);
    } else if (select_game_exit != NULL && event->widget() == select_game_exit) {
        ppltk::Event event(ppltk::Event::Close);
        event.setWidget(this);
        event.setCustomId(2);
        this->getParent()->closeEvent(&event);
    } else if (event->widget() == save_video_settings_button) {
        Config::WindowMode mode=static_cast<Config::WindowMode>(windowmode_combobox->currentIdentifier().toInt());

        ppltk::WindowManager_SDL2* wm=(ppltk::WindowManager_SDL2*)ppltk::GetWindowManager();
        wm->changeWindowMode(game.window(), mode);
        ppltk::Window::DisplayMode dmode;
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


void SettingsScreen::valueChangedEvent(ppltk::Event* event, int64_t value)
{
    if (event->widget() == video_device_combobox) updateVideoModes();
    else if (event->widget() == audio_total_slider) {
        game.config.volumeTotal=(float)audio_total_slider->value() / 255.0f;
        game.audiosystem.setGlobalVolume(game.config.volumeTotal);
        game.config.save();
        currentAudioSelection = SettingsAudio::Total;
    } else if (event->widget() == audio_music_slider) {
        game.config.volumeMusic=(float)audio_music_slider->value() / 512.0f;
        game.audiosystem.setVolume(AudioClass::Music, game.config.volumeMusic);
        game.config.save();
        currentAudioSelection = SettingsAudio::Music;
    } else if (event->widget() == audio_effects_slider) {
        game.config.volumeEffects=(float)audio_effects_slider->value() / 255.0f;
        game.audiosystem.setVolume(AudioClass::Effect, game.config.volumeEffects);
        game.config.save();
        currentAudioSelection = SettingsAudio::Effects;
    } else if (event->widget() == audio_ambience_slider) {
        game.config.volumeAmbience=(float)audio_ambience_slider->value() / 255.0f;
        game.audiosystem.setVolume(AudioClass::Ambience, game.config.volumeAmbience);
        game.config.save();
        currentAudioSelection = SettingsAudio::Ambience;
    } else if (event->widget() == audio_speech_slider) {
        game.config.volumeSpeech=(float)audio_speech_slider->value() / 255.0f;
        game.audiosystem.setVolume(AudioClass::Speech, game.config.volumeSpeech);
        game.config.save();
        currentAudioSelection = SettingsAudio::Speech;
    } else if (event->widget() == difficulty_slider) {
        game.config.difficulty=static_cast<Config::DifficultyLevel>(difficulty_slider->value());
        updateDifficultyDescription();
        game.config.save();
    } else if (event->widget() == controller_deadzone) {
        game.config.controller.deadzone=value;
        game.controller.setDeadzone(value);
        game.config.save();
    }

}

void SettingsScreen::valueChangedEvent(ppltk::Event* event, int value)
{
    if (event->widget() == text_language_combobox) {
        game.config.TextLanguage=text_language_combobox->currentIdentifier();
        game.config.save();
        GetTranslator().setLanguage(game.config.TextLanguage);
        retranslateUi();
    } else if (event->widget() == speech_language_combobox) {
        game.config.SpeechLanguage=speech_language_combobox->currentIdentifier();
        game.config.save();
    } else if (event->widget() == getCurrentControllerInput()) {
        ppltk::GetWindowManager()->setGameControllerFocus(this);
        if (event->widget() == controller_axis_walk) game.config.controller.axis_walk=value;
        else if (event->widget() == controller_axis_jump) game.config.controller.axis_jump=value;
        else if (event->widget() == controller_button_up) game.config.controller.button_up=value;
        else if (event->widget() == controller_button_down) game.config.controller.button_down=value;
        else if (event->widget() == controller_button_left) game.config.controller.button_left=value;
        else if (event->widget() == controller_button_right) game.config.controller.button_right=value;
        else if (event->widget() == controller_button_menu) game.config.controller.button_menu=value;
        else if (event->widget() == controller_button_back) game.config.controller.button_back=value;
        else if (event->widget() == controller_button_action) game.config.controller.button_action=value;
        else if (event->widget() == controller_button_jump) game.config.controller.button_jump=value;
        else if (event->widget() == controller_button_crouch) game.config.controller.button_crouch=value;
        else if (event->widget() == controller_button_flashlight) game.config.controller.button_flashlight=value;
        game.config.save();
        game.updateGameControllerMapping();
        //ppl7::PrintDebugTime("SettingsScreen::valueChangedEvent: %d\n", value);
    }


}

void SettingsScreen::resizeEvent(ppltk::ResizeEvent* event)
{
    //printf("SettingsScreen: we got a resize event: %d x %d\n", width(), height());
    setupUi();
    selectSettingsPage(currentMenueSelection);

    needsRedraw();

}

void SettingsScreen::toggledEvent(ppltk::Event* event, bool checked)
{
    if (event->widget() == skipIntro_checkbox) {
        //ppl7::PrintDebugTime("toggle\n");
        game.config.skipIntro=checked;
        game.config.save();
    } else if (event->widget() == noBlood_checkbox) {
        //ppl7::PrintDebugTime("toggle\n");
        game.config.noBlood=checked;
        game.config.save();
    } else if (event->widget() == controller_use_rumble) {
        //ppl7::PrintDebugTime("toggle\n");
        game.config.controller.use_rumble=checked;
        game.config.save();
    }
}
