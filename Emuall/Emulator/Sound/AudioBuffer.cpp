
#include <AL/al.h>

#include "AudioBuffer.h"
#include "AudioContext.h"
#include "AudioException.h"

AudioBuffer::AudioBuffer() :
	_buffer(AL_INVALID), _format(Mono16), _freq(0), _size(0)
{
	alGenBuffers(1, &_buffer);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not create openAL buffer");
	}
}

AudioBuffer::AudioBuffer(AudioBuffer &other) :
	_buffer(other._buffer), _format(other._format), _freq(other._freq), _size(other._size)
{
	other._buffer = AL_INVALID;
}

AudioBuffer::~AudioBuffer()
{
	if (alIsBuffer(_buffer)) {
		alDeleteBuffers(1, &_buffer);
	}
}

void AudioBuffer::BufferData(Format format, const void *data, int size, int freq)
{
	ALenum dataFormat;
	switch (format) {
	case Mono8:
		dataFormat = AL_FORMAT_MONO8;
		break;
	case Mono16:
		dataFormat = AL_FORMAT_MONO16;
		break;
	case Stereo8:
		dataFormat = AL_FORMAT_STEREO8;
		break;
	case Stereo16:
		dataFormat = AL_FORMAT_STEREO16;
		break;
	default:
		throw AudioException(AL_INVALID_VALUE, "Invalid data type");
		break;
	}

	_size = size;
	_freq = freq;
	_format = format;

	alBufferData(_buffer, dataFormat, data, size, freq);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not load data in openAL buffer");
	}
}

void AudioBuffer::AddBufferIDToList(std::vector<unsigned int> &list)
{
	list.push_back(_buffer);
}

AudioBuffer & AudioBuffer::operator=(AudioBuffer &other)
{
	if (alIsBuffer(_buffer)) {
		alDeleteBuffers(1, &_buffer);
	}

	_buffer = other._buffer;
	other._buffer = AL_INVALID;
	return *this;
}
