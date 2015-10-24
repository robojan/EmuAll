#ifndef _GBCHANNEL3_H
#define _GBCHANNEL3_H

#include "../defines.h"
#include "../mem/GbMem.h"
#include "GbChannel.h"

class GbChannel3: public GbMemEvent, public GbChannel
{
	friend class GbSound;
public:
	GbChannel3(Gameboy *master);
	~GbChannel3();

	void MemEvent(address_t address, gbByte val);
	inline void Tick();
private:
	void RegisterEvents();
	void UpdateStatus();
	// General variables
	Gameboy		*_gb;

	// Gameboy sound status
	bool _powered;
	bool _enabled;
	uint_fast8_t _outputLevel;
	uint8_t _waveBuffer[16];
	bool _playing;
	bool _lengthEnabled;


	// Sound generation variables
	int_fast16_t _volume;
	uint_fast8_t _volumeCtrl;
	uint_fast16_t _periodCounter;
	uint_fast16_t _frequency;
	uint_fast8_t _wavCounter;
	uint_fast8_t _lastSample;
	uint_fast16_t _soundLength;
	uint16_t _frameSequencer;
};

void GbChannel3::Tick()
{
	if (_periodCounter == 0)
	{
		_periodCounter = (2048-_frequency)*2;
		if (_enabled && _playing)
		{
			_wavCounter = (_wavCounter+1) & 0x1f;
			_lastSample = (_waveBuffer[_wavCounter >> 1] >> ((_wavCounter & 1) ? 0 : 4))&0xF; // Select the right sample
			if (_outputLevel == 0)
			{
				SetOutputValue(0);
			}
			else {
				SetOutputValue(_volume * 2 * (_lastSample>>(_outputLevel-1)) / 0xF  - _volume);
			}
		}

	}
	else {
		--_periodCounter;
	}
	if (_lengthEnabled && (_frameSequencer & 0x3FFF) == 0)
	{
		// Length clock (256 hz)
		if (_soundLength != 0)
		{
			--_soundLength;
		}
		else
		{
			_enabled = false;
			UpdateStatus();
		}
	}

	// Update counters
	++_frameSequencer;
	GbChannel::CpuTick();
}

#endif