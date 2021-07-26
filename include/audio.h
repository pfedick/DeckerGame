#include "stdlib.h"
#include "stdio.h"
#include <ppl7.h>
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


class Audio
{
public:
	virtual ~Audio()=0;
	virtual size_t addSamples(size_t num, ppl7::STEREOSAMPLE32 *buffer)=0;
};

class AudioStream : public Audio
{
private:
	ppl7::File ff;
	ppl7::AudioDecoder *decoder;
public:
	AudioStream();
	AudioStream(const ppl7::String &filename);
	virtual ~AudioStream();
	void open(const ppl7::String &filename);
	void rewind();
	virtual size_t addSamples(size_t num, ppl7::STEREOSAMPLE32 *buffer);
};

class AudioSample : public Audio
{
private:
	ppl7::ByteArray buffer;
public:
	AudioSample();
	AudioSample(const ppl7::String &filename);
	virtual ~AudioSample();
	void load(const ppl7::String &filename);
	void rewind();
	virtual size_t addSamples(size_t num, ppl7::STEREOSAMPLE32 *buffer);
};

class AudioSystem
{
private:
	int device_id;
	ppl7::Mutex mutex;
	std::set<Audio *> tracks;
	ppl7::STEREOSAMPLE32 *mixbuffer;


public:
	AudioSystem();
	~AudioSystem();

	void enumerateDrivers(std::list<ppl7::String> &driver_names) const;
	void enumerateDevices(std::list<ppl7::String> &device_names) const;
	void initDriver(const ppl7::String &driver_name);	// optional
	void init(const ppl7::String &device=ppl7::String());
	void play(Audio *audio);
	void shutdown();
	void test();

	void callback(Uint8* stream, int len);

};
