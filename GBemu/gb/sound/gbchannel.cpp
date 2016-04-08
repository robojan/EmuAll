#include "GbChannel.h"

#include "../../util/log.h"
#include "../defines.h"


GbChannel::GbChannel(int channel) : 
	_tickCounter(0), _currentValue(0),
	_playingCounter(0), _leftVolume(7), _rightVolume(7),
	_sampleFreq(1), _channel(channel), _audioEnabled(false), 
	_cpuWritten(0), _lastValue(0), _lastLVol(7), _lastRVol(7)
{

}

GbChannel::~GbChannel()
{
}

void GbChannel::InitAudio(unsigned int sampleRate, int channels)
{
	_sampleFreq = sampleRate;
	_numChannels = channels;
}

void GbChannel::GetAudio(short * buffer, int samples)
{
	if (_tickCounter < (FCPU / _sampleFreq)*(samples / 2) + _playingCounter)
	{
		// When there is not yet enough data to fill a buffer, skip it. 
		memset(buffer, 0, samples*sizeof(int16_t)); // clear the buffer
		Log(Warn, "Channel %d underrun", _channel);
		return;
	}

	/*if (_playingCounter < _tickCounter - (FCPU / _sampleFreq)*(samples / 2) * 3)
	{
		_playingCounter = _tickCounter - (FCPU / _sampleFreq)*(samples / 2) * 3;
	}*/

	ChannelEvent *event;
	if (_eventQueue.empty()) {
		event = NULL;
	}
	else {
		event = &_eventQueue.front();
	}
	for (int i = 0; i < samples; i += 1)
	{
		_playingCounter += FCPU / _sampleFreq;

		while (event != NULL && event->when <= _playingCounter) {
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
			_eventQueue.pop_front();
			if (_eventQueue.empty()) {
				event = NULL;
			}
			else {
				event = &_eventQueue.front();
			}
		}
		// Write the value to the buffer
		buffer[0] = (_leftVolume*_currentValue) / 7;
		if (_numChannels >= 2) {
			buffer[1] = (_rightVolume*_currentValue) / 7;
		}
		buffer += _numChannels;
	}
}

void GbChannel::CleanSoundOutput()
{
	if (_eventQueue.size() > 100000) {
		Log(Warn, "Sound event queue overflow");
	}

	while (_eventQueue.size() > 100000)
		_eventQueue.pop_front();
}

// 0 - tickCounter
// 8 - cpuWritten
// 16 - lastValue
// 18 - lastLVol
// 19 - lastRVol
// 20 - playingCounter
// 28 - currentValue
// 30 - leftVolume
// 31 - rightVolume
// 32 - audioEnabled
// 33

void GbChannel::SaveState(std::vector<uint8_t> &data)
{
	Endian conv(false);
	int dataLen = 33;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	*(uint64_t *)(ptr + 0) = conv.convu64(_tickCounter);
	*(uint64_t *)(ptr + 8) = conv.convu64(_cpuWritten);
	*(uint16_t *)(ptr + 16) = conv.convu16(_lastValue);
	ptr[18] = _lastLVol;
	ptr[19] = _lastRVol;
	*(uint64_t *)(ptr + 20) = conv.convu64(_playingCounter);
	*(uint16_t *)(ptr + 28) = conv.convu16(_currentValue);
	ptr[30] = _leftVolume;
	ptr[31] = _rightVolume;
	ptr[32] = _audioEnabled ? 1 : 0;
}

uint8_t *GbChannel::LoadState(uint8_t *data, int &len)
{
	Endian conv(false);
	// Clear eventqueue
	while (!_eventQueue.empty())
		_eventQueue.clear();

	if (len < 33) {
		Log(Error, "Save state corrupt");
		return data;
	}

	_tickCounter = conv.convu64(*(uint64_t *)(data + 0));
	_cpuWritten = conv.convu64(*(uint64_t *)(data + 8));
	_lastValue = conv.convu16(*(uint16_t *)(data + 16));
	_lastLVol = data[18];
	_lastRVol = data[19];
	_playingCounter = conv.convu64(*(uint64_t *)(data + 20));
	_currentValue = conv.convu16(*(uint16_t *)(data + 28));
	_leftVolume = data[30];
	_rightVolume = data[31];
	//_audioEnabled = data[32] != 0;

	len -= 33;

	return data + 33;
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
	_eventQueue.push_back(event);

	// fill up the buffer to now
	int16_t val = (_lastLVol * _lastValue) / 7;
	while (_cpuWritten < _tickCounter)
	{
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
	_eventQueue.push_back(event);

	// fill up the buffer to now
	int16_t val = (_lastLVol*_lastValue) / 7;
	while (_cpuWritten < _tickCounter)
	{
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
		_eventQueue.clear();
	}
}
