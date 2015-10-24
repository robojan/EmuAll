#ifndef _GBINPUT_H
#define _GBINPUT_H

#include "../mem/GbMem.h"
#include "../Gameboy.h"

class GbInput : public GbMemEvent 
{
public:
	GbInput(Gameboy *info);
	~GbInput();
	void update();

	void registerEvents();
	void Input(int key, bool pressed);
	void MemEvent(address_t address, gbByte val);
private:
	Gameboy			*m_gb;
	bool			m_u;
	bool			m_d;
	bool			m_l;
	bool			m_r;
	bool			m_a;
	bool			m_b;
	bool			m_se;
	bool			m_st;
};

#endif