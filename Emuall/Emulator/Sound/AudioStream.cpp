
#include "../../util/log.h"

#include "AudioStream.h"
#include "AudioBuffer.h"

AudioStream::AudioStream(AudioBuffer::Format format, int freq, int bufferSize, int numBuffers, callback_t callback, void *user) :
	_callback(callback), _user(user), _sampFreq(freq), _bufferSize(bufferSize), _format(format), _numBuffers(numBuffers)
{
	_source.SetLooping(false);
	_buffers.resize(numBuffers);
	// Clear buffers
	int size = _bufferSize * 2;
	void *dataPtr = nullptr;
	switch (_format) {
	case AudioBuffer::Mono8:
		size = _bufferSize;
	case AudioBuffer::Stereo8: {
		_workingBuffer.resize(size);
		memset(_workingBuffer.data(), 128, size);
		dataPtr = _workingBuffer.data();
		break;
	}
	default:
	case AudioBuffer::Mono16: 
		size = _bufferSize;
	case AudioBuffer::Stereo16: {
		size *= sizeof(short);
		_workingBuffer.resize(size);
		memset(_workingBuffer.data(), 0, size);
		break;
	}
	}
	for (auto &buffer : _buffers) {
		buffer.BufferData(_format, dataPtr, size, freq);
	}


	_source.QueueBuffers(_buffers);
}

AudioStream::AudioStream(AudioStream &other) :
	_user(other._user), _callback(other._callback), _format(other._format),
	_bufferSize(other._bufferSize),_numBuffers(other._numBuffers), _sampFreq(other._sampFreq),
	_buffers(std::move(other._buffers)), _source(other._source), _workingBuffer(std::move(other._workingBuffer))
{
	other._user = nullptr;
	other._callback = nullptr;
	other._bufferSize = 0;
	other._numBuffers = 0;
	other._sampFreq = 0;
}

AudioStream::~AudioStream()
{

}

AudioStream & AudioStream::operator=(AudioStream &other)
{
	_user = other._user;
	_callback = other._callback;
	_format = other._format;
	_bufferSize = other._bufferSize;
	_numBuffers = other._numBuffers;
	_sampFreq = other._sampFreq;
	_source = other._source;
	_buffers = std::move(other._buffers);
	_workingBuffer = std::move(other._workingBuffer);
	other._user = nullptr;
	other._callback = nullptr;
	other._bufferSize = 0;
	other._numBuffers = 0;
	other._sampFreq = 0;
	return *this;
}

void AudioStream::Tick()
{
	int processed = _source.GetNumBuffersProcessed();
	if (processed > 2) {
		Log(Debug, "Multiple buffers processed");
	}
	while (processed > 0) {
		AudioBuffer *buffer = _source.UnqueueBuffer();
		if (_callback == nullptr) {
			// Clear buffer
			switch (_format) {
			case AudioBuffer::Mono8:
			case AudioBuffer::Stereo8: 
				memset(_workingBuffer.data(), 128, _workingBuffer.size());
				buffer->BufferData(_format, _workingBuffer.data(), _workingBuffer.size(), _sampFreq);
				break;
			default:
			case AudioBuffer::Mono16:
			case AudioBuffer::Stereo16: 
				memset(_workingBuffer.data(), 0, _workingBuffer.size());
				buffer->BufferData(_format, _workingBuffer.data(), _workingBuffer.size(), _sampFreq);
				break;
			}
		}
		else {
			void *data = _workingBuffer.data();
			int dataSize = _bufferSize * 2;
			switch (_format) {
			case AudioBuffer::Mono8:
				dataSize = _bufferSize;
			case AudioBuffer::Stereo8:
				break;
			case AudioBuffer::Mono16:
				dataSize = _bufferSize;
			case AudioBuffer::Stereo16:
				dataSize *= sizeof(short);
				break;
			}
			_callback(_format, _sampFreq, _bufferSize, data, _user);
			buffer->BufferData(_format, data, dataSize, _sampFreq);
		}
		_source.QueueBuffers(1, buffer);
		processed--;
	}
	if (!_source.IsPlaying() && !_source.IsPaused()) {
		Log(Warn, "Stream stopped");
		_source.Play();
	}
}

void AudioStream::Play()
{
	_source.Play();
}

void AudioStream::Pause()
{
	_source.Pause();
}
