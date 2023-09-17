#include "decker.h"
#include "decker_sdl.h"

const int CONFIG_VERSION=3;

Config::Config()
{
    ppl7::String config_path=ppl7::Dir::applicationDataPath(APP_COMPANY, APP_NAME);
    ConfigFile=config_path + "/decker.conf";
    windowMode=WindowMode::FullscreenDesktop;
    CustomLevelPath=ppl7::Dir::documentsPath(APP_COMPANY, APP_NAME);
    SDL::DisplayMode mode=SDL::desktopDisplayMode();
    ScreenResolution.setSize(mode.width, mode.height);
    ScreenRefreshRate=mode.refresh_rate;
    volumeTotal=1.0f;
    volumeMusic=0.3f;
    volumeEffects=0.5f;
    volumeAmbience=0.7f;
    volumeSpeech=1.0f;
    videoDevice=0;
    TextLanguage="en";
    SpeechLanguage="en";
    tutorialPlayed=false;
    noBlood=false;
    skipIntro=false;
    difficulty=DifficultyLevel::normal;

    try {
        load();
    } catch (...) {}
}

Config::Controller::Controller() {
    GameControllerMapping gm;
    axis_walk=gm.getSDLAxis(GameControllerMapping::Axis::Walk);
    axis_jump=gm.getSDLAxis(GameControllerMapping::Axis::Jump);
    button_up=gm.getSDLButton(GameControllerMapping::Button::MenuUp);
    button_down=gm.getSDLButton(GameControllerMapping::Button::MenuDown);
    button_left=gm.getSDLButton(GameControllerMapping::Button::MenuLeft);
    button_right=gm.getSDLButton(GameControllerMapping::Button::MenuRight);
    button_action=gm.getSDLButton(GameControllerMapping::Button::Action);
    button_menu=gm.getSDLButton(GameControllerMapping::Button::Menu);
    button_back=gm.getSDLButton(GameControllerMapping::Button::Back);
    button_jump=gm.getSDLButton(GameControllerMapping::Button::Jump);
    GameController gc;
    deadzone=gc.deadzone();
    use_rumble=true;
}

Config::~Config()
{

}

void Config::load()
{
    if (!ppl7::File::exists(ConfigFile)) return;
    ppl7::ConfigParser conf;
    conf.load(ConfigFile);

    // Misc
    conf.setSection("misc");
    int version=conf.getInt("ConfigVersion", 1);
    if (version >= 3) {
        CustomLevelPath=conf.get("CustomLevelPath", CustomLevelPath);
    }
    LastEditorLevel=conf.get("LastEditorLevel", LastEditorLevel);
    TextLanguage=conf.get("TextLanguage", TextLanguage);
    SpeechLanguage=conf.get("SpeechLanguage", SpeechLanguage);
    tutorialPlayed=conf.getBool("tutorialPlayed", false);
    skipIntro=conf.getBool("skipIntro", false);
    noBlood=conf.getBool("noBlood", false);
    difficulty=static_cast<DifficultyLevel>(conf.getInt("difficulty", static_cast<int>(DifficultyLevel::normal)));


    // Video
    conf.setSection("video");
    ScreenResolution.width=conf.getInt("ScreenResolution.width", ScreenResolution.width);
    ScreenResolution.height=conf.getInt("ScreenResolution.height", ScreenResolution.height);
    ScreenRefreshRate=conf.getInt("ScreenRefreshRate", ScreenRefreshRate);
    videoDevice=conf.getInt("videoDevice", videoDevice);
    windowMode=static_cast<WindowMode>(conf.getInt("windowMode", static_cast<int>(windowMode)));

    // Audio
    conf.setSection("audio");
    volumeTotal=conf.get("volumeTotal", ppl7::ToString("%0.3f", volumeTotal)).toFloat();
    volumeMusic=conf.get("volumeMusic", ppl7::ToString("%0.3f", volumeMusic)).toFloat();
    volumeAmbience=conf.get("volumeAmbience", ppl7::ToString("%0.3f", volumeAmbience)).toFloat();
    volumeEffects=conf.get("volumeEffects", ppl7::ToString("%0.3f", volumeEffects)).toFloat();
    volumeSpeech=conf.get("volumeSpeech", ppl7::ToString("%0.3f", volumeSpeech)).toFloat();
    //if (volumeMusic > 0.5f) volumeMusic=0.5f;

    // Controller
    conf.setSection("controller");
    controller.deadzone=conf.getInt("deadzone", controller.deadzone);
    controller.use_rumble=conf.getBool("use_rumble", controller.use_rumble);
    controller.axis_walk=conf.getInt("axis_walk", controller.axis_walk);
    controller.axis_jump=conf.getInt("axis_jump", controller.axis_jump);
    controller.button_up=conf.getInt("button_up", controller.button_up);
    controller.button_down=conf.getInt("button_down", controller.button_down);
    controller.button_left=conf.getInt("button_left", controller.button_left);
    controller.button_right=conf.getInt("button_right", controller.button_right);
    controller.button_menu=conf.getInt("button_menu", controller.button_menu);
    controller.button_back=conf.getInt("button_back", controller.button_back);
    controller.button_action=conf.getInt("button_action", controller.button_action);
    controller.button_jump=conf.getInt("button_jump", controller.button_jump);
}

