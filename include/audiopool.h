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
		thruster,
		scarabeus1,
		scarabeus2,
		scarabeus3,
		scarabeus4,
		scarabeus5,
		scarabeus6,
		scarabeus7,
		scarabeus8,
		scarabeus9,
		scarabeus_death1,
		scarabeus_death2,
		scarabeus_death3,
		scarabeus_death4,
		scarabeus_death5,
		rat_squeek,
		turkey_sound,
		turkey_gobble,
		digging,
		wind2,

		// this must always be the last entry!
		maxClips
	};
};

class MusicTrack
{
public:
	ppl7::String Name;
	ppl7::String Filename;

	MusicTrack(const ppl7::String& Name, const ppl7::String& Filename) {
		this->Name=Name;
		this->Filename=Filename;
	}

};

class AudioPool
{
private:
	AudioSystem* audio;
public:
	AudioSample sample[AudioClip::maxClips + 1];

	std::list<MusicTrack> musictracks;

	AudioPool();
	~AudioPool();
	void load();
	void setAudioSystem(AudioSystem* audio);
	AudioInstance* getInstance(AudioClip::Id id, AudioClass a=AudioClass::Effect);
	void playOnce(AudioClip::Id id, float volume=1.0f, AudioClass a=AudioClass::Effect);
	void playOnce(AudioClip::Id id, const ppl7::grafix::Point& p, int max_distance=1600, float volume=1.0f, AudioClass a=AudioClass::Effect);
	void playInstance(AudioInstance* instance);
	void stopInstace(AudioInstance* instance);
};

AudioPool& getAudioPool();

#endif // INCLUDE_AUDIOPOOL_H_
