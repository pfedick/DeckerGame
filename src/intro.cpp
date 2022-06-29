#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "intro.h"


static AVPixelFormat correct_for_deprecated_pixel_format(AVPixelFormat pix_fmt) {
	// Fix swscaler deprecated pixel format warning
	// (YUVJ has been deprecated, change pixel format to regular YUV)
	switch (pix_fmt) {
	case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
	case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
	case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
	case AV_PIX_FMT_YUVJ440P: return AV_PIX_FMT_YUV440P;
	default:                  return pix_fmt;
	}
}

void Game::playIntroVideo()
{
	controlsEnabled=false;
	world_widget->setVisible(false);
	world_widget->setEnabled(false);
	ppl7::grafix::Color black(0, 0, 0, 255);
	SDL_Renderer* renderer=sdl.getRenderer();
	IntroVideo* intro_widget=new IntroVideo(sdl);
	intro_widget->create(0, 0, this->width(), this->height());
	//ppl7::String filename="/home/patrickf/Videos/Denic/Sprint 10/anchorman2-raw.avi";
	//ppl7::String filename="res/video/decker2_intro-vp9-4000k.webm";
	ppl7::String filename="res/video/decker2_intro.mp4";

	if (intro_widget->load(filename)) {
		this->addChild(intro_widget);
		wm->setKeyboardFocus(intro_widget);
		showUi(false);
		int frame=0;

		while (!intro_widget->stopSignal()) {
			wm->handleEvents();
			sdl.startFrame(black);
			ppl7::tk::MouseState mouse=wm->getMouseState();
			//drawWidgets();
			if (frame % 2 == 0)
				if (!intro_widget->nextFrame()) break;

			intro_widget->renderFrame(renderer);
			resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
			presentScreen();
			frame++;
		}
		this->removeChild(intro_widget);
	}
	delete intro_widget;
}


IntroVideo::IntroVideo(SDL& s)
	:sdl(s)
{
	wm=ppl7::tk::GetWindowManager();
	stop_playback=false;
	av_format_ctx=NULL;
	av_codec=NULL;
	av_codec_ctx=NULL;
	av_codec_params=NULL;
	overlay=NULL;
	sws_ctx = NULL;
	av_frame=NULL;
	av_packet=NULL;
	videoStream=-1;
}

IntroVideo::~IntroVideo()
{
	clear();
}

void IntroVideo::clear()
{
	if (av_packet) {
		av_packet_free(&av_packet);
		av_packet=NULL;
	}
	if (av_frame) {
		av_frame_free(&av_frame);
		av_frame=NULL;
	}
	if (sws_ctx) {
		sws_freeContext(sws_ctx);
		sws_ctx=NULL;
	}
	if (overlay) {
		SDL_DestroyTexture(overlay);
	}
	if (av_codec_ctx) {
		avcodec_free_context(&av_codec_ctx);
		av_codec_ctx=NULL;
	}
	if (av_format_ctx) {
		avformat_close_input(&av_format_ctx);
		avformat_free_context(av_format_ctx);
		av_format_ctx=NULL;
	}
}

