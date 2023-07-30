#include "audiopool.h"

static AudioPool* audiopool=NULL;

AudioPool& getAudioPool()
{
	if (!audiopool) throw AudioPoolNotInitialized();
	return *audiopool;
}


AudioPool::AudioPool()
{
	if (!audiopool) audiopool=this;
	audio=NULL;
}

AudioPool::~AudioPool()
{
	if (audiopool == this) audiopool=NULL;
}

size_t AudioPool::size() const
{
	size_t s=0;
	for (int i=1;i < AudioClip::maxClips;i++) s+=sample[i].size();
	return s;
}

void AudioPool::setAudioSystem(AudioSystem* audio)
{
	this->audio=audio;
}

void AudioPool::playOnce(AudioClip::Id id, float volume, AudioClass a)
{
	AudioInstance* instance=new AudioInstance(sample[id], a);
	instance->setVolume(volume);
	instance->setAutoDelete(true);
	audio->play(instance);
}

void AudioPool::playOnce(AudioClip::Id id, const ppl7::grafix::Point& p, int max_distance, float volume, AudioClass a)
{
	if (id < AudioClip::maxClips) {
		AudioInstance* instance=new AudioInstance(sample[id], a);
		instance->setVolume(volume);
		instance->setAutoDelete(true);
		instance->setPositional(p, max_distance);
		audio->play(instance);
	}
}

AudioInstance* AudioPool::getInstance(AudioClip::Id id, AudioClass a)
{
	if (id < AudioClip::maxClips) return new AudioInstance(sample[id], a);
	return NULL;
}

void AudioPool::playInstance(AudioInstance* instance)
{
	audio->play(instance);
}

void AudioPool::stopInstace(AudioInstance* instance)
{
	audio->stop(instance);
}

