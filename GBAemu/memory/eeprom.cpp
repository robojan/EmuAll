
#include <stdint.h>
#include <GBAemu/memory/eeprom.h>

Eeprom::Eeprom()
{

}

Eeprom::~Eeprom()
{

}

uint32_t Eeprom::GetSize()
{
	return CartridgeStorage::GetSize();
}

uint32_t Eeprom::Read32(uint32_t address)
{
	return CartridgeStorage::Read32(address);
}

uint32_t Eeprom::Read16(uint32_t address)
{
	return CartridgeStorage::Read16(address);
}

uint32_t Eeprom::Read8(uint32_t address)
{
	return CartridgeStorage::Read8(address);
}

void Eeprom::Write32(uint32_t address, uint32_t val)
{
	CartridgeStorage::Write32(address, val);
}

void Eeprom::Write16(uint32_t address, uint16_t val)
{
	CartridgeStorage::Write16(address, val);
}

void Eeprom::Write8(uint32_t address, uint8_t val)
{
	CartridgeStorage::Write8(address, val);
}
