#include "videoplayer.h"



VideoPlayer::VideoPlayer(SDL_Renderer* renderer)
{
	this->renderer=renderer;
	overlay=NULL;
	num_frames=0;
	fps[0]=0;
	fps[1]=0;
	timebase[0]=0;
	timebase[1]=0;
	dav1d_default_settings(&lib_settings);
	endreached=false;
	frame_width=0;
	frame_height=0;
	video_framerate=0.0f;
	context=NULL;
	data.sz=0;
}

VideoPlayer::~VideoPlayer()
{
	clear();
}

void VideoPlayer::setRenderer(SDL_Renderer* renderer)
{
	this->renderer=renderer;
}

void VideoPlayer::clear()
{
	if (data.sz > 0)
		dav1d_data_unref(&data);
	demuxer.close();
	if (overlay) {
		SDL_DestroyTexture(overlay);
	}
	if (context) {
		dav1d_close(&context);
		context=NULL;
	}
	data.sz=0;
}


bool VideoPlayer::load(const ppl7::String& filename)
{
	clear();
	if (!renderer) return false;
	num_frames=0;
	fps[0]=0;
	fps[1]=0;
	timebase[0]=0;
	timebase[1]=0;
	if (!demuxer.open(filename, fps, &num_frames, timebase)) {
		ppl7::PrintDebugTime("could not open input video\n");
		return false;
	}
	endreached=false;
	frame_width=demuxer.width();
	frame_height=demuxer.height();
	video_framerate=(float)fps[0];

	if (dav1d_open(&context, &lib_settings) != 0) {
		ppl7::PrintDebugTime("Failed opening dav1d decoder\n");
		return false;
	}
	int res=0;
	if ((res = demuxer.read(&data)) < 0) {
		ppl7::PrintDebugTime("Failed demuxing input\n");
		return false;
	}

	//ppl7::PrintDebugTime("width: %d, height: %d, framerate=%0.1f\n", demuxer.width(), demuxer.height(), video_framerate);
	return true;
}

bool VideoPlayer::nextFrame()
{
	Dav1dPicture* p=NULL;
	while (1) {
		if (decode_frame(&p)) {
			if (p) {
				dav1d_picture_unref(p);
				free(p);
			}
			endreached=true;
			return false;
		}
		if (p) {
			//ppl7::PrintDebugTime("we have a pixture\n");
			sdl_update_texture(p);
			dav1d_picture_unref(p);
			free(p);
			break;
		}
	}
	return true;
}

int VideoPlayer::width() const
{
	return frame_width;
}

int VideoPlayer::height() const
{
	return frame_height;
}

bool VideoPlayer::eof() const
{
	return endreached;
}

float VideoPlayer::framerate() const
{
	return video_framerate;
}

int VideoPlayer::decode_frame(Dav1dPicture** p)
{
	int res;
	// Send data packets we got from the demuxer to dav1d
	if ((res = dav1d_send_data(context, &data)) < 0) {
		// On EAGAIN, dav1d can not consume more data and
		// dav1d_get_picture needs to be called first, which
		// will happen below, so just keep going in that case
		// and do not error out.
		if (res != DAV1D_ERR(EAGAIN)) {
			dav1d_data_unref(&data);
			goto err;
		}
	}
	*p = (Dav1dPicture*)calloc(1, sizeof(**p));
	// Try to get a decoded frame
	if ((res = dav1d_get_picture(context, *p)) < 0) {
		// In all error cases, even EAGAIN, p needs to be freed as
		// it is never added to the queue and would leak.
		free(*p);
		*p = NULL;
		// On EAGAIN, it means dav1d has not enough data to decode
		// therefore this is not a decoding error but just means
		// we need to feed it more data, which happens in the next
		// run of the decoder loop.
		if (res != DAV1D_ERR(EAGAIN))
			goto err;
	}
	return data.sz == 0 ? demuxer.read(&data) : 0;
err:
	ppl7::PrintDebugTime("Error decoding frame: %s\n",
		strerror(-res));
	return res;
}


int VideoPlayer::sdl_update_texture(Dav1dPicture* dav1d_pic)
{
	if (dav1d_pic == NULL) {
		return 0;
	}

	frame_width = dav1d_pic->p.w;
	frame_height = dav1d_pic->p.h;
	int tex_w = frame_width;
	int tex_h = frame_height;

	enum Dav1dPixelLayout dav1d_layout = dav1d_pic->p.layout;

	if (DAV1D_PIXEL_LAYOUT_I420 != dav1d_layout || dav1d_pic->p.bpc != 8) {
		ppl7::PrintDebugTime("Unsupported pixel format, only 8bit 420 supported so far.\n");
		return 0;
	}

	if (overlay != NULL) {
		SDL_QueryTexture(overlay, NULL, NULL, &tex_w, &tex_h);
		if (tex_w != frame_width || tex_h != frame_height) {
			SDL_DestroyTexture(overlay);
			overlay = NULL;
		}
	}

	if (overlay == NULL) {
		overlay = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV,
			SDL_TEXTUREACCESS_STREAMING, frame_width, frame_height);
	}

	SDL_UpdateYUVTexture(overlay, NULL,
		(const Uint8*)dav1d_pic->data[0], (int)dav1d_pic->stride[0], // Y
		(const Uint8*)dav1d_pic->data[1], (int)dav1d_pic->stride[1], // U
		(const Uint8*)dav1d_pic->data[2], (int)dav1d_pic->stride[1]  // V
	);
	return 1;
}


void VideoPlayer::renderFrame(const SDL_Rect* dstrect)
{
	if (!renderer || !overlay) return;
	SDL_RenderCopy(renderer, overlay, NULL, dstrect);
}

SDL_Texture* VideoPlayer::getVideoTexture()
{
	return overlay;
}
