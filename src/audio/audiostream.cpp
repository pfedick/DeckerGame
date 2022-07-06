#include "audio.h"
#include <unistd.h>

AudioStream::AudioStream()
{
	decoder=NULL;
	volume=1.0f;
	buffersize=0;
	prebuffer=NULL;
	fade_start=0.0f;
	fade_start_volume=1.0f;
	fade_time=4.0f;
}

AudioStream::AudioStream(const ppl7::String &filename)
{
	decoder=NULL;
	volume=1.0f;
	buffersize=0;
	prebuffer=NULL;
	open(filename);
}



AudioStream::~AudioStream()
{
	if (decoder) delete decoder;
	if (prebuffer) free(prebuffer);
	decoder=NULL;
}

void AudioStream::open(const ppl7::String &filename)
{
	if (decoder) delete decoder;
	decoder=NULL;
	ppl7::AudioInfo info;
	ff.open(filename);
	if (!ppl7::IdentAudioFile(ff,info)) {
		throw UnknownAudioFormat("%s",(const char*)filename);
	}
	if (info.Format==ppl7::AudioInfo::AIFF) decoder=(ppl7::AudioDecoder *)new ppl7::AudioDecoder_Aiff();
	else if (info.Format==ppl7::AudioInfo::WAVE) decoder=(ppl7::AudioDecoder *)new ppl7::AudioDecoder_Wave();
	else if (info.Format==ppl7::AudioInfo::MP3) decoder=(ppl7::AudioDecoder *)new ppl7::AudioDecoder_MP3();
	if (!decoder) throw UnknownAudioFormat("%s",(const char*)filename);
	decoder->open(ff,&info);
}

void AudioStream::rewind()
{
	if (decoder) decoder->seekSample(0);
	fade_start=0;
	volume=fade_start_volume;
}

void AudioStream::setVolume(float volume)
{
	this->volume=volume;
	fade_start=0;
	if (this->volume<0) this->volume=0.0f;
	if (this->volume>1.0) this->volume=1.0f;
	this->fade_start_volume=this->volume;
}

void AudioStream::fadeout(float seconds)
{
	fade_start_volume=volume;
	fade_time=seconds;
	fade_start=ppl7::GetMicrotime();

}

size_t AudioStream::addSamples(size_t num, ppl7::STEREOSAMPLE32 *buffer)
{
	if (!decoder) {
		return 0;
	}
	if (buffersize<num*sizeof(ppl7::STEREOSAMPLE16)) {
		if (!prebuffer) prebuffer=(ppl7::STEREOSAMPLE16*)malloc(num*sizeof(ppl7::STEREOSAMPLE16));
		else prebuffer=(ppl7::STEREOSAMPLE16*)realloc(prebuffer,num*sizeof(ppl7::STEREOSAMPLE16));
		if (!prebuffer) return 0;
	}
	size_t read=decoder->getSamples(num, prebuffer);
	if (read<num) {
		memset(prebuffer+read,0,(num-read)*sizeof(ppl7::STEREOSAMPLE16));
	}
	if (fade_start>0.0f) {
		float in_fade=(ppl7::GetMicrotime()-fade_start)/fade_time;
		volume=fade_start_volume-in_fade*fade_start_volume;
		if (volume<=0) {
			fade_start=0;
			volume=fade_start_volume;
			decoder->seekSample(0);
			return 0;
		}
	}
	if (volume==1.0f) {
		for (size_t i=0;i<num;i++) {
			buffer[i].left+=prebuffer[i].left;
			buffer[i].right+=prebuffer[i].right;
		}
	} else {
		for (size_t i=0;i<num;i++) {
			buffer[i].left+=(ppl7::SAMPLE32)((float)prebuffer[i].left*volume);
			buffer[i].right+=(ppl7::SAMPLE32)((float)prebuffer[i].right*volume);
		}
	}


	return read;
}