void AudioPool::load()
{
	try {
		musictracks.push_back(MusicTrack("Patrick F. - In The Hall Of The Mountain King", "res/audio/PatrickF-In_The_Hall_Of_The_Mountain_King.mp3"));
		musictracks.push_back(MusicTrack("Patrick F. - ID", "res/audio/PatrickF-ID.mp3"));
		musictracks.push_back(MusicTrack("Patrick F. - Sonic Waves", "res/audio/PatrickF-Sonic_Waves.mp3"));
		musictracks.push_back(MusicTrack("Patrick F. - George Decker Theme", "res/audio/PatrickF-George_Decker_Theme.mp3"));
		musictracks.push_back(MusicTrack("Patrick F. - Heaven", "res/audio/PatrickF-Heaven.mp3"));
		musictracks.push_back(MusicTrack("Patrick F. - Spring", "res/audio/PatrickF-Spring.mp3"));

		sample[AudioClip::coin1].load("res/audio/coin1.mp3");
		sample[AudioClip::coin2].load("res/audio/coin2.mp3");
		sample[AudioClip::coin3].load("res/audio/coin3.mp3");
		sample[AudioClip::coin4].load("res/audio/coin4.mp3");
		sample[AudioClip::coin5].load("res/audio/coin5.mp3");
		sample[AudioClip::coin6].load("res/audio/coin6.mp3");
		sample[AudioClip::impact].load("res/audio/221626__ansel__body-impact.mp3");
		sample[AudioClip::electric].load("res/audio/65232__carbilicon__electr_stereo.mp3");
		sample[AudioClip::crash].load("res/audio/95078__sandyrb__the-crash.mp3");
		sample[AudioClip::skeleton_death].load("res/audio/skeleton_break.mp3");
		sample[AudioClip::skeleton_turn].load("res/audio/skeleton_turn.mp3");
		sample[AudioClip::threespeers_activation].load("res/audio/threespeers.mp3");
		sample[AudioClip::stone_drag_long].load("res/audio/stone_drag_long.mp3");
		sample[AudioClip::stone_drag_short].load("res/audio/stone_drag_short.mp3");
		sample[AudioClip::trap1].load("res/audio/trap1.mp3");
		sample[AudioClip::trap2].load("res/audio/trap2.mp3");
		sample[AudioClip::vent1].load("res/audio/vent1.mp3");
		sample[AudioClip::vent2].load("res/audio/vent2.mp3");
		sample[AudioClip::wind1].load("res/audio/wind1.mp3");
		sample[AudioClip::fire1].load("res/audio/fire1.mp3");
		sample[AudioClip::fire2].load("res/audio/fire2.mp3");
		sample[AudioClip::light_switch1].load("res/audio/light_switch_1.mp3");
		sample[AudioClip::crystal].load("res/audio/crystal.mp3");
		sample[AudioClip::birds1].load("res/audio/birds.mp3");
		sample[AudioClip::arrow_swoosh].load("res/audio/arrow-swoosh.mp3");
		sample[AudioClip::fall].load("res/audio/fall.mp3");
		sample[AudioClip::break1].load("res/audio/break1.mp3");
		sample[AudioClip::thruster].load("res/audio/thruster.mp3");
		sample[AudioClip::scarabeus1].load("res/audio/scarabeus1.mp3");
		sample[AudioClip::scarabeus2].load("res/audio/scarabeus2.mp3");
		sample[AudioClip::scarabeus3].load("res/audio/scarabeus3.mp3");
		sample[AudioClip::scarabeus4].load("res/audio/scarabeus4.mp3");
		sample[AudioClip::scarabeus5].load("res/audio/scarabeus5.mp3");
		sample[AudioClip::scarabeus6].load("res/audio/scarabeus6.mp3");
		sample[AudioClip::scarabeus7].load("res/audio/scarabeus7.mp3");
		sample[AudioClip::scarabeus8].load("res/audio/scarabeus8.mp3");
		sample[AudioClip::scarabeus9].load("res/audio/scarabeus9.mp3");
		sample[AudioClip::scarabeus_death1].load("res/audio/scarab_death1.mp3");
		sample[AudioClip::scarabeus_death2].load("res/audio/scarab_death2.mp3");
		sample[AudioClip::scarabeus_death3].load("res/audio/scarab_death3.mp3");
		sample[AudioClip::scarabeus_death4].load("res/audio/scarab_death4.mp3");
		sample[AudioClip::scarabeus_death5].load("res/audio/scarab_death5.mp3");
		sample[AudioClip::rat_squeek].load("res/audio/rat.mp3");
		sample[AudioClip::turkey_sound].load("res/audio/turkey-sound.mp3");
		sample[AudioClip::turkey_gobble].load("res/audio/turkey-gobble.mp3");
		sample[AudioClip::digging].load("res/audio/digging.mp3");
		sample[AudioClip::wind2].load("res/audio/wind2.mp3");
		sample[AudioClip::scorpion_breath].load("res/audio/scorpion_breath.mp3");
		sample[AudioClip::scorpion_run].load("res/audio/scorpion_run.mp3");
		sample[AudioClip::step1].load("res/audio/step1.mp3");
		sample[AudioClip::step2].load("res/audio/step2.mp3");
		sample[AudioClip::step3].load("res/audio/step3.mp3");
		sample[AudioClip::step4].load("res/audio/step4.mp3");
		sample[AudioClip::jungle1].load("res/audio/jungle1.mp3");
		sample[AudioClip::jungle2].load("res/audio/jungle2.mp3");
		sample[AudioClip::waterflow1].load("res/audio/waterflow1.mp3");
		sample[AudioClip::waterflow2].load("res/audio/waterflow2.mp3");
		sample[AudioClip::waterflow3].load("res/audio/waterflow3.mp3");
		sample[AudioClip::watersplash1].load("res/audio/watersplash1.mp3");
		sample[AudioClip::watersplash2].load("res/audio/watersplash2.mp3");
		sample[AudioClip::watersplash3].load("res/audio/watersplash3.mp3");
		sample[AudioClip::watersplash4].load("res/audio/watersplash4.mp3");
		sample[AudioClip::water_pouring1].load("res/audio/water_pouring1.mp3");
		sample[AudioClip::water_pouring2].load("res/audio/water_pouring2.mp3");
		sample[AudioClip::water_bubble1].load("res/audio/water_bubble.mp3");
		sample[AudioClip::gas_spray].load("res/audio/gas_spray.mp3");
		sample[AudioClip::underwaterloop1].load("res/audio/underwaterloop1.mp3");
		sample[AudioClip::crunch].load("res/audio/crunch.mp3");
		sample[AudioClip::hackstone].load("res/audio/hackstone.mp3");
		sample[AudioClip::birds2].load("res/audio/birds2.mp3");
		sample[AudioClip::birds3].load("res/audio/birds3.mp3");
		sample[AudioClip::rain1].load("res/audio/rain1.mp3");
		sample[AudioClip::rain2].load("res/audio/rain2.mp3");
		sample[AudioClip::rain3].load("res/audio/rain3.mp3");
		sample[AudioClip::birds_in_rain].load("res/audio/birds_in_the_rain.mp3");
		sample[AudioClip::desert_at_night].load("res/audio/desert_at_night.mp3");
		sample[AudioClip::wind3].load("res/audio/wind_grillen.mp3");

		sample[AudioClip::crystal2].load("res/audio/crystal2.mp3");
		sample[AudioClip::crystal3].load("res/audio/crystal3.mp3");
		sample[AudioClip::fanfare1].load("res/audio/fanfare1.mp3");
		sample[AudioClip::fanfare2].load("res/audio/fanfare2.mp3");
		sample[AudioClip::fanfare3].load("res/audio/fanfare3.mp3");
		sample[AudioClip::fanfare4].load("res/audio/fanfare4.mp3");
		sample[AudioClip::metaldoor].load("res/audio/metaldoor_open.mp3");
		sample[AudioClip::door_open].load("res/audio/door_open.mp3");
		sample[AudioClip::door_close].load("res/audio/door_close.mp3");
		sample[AudioClip::treasure_chest].load("res/audio/treasure_chest.mp3");
		sample[AudioClip::treasure_collect].load("res/audio/treasure_collect.mp3");
		sample[AudioClip::decker].load("res/audio/decker_theme_chiptune.mp3");
		sample[AudioClip::key].load("res/audio/key.mp3");
		sample[AudioClip::impact1].load("res/audio/impact1.mp3");
		sample[AudioClip::squash1].load("res/audio/squash1.mp3");
		sample[AudioClip::bricks_falling1].load("res/audio/bricks_falling1.mp3");
		sample[AudioClip::bricks_falling2].load("res/audio/bricks_falling2.mp3");
		sample[AudioClip::bricks_falling3].load("res/audio/bricks_falling3.mp3");
		sample[AudioClip::hammer].load("res/audio/hammer.mp3");
		sample[AudioClip::waves1].load("res/audio/waves1.mp3");
		sample[AudioClip::waterdrips].load("res/audio/waterdrips.mp3");
		sample[AudioClip::wind_strong].load("res/audio/wind_strong.mp3");
		sample[AudioClip::cave1].load("res/audio/cave1.mp3");
		sample[AudioClip::cave2].load("res/audio/cave2.mp3");
		sample[AudioClip::cave3].load("res/audio/cave3.mp3");
		sample[AudioClip::breakingwall].load("res/audio/breakingwall.mp3");
		sample[AudioClip::crow_flying].load("res/audio/crow_flying.mp3");
		sample[AudioClip::crow_scream].load("res/audio/crow_scream.mp3");
		sample[AudioClip::crow_impact].load("res/audio/crow_impact.mp3");
		sample[AudioClip::bat_wings].load("res/audio/bat.mp3");


	} catch (const ppl7::Exception& exp) {
		exp.print();
		throw;
	}

}
