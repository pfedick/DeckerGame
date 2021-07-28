#ifndef INCLUDE_AUDIOPOOL_H_
#define INCLUDE_AUDIOPOOL_H_
#include "stdlib.h"
#include "stdio.h"
#include <ppl7.h>
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

		maxClips,
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
	void playOnce(AudioClip::Id id, int volume=192);

};

AudioPool &getAudioPool();

#endif // INCLUDE_AUDIOPOOL_H_
