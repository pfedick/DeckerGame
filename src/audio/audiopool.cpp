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

void AudioPool::playOnce(AudioClip::Id id, int volume)
{
	AudioInstance *instance=new AudioInstance(sample[id]);
	instance->setVolume(volume,volume);
	instance->setAutoDelete(true);
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

}
