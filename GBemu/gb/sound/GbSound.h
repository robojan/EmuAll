#ifndef _GBSOUND_H
#define _GBSOUND_H

#include "../mem/GbMem.h"
#include "../Gameboy.h"
#include "GbChannel12.h"
#include "GbChannel3.h"
#include "GbChannel4.h"
#include <vector>

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

	bool LoadState(const SaveData_t *data);
	bool SaveState(std::vector<uint8_t> &data);
private:
	Gameboy			*m_gb;

	GbChannel12		_channel1;
	GbChannel12		_channel2;
	GbChannel3		_channel3;
	GbChannel4		_channel4;
};


inline void GbSound::Tick()
{
	_channel1.Tick();
	_channel2.Tick();
	_channel3.Tick();
	_channel4.Tick();

}

#endif