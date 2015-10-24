#ifndef _GBSOUND_H
#define _GBSOUND_H

#include "../mem/GbMem.h"
#include "../Gameboy.h"
#include "GbChannel12.h"
#include "GbChannel3.h"
#include "GbChannel4.h"

class GbSound : public GbMemEvent
{
	friend class Gameboy;
	friend class GbChannel12;
	friend class GbChannel3;
	friend class GbChannel4;
public:
	GbSound(Gameboy *master);
	~GbSound();
	void registerEvents(void);
	void MemEvent(address_t address, gbByte val);
	inline void Tick();
	void SlowTick();
	void BeginTick();
	void EnableAudio(bool enable);
	uint_fast8_t GetEnabledFlags();
private:
	Gameboy			*m_gb;

	GbChannel12		m_channel1;
	GbChannel12		m_channel2;
	GbChannel3		m_channel3;
	GbChannel4		m_channel4;
};


inline void GbSound::Tick()
{
	m_channel1.Tick();
	m_channel2.Tick();
	m_channel3.Tick();
	m_channel4.Tick();

}

#endif