#include "decker.h"
#include "decker_sdl.h"


Config::Config()
{
    ppl7::String config_path=ppl7::Dir::applicationDataPath(APP_COMPANY, APP_NAME);
    ConfigFile=config_path + "/decker.conf";

    windowMode=WindowMode::FullscreenDesktop;
    CustomLevelPath=ppl7::Dir::applicationDataPath(APP_COMPANY, APP_NAME);
    SDL::DisplayMode mode=SDL::desktopDisplayMode();
    ScreenResolution.setSize(mode.width, mode.height);
    ScreenRefreshRate=mode.refresh_rate;
    volumeTotal=1.0f;
    volumeMusic=0.5f;
    volumeEffects=1.0f;
    videoDevice=0;
    audioDevice="";
    Language="en";
    try {
        load();
    }
    catch (...) {}
}

Config::~Config()
{

}

void Config::load()
{
    if (!ppl7::File::exists(ConfigFile)) return;
    ppl7::ConfigParser conf;
    conf.load(ConfigFile);

    // Video
    conf.setSection("video");
    ScreenResolution.width=conf.getInt("ScreenResolution.width", ScreenResolution.width);
    ScreenResolution.height=conf.getInt("ScreenResolution.height", ScreenResolution.height);
    ScreenRefreshRate=conf.getInt("ScreenRefreshRate", ScreenRefreshRate);
    videoDevice=conf.getInt("videoDevice", videoDevice);
    windowMode=static_cast<WindowMode>(conf.getInt("windowMode", static_cast<int>(windowMode)));

    // Audio
    conf.setSection("audio");
    audioDevice=conf.get("audioDevice", audioDevice);
    volumeTotal=conf.get("volumeTotal", ppl7::ToString("%0.3f", volumeTotal)).toFloat();
    volumeMusic=conf.get("volumeMusic", ppl7::ToString("%0.3f", volumeMusic)).toFloat();
    volumeMusic=conf.get("volumeEffects", ppl7::ToString("%0.3f", volumeEffects)).toFloat();

    // Misc
    conf.setSection("misc");
    CustomLevelPath=conf.get("CustomLevelPath", CustomLevelPath);
    LastEditorLevel=conf.get("LastEditorLevel", LastEditorLevel);
    Language=conf.get("Language", Language);
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
    conf.add("audioDevice", audioDevice);
    conf.add("volumeTotal", ppl7::ToString("%0.3f", volumeTotal));
    conf.add("volumeMusic", ppl7::ToString("%0.3f", volumeMusic));
    conf.add("volumeEffects", ppl7::ToString("%0.3f", volumeEffects));

    // Misc
    conf.setSection("misc");
    conf.add("CustomLevelPath", CustomLevelPath);
    conf.add("LastEditorLevel", LastEditorLevel);
    conf.add("Language", Language);

    conf.save(ConfigFile);
}
