#ifndef INCLUDE_VIDEOPLAYER_H_
#define INCLUDE_VIDEOPLAYER_H_

#include <SDL.h>

#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "decker_sdl.h"

#include "audio.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}


class VideoPlayer
{
private:
    SDL_Renderer* renderer;
    SDL_Texture* overlay;
    AVFormatContext* av_format_ctx;
    const AVCodec* av_codec;
    AVCodecContext* av_codec_ctx;
    AVCodecParameters* av_codec_params;
    struct SwsContext* sws_ctx;
    AVFrame* av_frame;
    AVPacket* av_packet;
    int videoStream;

    void updateOverlay(AVFrame* frame);

public:
    VideoPlayer(SDL_Renderer* renderer);
    ~VideoPlayer();
    void clear();
    bool load(const ppl7::String& filename);
    bool nextFrame();
    int width() const;
    int height() const;

    void renderFrame(const SDL_Rect* dstrect=NULL);
    SDL_Texture* getVideoTexture();

};

#endif
