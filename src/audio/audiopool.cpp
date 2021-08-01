#include "audiopool.h"

static AudioPool *audiopool=NULL;

AudioPool &getAudioPool()
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
	if (audiopool==this) audiopool=NULL;
}

void AudioPool::setAudioSystem(AudioSystem *audio)
{
	this->audio=audio;
}

void AudioPool::playOnce(AudioClip::Id id, float volume)
{
	AudioInstance *instance=new AudioInstance(sample[id]);
	instance->setVolume(volume);
	instance->setAutoDelete(true);
	audio->play(instance);
}

void AudioPool::playOnce(AudioClip::Id id, const ppl7::grafix::Point &p, int max_distance, float volume)
{
	if (id<AudioClip::maxClips) {
		AudioInstance *instance=new AudioInstance(sample[id]);
		instance->setVolume(volume);
		instance->setAutoDelete(true);
		instance->setPositional(p, max_distance);
		audio->play(instance);
	}
}

AudioInstance *AudioPool::getInstance(AudioClip::Id id)
{
	if (id<AudioClip::maxClips) return new AudioInstance(sample[id]);
	return NULL;
}

void AudioPool::playInstance(AudioInstance *instance)
{
	audio->play(instance);
}

void AudioPool::load()
{
	song[0].open("res/audio/PatrickF-In_The_Hall_Of_The_Mountain_King.mp3");
	song[1].open("res/audio/PatrickF-ID.mp3");
	song[2].open("res/audio/PatrickF-Sonic_Waves.mp3");

	sample[AudioClip::coin1].load("res/audio/341695__projectsu012__coins-1.mp3");
	sample[AudioClip::coin2].load("res/audio/402766__matrixxx__retro-coin-04.wav");
	sample[AudioClip::impact].load("res/audio/221626__ansel__body-impact.mp3");
	sample[AudioClip::electric].load("res/audio/65232__carbilicon__electr_stereo.mp3");
	sample[AudioClip::crash].load("res/audio/95078__sandyrb__the-crash.mp3");
	sample[AudioClip::skeleton_death].load("res/audio/skeleton_break.wav");
	sample[AudioClip::skeleton_turn].load("res/audio/skeleton_turn.wav");
	sample[AudioClip::threespeers_activation].load("res/audio/threespeers.mp3");
	sample[AudioClip::stone_drag_long].load("res/audio/stone_drag_long.mp3");
	sample[AudioClip::stone_drag_short].load("res/audio/stone_drag_short.mp3");
	sample[AudioClip::trap1].load("res/audio/trap1.wav");
	sample[AudioClip::trap2].load("res/audio/trap2.wav");
	sample[AudioClip::vent1].load("res/audio/vent1.mp3");
	sample[AudioClip::vent2].load("res/audio/vent2.mp3");
	sample[AudioClip::wind1].load("res/audio/wind1.mp3");
	sample[AudioClip::fire1].load("res/audio/fire1.mp3");
	sample[AudioClip::fire2].load("res/audio/fire2.mp3");
	sample[AudioClip::light_switch1].load("res/audio/light_switch_1.wav");
	sample[AudioClip::crystal].load("res/audio/crystal.wav");
	sample[AudioClip::birds1].load("res/audio/birds.mp3");
	sample[AudioClip::arrow_swoosh].load("res/audio/arrow-swoosh.mp3");

}
