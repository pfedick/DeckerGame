#include "audio.h"
#include <unistd.h>

Audio::Audio()
{
	autoDeleteFlag=false;
	a_class=AudioClass::Unknown;
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

void Audio::setAudioClass(AudioClass a)
{
	a_class=a;
}

AudioClass Audio::audioclass() const
{
	return a_class;
}


AudioSystem::AudioSystem()
{
	device_id=0;
	mixbuffer=NULL;
	if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
		//printf ("SDL_InitSubSystem(SDL_INIT_AUDIO)\n");
		if (0 != SDL_InitSubSystem(SDL_INIT_AUDIO)) {
			throw AudioSystemFailed("could not init audio subsystem!");
		}
	}
	globalVolume=1.0f;
	a_class_volume[static_cast<int>(AudioClass::Effect)]=1.0;
	a_class_volume[static_cast<int>(AudioClass::Music)]=1.0;
	a_class_volume[static_cast<int>(AudioClass::Speech)]=1.0;
	a_class_volume[static_cast<int>(AudioClass::Ambience)]=1.0;
}

AudioSystem::~AudioSystem()
{
	shutdown();
}

void AudioSystem::shutdown()
{
	std::set<Audio*>::iterator it;
	if (device_id > 0) {
		SDL_CloseAudioDevice(device_id);
	}
	mutex.lock();
	for (it=tracks.begin();it != tracks.end();++it) {
		if ((*it)->autoDelete()) {
			delete (*it);
		}
	}
	tracks.clear();
	mutex.unlock();
	device_id=0;
	free(mixbuffer);
	mixbuffer=NULL;
}

void AudioSystem::enumerateDrivers(std::list<ppl7::String>& driver_names) const
{
	driver_names.clear();
	for (int i=0; i < SDL_GetNumAudioDrivers(); ++i) {
		const char* driver_name = SDL_GetAudioDriver(i);
		driver_names.push_back(ppl7::String(driver_name));
	}
}

void AudioSystem::enumerateDevices(std::list<ppl7::String>& device_names) const
{
	device_names.clear();
	for (int i=0; i < SDL_GetNumAudioDevices(0); ++i) {
		const char* device_name = SDL_GetAudioDeviceName(i, 0);
		device_names.push_back(ppl7::String(device_name));
	}
}

void AudioSystem::initDriver(const ppl7::String& driver_name)
/*!Initialize a specific audio driver
 *
 * @param[in] driver_name String with the name of a specific driver
 *
 * @note
 * If not calling this method, a default driver is used
 */
{
	if (0 != SDL_AudioInit((const char*)driver_name)) {
		throw AudioSystemFailed("failed to initialize audio driver %s\n", (const char*)driver_name);
	}
}

static void AudioSystem_AudioCallback(void* userdata, Uint8* stream, int len)
{
	((AudioSystem*)userdata)->callback(stream, len);
}

void AudioSystem::init(const ppl7::String& device)
{
	shutdown();
	const char* device_name=NULL;
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
	device_id=SDL_OpenAudioDevice(
		device_name, 0, &desired, &obtained, 0);
	if (device_id == 0) {
		throw AudioSystemFailed("could not open audio device: %s", SDL_GetError());
	}
	mixbuffer=(ppl7::STEREOSAMPLE32*)malloc(obtained.samples * obtained.channels * sizeof(ppl7::STEREOSAMPLE32));
	SDL_PauseAudioDevice(device_id, 0); /* start audio playing. */
}

static inline int clamp(int value)
{
	if (value > 32767) return 32767;
	if (value < -32767) return -32767;
	return value;
}


void AudioSystem::callback(Uint8* stream, int len)
{
	double start_time=ppl7::GetMicrotime();
	size_t tracks_hearable=0;
	size_t samples=len / sizeof(ppl7::STEREOSAMPLE16);
	memset(mixbuffer, 0, samples * sizeof(ppl7::STEREOSAMPLE32));
	//ppl7::PrintDebugTime("callback called, len=%d\n", len);
	std::set<Audio*>::iterator it;
	std::set<Audio*> to_remove;
	mutex.lock();
	size_t num_tracks=tracks.size();
	if (num_tracks) {
		//ppl7::PrintDebugTime("AudioSystem::callback, we have %zd Tracks\n", num_tracks);
		for (it=tracks.begin();it != tracks.end();++it) {
			Audio* audio=(*it);
			if (audio->isHearable()) tracks_hearable++;
			float volume=globalVolume * a_class_volume[static_cast<int>(audio->audioclass())];
			if (audio->addSamples(samples, mixbuffer, volume) != samples) {
				to_remove.insert(audio);
			}
		}
		ppl7::STEREOSAMPLE16* mergebuffer=(ppl7::STEREOSAMPLE16*)stream;
		for (size_t i=0;i < samples;i++) {
			mergebuffer[i].left=clamp(mixbuffer[i].left);
			mergebuffer[i].right=clamp(mixbuffer[i].right);
		}
		if (to_remove.size() > 0) {
			//ppl7::PrintDebugTime("AudioSystem::callback, we have %zd Tracks to delete\n", to_remove.size());
			for (it=to_remove.begin();it != to_remove.end();++it) {
				Audio* audio=(*it);
				std::set<Audio*>::iterator del=tracks.find(audio);
				if (del != tracks.end()) {
					tracks.erase(del);
					if (audio->autoDelete()) {
						delete audio;
					}
				}
			}
		}
	} else {
		memset(stream, 0, len);
	}
	metrics_mutex.lock();
	metrics.tracks_total=num_tracks;
	metrics.tracks_played=tracks_hearable;

	mutex.unlock();
	double total_time=ppl7::GetMicrotime() - start_time;
	//ppl7::PrintDebugTime("Audio Time=%0.3f ms, total: %zd, hearable: %zd\n", total_time * 1000.0f, num_tracks, tracks_hearable);
	metrics.time+=total_time;
	metrics_mutex.unlock();

}

void AudioSystem::play(Audio* audio)
{
	//ppl7::PrintDebugTime("AudioSystem::play\n");
	mutex.lock();
	tracks.insert(audio);
	mutex.unlock();
}

void AudioSystem::stop(Audio* audio)
{
	//ppl7::PrintDebugTime("AudioSystem::stop\n");
	mutex.lock();
	std::set<Audio*>::iterator it=tracks.find(audio);
	if (it != tracks.end()) tracks.erase(it);
	mutex.unlock();
}

bool AudioSystem::isPlaying(Audio* audio)
{
	bool result=false;
	mutex.lock();
	std::set<Audio*>::const_iterator it=tracks.find(audio);
	if (it != tracks.end()) result=true;
	mutex.unlock();
	return result;
}

void AudioSystem::setGlobalVolume(float volume)
{
	globalVolume=volume;
}

void AudioSystem::setVolume(AudioClass a_class, float volume)
{
	a_class_volume[static_cast<int>(a_class)]=volume;
}

AudioSystem::Metrics AudioSystem::getMetrics(bool reset)
{
	metrics_mutex.lock();
	Metrics m=metrics;
	if (reset) metrics.time=0.0f;
	metrics_mutex.unlock();
	//ppl7::PrintDebugTime("AudioSystem::getMetrics Time=%0.3f ms, total: %zd, hearable: %zd\n", m.time * 1000.0f, m.tracks_total, m.tracks_played);
	return m;
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
	effect2.setVolume(32768.0f);
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
