#include "../../util/memDbg.h"

#include "GbChannel12.h"
#include "../../util/log.h"
#include "GbSound.h"
GbChannel12::GbChannel12(Gameboy *master, bool channel2) : 
	_gb(master), _channel2(channel2), GbChannel(44100, channel2 ? 2 : 1),
	_frameSequencer(0), _periodCounter(0), _frequency(0x77D),
	_dutyCounter(0), _dutyMode(2), _shadowFrequency(0), _sweepEnabled(false),
	_enabled(true), _soundLength(0), _sweepTimer(0), _volume(CHANNEL_AMPLITUDE),
	_lengthEnabled(false), _volumeCounter(0), _volumeCtrl(0xf), _powered(true)
{
	Log(Message, "Gameboy sound channel %d initialized", channel2 ? 2:1);
}

GbChannel12::~GbChannel12()
{
}

void GbChannel12::MemEvent(address_t address, gbByte val)
{
	gbByte temp;
	bool tempbool;
	uint_fast8_t l,r;

	switch (address)
	{
	case NR10:
		_sweepEnabled = (val & 0x70) != 0 && !_channel2;
		_gb->_mem->write(NR10, val | 0x80, false); // write only memory
		break;
	case NR11:
	case NR21:
		_dutyMode = (val >> 6)&3;
		_soundLength = 63-(val & 63); 
		_gb->_mem->write(address, val | 0x3f, false); // write only memory
		break;
	case NR13:
	case NR23:
		_frequency = (_frequency & 0x700) | val;
		_gb->_mem->write(address, 0xFF, false);
		break;
	case NR14:
	case NR24:
		_frequency = (_frequency & 0xFF) | ((val & 0x7) << 8);
		_lengthEnabled = (val&(1<<6)) != 0;
		if (val & 0x80)
		{
			// Trigger event
			if (_soundLength == 0) _soundLength = 63; // if length counter is 0 reset to 64
			_enabled = true; // Enable the channel
			_periodCounter = (2048 - _frequency) * 4; // Frequency timer is reloaded with period
			_shadowFrequency = _frequency; // copy the frequency to the shadow register
			gbByte nr10 = _gb->_mem->read(NR10); // Read NR10
			_sweepEnabled = !_channel2 && ((nr10 & 0xf7) != 0); // sweep is enabled if the sweep period or shift are non-zero and it it channel 1
			_sweepTimer = (nr10 & 0x70) >> 4;
			if (nr10&0x7 && !_channel2) _frequency = _frequency + (((nr10 & 0x8) != 0) ? -1 : 1) * (_frequency >> (nr10&0x7));
			if (_frequency > 2047) _enabled = false;
			_volumeCtrl = (_gb->_mem->read(_channel2 ? NR22 : NR12) >> 4) & 0xF;
			_volume = (CHANNEL_AMPLITUDE * _volumeCtrl) / 0xF;
			UpdateStatus();
			_frameSequencer = 1;
		}
		_gb->_mem->write(address, val | 0xB7, false); // write only memory
		break;
	case NR50:
		temp = _gb->_mem->read(NR51);
		if ((temp & (_channel2 ? 0x2 : 0x1)) != 0)
			l = val & 7;
		else
			l = 0;
		if ((temp & (_channel2 ? 0x20 : 0x10)) != 0)
			r = (val >> 4) & 7;
		else
			r = 0;

		GbChannel::SetAmplitude(l,r);
		break;
	case NR51:
		temp = _gb->_mem->read(NR50);
		if ((val & (_channel2 ? 0x2 : 0x1)) != 0)
			l = temp & 7;
		else 
			l = 0;

		if ((val & (_channel2 ? 0x20 : 0x10)) != 0)
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
			if (_channel2)
			{
				_gb->_mem->write(NR21, 0x00);
				_gb->_mem->write(NR22, 0x00);
				_gb->_mem->write(NR23, 0x00);
				_gb->_mem->write(NR24, 0x00);
			}
			else {
				_gb->_mem->write(NR10, 0x00);
				_gb->_mem->write(NR11, 0x00);
				_gb->_mem->write(NR12, 0x00);
				_gb->_mem->write(NR13, 0x00);
				_gb->_mem->write(NR14, 0x00);
			}
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

// 0 - periodCounter
// 2 - frameSequencer
// 4 - dutyCouter
// 5 - shadowFrequency
// 7 - sweepCounter
// 8 - sweepTimer
// 9 - volume
// 11 - volumeCtrl
// 12 - volumeCounter
// 13 - frequency
// 15 - dutyMode
// 16 - soundLength
// 17 - powered << 0 | enabled << 1 | sweepEnabled << 2 | lengthEnabled << 3
// 18

void GbChannel12::SaveState(std::vector<uint8_t> &data)
{
	GbChannel::SaveState(data);

	Endian conv(false);
	int dataLen = 18;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	*(uint16_t *)(ptr + 0) = conv.convu16(_periodCounter);
	*(uint16_t *)(ptr + 2) = conv.convu16(_frameSequencer);
	ptr[4] = _dutyCounter;
	*(uint16_t *)(ptr + 5) = conv.convu16(_shadowFrequency);
	ptr[7] = _sweepCounter;
	ptr[8] = _sweepTimer;
	*(uint16_t *)(ptr + 9) = conv.convu16(_volume);
	ptr[11] = _volumeCtrl;
	ptr[12] = _volumeCounter;
	*(uint16_t *)(ptr + 13) = conv.convu16(_frequency);
	ptr[15] = _dutyMode;
	ptr[16] = _soundLength;
	ptr[17] = (_powered ? 0x01 : 0x00) |
		(_enabled ? 0x02 : 0x00) |
		(_sweepEnabled ? 0x04 : 0x00) | 
		(_lengthEnabled  ? 0x08 : 0x00);

}

uint8_t * GbChannel12::LoadState(uint8_t *data, int &len)
{
	data = GbChannel::LoadState(data, len);
	Endian conv(false);


	if (len < 18) {
		Log(Error, "Save state corrupt");
		return data;
	}

	_periodCounter = conv.convu16(*(uint16_t *)(data + 0));
	_frameSequencer = conv.convu16(*(uint16_t *)(data + 2));
	_dutyCounter = data[4];
	_shadowFrequency = conv.convu16(*(uint16_t *)(data + 5));
	_sweepCounter = data[7];
	_sweepTimer = data[8];
	_volume = conv.convu16(*(uint16_t *)(data + 9));
	_volumeCtrl = data[11];
	_volumeCounter = data[12];
	_frequency = conv.convu16(*(uint16_t *)(data + 13));
	_dutyMode = data[15];
	_soundLength = data[16];
	_powered = (data[17] & 0x1) != 0;
	_enabled = (data[17] & 0x2) != 0;
	_sweepEnabled = (data[17] & 0x4) != 0;
	_lengthEnabled = (data[17] & 0x8) != 0;
	len -= 18;
	return data + 18;
}

void GbChannel12::RegisterEvents()
{
	// Gameboy sound channel 1/2 initialize
	if (_channel2)
	{
		_gb->_mem->registerEvent(NR21, NR24, this);
		/*_gb->_mem->registerEvent(NR22, this);
		_gb->_mem->registerEvent(NR23, this);
		_gb->_mem->registerEvent(NR24, this);*/
	}
	else {
		_gb->_mem->registerEvent(NR10, NR14, this);
		/*_gb->_mem->registerEvent(NR11, this);
		_gb->_mem->registerEvent(NR12, this);
		_gb->_mem->registerEvent(NR13, this);
		_gb->_mem->registerEvent(NR14, this);*/
	}
	_gb->_mem->registerEvent(NR50, this);
	_gb->_mem->registerEvent(NR51, this);
	_gb->_mem->registerEvent(NR52, this);
}

void GbChannel12::UpdateStatus()
{
	gbByte nr52 = _gb->_mem->read(NR52);
	nr52 &= _channel2 ? ~0x2 : ~0x1;
	if (_enabled)
		nr52 |= _channel2 ? 0x2 : 0x1;
	_gb->_mem->write(NR52, nr52, false);
}