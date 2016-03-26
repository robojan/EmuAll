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

static const uint32_t StateSNDid = 0x534E4420;

// 0 - id
// 4 - size
// 8 - tickCounter
// 16 - 

bool GbSound::LoadState(const SaveData_t *data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	uint8_t *ptr = (uint8_t *)data->miscData;
	int miscLen = data->miscDataLen;
	// Find cpu segment
	while (miscLen >= 8) {
		uint32_t id = conv->convu32(*(uint32_t *)(ptr + 0));
		uint32_t len = conv->convu32(*(uint32_t *)(ptr + 4));
		if (id == StateSNDid && len >= 8) {
			ptr += 8;
			len -= 8;
			int dataLen = len;
			ptr = m_channel1.LoadState(ptr, dataLen);
			ptr = m_channel2.LoadState(ptr, dataLen);
			ptr = m_channel3.LoadState(ptr, dataLen);
			ptr = m_channel4.LoadState(ptr, dataLen);
			return true;
		}
		ptr += len;
		miscLen -= len;
	}
	return false;
}

bool GbSound::SaveState(std::vector<uint8_t> &data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	int dataLen = 8;
	int size = data.size();
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	*(uint32_t *)(ptr + 0) = conv->convu32(StateSNDid);
	m_channel1.SaveState(data);
	m_channel2.SaveState(data);
	m_channel3.SaveState(data);
	m_channel4.SaveState(data);

	dataLen = data.size() - size;
	*(uint32_t *)(ptr + 4) = conv->convu32(dataLen);
	return true;
}