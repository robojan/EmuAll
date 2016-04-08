#ifndef AUDIOSTREAM_H_
#define AUDIOSTREAM_H_

#include <vector>

#include "AudioSource.h"
#include "AudioBuffer.h"

class AudioStream {
public:
	typedef void(*callback_t)(AudioBuffer::Format format, int freq, int elements, void *data, void *user);
	AudioStream(AudioBuffer::Format format, int freq, int bufferSize, int numBuffers, callback_t callback, void *user);
	AudioStream(AudioStream &other);
	~AudioStream();

	AudioStream &operator=(AudioStream &other);
	
	void Tick();

	void Play();
	void Pause();
private:
	void *_user;
	callback_t _callback;

	AudioBuffer::Format _format;
	int _bufferSize;
	int _numBuffers;
	int _sampFreq;
	std::vector<AudioBuffer> _buffers;
	AudioSource _source;

	std::vector<unsigned char> _workingBuffer;

};

#endif