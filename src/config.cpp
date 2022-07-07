#include "decker.h"


Config::Config()
{
    bFullScreen=false;
    CustomLevelPath=ppl7::Dir::applicationDataPath(APP_COMPANY, APP_NAME);
    ScreenResolution.setSize(1920,1080);
}

Config::~Config()
{

}

void Config::load()
{

}

void Config::save()
{

}