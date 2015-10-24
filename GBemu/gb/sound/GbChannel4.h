#ifndef _GBCHANNEL4_H
#define _GBCHANNEL4_H

#include "../mem/GbMem.h"
#include "GbChannel.h"

class GbChannel4: public GbMemEvent, public GbChannel
{
	friend class GbSound;
public:
	GbChannel4(Gameboy *master);
	~GbChannel4();

	void MemEvent(address_t address, gbByte val);
	inline void Tick();
private:
	void RegisterEvents();
	void UpdateStatus();

	// Gameboy sound status
	bool _powered;
	bool _enabled;
	bool _lengthEnabled;

	// General variables
	Gameboy		*_gb;
	
	// Sound generation variables
	int_fast16_t _volume;
	uint_fast8_t _volumeCtrl;
	uint_fast8_t _volumeCounter;
	uint_fast16_t _periodCounter;
	uint_fast16_t _soundLength;
	uint8_t _divider;
	uint8_t _shift;
	uint16_t _frameSequencer;
	uint_fast16_t _lfsr;
	bool _width7;
};

void GbChannel4::Tick()
{
	if (_periodCounter == 0)
	{
		if (_divider == 0) {
			_periodCounter = 4;
		}
		else {
			_periodCounter = 8 * _divider;
		}
		_periodCounter *= (1 << (_shift + 1));
		if (_enabled)
		{
			uint_fast8_t loop = (_lfsr ^ (_lfsr >> 1)) & 1;
			if (_width7) {
				_lfsr = ((_lfsr >> 1) & 0x7FDF) | (loop << 6);
			}
			else {
				_lfsr = ((_lfsr >> 1) & 0x3FFF) | (loop << 14);
			}
			if (_lfsr & 0x1) {
				SetOutputValue(-_volume);
			}
			else {
				SetOutputValue(_volume);
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
	if (_frameSequencer == (7 << 13))
	{
		// Volume envelope (64 hz)
		gbByte ctrl = _gb->_mem->read(NR42);
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