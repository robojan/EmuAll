#include "GbChannel.h"

#include "../../util/log.h"
#include "../defines.h"
#include <AL/al.h>


GbChannel::GbChannel(unsigned int sampleFreq, int channel) : 
	_source(-1), _tickCounter(0), _currentValue(0), _workingBuffer(NULL), 
	_workingBufferSamples(0), _playingCounter(0), _leftVolume(7), _rightVolume(7),
	_sampleFreq(sampleFreq), _channel(channel), _audioEnabled(false), 
	_audioOut(NULL), _audioCpuOut(NULL), _cpuWritten(0), _lastValue(0), _lastLVol(7), _lastRVol(7)
{
	ALenum alRet;

	std::string outputname = "ch";
	outputname.append(std::to_string(channel));
	outputname.append(".wav");

	_audioOut = new robojan::WavFileOutStream(outputname.c_str(), robojan::WavFileOutStream::PCM16, _sampleFreq, 2);
	outputname = "ch";
	outputname.append(std::to_string(channel));
	outputname.append("_cpu.wav");
	_audioCpuOut = new robojan::WavFileOutStream(outputname.c_str(), robojan::WavFileOutStream::PCM16, _sampleFreq, 1);

	// Reset openAL errors
	alGetError();
	// Create source 
	alGenSources(1, &_source);
	if ((alRet = alGetError()) != AL_NO_ERROR)
	{
		Log(Error, "Could not generate audio source for channel %d: %d", channel, alRet);
	}
	alSourcei(_source, AL_LOOPING, AL_FALSE);

	// Create buffers
	alGenBuffers(NUM_BUFFERS, _buffers);
	if ((alRet = alGetError()) != AL_NO_ERROR)
	{
		Log(Error, "Could not generate audio buffer for channel %d: %d", channel, alRet);
	}

	// Load buffers
	for (int i = 0; i < NUM_BUFFERS; ++i)
	{
		LoadBuffer(_buffers[i]);
	}

	// Push the buffers on the stack
	alSourceQueueBuffers(_source, NUM_BUFFERS, _buffers);
	if ((alRet = alGetError()) != AL_NO_ERROR)
	{
		Log(Error, "Could not push audio buffers in the queue for channel %d: %d", channel, alRet);
	}

	alSourcePlay(_source);
}

GbChannel::~GbChannel()
{

	if (_audioOut)
	{
		delete _audioOut;
	}
	if (_audioCpuOut)
	{
		delete _audioCpuOut;
	}
	if (_workingBuffer)
	{
		delete[] _workingBuffer;
	}
	if (alIsSource(_source))
	{
		alDeleteSources(1, &_source);
	}
	if (alIsBuffer(_buffers[0]))
	{
		alDeleteBuffers(NUM_BUFFERS, _buffers);
	}
}

void GbChannel::SlowTick()
{
	ALenum alRet;
	if (alIsSource(_source) == AL_FALSE)
		return; // Audio not loaded

	// Check whether data is loaded
	ALint buffersProcessed;
	alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffersProcessed);

	while (buffersProcessed > 0)
	{
		uint32_t buffer;
		alSourceUnqueueBuffers(_source, 1, &buffer);
		if ((alRet = alGetError()) != AL_NO_ERROR)
		{
			Log(Error, "Could not unqueue audio buffer for channel %d: %d", _channel, alRet);
		}
		else {
			LoadBuffer(buffer);
			alSourceQueueBuffers(_source, 1, &buffer);
			if ((alRet = alGetError()) != AL_NO_ERROR)
			{
				Log(Error, "Could not queue audio buffer for channel %d: %d", _channel, alRet);
			}
		}
		--buffersProcessed;
	}

	// Check whether the source is playing, if not start it.
	ALint sourceState;
	alGetSourcei(_source, AL_SOURCE_STATE, &sourceState);
	if (sourceState != AL_PLAYING && sourceState != AL_PAUSED)
	{
		alSourcePlay(_source);
	}
}

