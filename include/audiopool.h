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
	// DO NOT CHANGE THE ORDER OF THE IDs!!!
	// They are stored in the level files, e.g. for the speaker
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
		rat_noise,
		turkey_sound,
		turkey_gobble,
		digging,
		wind2,
		scorpion_breath,
		scorpion_run,
		step1,
		step2,
		step3,
		step4,
		jungle1,
		jungle2,
		waterflow1,
		waterflow2,
		waterflow3,
		watersplash1,
		watersplash2,
		watersplash3,
		watersplash4,
		water_pouring1,
		water_pouring2,
		water_bubble1,
		gas_spray,
		underwaterloop1,
		crunch,
		coin3,
		coin4,
		coin5,
		coin6,
		hackstone,
		birds2,
		birds3,
		rain1,
		rain2,
		rain3,
		birds_in_rain,
		cave1,
		desert_at_night,
		wind3,
		crystal2,
		crystal3,
		fanfare1,
		fanfare2,
		fanfare3,
		fanfare4,
		metaldoor,
		door_open,
		door_close,
		treasure_chest,
		treasure_collect,
		decker,
		key,
		impact1,
		squash1,
		bricks_falling1,
		bricks_falling2,
		bricks_falling3,
		hammer,
		breakingwall,
		waves1,
		waterdrips,
		wind_strong,
		cave2,
		cave3,
		crow_flying,
		crow_scream,
		crow_impact,
		bat_wings,
		george_step1,
		george_step2,
		george_step3,
		george_step4,
		george_step5,
		george_jump_land,
		fire3,
		cheese,
		rat_squeek,

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
	size_t size() const;
	AudioInstance* getInstance(AudioClip::Id id, AudioClass a=AudioClass::Effect);
	void playOnce(AudioClip::Id id, float volume=1.0f, AudioClass a=AudioClass::Effect);
	void playOnce(AudioClip::Id id, const ppl7::grafix::Point& p, int max_distance=1600, float volume=1.0f, AudioClass a=AudioClass::Effect);
	void playInstance(AudioInstance* instance);
	void stopInstace(AudioInstance* instance);
};

AudioPool& getAudioPool();

#endif // INCLUDE_AUDIOPOOL_H_
