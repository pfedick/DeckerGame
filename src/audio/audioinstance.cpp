#include "audio.h"
#include <unistd.h>

AudioInstance::AudioInstance()
{
	position=0;
	volume_left=255;
	volume_right=255;
	sample=NULL;
	loop=false;
}

AudioInstance::AudioInstance(const AudioSample &sample)
{
	position=0;
	volume_left=255;
	volume_right=255;
	this->sample=NULL;
	loop=false;
	load(sample);
}

void AudioInstance::load(const AudioSample &sample)
{
	position=0;
	this->sample=&sample;
}

void AudioInstance::rewind()
{
	position=0;
}

void AudioInstance::setVolume(int left, int right)
{
	volume_left=left;
	volume_right=right;
	if (volume_left<0) volume_left=0;
	//if (volume_left>255) volume_left=255;
	if (volume_right<0) volume_right=0;
	//if (volume_right>255) volume_right=255;
}

void AudioInstance::setLoop(bool loop)
{
	this->loop=loop;
}

size_t AudioInstance::addSamples(size_t num, ppl7::STEREOSAMPLE32 *buffer)
{
	if (!sample) return 0;
	size_t samples_read=0;
	if (loop==true) {
		size_t rest=num;
		while (rest) {
			samples_read=sample->addSamples(position, rest, buffer, volume_left, volume_right);
			position+=samples_read;
			if (samples_read<rest) {
				position=0;
			}
			buffer+=samples_read;
			rest-=samples_read;
		}
		return num;
	}
	samples_read=sample->addSamples(position, num, buffer, volume_left, volume_right);
	position+=samples_read;
	return samples_read;
}

