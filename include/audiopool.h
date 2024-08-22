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
		shoot,	// Helena
		holster_pistol,
		fabric,
		bullet_hits_player,
		bullet_hits_wall,	// TODO
		helena_step1,
		helena_step2,
		helena_step3,
		helena_step4,
		helena_step5,
		helena_jump,
		wallenstein_step1,
		wallenstein_step2,
		wallenstein_step3,
		wallenstein_step4,
		wallenstein_step5,
		wallenstein_jump,
		yeti_step1,
		yeti_step2,
		yeti_step3,
		yeti_step4,
		yeti_step5,
		yeti_jump,
		yeti_tear1,
		yeti_tear2,
		yeti_tear3,
		yeti_growl1,
		yeti_growl2,
		yeti_growl3,
		yeti_growl4,
		yeti_angry_growl1,
		yeti_angry_growl2,
		yeti_angry_growl3,
		yeti_angry_growl4,
		cave4,
		night1,
		night2,
		night3,
		night4,
		nightowl1,
		nightowl2,
		nightowl3,
		waterdrips2,
		waterdrips3,
		zombie_step1,
		zombie_step2,
		zombie_step3,
		zombie_step4,
		zombie_step5,
		stamper_down,
		stamper_up,
		stamper_echo,
		stamper_squish,
		fireball_fly,
		fireball_impact,
		flashlight_pickup,
		ghost_chains,
		ghost_turn,
		ghost_land,
		skull_voice1,
		skull_voice2,
		skull_voice3,
		skull_hui,
		skull_death,
		skull_fireball,
		skull_impact,
		skull_shoot,
		explosion1,
		skullmaster_burp,
		skullmaster_hurt1,
		skullmaster_hurt2,
		skullmaster_hurt3,
		skullmaster_laugh1,
		skullmaster_laugh2,
		skullmaster_laugh3,
		lavaloop1,
		lavaloop2,
		lavabubbles,
		touchplateswitch1,
		touchplateswitch2,
		spider_walk_loop,
		spider_spit,
		spider_spit_hit,
		spider_net_unfold,
		ladder_step1,
		ladder_step2,
		ladder_step3,
		ladder_step4,
		ladder_step5,
		ladder_step6,
		ladder_step7,
		crate_loop,
		crate_falling,
		powercell_collected,
		powercell_change,
		powercells_depleted,
		peach,
		fireworks_loop,
		glimmer_plopp1,
		glimmer_plopp2,
		glimmer_plopp3,
		glimmer_plopp4,
		glimmer_plopp5,
		glimmer_plopp6,
		glimmer_effect1,
		glimmer_effect2,
		glimmer_chimes_loop1,
		glimmer_awakens,
		glimmer_up,
		glimmer_down,
		elevator_start,
		elevator_noiseloop,
		elevator_exit,
		waves2,
		waves3,
		waves4,
		gas1,
		gas2,
		gas3,
		water_bubble2,
		water_bubble3,
		water_bubble4,
		water_bubble5,
		wind4,
		wind5,
		earthquake,
		rumble,
		waterpuddle,
		// this must always be the last entry!
		maxClips
	};
};

class VoiceGeorge
{
public:
	enum Id {
		none=0,
		aua1,
		aua2,
		aua3,
		aua4,
		snort,
		hello,
		hello_questioned,
		hello_here_i_am,
		hello_im_george,
		play_with_me,
		hepp1,
		hepp2,
		hepp3,
		hepp4,
		hepp5,
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
	AudioSample sample[AudioClip::Id::maxClips + 1];
	AudioSample voice_george[VoiceGeorge::maxClips + 1];

	std::list<MusicTrack> musictracks;

	AudioPool();
	~AudioPool();
	void load();
	void load_speech(const ppl7::String& lang);
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
