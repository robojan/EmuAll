#ifndef _GBCHANNEL12_H
#define _GBCHANNEL12_H

#include "../mem/GbMem.h"
#include "../Gameboy.h"
#include "GbChannel.h"

#include <queue>

class GbChannel12: public GbMemEvent, public GbChannel
{
	friend class GbSound;
public:
	GbChannel12(Gameboy *master, bool channel2);
	~GbChannel12();

	void MemEvent(address_t address, gbByte val);
	inline void Tick();
private:
	void RegisterEvents();
	void UpdateStatus();

	// General variables
	Gameboy		*_gb;
	bool		_channel2;

	// Sound generation variables
	uint_fast16_t _periodCounter;
	uint16_t _frameSequencer;
	uint_fast8_t _dutyCounter;
	uint_fast16_t _shadowFrequency;
	uint_fast8_t _sweepCounter;
	uint_fast8_t _sweepTimer;
	int_fast16_t _volume;
	uint_fast8_t _volumeCtrl;
	uint_fast8_t _volumeCounter;
	
	// Gameboy sound status
	uint_fast16_t _frequency;
	uint_fast8_t _dutyMode;
	uint_fast8_t _soundLength;
	bool _powered;
	bool _enabled;
	bool _sweepEnabled;
	bool _lengthEnabled;
};

void GbChannel12::Tick()
{
	if (_periodCounter != 0)
	{
		--_periodCounter;
	} else
	{
		_periodCounter = (2048 - _frequency) * 4;
		if (_dutyCounter != 7)
		{
			_dutyCounter++;
		}
		else {
			_dutyCounter = 0;
		}
		if (_enabled)
		{
			switch (_dutyMode)
			{
			case 0: // 12.5%
				if (_dutyCounter == 0)
					SetOutputValue(_volume);
				else if (_dutyCounter == 1)
					SetOutputValue(-_volume);
				break;
			case 1: // 25%
				if (_dutyCounter == 7)
					SetOutputValue(_volume);
				else if (_dutyCounter == 1)
					SetOutputValue(-_volume);
				break;
			case 2: // 50%
				if (_dutyCounter == 7)
					SetOutputValue(_volume);
				else if (_dutyCounter == 3)
					SetOutputValue(-_volume);
				break;
			case 3: // 75%
				if (_dutyCounter == 1)
					SetOutputValue(_volume);
				else if (_dutyCounter == 7)
					SetOutputValue(-_volume);
				break;
			}
		}			
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
	if (_sweepEnabled && (_frameSequencer & 0x7FFF) == (2 << 13))
	{
		// Sweep clock (128 hz)
		if (_sweepTimer != 0)
		{
			--_sweepTimer;
		}
		else
		{
			gbByte nr10 = _gb->_mem->read(NR10); // Read NR10
			
			_sweepTimer = (nr10 >> 4) & 0x7;
			if ((nr10 & 0x7) && !_channel2) 
			{
				uint16_t newFreq = _frequency + (((nr10 & 0x8) != 0) ? -1 : 1) * (_frequency >> (nr10 & 0x7));
				if (newFreq > 2047)
				{
					_enabled = false;
					UpdateStatus();
				}
				else
				{
					_frequency = newFreq;
				}
			}
		}
	}
	if (_frameSequencer == (7 << 13))
	{
		// Volume envelope (64 hz)
		gbByte ctrl = _gb->_mem->read(_channel2 ? NR22 : NR12);
		if ((ctrl & 7) != 0)
		{
			if (_volumeCounter != 0)
			{
				--_volumeCounter;
			}
			else
			{
				_volumeCounter = ctrl & 7;
				uint_fast8_t newVolume = _volumeCtrl + ((ctrl & 0x8) != 0 ? 1 : -1);
				if (newVolume < 0x10)
				{
					_volumeCtrl = newVolume;
					_volume = (CHANNEL_AMPLITUDE * _volumeCtrl) / 0xF;
				}
			}
		}
	}

	// Update counters
	++_frameSequencer;
	GbChannel::CpuTick();
}


#endif