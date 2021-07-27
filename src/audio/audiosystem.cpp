#include "audio.h"
#include <unistd.h>


AudioSystem::AudioSystem()
{
	device_id=0;
	mixbuffer=NULL;
	if(SDL_WasInit(SDL_INIT_AUDIO) == 0) {
		printf ("SDL_InitSubSystem(SDL_INIT_AUDIO)\n");
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
	desired.samples=4096;
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


void AudioSystem::callback(Uint8* stream, int len)
{
	//memset(stream,0,len);
	int samples=len/sizeof(ppl7::STEREOSAMPLE16);
	memset(mixbuffer,0,samples*sizeof(ppl7::STEREOSAMPLE32));
	//printf ("callback called, len=%d\n",len);
	std::set<Audio *>::iterator it;
	mutex.lock();
	for (it=tracks.begin();it!=tracks.end();++it) {
		Audio *audio=(*it);
		audio->addSamples(samples, mixbuffer);
	}
	size_t num_tracks=tracks.size();
	ppl7::STEREOSAMPLE16 *mergebuffer=(ppl7::STEREOSAMPLE16*) stream;
	for (int i=0;i<samples;i++) {
		mergebuffer[i].left=mixbuffer[i].left/num_tracks;
		mergebuffer[i].right=mixbuffer[i].right/num_tracks;
	}
	mutex.unlock();
	// TODO: we need to detect when a track is at its end and remove it from the set
}

void AudioSystem::play(Audio *audio)
{
	mutex.lock();
	tracks.insert(audio);
	mutex.unlock();
}


void AudioSystem::test()
{
	//std::list<ppl7::String> name_list;
	//enumerateDrivers(name_list);
	//initDriver(name_list.front());
	//enumerateDevices(name_list);
	init();
	printf ("open audio file\n");
	AudioStream song1("res/audio/PatrickF-ID.mp3");
	AudioStream song2("res/audio/PatrickF-In_The_Hall_Of_The_Mountain_King.mp3");
	AudioStream song3("/home/patrickf/Patrick F. - Experiments (Album)/027-Patrick F. - ID (Extended Mix).aiff");

	play(&song3);
	//play(&song2);
	SDL_Delay(20000); /* let the audio callback play some sound for 5 seconds. */

	shutdown();
}
