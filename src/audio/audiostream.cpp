#include "audio.h"
#include <unistd.h>

AudioStream::AudioStream()
{
	decoder=NULL;
}

AudioStream::AudioStream(const ppl7::String &filename)
{
	decoder=NULL;
	open(filename);
}



AudioStream::~AudioStream()
{
	if (decoder) delete decoder;
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

size_t AudioStream::addSamples(size_t num, ppl7::STEREOSAMPLE32 *buffer)
{
	if (!decoder) {
		memset(buffer,0,num*sizeof(ppl7::STEREOSAMPLE32));
		return 0;
	}
	size_t read=decoder->addSamples(num, buffer);
	if (read<num) {
		memset(buffer+read,0,(num-read)*sizeof(ppl7::STEREOSAMPLE32));
	}
	return read;
}



