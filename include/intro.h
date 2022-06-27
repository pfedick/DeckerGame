#ifndef INCLUDE_INTRO_H_
#define INCLUDE_INTRO_H_

#include <SDL.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "decker_sdl.h"

#include "audio.h"

class IntroVideo : public ppl7::tk::Widget
{
private:
    SDL& sdl;
    ppl7::tk::WindowManager* wm;
    bool stop_playback;

    SDL_Texture* overlay;

    AVFormatContext* av_format_ctx;
    const AVCodec* av_codec;
    AVCodecContext* av_codec_ctx;
    AVCodecParameters* av_codec_params;
    struct SwsContext* sws_ctx;
    AVFrame* av_frame;

    uint8_t* yPlane, * uPlane, * vPlane;
    size_t yPlaneSz, uvPlaneSz;
    AVPacket* av_packet;
    int videoStream;
    int uvPitch;

public:
    IntroVideo(SDL& s);
    ~IntroVideo();

    void clear();
    bool load(const ppl7::String& filename);
    void nextFrame(SDL_Renderer* renderer);

    bool stopSignal() const;

    // EventHandler
    void keyDownEvent(ppl7::tk::KeyEvent* event);

};


#endif // INCLUDE_INTRO_H_
