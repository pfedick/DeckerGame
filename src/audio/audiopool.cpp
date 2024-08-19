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
	for (int i=1;i < AudioClip::maxClips;i++) s+=sample[i].bufferSize();
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
		musictracks.push_back(MusicTrack("Patrick F. - Into The Deep", "res/audio/PatrickF-Into_The_Deep.mp3"));

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
		sample[AudioClip::peach].load("res/audio/peach.mp3");
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
		sample[AudioClip::rat_noise].load("res/audio/rat.mp3");
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
		sample[AudioClip::george_step1].load("res/audio/george_step1.mp3");
		sample[AudioClip::george_step2].load("res/audio/george_step2.mp3");
		sample[AudioClip::george_step3].load("res/audio/george_step3.mp3");
		sample[AudioClip::george_step4].load("res/audio/george_step4.mp3");
		sample[AudioClip::george_step5].load("res/audio/george_step5.mp3");
		sample[AudioClip::george_jump_land].load("res/audio/george_jump_land.mp3");
		sample[AudioClip::fire3].load("res/audio/fire3.mp3");
		sample[AudioClip::cheese].load("res/audio/cheese.mp3");
		sample[AudioClip::rat_squeek].load("res/audio/rat_squeek.mp3");
		sample[AudioClip::shoot].load("res/audio/shoot.mp3");
		sample[AudioClip::holster_pistol].load("res/audio/holster-pistol.mp3");
		sample[AudioClip::fabric].load("res/audio/fabric.mp3");
		sample[AudioClip::bullet_hits_player].load("res/audio/bullet_hits_player.mp3");
		sample[AudioClip::bullet_hits_wall].load("res/audio/bullet_hits_wall.mp3");
		sample[AudioClip::helena_step1].load("res/audio/helena_step1.mp3");
		sample[AudioClip::helena_step2].load("res/audio/helena_step2.mp3");
		sample[AudioClip::helena_step3].load("res/audio/helena_step3.mp3");
		sample[AudioClip::helena_step4].load("res/audio/helena_step4.mp3");
		sample[AudioClip::helena_step5].load("res/audio/helena_step5.mp3");
		sample[AudioClip::helena_jump].load("res/audio/helena_jump.mp3");
		sample[AudioClip::wallenstein_step1].load("res/audio/wallenstein_step1.mp3");
		sample[AudioClip::wallenstein_step2].load("res/audio/wallenstein_step2.mp3");
		sample[AudioClip::wallenstein_step3].load("res/audio/wallenstein_step3.mp3");
		sample[AudioClip::wallenstein_step4].load("res/audio/wallenstein_step4.mp3");
		sample[AudioClip::wallenstein_step5].load("res/audio/wallenstein_step5.mp3");
		sample[AudioClip::wallenstein_jump].load("res/audio/wallenstein_jump.mp3");
		sample[AudioClip::yeti_step1].load("res/audio/yeti/yeti_step1.mp3");
		sample[AudioClip::yeti_step2].load("res/audio/yeti/yeti_step2.mp3");
		sample[AudioClip::yeti_step3].load("res/audio/yeti/yeti_step3.mp3");
		sample[AudioClip::yeti_step4].load("res/audio/yeti/yeti_step4.mp3");
		sample[AudioClip::yeti_step5].load("res/audio/yeti/yeti_step5.mp3");
		sample[AudioClip::yeti_jump].load("res/audio/yeti/yeti_jump.mp3");
		sample[AudioClip::yeti_tear1].load("res/audio/yeti/yeti_tear1.mp3");
		sample[AudioClip::yeti_tear2].load("res/audio/yeti/yeti_tear2.mp3");
		sample[AudioClip::yeti_tear3].load("res/audio/yeti/yeti_tear3.mp3");
		sample[AudioClip::yeti_growl1].load("res/audio/yeti/yeti_growl1.mp3");
		sample[AudioClip::yeti_growl2].load("res/audio/yeti/yeti_growl2.mp3");
		sample[AudioClip::yeti_growl3].load("res/audio/yeti/yeti_growl3.mp3");
		sample[AudioClip::yeti_growl4].load("res/audio/yeti/yeti_growl4.mp3");
		sample[AudioClip::yeti_angry_growl1].load("res/audio/yeti/yeti_angry_growl1.mp3");
		sample[AudioClip::yeti_angry_growl2].load("res/audio/yeti/yeti_angry_growl2.mp3");
		sample[AudioClip::yeti_angry_growl3].load("res/audio/yeti/yeti_angry_growl3.mp3");
		sample[AudioClip::yeti_angry_growl4].load("res/audio/yeti/yeti_angry_growl4.mp3");
		sample[AudioClip::cave4].load("res/audio/cave4.mp3");
		sample[AudioClip::night1].load("res/audio/night1.mp3");
		sample[AudioClip::night2].load("res/audio/night2.mp3");
		sample[AudioClip::night3].load("res/audio/night3.mp3");
		sample[AudioClip::night4].load("res/audio/night4.mp3");
		sample[AudioClip::nightowl1].load("res/audio/nightowl1.mp3");
		sample[AudioClip::nightowl2].load("res/audio/nightowl2.mp3");
		sample[AudioClip::nightowl3].load("res/audio/nightowl3.mp3");
		sample[AudioClip::waterdrips2].load("res/audio/waterdrips2.mp3");
		sample[AudioClip::waterdrips3].load("res/audio/waterdrips3.mp3");
		sample[AudioClip::zombie_step1].load("res/audio/zombie_step1.mp3");
		sample[AudioClip::zombie_step2].load("res/audio/zombie_step2.mp3");
		sample[AudioClip::zombie_step3].load("res/audio/zombie_step3.mp3");
		sample[AudioClip::zombie_step4].load("res/audio/zombie_step4.mp3");
		sample[AudioClip::zombie_step5].load("res/audio/zombie_step5.mp3");
		sample[AudioClip::stamper_down].load("res/audio/stamper_down.mp3");
		sample[AudioClip::stamper_up].load("res/audio/stamper_up.mp3");
		sample[AudioClip::stamper_echo].load("res/audio/stamper_echo.mp3");
		sample[AudioClip::stamper_squish].load("res/audio/stamper_squish.mp3");
		sample[AudioClip::fireball_fly].load("res/audio/fireball_fly.mp3");
		sample[AudioClip::fireball_impact].load("res/audio/fireball_impact.mp3");
		sample[AudioClip::flashlight_pickup].load("res/audio/flashlight_pickup.mp3");
		sample[AudioClip::ghost_chains].load("res/audio/ghost_chains.mp3");
		sample[AudioClip::ghost_turn].load("res/audio/ghost_turn.mp3");
		sample[AudioClip::ghost_land].load("res/audio/ghost_land.mp3");
		sample[AudioClip::skull_voice1].load("res/audio/skull_voice1.mp3");
		sample[AudioClip::skull_voice2].load("res/audio/skull_voice2.mp3");
		sample[AudioClip::skull_voice3].load("res/audio/skull_voice3.mp3");
		sample[AudioClip::skull_hui].load("res/audio/skull_hui.mp3");
		sample[AudioClip::skull_death].load("res/audio/skull_death.mp3");
		sample[AudioClip::skull_fireball].load("res/audio/skull_fireball.mp3");
		sample[AudioClip::skull_impact].load("res/audio/skull_impact.mp3");
		sample[AudioClip::skull_shoot].load("res/audio/skull_shoot.mp3");
		sample[AudioClip::explosion1].load("res/audio/explosion1.mp3");
		sample[AudioClip::skullmaster_burp].load("res/audio/skullmaster_burp.mp3");
		sample[AudioClip::skullmaster_hurt1].load("res/audio/skullmaster_hurt1.mp3");
		sample[AudioClip::skullmaster_hurt2].load("res/audio/skullmaster_hurt2.mp3");
		sample[AudioClip::skullmaster_hurt3].load("res/audio/skullmaster_hurt3.mp3");
		sample[AudioClip::skullmaster_laugh1].load("res/audio/skullmaster_laugh1.mp3");
		sample[AudioClip::skullmaster_laugh2].load("res/audio/skullmaster_laugh2.mp3");
		sample[AudioClip::skullmaster_laugh3].load("res/audio/skullmaster_laugh3.mp3");

		sample[AudioClip::lavaloop1].load("res/audio/lavaloop1.mp3");
		sample[AudioClip::lavaloop2].load("res/audio/lavaloop2.mp3");
		sample[AudioClip::lavabubbles].load("res/audio/lavabubbles.mp3");

		sample[AudioClip::touchplateswitch1].load("res/audio/touchplateswitch1.mp3");
		sample[AudioClip::touchplateswitch2].load("res/audio/touchplateswitch2.mp3");


		sample[AudioClip::spider_walk_loop].load("res/audio/spider_walk_loop.mp3");
		sample[AudioClip::spider_spit].load("res/audio/spider_spit.mp3");
		sample[AudioClip::spider_spit_hit].load("res/audio/spider_spit_hit.mp3");
		sample[AudioClip::spider_net_unfold].load("res/audio/spider_net_unfold.mp3");

		sample[AudioClip::ladder_step1].load("res/audio/ladder_step1.mp3");
		sample[AudioClip::ladder_step2].load("res/audio/ladder_step2.mp3");
		sample[AudioClip::ladder_step3].load("res/audio/ladder_step3.mp3");
		sample[AudioClip::ladder_step4].load("res/audio/ladder_step4.mp3");
		sample[AudioClip::ladder_step5].load("res/audio/ladder_step5.mp3");
		sample[AudioClip::ladder_step6].load("res/audio/ladder_step6.mp3");
		sample[AudioClip::ladder_step7].load("res/audio/ladder_step7.mp3");

		sample[AudioClip::crate_loop].load("res/audio/crate_loop.mp3");
		sample[AudioClip::crate_falling].load("res/audio/crate_falling.mp3");

		sample[AudioClip::powercell_collected].load("res/audio/powercell_collected.mp3");
		sample[AudioClip::powercell_change].load("res/audio/powercell_change.mp3");
		sample[AudioClip::powercells_depleted].load("res/audio/powercells_depleted.mp3");

		sample[AudioClip::fireworks_loop].load("res/audio/fireworks_loop.mp3");

		sample[AudioClip::glimmer_plopp1].load("res/audio/glimmer/plopp1.mp3");
		sample[AudioClip::glimmer_plopp2].load("res/audio/glimmer/plopp2.mp3");
		sample[AudioClip::glimmer_plopp3].load("res/audio/glimmer/plopp3.mp3");
		sample[AudioClip::glimmer_plopp4].load("res/audio/glimmer/plopp4.mp3");
		sample[AudioClip::glimmer_plopp5].load("res/audio/glimmer/plopp5.mp3");
		sample[AudioClip::glimmer_plopp6].load("res/audio/glimmer/plopp6.mp3");
		sample[AudioClip::glimmer_effect1].load("res/audio/glimmer/effect1.mp3");
		sample[AudioClip::glimmer_effect2].load("res/audio/glimmer/effect2.mp3");
		sample[AudioClip::glimmer_chimes_loop1].load("res/audio/glimmer/chimes_loop1.mp3");
		sample[AudioClip::glimmer_awakens].load("res/audio/glimmer/awakens.mp3");
		sample[AudioClip::glimmer_up].load("res/audio/glimmer/up.mp3");
		sample[AudioClip::glimmer_down].load("res/audio/glimmer/down.mp3");

		sample[AudioClip::elevator_start].load("res/audio/elevator_start.mp3");
		sample[AudioClip::elevator_noiseloop].load("res/audio/elevator_noiseloop.mp3");
		sample[AudioClip::elevator_exit].load("res/audio/elevator_exit.mp3");


	} catch (const ppl7::Exception& exp) {
		exp.print();
		throw;
	}

}


