#include "audio.h"
#include <unistd.h>

AudioStream::AudioStream()
{
	decoder=NULL;
	volume=1.0f;
	buffersize=0;
	prebuffer=NULL;
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
}

void AudioStream::setVolume(float volume)
{
	this->volume=volume;
	if (this->volume<0) this->volume=0.0f;
	if (this->volume>1.0) this->volume=1.0f;
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



