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

// 0 - powered << 0 | enabled << 1 | playing << 2 | lengthEnabled << 3
// 1 - outputLevel
// 2 - waveBuffer
// 18 - volume
// 20 - volumeCtrl
// 21 - periodCounter
// 23 - frequency
// 25 - wavCounter
// 26 - lastSample
// 27 - soundLength
// 29 - frameSequencer
// 31

void GbChannel3::SaveState(std::vector<uint8_t> &data)
{
	GbChannel::SaveState(data);

	Endian conv(false);
	int dataLen = 31;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	ptr[0] = (_powered ? 0x01 : 0x00) |
		(_enabled ? 0x02 : 0x00) |
		(_playing ? 0x04 : 0x00) |
		(_lengthEnabled ? 0x08 : 0x00);
	ptr[1] = _outputLevel;
	for (int i = 0; i < 16; i++) {
		ptr[2 + i] = _waveBuffer[i];
	}
	*(uint16_t *)(ptr + 18) = conv.convu16(_volume);
	ptr[20] = _volumeCtrl;
	*(uint16_t *)(ptr + 21) = conv.convu16(_periodCounter);
	*(uint16_t *)(ptr + 23) = conv.convu16(_frequency);
	ptr[25] = _wavCounter;
	ptr[26] = _lastSample;
	*(uint16_t *)(ptr + 27) = conv.convu16(_soundLength);
	*(uint16_t *)(ptr + 29) = conv.convu16(_frameSequencer);
}

uint8_t * GbChannel3::LoadState(uint8_t *data, int &len)
{
	data = GbChannel::LoadState(data, len);
	Endian conv(false);


	if (len < 31) {
		Log(Error, "Save state corrupt");
		return data;
	}
	_powered = (data[0] & 0x1) != 0;
	_enabled = (data[0] & 0x2) != 0;
	_playing = (data[0] & 0x4) != 0;
	_lengthEnabled = (data[0] & 0x8) != 0;
	_outputLevel = data[1];
	for (int i = 0; i < 16; i++) {
		_waveBuffer[i] = data[2 + i];
	}
	_volume = conv.convu16(*(uint16_t *)(data + 18));
	_volumeCtrl = data[20];
	_periodCounter = conv.convu16(*(uint16_t *)(data + 21));
	_frequency = conv.convu16(*(uint16_t *)(data + 23));
	_wavCounter = data[25];
	_lastSample = data[26];
	_soundLength = conv.convu16(*(uint16_t *)(data + 27));
	_frameSequencer = conv.convu16(*(uint16_t *)(data + 29));
	len -= 31;
	return data + 31;
}