#include "audio.h"
#include <unistd.h>
#include <stdlib.h>


extern ppl7::grafix::Point GetViewPos();

AudioInstance::AudioInstance()
{
	position=0;
	volume=32768.0f;
	sample=NULL;
	loop=false;
	positional=false;
	max_distance=1600;
}

AudioInstance::AudioInstance(const AudioSample &sample)
{
	position=0;
	volume=32768.0f;
	this->sample=NULL;
	loop=false;
	positional=false;
	max_distance=1600;
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

void AudioInstance::setVolume(float volume)
{
	this->volume=volume;
	if (this->volume<0) this->volume=0.0f;
	if (this->volume>32768) this->volume=32768.0f;
}

void AudioInstance::setLoop(bool loop)
{
	this->loop=loop;
}

void AudioInstance::setPositional(const ppl7::grafix::Point &p, int max_distance)
{
	this->p=p;
	positional=true;
	this->max_distance=max_distance;
}

size_t AudioInstance::addSamples(size_t num, ppl7::STEREOSAMPLE32 *buffer)
{
	if (!sample) return 0;
	int vol_l=32768*volume;
	int vol_r=32768*volume;
	if (positional) {
		ppl7::grafix::Point v=GetViewPos();
		float d=ppl7::grafix::Distance(p,v);
		if ((int)d>max_distance) return num;
		float leftness=1.0f;
		float rightness=1.0f;
		float dx=abs(v.x-p.x);
		if (dx>1500) dx=1500;
		if (p.x<v.x) {
			rightness=1.0f-dx/1500.0f;
		} else {
			leftness=1.0f-dx/1500.0f;
		}
		vol_l=(int)((float)vol_l*((float)max_distance-d)/(float)max_distance*leftness);
		vol_r=(int)((float)vol_r*((float)max_distance-d)/(float)max_distance*rightness);
		/*
		printf ("distance: %d, max_distance=%d, vol_org=%d, vol_l=%d, vol_r=%d, leftness=%0.3f, rightness=%0.3f\n",
				(int)d, max_distance,volume_left,vol_l,vol_r,leftness,rightness);
				*/
	}

	size_t samples_read=0;
	if (loop==true) {
		size_t rest=num;
		while (rest) {
			samples_read=sample->addSamples(position, rest, buffer, vol_l, vol_r);
			position+=samples_read;
			if (samples_read<rest) {
				position=0;
			}
			buffer+=samples_read;
			rest-=samples_read;
		}
		return num;
	}
	samples_read=sample->addSamples(position, num, buffer, vol_l, vol_r);
	position+=samples_read;
	return samples_read;
}

