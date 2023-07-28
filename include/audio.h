#ifndef INCLUDE_AUDIO_H_
#define INCLUDE_AUDIO_H_
#include "stdlib.h"
#include "stdio.h"
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-audio.h>
#include <SDL.h>
#include <list>
#include <set>


#ifndef DECKER_EXCEPTION
#define DECKER_EXCEPTION
#define STR_VALUE(arg)      #arg
#define EXCEPTION(name,inherit)	class name : public inherit { public: \
	name() throw() {}; \
	name(const char *msg, ...) throw() {  \
		va_list args; va_start(args, msg); copyText(msg,args); \
		va_end(args); } \
		virtual const char* what() const throw() { return (STR_VALUE(name)); } \
	};
#endif

EXCEPTION(AudioSystemFailed, ppl7::Exception);
EXCEPTION(UnknownAudioFormat, ppl7::Exception);

enum class AudioClass
{
	Unknown=0,
	Effect,
	Music,
	Speech
};


class Audio
{
private:
	AudioClass a_class;
	bool autoDeleteFlag;
public:
	Audio();
	virtual ~Audio();
	void setAutoDelete(bool flag);
	void setAudioClass(AudioClass a);
	AudioClass audioclass() const;
	bool autoDelete() const;
	virtual size_t addSamples(size_t num, ppl7::STEREOSAMPLE32* buffer, float volume)=0;
};

class AudioStream : public Audio
{
private:
	ppl7::File ff;
	ppl7::AudioDecoder* decoder;
	float volume;
	ppl7::STEREOSAMPLE16* prebuffer;
	size_t buffersize;
	float fade_start_volume;
	float fade_time;
	double fade_start;
public:
	AudioStream();
	AudioStream(AudioClass a);
	AudioStream(const ppl7::String& filename, AudioClass a=AudioClass::Music);
	virtual ~AudioStream();
	void open(const ppl7::String& filename);
	void rewind();
	void setVolume(float volume);
	void fadeout(float seconds=4.0f);
	virtual size_t addSamples(size_t num, ppl7::STEREOSAMPLE32* buffer, float volume);
};

class AudioSample
{
private:
	ppl7::ByteArray buffer;
public:
	AudioSample();
	AudioSample(const ppl7::String& filename);
	~AudioSample();
	void load(const ppl7::String& filename);
	size_t size() const;
	size_t addSamples(size_t position, size_t num, ppl7::STEREOSAMPLE32* buffer, int vol_left=32768, int vol_right=32768) const;
	size_t skipSamples(size_t position, size_t num) const;
};

class AudioInstance : public Audio
{
private:
	const AudioSample* sample;
	size_t position;
	float volume;
	int max_distance;
	bool loop;
	bool positional;
	float fade_start_volume;
	float fade_time;
	double fade_start;
	ppl7::grafix::Point p;
	size_t skipSamples(size_t num);

public:
	AudioInstance();
	AudioInstance(AudioClass a);
	AudioInstance(const AudioSample& sample, AudioClass a=AudioClass::Effect);
	void load(const AudioSample& sample);
	void rewind();
	void setVolume(float volume);
	void setLoop(bool loop);
	void fadeout(float seconds=4.0f);
	void setPositional(const ppl7::grafix::Point& p, int max_distance=1600);
	virtual size_t addSamples(size_t num, ppl7::STEREOSAMPLE32* buffer, float volume);
};

class AudioSystem
{
private:
	int device_id;
	ppl7::Mutex mutex;
	std::set<Audio*> tracks;
	ppl7::STEREOSAMPLE32* mixbuffer;
	float globalVolume;
	float a_class_volume[4];


public:
	AudioSystem();
	~AudioSystem();

	void enumerateDrivers(std::list<ppl7::String>& driver_names) const;
	void enumerateDevices(std::list<ppl7::String>& device_names) const;
	void initDriver(const ppl7::String& driver_name);	// optional
	void init(const ppl7::String& device=ppl7::String());
	void play(Audio* audio);
	void stop(Audio* audio);
	bool isPlaying(Audio* audio);
	void shutdown();
	void test();

	void setGlobalVolume(float volume);
	void setVolume(AudioClass a_class, float volume);

	void callback(Uint8* stream, int len);

};


#endif //INCLUDE_AUDIO_H_
