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
	_soundLength(0), _lengthEnabled(false), _lfsr(0xffff), _volumeCounter(0)
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
		if (_width7 != ((val & 0x8) != 0)) {
			_lfsr = 0xFFFF;
		}
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

// 0 - powered << 0 | enabled << 1 | width7 << 2 | lengthEnabled << 3
// 1 - volume
// 3 - volumeCtrl
// 4 - volumeCounter
// 5 - periodCounter
// 7 - soundLength
// 9 - divider
// 10 - shift
// 11 - frameSequencer
// 13 - lfsr
// 15

void GbChannel4::SaveState(std::vector<uint8_t> &data)
{
	GbChannel::SaveState(data);

	const EndianFuncs *conv = getEndianFuncs(0);
	int dataLen = 15;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	ptr[0] = (_powered ? 0x01 : 0x00) |
		(_enabled ? 0x02 : 0x00) |
		(_width7 ? 0x04 : 0x00) |
		(_lengthEnabled ? 0x08 : 0x00);
	*(uint16_t *)(ptr + 1) = conv->convu16(_volume);
	ptr[3] = _volumeCtrl;
	ptr[4] = _volumeCounter;
	*(uint16_t *)(ptr + 5) = conv->convu16(_periodCounter);
	*(uint16_t *)(ptr + 7) = conv->convu16(_soundLength);
	ptr[9] = _divider;
	ptr[10] = _shift;
	*(uint16_t *)(ptr + 11) = conv->convu16(_frameSequencer);
	*(uint16_t *)(ptr + 13) = conv->convu16(_lfsr);
}

uint8_t * GbChannel4::LoadState(uint8_t *data, int &len)
{
	data = GbChannel::LoadState(data, len);
	const EndianFuncs *conv = getEndianFuncs(0);


	if (len < 15) {
		Log(Error, "Save state corrupt");
		return data;
	}
	_powered = (data[0] & 0x1) != 0;
	_enabled = (data[0] & 0x2) != 0;
	_width7 = (data[0] & 0x4) != 0;
	_lengthEnabled = (data[0] & 0x8) != 0;
	_volume = conv->convu16(*(uint16_t *)(data + 1));
	_volumeCtrl = data[3];
	_volumeCounter = data[4];
	_periodCounter = conv->convu16(*(uint16_t *)(data + 5));
	_soundLength = conv->convu16(*(uint16_t *)(data + 7));
	_divider = data[9];
	_shift = data[10];
	_frameSequencer = conv->convu16(*(uint16_t *)(data + 11));
	_lfsr = conv->convu16(*(uint16_t *)(data + 13));
	len -= 15;
	return data + 15;
}