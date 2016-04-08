#include "../../util/memDbg.h"

#include "GbSound.h"
#include "../defines.h"

GbSound::GbSound(Gameboy *master) : 
	_channel1(master, false), 
	_channel2(master, true),
	_channel3(master),
	_channel4(master)
{
	_gb = master;
}

GbSound::~GbSound()
{

}

void GbSound::registerEvents(void)
{
	_channel1.RegisterEvents();
	_channel2.RegisterEvents();
	_channel3.RegisterEvents();
	_channel4.RegisterEvents();
}

void GbSound::MemEvent(address_t address, gbByte val)
{

}

void GbSound::BeginTick()
{
	_channel1.CleanSoundOutput();
	_channel2.CleanSoundOutput();
	_channel3.CleanSoundOutput();
	_channel4.CleanSoundOutput();
}

void GbSound::EnableAudio(bool enable)
{
	_channel1.EnableAudio(enable);
	_channel2.EnableAudio(enable);
	_channel3.EnableAudio(enable);
	_channel4.EnableAudio(enable);
}

uint_fast8_t GbSound::GetEnabledFlags()
{
	uint_fast8_t ret = 0;
	ret = _channel1._enabled ? 0x1 : 0x0;
	ret |= _channel2._enabled ? 0x2 : 0x0;
	ret |= _channel3._enabled ? 0x4 : 0x0;
	ret |= _channel4._enabled ? 0x8 : 0x0;
	return ret;
}

static const uint32_t StateSNDid = 0x534E4420;

// 0 - id
// 4 - size
// 8 - tickCounter
// 16 - 

bool GbSound::LoadState(const SaveData_t *data)
{
	Endian conv(false);
	uint8_t *ptr = (uint8_t *)data->miscData;
	size_t miscLen = data->miscDataLen;
	// Find cpu segment
	while (miscLen >= 8) {
		uint32_t id = conv.convu32(*(uint32_t *)(ptr + 0));
		uint32_t len = conv.convu32(*(uint32_t *)(ptr + 4));
		if (id == StateSNDid && len >= 8) {
			ptr += 8;
			len -= 8;
			int dataLen = len;
			ptr = _channel1.LoadState(ptr, dataLen);
			ptr = _channel2.LoadState(ptr, dataLen);
			ptr = _channel3.LoadState(ptr, dataLen);
			ptr = _channel4.LoadState(ptr, dataLen);
			return true;
		}
		ptr += len;
		miscLen -= len;
	}
	return false;
}

bool GbSound::SaveState(std::vector<uint8_t> &data)
{
	Endian conv(false);
	size_t dataLen = 8;
	size_t size = data.size();
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	*(uint32_t *)(ptr + 0) = conv.convu32(StateSNDid);
	_channel1.SaveState(data);
	_channel2.SaveState(data);
	_channel3.SaveState(data);
	_channel4.SaveState(data);

	dataLen = data.size() - size;
	*(uint32_t *)(ptr + 4) = conv.convu32((uint32_t)dataLen);
	return true;
}

void GbSound::InitAudio(int source, unsigned int sampleRate, int channels)
{	
	GetChannel(source).InitAudio(sampleRate, channels);
}

void GbSound::GetAudio(int source, short *buffer, int samples)
{
	GetChannel(source).GetAudio(buffer, samples);
}

GbChannel & GbSound::GetChannel(int source)
{
	GbChannel *channel = nullptr;
	switch (source) {
	case 0:
		channel = &_channel1;
		break;
	case 1:
		channel = &_channel2;
		break;
	case 2:
		channel = &_channel3;
		break;
	case 3:
		channel = &_channel4;
		break;
	default:
		Log(Error, "Unknown initAudio channel received: %d", source);
		break;
	}
	return *channel;
}
