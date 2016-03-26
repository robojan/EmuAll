#include "../../util/memDbg.h"

#include "GbInput.h"

GbInput::GbInput(Gameboy *master)
{
	m_gb = master;
	m_u = false;
	m_d = false;
	m_l = false;
	m_r = false;
	m_a = false;
	m_b = false;
	m_st = false;
	m_se = false;
}

GbInput::~GbInput()
{

}

void GbInput::update()
{
	if (m_gb == NULL || m_gb->_mem == NULL)
	{
		return;
	}
	gbByte joyp = m_gb->_mem->read(JOYP);
	joyp &= 0xF0;
	joyp |= 0xCF;
	if((joyp & BUTTON_KEYS) == 0)
	{
		if(m_st) joyp &= ~J_START;
		if(m_se) joyp &= ~J_SELECT;
		if(m_a) joyp &= ~J_A;
		if(m_b) joyp &= ~J_B;
	}
	if((joyp & DIR_KEYS) == 0)
	{
		if(m_d) joyp &= ~J_DOWN;
		if(m_u) joyp &= ~J_UP;
		if(m_l) joyp &= ~J_LEFT;
		if(m_r) joyp &= ~J_RIGHT;
	}
	m_gb->_mem->write(JOYP, joyp, false);
}

void GbInput::registerEvents()
{
	if (m_gb != NULL && m_gb->_mem != NULL)
	{
		m_gb->_mem->registerEvent(JOYP, this);
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
		m_u = pressed;
		interrupt = true;
		break;
	case INPUT_D:
		m_d = pressed;
		interrupt = true;
		break;
	case INPUT_L:
		m_l = pressed;
		interrupt = true;
		break;
	case INPUT_R:
		m_r = pressed;
		interrupt = true;
		break;
	case INPUT_B:
		m_b = pressed;
		interrupt = true;
		break;
	case INPUT_A:
		m_a = pressed;
		interrupt = true;
		break;
	case INPUT_START:
		m_st = pressed;
		interrupt = true;
		break;
	case INPUT_SELECT:
		m_se = pressed;
		interrupt = true;
		break;
	}
	if(interrupt)
	{
		m_gb->_mem->write(IF, m_gb->_mem->read(IF) | INT_JOYPAD);
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
	const EndianFuncs *conv = getEndianFuncs(0);
	uint8_t *ptr = (uint8_t *)data->miscData;
	int miscLen = data->miscDataLen;
	// Find cpu segment
	while (miscLen >= 8) {
		uint32_t id = conv->convu32(*(uint32_t *)(ptr + 0));
		uint32_t len = conv->convu32(*(uint32_t *)(ptr + 4));
		if (id == StateINPTid && len >= 9) {
			m_u = (ptr[8] & 0x01) != 0;
			m_d = (ptr[8] & 0x02) != 0;
			m_l = (ptr[8] & 0x04) != 0;
			m_r = (ptr[8] & 0x08) != 0;
			m_a = (ptr[8] & 0x10) != 0;
			m_b = (ptr[8] & 0x20) != 0;
			m_se = (ptr[8] & 0x40) != 0;
			m_st = (ptr[8] & 0x80) != 0;
			return true;
		}
		ptr += len;
		miscLen -= len;
	}
	return false;
}

bool GbInput::SaveState(std::vector<uint8_t> &data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	int dataLen = 9;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	*(uint32_t *)(ptr + 0) = conv->convu32(StateINPTid);
	*(uint32_t *)(ptr + 4) = conv->convu32(dataLen);
	ptr[8] =
		(m_u ? 0x01 : 0x00) |
		(m_d ? 0x02 : 0x00) |
		(m_l ? 0x04 : 0x00) |
		(m_r ? 0x08 : 0x00) |
		(m_a ? 0x10 : 0x00) |
		(m_b ? 0x20 : 0x00) |
		(m_se ? 0x40 : 0x00) |
		(m_st ? 0x80 : 0x00);
	return true;
}