#include "audio.h"
#include <unistd.h>

Audio::Audio()
{
	autoDeleteFlag=false;
}

Audio::~Audio()
{

}

void Audio::setAutoDelete(bool flag)
{
	autoDeleteFlag=flag;
}

bool Audio::autoDelete() const
{
	return autoDeleteFlag;
}


AudioSystem::AudioSystem()
{
	device_id=0;
	mixbuffer=NULL;
	if(SDL_WasInit(SDL_INIT_AUDIO) == 0) {
		//printf ("SDL_InitSubSystem(SDL_INIT_AUDIO)\n");
		if (0!=SDL_InitSubSystem(SDL_INIT_AUDIO)) {
			throw AudioSystemFailed("could not init audio subsystem!");
		}
	}
}

AudioSystem::~AudioSystem()
{
	shutdown();
}

void AudioSystem::shutdown()
{
	mutex.lock();
	tracks.clear();
	mutex.unlock();
	if (device_id>0) {
		SDL_CloseAudioDevice(device_id);
	}
	device_id=0;
	free(mixbuffer);
	mixbuffer=NULL;
}

void AudioSystem::enumerateDrivers(std::list<ppl7::String> &driver_names) const
{
	driver_names.clear();
	for(int i=0; i<SDL_GetNumAudioDrivers(); ++i) {
		const char* driver_name = SDL_GetAudioDriver(i);
		driver_names.push_back(ppl7::String(driver_name));
	}
}

void AudioSystem::enumerateDevices(std::list<ppl7::String> &device_names) const
{
	device_names.clear();
	for(int i=0; i<SDL_GetNumAudioDevices(0); ++i) {
		const char* device_name = SDL_GetAudioDeviceName(i,0);
		device_names.push_back(ppl7::String(device_name));
	}
}

void AudioSystem::initDriver(const ppl7::String &driver_name)
/*!Initialize a specific audio driver
 *
 * @param[in] driver_name String with the name of a specific driver
 *
 * @note
 * If not calling this method, a default driver is used
 */
{
	if (0!=SDL_AudioInit((const char*) driver_name)) {
		throw AudioSystemFailed("failed to initialize audio driver %s\n",(const char*) driver_name);
	}
}

static void AudioSystem_AudioCallback(void *userdata, Uint8* stream, int len)
{
	((AudioSystem*)userdata)->callback(stream, len);
}

void AudioSystem::init(const ppl7::String &device)
{
	shutdown();
	const char *device_name=NULL;
	if (device.notEmpty()) device_name=(const char*)device;
	SDL_AudioSpec desired;
	SDL_AudioSpec obtained;
	SDL_memset(&desired, 0, sizeof(desired));
	SDL_memset(&obtained, 0, sizeof(obtained));
	desired.freq=44100;
	desired.format=AUDIO_S16LSB;
	desired.channels=2;
	desired.samples=1024;
	desired.callback=AudioSystem_AudioCallback;
	desired.userdata=this;
	mixbuffer=(ppl7::STEREOSAMPLE32*)malloc(desired.samples*desired.channels*sizeof(ppl7::STEREOSAMPLE32));

	device_id=SDL_OpenAudioDevice(
			device_name, 0, &desired, &obtained, 0);
	if (device_id==0) {
		throw AudioSystemFailed("could not open audio device: %s", SDL_GetError());
	}
	SDL_PauseAudioDevice(device_id, 0); /* start audio playing. */
}

static inline int clamp(int value)
{
	if (value>32767) return 32767;
	if (value<-32767) return -32767;
	return value;
}


void AudioSystem::callback(Uint8* stream, int len)
{
	size_t samples=len/sizeof(ppl7::STEREOSAMPLE16);
	memset(mixbuffer,0,samples*sizeof(ppl7::STEREOSAMPLE32));
	//printf ("callback called, len=%d\n",len);
	std::set<Audio *>::iterator it;
	std::set<Audio *> to_remove;
	mutex.lock();
	size_t num_tracks=tracks.size();
	if (num_tracks) {
		for (it=tracks.begin();it!=tracks.end();++it) {
			Audio *audio=(*it);
			if (audio->addSamples(samples, mixbuffer)!=samples) {
				to_remove.insert(audio);
			}
		}
		ppl7::STEREOSAMPLE16 *mergebuffer=(ppl7::STEREOSAMPLE16*) stream;
		for (size_t i=0;i<samples;i++) {
			mergebuffer[i].left=clamp(mixbuffer[i].left);
			mergebuffer[i].right=clamp(mixbuffer[i].right);
		}
		if (to_remove.size()>0) {
			for (it=to_remove.begin();it!=to_remove.end();++it) {
				Audio *audio=(*it);
				std::set<Audio *>::iterator del=tracks.find(audio);
				if (del!=tracks.end()) {
					tracks.erase(del);
					if (audio->autoDelete()) {
						delete audio;
					}
				}
			}
		}
	} else {
		memset(stream,0,len);
	}
	mutex.unlock();
}

void AudioSystem::play(Audio *audio)
{
	mutex.lock();
	tracks.insert(audio);
	mutex.unlock();
}

void AudioSystem::stop(Audio *audio)
{
	mutex.lock();
	std::set<Audio *>::iterator it=tracks.find(audio);
	if (it!=tracks.end()) tracks.erase(it);
	mutex.unlock();
}

bool AudioSystem::isPlaying(Audio *audio)
{
	bool result=false;
	mutex.lock();
	std::set<Audio *>::const_iterator it=tracks.find(audio);
	if (it!=tracks.end()) result=true;
	mutex.unlock();
	return result;
}

void AudioSystem::test()
{
	//std::list<ppl7::String> name_list;
	//enumerateDrivers(name_list);
	//initDriver(name_list.front());
	//enumerateDevices(name_list);
	init();
	//printf ("open audio file\n");
	AudioSample sample1("res/audio/95078__sandyrb__the-crash.mp3");
	AudioSample sample2("res/audio/65232__carbilicon__electr_stereo.mp3");
	AudioStream song1("res/audio/PatrickF-ID.mp3");
	AudioStream song2("res/audio/PatrickF-In_The_Hall_Of_The_Mountain_King.mp3");
	//AudioStream song3("/home/patrickf/Patrick F. - Experiments (Album)/027-Patrick F. - ID (Extended Mix).aiff");
	AudioInstance effect1(sample1);
	AudioInstance effect2(sample2);
	AudioInstance effect3(sample1);
	effect2.setVolume(127, 127);
	effect2.setLoop(true);
	play(&effect1);
	play(&effect2);
	SDL_Delay(3000);
	play(&effect3);
	/*
	play(&song2);
	SDL_Delay(2000);
	song2.setVolume(512, 512);
	play(&effect2);
	SDL_Delay(4000);
	song2.setVolume(1024, 1024);
	effect1.rewind();
	play(&effect1);
	SDL_Delay(10000);
	stop(&effect2);
	song2.setVolume(127, 127);
	*/
	SDL_Delay(10000);
	shutdown();
}
