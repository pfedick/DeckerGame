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

    try {
        load();
    } catch (...) {}
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
    deadzone=conf.getInt("deadzone", deadzone);
    axis_walk=conf.getInt("axis_walk", axis_walk);
    axis_jump=conf.getInt("axis_jump", axis_jump);
    button_up=conf.getInt("button_up", button_up);
    button_down=conf.getInt("button_down", button_down);
    button_left=conf.getInt("button_left", button_left);
    button_right=conf.getInt("button_right", button_right);
    button_menu=conf.getInt("button_menu", button_menu);
    button_back=conf.getInt("button_back", button_back);
    button_action=conf.getInt("button_action", button_action);
    button_jump=conf.getInt("button_jump", button_jump);
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
    conf.add("deadzone", deadzone);
    conf.add("axis_walk", axis_walk);
    conf.add("axis_jump", axis_jump);
    conf.add("button_up", button_up);
    conf.add("button_down", button_down);
    conf.add("button_left", button_left);
    conf.add("button_right", button_right);
    conf.add("button_menu", button_menu);
    conf.add("button_back", button_back);
    conf.add("button_action", button_action);
    conf.add("button_jump", button_jump);


}