bool IntroVideo::load(const ppl7::String& filename)
{
	clear();
	av_format_ctx = avformat_alloc_context();
	if (!av_format_ctx) {
		return false;
	}
	// Open video file
	if (avformat_open_input(&av_format_ctx, (const char*)filename, NULL, NULL) != 0) {
		return false;
	}
	// Retrieve stream information
	if (avformat_find_stream_info(av_format_ctx, NULL) < 0)
		return false; // Couldn't find stream information
	// Dump information about file onto standard error
	//av_dump_format(av_format_ctx, 0, (const char*)filename, 0);

	av_codec_params=NULL;
	av_codec=NULL;

	// Find the first video stream
	videoStream=-1;
	for (unsigned int i=0; i < av_format_ctx->nb_streams; ++i) {
		AVCodecParameters* local_av_codec_params=av_format_ctx->streams[i]->codecpar;
		const AVCodec* local_av_codec=avcodec_find_decoder(local_av_codec_params->codec_id);
		if (!local_av_codec) {
			continue;
		}
		if (local_av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
			if (videoStream == -1) {
				videoStream=i;
				av_codec=local_av_codec;
				av_codec_params=local_av_codec_params;
			}
		}
		/*
		printf("Codec %s ID %d bit_rate %ld, resolution: %d x %d\n",
			local_av_codec->name, local_av_codec->id, local_av_codec_params->bit_rate,
			local_av_codec_params->width, local_av_codec_params->height);
			*/
	}
	if (videoStream == -1)
		return false; // Didn't find a video stream


	// Get a pointer to the codec context for the video stream
	av_codec_ctx=avcodec_alloc_context3(av_codec);
	if (!av_codec_ctx) {
		return false;
	}

	if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
		return false;
	}

	if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
		return false;
	}

	// Allocate video frame
	av_frame = av_frame_alloc();
	if (!av_frame) return false;

	av_packet = av_packet_alloc();
	if (!av_packet) {
		return false;
	}


	overlay = SDL_CreateTexture(
		sdl.getRenderer(),
		SDL_PIXELFORMAT_YV12,
		SDL_TEXTUREACCESS_STREAMING,
		av_codec_ctx->width,
		av_codec_ctx->height
	);
	if (!overlay) return false;
	// initialize SWS context for software scaling
	auto source_pix_fmt = correct_for_deprecated_pixel_format(av_codec_ctx->pix_fmt);
	sws_ctx = sws_getContext(av_codec_ctx->width,
		av_codec_ctx->height,
		source_pix_fmt,
		av_codec_ctx->width,
		av_codec_ctx->height,
		AV_PIX_FMT_YUV420P,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
	);
	if (!sws_ctx) return false;
	return true;
}

bool IntroVideo::nextFrame()
{
	int response;
	while (av_read_frame(av_format_ctx, av_packet) >= 0) {
		//printf("read frame of size %d, pos: %lu, dts: %lu\n", av_packet->size, av_packet->pos,
		//	av_packet->dts);
		// Is this a packet from the video stream?
		if (av_packet->stream_index == videoStream) {
			response = avcodec_send_packet(av_codec_ctx, av_packet);
			if (response < 0) {
				printf("avcodec_send_packet failed\n");
				av_packet_unref(av_packet);
				return false;
			}
			response = avcodec_receive_frame(av_codec_ctx, av_frame);
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				av_packet_unref(av_packet);
				continue;
			}
			if (response < 0) {
				printf("avcodec_receive_frame failed\n");
				av_packet_unref(av_packet);
				return false;
			}
			av_packet_unref(av_packet);
			//printf("we have a frame: %d x %d\n", av_frame->width, av_frame->height);
			updateOverlay(av_frame);
			return true;
		}
		av_packet_unref(av_packet);
	}
	return false;
}

void IntroVideo::updateOverlay(AVFrame* frame)
{

	AVFrame* frame2 = av_frame_alloc();
	int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, frame->width, frame->height, 32);
	uint8_t* frame2_buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
	av_image_fill_arrays(frame2->data, frame2->linesize, frame2_buffer, AV_PIX_FMT_YUV420P, av_codec_ctx->width,
		av_codec_ctx->height, 32);
	sws_scale(sws_ctx, frame->data, frame->linesize, 0, av_codec_ctx->height, frame2->data, frame2->linesize);

	//int w, h;
	//SDL_QueryTexture(vs->Texture, NULL, NULL, &w, &h);

	//sws_scale(sws_ctx,frame->data, frame->linesize,);

	SDL_UpdateYUVTexture(overlay,
		NULL,
		frame2->data[0],
		frame2->linesize[0],
		frame2->data[1],
		frame2->linesize[1],
		frame2->data[2],
		frame2->linesize[2]);
	av_freep(&frame2_buffer);
	av_freep(&frame2);

}

void IntroVideo::renderFrame(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, overlay, NULL, NULL);
}



bool IntroVideo::stopSignal() const
{
	return stop_playback;
}

void IntroVideo::keyDownEvent(ppl7::tk::KeyEvent* event)
{
	if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE) {
		stop_playback=true;

	}
}
