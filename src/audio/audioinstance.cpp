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

AudioInstance::AudioInstance(AudioClass a)
{
	position=0;
	volume=32768.0f;
	sample=NULL;
	loop=false;
	positional=false;
	max_distance=1600;
	fade_start=0.0f;
	fade_start_volume=1.0f;
	fade_time=4.0f;
	setAudioClass(a);
}

AudioInstance::AudioInstance(const AudioSample& sample, AudioClass a)
{
	position=0;
	volume=32768.0f;
	this->sample=NULL;
	loop=false;
	positional=false;
	max_distance=1600;
	fade_start=0.0f;
	fade_start_volume=1.0f;
	fade_time=4.0f;
	setAudioClass(a);
	load(sample);
}

void AudioInstance::load(const AudioSample& sample)
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
	if (this->volume < 0) this->volume=0.0f;
	if (this->volume > 1.0) this->volume=1.0f;
	fade_start=0;
	this->fade_start_volume=this->volume;
}

void AudioInstance::setLoop(bool loop)
{
	this->loop=loop;
}

void AudioInstance::fadeout(float seconds)
{
	fade_start_volume=volume;
	fade_time=seconds;
	fade_start=ppl7::GetMicrotime();

}

void AudioInstance::setPositional(const ppl7::grafix::Point& p, int max_distance)
{
	this->p=p;
	positional=true;
	this->max_distance=max_distance;
}

size_t AudioInstance::skipSamples(size_t num)
{
	size_t samples_read=0;
	if (loop == true) {
		size_t rest=num;
		while (rest) {
			samples_read=sample->skipSamples(position, rest);
			position+=samples_read;
			if (samples_read < rest) {
				position=0;
			}
			rest-=samples_read;
		}
		return num;
	}
	samples_read=sample->skipSamples(position, num);
	position+=samples_read;
	return samples_read;
}

size_t AudioInstance::addSamples(size_t num, ppl7::STEREOSAMPLE32* buffer, float volume)
{
	if (!sample) return 0;
	if (fade_start > 0.0f) {
		float in_fade=(ppl7::GetMicrotime() - fade_start) / fade_time;
		this->volume=fade_start_volume - in_fade * fade_start_volume;
		if (this->volume <= 0) {
			fade_start=0;
			volume=fade_start_volume;
			position=0;
			return 0;
		}
	}
	int vol_l=32768 * (volume * this->volume);
	int vol_r=32768 * (volume * this->volume);
	if (positional) {
		ppl7::grafix::Point v=GetViewPos();
		float d=ppl7::grafix::Distance(p, v);
		if ((int)d > max_distance || (int)d < 0) {
			return skipSamples(num);
		}
		float leftness=1.0f;
		float rightness=1.0f;
		float dx=abs(v.x - p.x);
		if (dx > 1500) dx=1500;
		if (p.x < v.x) {
			rightness=1.0f - dx / 1500.0f;
		} else {
			leftness=1.0f - dx / 1500.0f;
		}
		vol_l=(int)((float)vol_l * ((float)max_distance - d) / (float)max_distance * leftness);
		vol_r=(int)((float)vol_r * ((float)max_distance - d) / (float)max_distance * rightness);

		//ppl7::PrintDebugTime("distance: %d, max_distance=%d, vol_org=%d, vol_l=%d, vol_r=%d, leftness=%0.3f, rightness=%0.3f\n",
		//	(int)d, max_distance, (int)(32768 * (volume * this->volume)), vol_l, vol_r, leftness, rightness);

	}
	if (vol_l > 32768) vol_l=32768;
	if (vol_r > 32768) vol_r=32768;

	size_t samples_read=0;
	if (loop == true) {
		size_t rest=num;
		while (rest) {
			samples_read=sample->addSamples(position, rest, buffer, vol_l, vol_r);
			position+=samples_read;
			if (samples_read < rest) {
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