void AudioPool::load_speech(const ppl7::String& lang)
{

	try {
		voice_george[VoiceGeorge::aua1].load("res/audio/george/common/aua1.mp3");
		voice_george[VoiceGeorge::aua2].load("res/audio/george/common/aua2.mp3");
		voice_george[VoiceGeorge::aua3].load("res/audio/george/common/aua3.mp3");
		voice_george[VoiceGeorge::aua4].load("res/audio/george/common/aua4.mp3");
		voice_george[VoiceGeorge::snort].load("res/audio/george/common/snort_loop.mp3");
		voice_george[VoiceGeorge::hepp1].load("res/audio/george/common/hepp1.mp3");
		voice_george[VoiceGeorge::hepp2].load("res/audio/george/common/hepp2.mp3");
		voice_george[VoiceGeorge::hepp3].load("res/audio/george/common/hepp3.mp3");
		voice_george[VoiceGeorge::hepp4].load("res/audio/george/common/hepp4.mp3");
		voice_george[VoiceGeorge::hepp5].load("res/audio/george/common/hepp5.mp3");


		voice_george[VoiceGeorge::hello].load("res/audio/george/de/hello.mp3");
		voice_george[VoiceGeorge::hello_questioned].load("res/audio/george/de/hello_questioned.mp3");
		voice_george[VoiceGeorge::hello_here_i_am].load("res/audio/george/de/hello_here_i_am.mp3");
		voice_george[VoiceGeorge::hello_im_george].load("res/audio/george/de/hello_im_george.mp3");
		voice_george[VoiceGeorge::play_with_me].load("res/audio/george/de/play_with_me.mp3");


		if (lang == "en") {

		}

	} catch (const ppl7::Exception& exp) {
		exp.print();
		throw;
	}

}
