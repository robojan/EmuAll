#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG   
#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
#endif
#endif  // _DEBUG

#include "GbChannel4.h"
#include "GbSound.h"
#include "../../util/log.h"

GbChannel4::GbChannel4(Gameboy *master) :
	GbChannel(44100, 4), _gb(master),
	_enabled(true), _volume(CHANNEL_AMPLITUDE), _volumeCtrl(0xf),
	_powered(true), _periodCounter(0),
	_soundLength(0), _lengthEnabled(false), _lfsr(0), _volumeCounter(0)
{
	Log(Message, "Gameboy sound channel 4 initialized");
}

GbChannel4::~GbChannel4()
{

}

void GbChannel4::MemEvent(address_t address, gbByte val)
{
	gbByte temp;
	uint_fast8_t l, r;
	bool tempbool;

	switch (address) {
	case NR41:
		_soundLength = 63 - (val & 63);
		_gb->_mem->write(address, 0xFF, false);
		break;
	case NR43:
		_divider = val & 0x7;
		_shift = (val >> 4) & 0xF;
		_width7 = (val & 0x8) != 0;
		_gb->_mem->write(address, 0xFF, false);
		break;
	case NR44:
		_lengthEnabled = (val & (1 << 6)) != 0;
		if (val & 0x80) {
			// Trigger event
			if (_soundLength == 0) _soundLength = 63; // if length counter is 0 reset to 64
			_enabled = true; // Enable the channel
			if (_divider == 0) {
				_periodCounter = 4;
			}
			else {
				_periodCounter = 8 * _divider;
			}
			_periodCounter *= (1 << (_shift + 1));
			_volumeCtrl = (_gb->_mem->read(NR42) >> 4) & 0xF;
			_volume = (CHANNEL_AMPLITUDE * _volumeCtrl) / 0xF;
			UpdateStatus();
			_frameSequencer = 1;
		}
		break;
	case NR50:
		temp = _gb->_mem->read(NR51);
		if ((temp & 0x8) != 0)
			l = val & 7;
		else
			l = 0;
		if ((temp & 0x80) != 0)
			r = (val >> 4) & 7;
		else
			r = 0;
		GbChannel::SetAmplitude(l, r);
		break;
	case NR51:
		temp = _gb->_mem->read(NR50);
		if ((val & 0x8) != 0)
			l = temp & 7;
		else
			l = 0;

		if ((val & 0x80) != 0)
			r = (temp >> 4) & 7;
		else
			r = 0;

		GbChannel::SetAmplitude(l, r);
		break;
	case NR52:
		tempbool = _powered;
		_powered = (val & 0x80) != 0;
		if (!_powered)
		{
			_enabled = false;
			_gb->_mem->write(NR41, 0x00);
			_gb->_mem->write(NR42, 0x00);
			_gb->_mem->write(NR43, 0x00);
			_gb->_mem->write(NR44, 0x00);
		}
		else {
			if (_powered != tempbool)
			{
				// Set to initial values
				_frameSequencer = 0;
			}
		}
		_gb->_mem->write(address, (val & 0x80) | 0x70 | _gb->_sound->GetEnabledFlags(), false);
		break;
	}
}

void GbChannel4::RegisterEvents()
{
	// Gameboy sound channel 4 initialize
	_gb->_mem->registerEvent(NR41, NR44, this);
	_gb->_mem->registerEvent(NR50, this);
	_gb->_mem->registerEvent(NR51, this);
	_gb->_mem->registerEvent(NR52, this);
}

void GbChannel4::UpdateStatus()
{
	gbByte nr52 = _gb->_mem->read(NR52) & ~0x4;
	if (_enabled)
		nr52 |= 0x4;
	_gb->_mem->write(NR52, nr52, false);
}