void Config::save()
{
    if (!ppl7::File::exists(ConfigFile)) {
        ppl7::String config_path=ppl7::Dir::applicationDataPath(APP_COMPANY, APP_NAME);
        if (!ppl7::Dir::exists(config_path)) {
            ppl7::Dir::mkDir(config_path, true);
        }
    }
    ppl7::ConfigParser conf;

    // Video
    conf.setSection("video");
    conf.add("ScreenResolution.width", ScreenResolution.width);
    conf.add("ScreenResolution.height", ScreenResolution.height);
    conf.add("ScreenRefreshRate", ScreenRefreshRate);
    conf.add("videoDevice", videoDevice);
    conf.add("windowMode", static_cast<int>(windowMode));

    // Audio
    conf.setSection("audio");
    conf.add("volumeTotal", ppl7::ToString("%0.3f", volumeTotal));
    conf.add("volumeMusic", ppl7::ToString("%0.3f", volumeMusic));
    conf.add("volumeEffects", ppl7::ToString("%0.3f", volumeEffects));
    conf.add("volumeAmbience", ppl7::ToString("%0.3f", volumeAmbience));
    conf.add("volumeSpeech", ppl7::ToString("%0.3f", volumeSpeech));

    // Misc
    conf.setSection("misc");
    conf.add("CustomLevelPath", CustomLevelPath);
    conf.add("LastEditorLevel", LastEditorLevel);
    conf.add("TextLanguage", TextLanguage);
    conf.add("SpeechLanguage", SpeechLanguage);
    conf.add("ConfigVersion", CONFIG_VERSION);
    conf.add("tutorialPlayed", tutorialPlayed);
    conf.add("skipIntro", skipIntro);
    conf.add("noBlood", noBlood);
    conf.add("difficulty", static_cast<int>(difficulty));
    conf.save(ConfigFile);

    // Controller
    conf.add("deadzone", controller.deadzone);
    conf.add("use_rumble", controller.use_rumble);
    conf.add("axis_walk", controller.axis_walk);
    conf.add("axis_jump", controller.axis_jump);
    conf.add("button_up", controller.button_up);
    conf.add("button_down", controller.button_down);
    conf.add("button_left", controller.button_left);
    conf.add("button_right", controller.button_right);
    conf.add("button_menu", controller.button_menu);
    conf.add("button_back", controller.button_back);
    conf.add("button_action", controller.button_action);
    conf.add("button_jump", controller.button_jump);


}
