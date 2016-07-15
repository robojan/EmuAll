
#include <GBAemu/memory/sram.h>
#include <GBAemu/cpu/armException.h>
#include <GBAemu/util/log.h>
#include <string.h>

Sram::Sram(Memory &memory) : 
	CartridgeStorage(memory)
{
	memset(_dataStorage, 0xFF, 0x8000);
}

Sram::~Sram()
{

}

uint32_t Sram::GetSize()
{
	return 0x8000;
}

uint8_t Sram::ReadMemory(uint32_t address)
{
	if (address < 0x8000) {
		return _dataStorage[address];
	}
	throw DataAbortARMException(address);
}

uint32_t Sram::Read32(uint32_t address)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Log(Warn, "32 bit read from SRAM");
		return Read(address);
	}
	return CartridgeStorage::Read32(address);
}

uint32_t Sram::Read16(uint32_t address)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Log(Warn, "16 bit read from SRAM");
		return Read(address);
	}
	return CartridgeStorage::Read16(address);
}

uint32_t Sram::Read8(uint32_t address)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		return Read(address);
	}
	return CartridgeStorage::Read8(address);
}

void Sram::Write32(uint32_t address, uint32_t val)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Log(Warn, "32 bit write to SRAM");
		Write(address, val & 0xFF);
	}
	else {
		CartridgeStorage::Write32(address, val);
	}
}

void Sram::Write16(uint32_t address, uint16_t val)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Log(Warn, "16 bit write to SRAM");
		Write(address, val & 0xFF);
	}
	else {
		CartridgeStorage::Write16(address, val);
	}
}

void Sram::Write8(uint32_t address, uint8_t val)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Write(address, val);
	}
	else {
		CartridgeStorage::Write8(address, val);
	}
}

uint8_t Sram::Read(uint_least16_t address)
{
	return _dataStorage[address & 0x7FFF];
}

void Sram::Write(uint_least16_t address, uint8_t val)
{
	_dataStorage[address & 0x7FFF] = val;
}
