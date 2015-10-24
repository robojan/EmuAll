#include "../../util/memDbg.h"

#include "GbSound.h"
#include "GbChannel3.h"
#include "../../util/log.h"

#include "../cpu/GbCpu.h"
#include "../../util/exceptions.h"

GbChannel3::GbChannel3(Gameboy *master) :
	GbChannel(44100, 3), _gb(master),
	_enabled(true), _volume(CHANNEL_AMPLITUDE),	_volumeCtrl(0xf),
	_powered(true), _periodCounter(0), _frequency(10), _outputLevel(0),
	_playing(false), _soundLength(0), _lengthEnabled(false)
{
	Log(Message, "Gameboy sound channel 3 initialized");
}

GbChannel3::~GbChannel3()
{
}

void GbChannel3::MemEvent(address_t address, gbByte val)
{
	gbByte temp;
	bool tempbool;
	uint_fast8_t l,r;
	

	// Wave mem access
	if (address >= WAVERAMSTART && address <= WAVERAMSTOP && ((_gb->_mem->read(NR30) & 0x80) == 0))
	{
		_waveBuffer[address - WAVERAMSTART] = val;
	}

	switch (address)
	{
	case NR30:
		_playing = (val & 0x80) != 0;
		_gb->_mem->write(address, val | 0x7F, false);
		break;
	case NR31:
		_soundLength = 255 - val;
		break;
	case NR32:
		_outputLevel = (val >> 5) & 3;
		_gb->_mem->write(address, val | 0x9f, false);
		break;
	case NR33:
		_frequency = (_frequency & 0x700) | val;
		_gb->_mem->write(address, 0xFF, false);
		break;
	case NR34:
		_frequency = (_frequency & 0xFF) | ((val & 0x7) << 8);
		_lengthEnabled = (val&(1 << 6)) != 0;
		if (val & 0x80)
		{
			// Trigger event
			if (_soundLength == 0) _soundLength = 255; // if length counter is 0 reset to 256
			_enabled = true; // Enable the channel
			_periodCounter = (2048 - _frequency) * 2; // Frequency timer is reloaded with period
			UpdateStatus();
		}
		_gb->_mem->write(address, val | 0xB7, false); // write only memory
		break;
	case NR50:
		temp = _gb->_mem->read(NR51);
		if ((temp & 0x4) != 0)
			l = val & 7;
		else 
			l = 0;
		if ((temp & 0x40) != 0)
			r = (val >> 4) & 7;
		else
			r = 0;
		GbChannel::SetAmplitude(l, r);
		break;
	case NR51:
		temp = _gb->_mem->read(NR50);
		if ((val & 0x4) != 0)
			l = temp & 7;
		else
			l = 0;

		if ((val & 0x40) != 0)
			r = (temp >> 4) & 7;
		else
			r = 0;

		GbChannel::SetAmplitude(l, r);
		//Log(Debug, "NR51 amplitude %d %d; PC=%04X, bank=%d, time=%"PRId64"(%f)", l, r, _gb->_cpu->GetRegisterPair(REG_PC), _gb->_mem->GetROMBank(), GbChannel::_tickCounter, GbChannel::_tickCounter / ((double) FCPU));
		break;
	case NR52:
		tempbool = _powered;
		_powered = (val & 0x80) != 0;
		if (!_powered)
		{
			_enabled = false;
		}
		else {
			if (_powered != tempbool)
			{
				// Set to initial values
			}
		}
		_gb->_mem->write(address, (val & 0x80) | 0x70 | _gb->_sound->GetEnabledFlags(), false);
		break;
	}
}


void GbChannel3::RegisterEvents()
{
	// Gameboy sound channel 3 initialize
	_gb->_mem->registerEvent(NR30, NR34, this);
	_gb->_mem->registerEvent(WAVERAMSTART, WAVERAMSTOP, this);
	_gb->_mem->registerEvent(NR50, this);
	_gb->_mem->registerEvent(NR51, this);
	_gb->_mem->registerEvent(NR52, this);
}

void GbChannel3::UpdateStatus()
{
	gbByte nr52 = _gb->_mem->read(NR52) & ~0x4;
	if (_enabled)
		nr52 |= 0x4;
	_gb->_mem->write(NR52, nr52, false);
}