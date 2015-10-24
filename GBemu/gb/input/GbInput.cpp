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