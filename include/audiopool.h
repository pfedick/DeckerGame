#ifndef INCLUDE_AUDIOPOOL_H_
#define INCLUDE_AUDIOPOOL_H_
#include "stdlib.h"
#include "stdio.h"
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-audio.h>

#include "audio.h"
#include "exceptions.h"

class AudioClip
{
public:
	enum Id {
		none=0,
		coin1=1,
		coin2,
		impact,
		electric,
		crash,
		skeleton_death,
		skeleton_turn,
		threespeers_activation,
		stone_drag_long,
		stone_drag_short,
		trap1,
		trap2,
		vent1,
		vent2,
		wind1,
		fire1,
		fire2,
		light_switch1,
		crystal,
		birds1,
		arrow_swoosh,
		fall,
		break1,

		// this must always be the last entry!
		maxClips
	};
};

class AudioPool
{
private:
	AudioSystem *audio;
public:
	AudioStream song[3];
	AudioSample sample[AudioClip::maxClips+1];

	AudioPool();
	~AudioPool();
	void load();
	void setAudioSystem(AudioSystem *audio);
	AudioInstance *getInstance(AudioClip::Id id);
	void playOnce(AudioClip::Id id, float volume=1.0f);
	void playOnce(AudioClip::Id id, const ppl7::grafix::Point &p, int max_distance=1600, float volume=1.0f);
	void playInstance(AudioInstance *instance);
};

AudioPool &getAudioPool();

#endif // INCLUDE_AUDIOPOOL_H_
