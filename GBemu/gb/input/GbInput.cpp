#include "../../util/memDbg.h"

#include "GbInput.h"

GbInput::GbInput(Gameboy *master)
{
	_gb = master;
	_u = false;
	_d = false;
	_l = false;
	_r = false;
	_a = false;
	_b = false;
	_st = false;
	_se = false;
}

GbInput::~GbInput()
{

}

void GbInput::update()
{
	if (_gb == NULL || _gb->_mem == NULL)
	{
		return;
	}
	gbByte joyp = _gb->_mem->read(JOYP);
	joyp &= 0xF0;
	joyp |= 0xCF;
	if((joyp & BUTTON_KEYS) == 0)
	{
		if(_st) joyp &= ~J_START;
		if(_se) joyp &= ~J_SELECT;
		if(_a) joyp &= ~J_A;
		if(_b) joyp &= ~J_B;
	}
	if((joyp & DIR_KEYS) == 0)
	{
		if(_d) joyp &= ~J_DOWN;
		if(_u) joyp &= ~J_UP;
		if(_l) joyp &= ~J_LEFT;
		if(_r) joyp &= ~J_RIGHT;
	}
	_gb->_mem->write(JOYP, joyp, false);
}

void GbInput::registerEvents()
{
	if (_gb != NULL && _gb->_mem != NULL)
	{
		_gb->_mem->registerEvent(JOYP, this);
	}
}

void GbInput::MemEvent(address_t address, gbByte val)
{
	if(address == JOYP)
	{
		update();
	}
}

void GbInput::Input(int key, bool pressed)
{
	bool interrupt = false;
	switch(key)
	{
	case INPUT_U:
		_u = pressed;
		interrupt = true;
		break;
	case INPUT_D:
		_d = pressed;
		interrupt = true;
		break;
	case INPUT_L:
		_l = pressed;
		interrupt = true;
		break;
	case INPUT_R:
		_r = pressed;
		interrupt = true;
		break;
	case INPUT_B:
		_b = pressed;
		interrupt = true;
		break;
	case INPUT_A:
		_a = pressed;
		interrupt = true;
		break;
	case INPUT_START:
		_st = pressed;
		interrupt = true;
		break;
	case INPUT_SELECT:
		_se = pressed;
		interrupt = true;
		break;
	}
	if(interrupt)
	{
		_gb->_mem->write(IF, _gb->_mem->read(IF) | INT_JOYPAD);
	}
	update();
}

static const uint32_t StateINPTid = 0x494e5054;

// 0 - id
// 4 - size
// 8 - input
// 9 - 

bool GbInput::LoadState(const SaveData_t *data)
{
	Endian conv(false);
	uint8_t *ptr = (uint8_t *)data->miscData;
	size_t miscLen = data->miscDataLen;
	// Find cpu segment
	while (miscLen >= 8) {
		uint32_t id = conv.convu32(*(uint32_t *)(ptr + 0));
		uint32_t len = conv.convu32(*(uint32_t *)(ptr + 4));
		if (id == StateINPTid && len >= 9) {
			_u = (ptr[8] & 0x01) != 0;
			_d = (ptr[8] & 0x02) != 0;
			_l = (ptr[8] & 0x04) != 0;
			_r = (ptr[8] & 0x08) != 0;
			_a = (ptr[8] & 0x10) != 0;
			_b = (ptr[8] & 0x20) != 0;
			_se = (ptr[8] & 0x40) != 0;
			_st = (ptr[8] & 0x80) != 0;
			return true;
		}
		ptr += len;
		miscLen -= len;
	}
	return false;
}

bool GbInput::SaveState(std::vector<uint8_t> &data)
{
	Endian conv(false);
	int dataLen = 9;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	*(uint32_t *)(ptr + 0) = conv.convu32(StateINPTid);
	*(uint32_t *)(ptr + 4) = conv.convu32(dataLen);
	ptr[8] =
		(_u ? 0x01 : 0x00) |
		(_d ? 0x02 : 0x00) |
		(_l ? 0x04 : 0x00) |
		(_r ? 0x08 : 0x00) |
		(_a ? 0x10 : 0x00) |
		(_b ? 0x20 : 0x00) |
		(_se ? 0x40 : 0x00) |
		(_st ? 0x80 : 0x00);
	return true;
}