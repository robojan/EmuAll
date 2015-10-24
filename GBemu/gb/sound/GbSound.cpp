#include "../../util/memDbg.h"

#include "GbSound.h"
#include "../defines.h"

#include <AL/al.h>

GbSound::GbSound(Gameboy *master) : 
	m_channel1(master, false), 
	m_channel2(master, true),
	m_channel3(master),
	m_channel4(master)
{
	m_gb = master;

	// Init Audio state
	alDistanceModel(AL_NONE);
	alDopplerFactor(0.0);

	// Init Audio listener
	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	ALfloat orientation[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };
	alListenerfv(AL_ORIENTATION, orientation);
}

GbSound::~GbSound()
{

}

void GbSound::registerEvents(void)
{
	m_channel1.RegisterEvents();
	m_channel2.RegisterEvents();
	m_channel3.RegisterEvents();
	m_channel4.RegisterEvents();
}

void GbSound::MemEvent(address_t address, gbByte val)
{

}

void GbSound::SlowTick()
{
	m_channel1.SlowTick();
	m_channel2.SlowTick();
	m_channel3.SlowTick();
	m_channel4.SlowTick();
}

void GbSound::BeginTick()
{
	m_channel1.CleanSoundOutput();
	m_channel2.CleanSoundOutput();
	m_channel3.CleanSoundOutput();
	m_channel4.CleanSoundOutput();
}

void GbSound::EnableAudio(bool enable)
{
	m_channel1.EnableAudio(enable);
	m_channel2.EnableAudio(enable);
	m_channel3.EnableAudio(enable);
	m_channel4.EnableAudio(enable);
}

uint_fast8_t GbSound::GetEnabledFlags()
{
	uint_fast8_t ret = 0;
	ret = m_channel1._enabled ? 0x1 : 0x0;
	ret |= m_channel2._enabled ? 0x2 : 0x0;
	ret |= m_channel3._enabled ? 0x4 : 0x0;
	ret |= m_channel4._enabled ? 0x8 : 0x0;
	return ret;
}