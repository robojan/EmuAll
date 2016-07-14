
#include <stdint.h>

#include <GBAemu/memory/cartridgeStorage.h>
#include <GBAemu/util/log.h>

CartridgeStorage::CartridgeStorage()
{
	
}

CartridgeStorage::~CartridgeStorage()
{

}

uint32_t CartridgeStorage::GetSize()
{
	return 0;
}

uint8_t CartridgeStorage::ReadMemory(uint32_t address)
{
	return 0;
}

uint32_t CartridgeStorage::Read32(uint32_t address)
{
	Log(Warn, "32 Bit read from unimplemented cartridge at address 0x%08x", address);
	return 0;
}

uint32_t CartridgeStorage::Read16(uint32_t address)
{
	Log(Warn, "16 Bit read from unimplemented cartridge at address 0x%08x", address);
	return 0;
}

uint32_t CartridgeStorage::Read8(uint32_t address)
{
	Log(Warn, "8 Bit read from unimplemented cartridge at address 0x%08x", address);
	return 0;
}

void CartridgeStorage::Write32(uint32_t address, uint32_t val)
{
	Log(Warn, "32 Bit write to unimplemented cartridge at address 0x%08x with data 0x%08x", address, val);
}

void CartridgeStorage::Write16(uint32_t address, uint16_t val)
{
	Log(Warn, "16 Bit write to unimplemented cartridge at address 0x%08x with data 0x%04x", address, val);
}

void CartridgeStorage::Write8(uint32_t address, uint8_t val)
{
	Log(Warn, "8 Bit write to unimplemented cartridge at address 0x%08x with data 0x%02x", address, val);
}
