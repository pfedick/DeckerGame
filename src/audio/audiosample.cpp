#include "audio.h"
#include <unistd.h>

AudioSample::AudioSample()
{

}


AudioSample::AudioSample(const ppl7::String& filename)
{
	load(filename);
}

AudioSample::~AudioSample()
{

}

void AudioSample::load(const ppl7::String& filename)
{
	ppl7::File ff;
	ppl7::AudioInfo info;
	ff.open(filename);
	if (!ppl7::IdentAudioFile(ff, info)) {
		throw UnknownAudioFormat("%s", (const char*)filename);
	}
	ppl7::PrintDebug("loading audio file: %s, format: %d\n", (const char*)filename, (int)info.Format);
	ppl7::AudioDecoder* decoder=NULL;
	if (info.Format == ppl7::AudioInfo::AIFF) decoder=(ppl7::AudioDecoder*)new ppl7::AudioDecoder_Aiff();
	else if (info.Format == ppl7::AudioInfo::WAVE) decoder=(ppl7::AudioDecoder*)new ppl7::AudioDecoder_Wave();
	else if (info.Format == ppl7::AudioInfo::MP3) decoder=(ppl7::AudioDecoder*)new ppl7::AudioDecoder_MP3();
	//else if (info.Format == ppl7::AudioInfo::OGG) decoder=(ppl7::AudioDecoder*)new ppl7::AudioDecoder_Ogg();
	if (!decoder) throw UnknownAudioFormat("%s", (const char*)filename);
	decoder->open(ff, &info);
	ppl7::STEREOSAMPLE16* b=(ppl7::STEREOSAMPLE16*)buffer.malloc(info.Samples * sizeof(ppl7::STEREOSAMPLE16));
	size_t read_samples=decoder->getSamples(info.Samples, b);
	buffer.truncate(read_samples * sizeof(ppl7::STEREOSAMPLE16));
	//ppl7::PrintDebugTime("%-40s: %10zd KB, memory: %10zd KB\n", (const char*)filename, ff.size() / 1024, buffer.size() / 1024);
	delete decoder;
}

size_t AudioSample::size() const
{
	return buffer.size() / sizeof(ppl7::STEREOSAMPLE16);
}

size_t AudioSample::bufferSize() const
{
	return buffer.size();
}

size_t AudioSample::addSamples(size_t position, size_t num, ppl7::STEREOSAMPLE32* buffer, int vol_left, int vol_right) const
{
	if (position + num > size()) num=size() - position;
	const ppl7::STEREOSAMPLE16* samples=(const ppl7::STEREOSAMPLE16*)this->buffer.ptr();
	samples+=position;
	if (vol_left == 32768 && vol_right == 32768) {
		for (size_t i=0;i < num;i++) {
			buffer[i].left+=samples[i].left;
			buffer[i].right+=samples[i].right;
		}
	} else {
		for (size_t i=0;i < num;i++) {
			buffer[i].left+=samples[i].left * vol_left / 32768;
			buffer[i].right+=samples[i].right * vol_right / 32768;
		}
	}

	return num;
}

size_t AudioSample::skipSamples(size_t position, size_t num) const
{
	if (position + num > size()) num=size() - position;
	return num;
}