void GbChannel::LoadBuffer(uint32_t buffer)
{
	ALenum alRet;
	//alGetSourcei(buffer, AL_FREQUENCY, &sampleFrequency);
	if (!_workingBuffer)
	{
		_workingBufferSamples = (_sampleFreq*BUFFER_TIME * 2) / 1000UL;
		_workingBuffer = new int16_t[_workingBufferSamples]; // create a working buffer
	}
	if (_tickCounter < (FCPU / _sampleFreq)*(_workingBufferSamples / 2) + _playingCounter)
	{
		// When there is not yet enough data to fill a buffer, skip it. 
		memset(_workingBuffer, 0, _workingBufferSamples*sizeof(int16_t)); // clear the buffer
		Log(Warn, "Channel %d underrun", _channel);
		alBufferData(buffer, AL_FORMAT_STEREO16, (ALvoid*) _workingBuffer, _workingBufferSamples*sizeof(int16_t), _sampleFreq);
		if ((alRet = alGetError()) != AL_NO_ERROR)
		{
			Log(Error, "Could not load audio buffer for channel %d: %d", _channel, alRet);
		}
		return;
	}

	if (_playingCounter < _tickCounter - (FCPU / _sampleFreq)*(_workingBufferSamples / 2) * NUM_BUFFERS)
	{
		_playingCounter = _tickCounter - (FCPU / _sampleFreq)*(_workingBufferSamples / 2) * NUM_BUFFERS;
	}

	ChannelEvent *event;
	if (_eventQueue.empty()) {
		event = NULL;
	}
	else {
		event = &_eventQueue.front();
	}
	for (size_t i = 0; i < _workingBufferSamples; i += 2)
	{
		_playingCounter += FCPU / _sampleFreq;

		if (event != NULL && event->when <= _playingCounter) {
			// Handle the event
			switch (event->type) {
			case ChannelEvent::EVT_TYPE_SOUND_VALUE:
				_currentValue = event->soundValue.value;
				break;
			case ChannelEvent::EVT_TYPE_VOLUME:
				_leftVolume = event->soundVolume.lVol;
				_rightVolume = event->soundVolume.rVol;
				break;
			default:
				Log(Warn, "Unknown channel event %d received.", event->type);
			}

			// Get a new element
			_eventQueue.pop();
			if (_eventQueue.empty()) {
				event = NULL;
			}
			else {
				event = &_eventQueue.front();
			}
		}
		// Write the value to the buffer
		_workingBuffer[i] = (_leftVolume*_currentValue) / 7;
		_workingBuffer[i + 1] = (_rightVolume*_currentValue) / 7;
	}
	alBufferData(buffer, AL_FORMAT_STEREO16, (ALvoid*) _workingBuffer, _workingBufferSamples*sizeof(int16_t), _sampleFreq);
	if (_audioOut)
		_audioOut->WritePCM16(_workingBuffer, _workingBufferSamples/2);
	if ((alRet = alGetError()) != AL_NO_ERROR)
	{
		Log(Error, "Could not load audio buffer for channel %d: %d", _channel, alRet);
	}
}

void GbChannel::CleanSoundOutput()
{
	if (_eventQueue.size() > 100000) {
		Log(Warn, "Sound event queue overflow");
	}

	while (_eventQueue.size() > 100000)
		_eventQueue.pop();
}

void GbChannel::SetAmplitude(uint_fast8_t left, uint_fast8_t right)
{
	if (!_audioEnabled || (left == _lastLVol && right == _lastRVol))
		return;

	ChannelEvent event;
	event.when = _tickCounter;
	event.type = ChannelEvent::EVT_TYPE_VOLUME;
	event.soundVolume.lVol = left;
	event.soundVolume.rVol = right;
	_eventQueue.push(event);

	// fill up the buffer to now
	int16_t val = (_lastLVol * _lastValue) / 7;
	while (_cpuWritten < _tickCounter)
	{
		_audioCpuOut->WritePCM16(&val, 1);
		_cpuWritten += FCPU / _sampleFreq;
	}

	_lastLVol = left;
	_lastRVol = right;
}

void GbChannel::SetOutputValue(int16_t value)
{
	if (!_audioEnabled || value == _lastValue)
		return;

	ChannelEvent event;
	event.when = _tickCounter;
	event.type = ChannelEvent::EVT_TYPE_SOUND_VALUE;
	event.soundValue.value = value;
	_eventQueue.push(event);

	// fill up the buffer to now
	int16_t val = (_lastLVol*_lastValue) / 7;
	while (_cpuWritten < _tickCounter)
	{
		_audioCpuOut->WritePCM16(&val, 1);
		_cpuWritten += FCPU/_sampleFreq;
	}

	_lastValue = value;
}

void GbChannel::EnableAudio(bool enable)
{
	_audioEnabled = enable;
	if (enable)
	{
		_playingCounter = 0;
		_tickCounter = 0;
		CleanSoundOutput();
		alSourcePlay(_source);
	}
	else {
		alSourcePause(_source);
	}
}
