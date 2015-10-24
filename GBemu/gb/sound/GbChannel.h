#ifndef GBCHANNEL_H
#define GBCHANNEL_H

#include <stdint.h>
#include <queue>

#include "../../util/wavaudiofile.h"

// The number of buffers
#define NUM_BUFFERS 2
// How long each buffer wil contain audio data
#define BUFFER_TIME 50
// The amplitude of the signal
#define CHANNEL_AMPLITUDE 6000

class GbChannel
{
public:
	GbChannel(unsigned int sampleFreq, int channel);
	~GbChannel();

	void EnableAudio(bool enable);
	void SlowTick();
	void SetAmplitude(uint_fast8_t left, uint_fast8_t right);

protected:
	inline void CpuTick() { _tickCounter++; }
	void SetOutputValue(int16_t value);
	void CleanSoundOutput();

	uint64_t _tickCounter; 
private:

	// subtypes
	struct ChannelEvent {
		uint64_t when;
		enum {
			EVT_TYPE_INVALID,
			EVT_TYPE_SOUND_VALUE,
			EVT_TYPE_VOLUME
		} type;
		union {
			struct {
				int16_t value;
			} soundValue;
			struct {
				uint8_t lVol;
				uint8_t rVol;
			} soundVolume;
		};
	};

	void LoadBuffer(uint32_t buffer);

	// Misc
	int _channel;
	robojan::WavFileOutStream *_audioOut;
	robojan::WavFileOutStream *_audioCpuOut;
	uint64_t _cpuWritten;
	int16_t _lastValue;
	int8_t _lastLVol;
	int8_t _lastRVol;

	// openAL members
	uint32_t _source;
	uint32_t _buffers[NUM_BUFFERS];
	uint64_t _playingCounter;
	int16_t _currentValue;
	int16_t *_workingBuffer;
	size_t _workingBufferSamples;
	uint_fast8_t _leftVolume;
	uint_fast8_t _rightVolume;
	const unsigned int _sampleFreq;
	std::queue<struct ChannelEvent, std::deque<struct ChannelEvent>> _eventQueue;
	bool _audioEnabled;
};

#endif