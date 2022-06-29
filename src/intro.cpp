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

		while (!intro_widget->stopSignal()) {
			wm->handleEvents();
			sdl.startFrame(black);
			ppl7::tk::MouseState mouse=wm->getMouseState();
			//drawWidgets();
			intro_widget->nextFrame(renderer);
			resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
			presentScreen();
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
	yPlane=uPlane=vPlane=NULL;
	av_packet=NULL;
	yPlaneSz=0;
	uvPlaneSz=0;
	uvPitch=0;
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
	free(yPlane);
	free(uPlane);
	free(vPlane);
	yPlane=uPlane=vPlane=NULL;
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
	av_dump_format(av_format_ctx, 0, (const char*)filename, 0);

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
	printf("video width=%d, height=%d\n", av_codec_ctx->width,
		av_codec_ctx->height);
	// initialize SWS context for software scaling
	/*
	auto source_pix_fmt = correct_for_deprecated_pixel_format(av_codec_ctx->pix_fmt);
	printf("debug 1\n");
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
	*/
	printf("debug 2\n");


	// set up YV12 pixel array (12 bits per pixel)
	yPlaneSz = av_codec_ctx->width * av_codec_ctx->height;
	uvPlaneSz = av_codec_ctx->width * av_codec_ctx->height / 4;
	yPlane = (Uint8*)malloc(yPlaneSz);
	uPlane = (Uint8*)malloc(uvPlaneSz);
	vPlane = (Uint8*)malloc(uvPlaneSz);
	if (!yPlane || !uPlane || !vPlane) {
		return false;
	}
	uvPitch = av_codec_ctx->width / 2;
	printf("init complete\n");
	return true;
}

void IntroVideo::nextFrame(SDL_Renderer* renderer)
{
	int response;
	while (av_read_frame(av_format_ctx, av_packet) >= 0) {
		printf("read frame of size %d, pos: %lu, dts: %lu\n", av_packet->size, av_packet->pos,
		av_packet->dts);
		// Is this a packet from the video stream?
		if (av_packet->stream_index == videoStream) {
			response = avcodec_send_packet(av_codec_ctx, av_packet);
			if (response < 0) {
				printf("avcodec_send_packet failed\n");
				av_packet_unref(av_packet);
				return;
			}
			response = avcodec_receive_frame(av_codec_ctx, av_frame);
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				av_packet_unref(av_packet);
				continue;
			}
			if (response < 0) {
				printf("avcodec_receive_frame failed\n");
				av_packet_unref(av_packet);
				return;
			}
			av_packet_unref(av_packet);
			printf ("we have a frame: %d x %d\n", av_frame->width, av_frame->height);
			break;
		}
		av_packet_unref(av_packet);
	}
	return;
	/*

		printf("avcodec_send_packet\n");
		response = avcodec_send_packet(av_codec_ctx, av_packet);
		if (response < 0) {
			av_packet_unref(av_packet);
			return;
		}
		printf("debug 3\n");
		response = avcodec_receive_frame(av_codec_ctx, av_frame);
		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			av_packet_unref(av_packet);
			continue;
		} else if (response < 0) {
			//printf("Failed to decode packet: %s\n", av_make_error(response));
			return;
		}
		av_packet_unref(av_packet);
		break;

	}
	*/
	printf("soweit so gut...\n");
	if (!sws_ctx) {
		auto source_pix_fmt = correct_for_deprecated_pixel_format(av_codec_ctx->pix_fmt);
		printf("sws_getContext 1\n");
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
		if (!sws_ctx) return;
	}

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
