#ifndef GBCHANNEL_H
#define GBCHANNEL_H

#include <stdint.h>
#include <queue>

#include <emuall/util/wavaudiofile.h>
#include "../../types/EventQueue.h"

// The amplitude of the signal
#define CHANNEL_AMPLITUDE 6000

class GbChannel
{
public:
	GbChannel(int channel);
	virtual ~GbChannel();

	void EnableAudio(bool enable);
	void SetAmplitude(uint_fast8_t left, uint_fast8_t right);

	virtual void InitAudio(unsigned int sampleRate, int channels);
	virtual void GetAudio(short * buffer, int samples);

protected:
	inline void CpuTick() { _tickCounter++; }
	void SetOutputValue(int16_t value);
	void CleanSoundOutput();

	virtual void SaveState(std::vector<uint8_t> &data);
	virtual uint8_t *LoadState(uint8_t *data, int &len);


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

	// Misc
	int _channel;
	uint64_t _cpuWritten;
	int16_t _lastValue;
	int8_t _lastLVol;
	int8_t _lastRVol;

	// openAL members
	uint64_t _playingCounter;
	int16_t _currentValue;
	uint_fast8_t _leftVolume;
	uint_fast8_t _rightVolume;
	unsigned int _sampleFreq;
	int _numChannels;
	EventQueue<struct ChannelEvent> _eventQueue;
	bool _audioEnabled;
};

#endif