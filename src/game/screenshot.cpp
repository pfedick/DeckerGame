#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>


Screenshot::Screenshot(Mode m)
{
    ppl7::DateTime now=ppl7::DateTime::currentTime();
    Filename.setf("decker_%s", (const char*)now.getISO8601withMsec());
    Filename.replace(":", "-");
    myMode=m;
    done_flag=false;
}

void Screenshot::setPath(const ppl7::String& path)
{
    this->Path=path;
}

const ppl7::grafix::Drawable& Screenshot::image() const
{
    return img;
}

bool Screenshot::isDone() const
{
    return done_flag;
}

Screenshot::Mode Screenshot::mode() const
{
    return myMode;
}

void Screenshot::save(LightPlaneId lplane, LightPlayerPlaneMatrix pplane, Type type, SDL_Texture* texture)
{
    if (done_flag) return;
    Layer layer=Layer::Complete;
    switch (lplane) {
        case LightPlaneId::Horizon: layer=Layer::Horizon; break;
        case LightPlaneId::Far: layer=Layer::Far; break;
        case LightPlaneId::Middle: layer=Layer::Middle; break;
        case LightPlaneId::Player:
            switch (pplane) {
                case LightPlayerPlaneMatrix::Back: layer=Layer::Back; break;
                case LightPlayerPlaneMatrix::Player: layer=Layer::Player; break;
                case LightPlayerPlaneMatrix::Front: layer=Layer::Front; break;
                default: break;
            }
            break;
        case LightPlaneId::Near: layer=Layer::Near; break;
        default: break;
    }
    save(layer, type, texture);
}

void Screenshot::save(Layer layer, Type type, SDL_Texture* texture)
{
    if (done_flag) return;
    if (myMode == Mode::Memory && (layer != Layer::Complete || type != Type::Final)) return;
    ppl7::String filename=Path;
    if (Path.notEmpty()) filename+="/";
    filename+=this->Filename;
    filename.appendf("_layer%02d_", static_cast<int>(layer));
    switch (layer) {
        case Layer::Background: filename+="background"; break;
        case Layer::Horizon: filename+="horizon"; break;
        case Layer::Far: filename+="far"; break;
        case Layer::Middle: filename+="middle"; break;
        case Layer::Back: filename+="back"; break;
        case Layer::Player: filename+="player"; break;
        case Layer::Front: filename+="front"; break;
        case Layer::Near: filename+="near"; break;
        case Layer::Complete: filename+="complete"; break;
    }
    filename+="_";
    switch (type) {
        case Type::Color: filename+="0_color"; break;
        case Type::Lightmap: filename+="1_lightmap"; break;
        case Type::Final: filename+="2_final"; break;
    }
    filename+=".png";
    ppl7::PrintDebugTime("screenshot: %s\n", (const char*)filename);

    SDL_Renderer* renderer=GetGame().getSDLRenderer();
    if (!renderer) return;
    int width, height;
    if (SDL_QueryTexture(texture, NULL, NULL, &width, &height) != 0) return;
    void* pixels=malloc(width * 4 * height);
    if (!pixels) return;

    try {
        //ppl7::PrintDebug("try read pixel: %d x %d\n", width, height);
        if (SDL_RenderReadPixels(renderer, NULL, 0, pixels, width * 4) != 0) {
            ppl7::String err(SDL_GetError());
            throw SDLException("Couldn't read pixel from render target: " + err);
        }
        //ppl7::PrintDebug("create drawable\n");
        ppl7::grafix::Drawable draw(pixels, 4 * width, width, height, ppl7::grafix::RGBFormat::A8R8G8B8);
        if (myMode == Mode::Memory) {
            done_flag=true;
            img=draw;
        } else {
            ppl7::grafix::ImageFilter_PNG png;
            //ppl7::PrintDebug("save png\n");
            png.saveFile(filename, draw);
        }
        //ppl7::PrintDebug("done\n");
    } catch (const ppl7::Exception& exp) {
        exp.print();
        ppl7::PrintDebug("\n");
    }
    if (pixels) free(pixels);
}



void Game::TakeScreenshot()
{
    if (screenshot) level.TakeScreenshot(NULL);
    delete screenshot;
    screenshot=new Screenshot(Screenshot::Mode::File);
    level.TakeScreenshot(screenshot);
}


void Game::TakeScreenshot(Screenshot* screenshot)
{
    if (this->screenshot) {
        ppl7::PrintDebugTime("ERROR: can't take screenshot, as it always seems to be in progress!\n");
        return;
    }
    this->screenshot=screenshot;
    level.TakeScreenshot(screenshot);
}
