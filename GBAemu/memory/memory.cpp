
#include <cstring>
#include <cassert>
#include <cstdio>

#include <Emuall/math/math.h>

#include <GBAemu/memory/memory.h>
#include <GBAemu/util/log.h>
#include <GBAemu/defines.h>


Memory::Memory(Gba &gba) :
	_system(gba), _bios(nullptr), _wram(nullptr), _chipWram(nullptr), _ioRegisters(nullptr), 
	_pram(nullptr), _vram(nullptr), _oram(nullptr), _sram(nullptr), 
	_sramLength(0), _rom(nullptr), _romLength(0)
{
	// Allocate all the memory buffers
	_bios = new uint8_t[BIOSSIZE];
	_wram = new uint8_t[WRAMSIZE];
	_chipWram = new uint8_t[WRAMCHIPSIZE];
	_ioRegisters = new uint8_t[IOREGISTERSSIZE];
	_pram = new uint8_t[PRAMSIZE];
	_vram = new uint8_t[VRAMSIZE];
	_oram = new uint8_t[ORAMSIZE];

	// Initialize memory
	memset(_bios, 0, BIOSSIZE);
	memset(_wram, 0, WRAMSIZE);
	memset(_chipWram, 0, WRAMCHIPSIZE);
	memset(_ioRegisters, 0, IOREGISTERSSIZE);
	memset(_pram, 0, PRAMSIZE);
	memset(_vram, 0, VRAMSIZE);
	memset(_oram, 0, ORAMSIZE);
	
	// Setup the memory map
	_memmap[0x0] = _bios;
	_memmap[0x1] = nullptr;
	_memmap[0x2] = _wram;
	_memmap[0x3] = _chipWram;
	_memmap[0x4] = _ioRegisters;
	_memmap[0x5] = _pram;
	_memmap[0x6] = _vram;
	_memmap[0x7] = _oram;
	_memmap[0x8] = nullptr;
	_memmap[0x9] = nullptr;
	_memmap[0xA] = nullptr;
	_memmap[0xB] = nullptr;
	_memmap[0xC] = nullptr;
	_memmap[0xD] = nullptr;
	_memmap[0xE] = nullptr;
	_memmap[0xF] = nullptr;

	// Setup the invalid mask ptrs
	_memMask[0x0] = BIOSMASK;
	_memMask[0x1] = 0;
	_memMask[0x2] = WRAMMASK;
	_memMask[0x3] = WRAMCHIPMASK;
	_memMask[0x4] = IOREGISTERSMASK;
	_memMask[0x5] = PRAMMASK;
	_memMask[0x6] = VRAMMASK;
	_memMask[0x7] = ORAMMASK;
	_memMask[0x8] = 0;
	_memMask[0x9] = 0;
	_memMask[0xA] = 0;
	_memMask[0xB] = 0;
	_memMask[0xC] = 0;
	_memMask[0xD] = 0;
	_memMask[0xE] = 0;
	_memMask[0xF] = 0;

	// Load bios from file
	FILE *biosFile;
	fopen_s(&biosFile, "D:\\Dropbox\\Visual studio\\Projects\\emuall\\rom\\gba\\GBA.BIOS", "rb");
	fread(_bios, 16 * 1024, 1, biosFile);
	fclose(biosFile);

}

Memory::~Memory()
{
	if (_bios != nullptr) {
		delete[] _bios; _bios = nullptr;
	}
	if (_wram != nullptr) {
		delete[] _wram; _wram = nullptr;
	}
	if (_chipWram != nullptr) {
		delete[] _chipWram; _chipWram = nullptr;
	}
	if (_ioRegisters != nullptr) {
		delete[] _ioRegisters; _ioRegisters = nullptr;
	}
	if (_pram != nullptr) {
		delete[] _pram; _pram = nullptr;
	}
	if (_vram != nullptr) {
		delete[] _vram; _vram = nullptr;
	}
	if (_oram != nullptr) {
		delete[] _oram; _oram = nullptr;
	}
	if (_rom != nullptr) {
		delete[] _rom;
		_romLength = 0;
		_rom = nullptr;
	}
	if (_sram != nullptr) {
		delete[] _sram;
		_sramLength = 0;
		_sram = nullptr;
	}
}

int Memory::Load(const SaveData_t *data)
{
	// Initialize ROM
	if (_rom != nullptr) {
		delete[] _rom;
		_rom = nullptr;
		_romLength = 0;
	}
	if (_sram != nullptr) {
		delete[] _sram;
		_sram = nullptr;
		_sramLength = 0;
	}

	// Copy the rom 
	_romLength = data->romDataLen;
	Log(Debug, "RomLength: %dkB", _romLength / 1024);
	if (_romLength > 0x2000000) {
		Log(Error, "Rom is too big. GBA roms can be maximally 32MB");
		_romLength = 0x2000000;
	}
	if (_romLength > 0) {
		_rom = new uint8_t[_romLength];
		memcpy(_rom, data->romData, _romLength);
		_memmap[0x8] = _rom;
		_memmap[0xA] = _rom;
		_memmap[0xC] = _rom;
		if (_romLength >= 0x1000000) {
			_memMask[0x8] = 0xFFFFFF;
			_memMask[0xA] = 0xFFFFFF;
			_memMask[0xC] = 0xFFFFFF;
		}
		else {
			if(!isPowerOf2(_romLength)) {
				Log(Error, "Rom length is not a power of 2");
				return 0;
			}
			uint32_t romMask = _romLength - 1;
			_memMask[0x8] = romMask;
			_memMask[0xA] = romMask;
			_memMask[0xC] = romMask;
		}
	}
	if (_romLength > 0x1000000) {
		_memmap[0x9] = _rom + 0x1000000;
		_memmap[0xB] = _rom + 0x1000000;
		_memmap[0xD] = _rom + 0x1000000;
		if (_romLength == 0x2000000) {
			_memMask[0x9] = 0xFFFFFF;
			_memMask[0xB] = 0xFFFFFF;
			_memMask[0xD] = 0xFFFFFF;
		}
		else {
			if (!isPowerOf2(_romLength-0x1000000)) {
				Log(Error, "Rom length is not a power of 2");
				return 0;
			}
			uint32_t romMask = _romLength - 1;
			_memMask[0x9] = romMask;
			_memMask[0xB] = romMask;
			_memMask[0xD] = romMask;
		}
	}

	// Setup SRAM
	// Assume 64kB TODO: detect in some way
	_sramLength = 64 * 1024;
	if (_sramLength > 0) {
		_sram = new uint8_t[_sramLength];
		unsigned int ramdataLen = data->ramDataLen;
		if (ramdataLen > _sramLength) {
			Log(Error, "SRAM length is larger than 64kB.");
			ramdataLen = _sramLength;
		}
		if (ramdataLen > 0) {
			memcpy(_sram, data->ramData, ramdataLen);
		}
	}

	return 1;
}

void Memory::Tick()
{

}

uint8_t Memory::Read8(uint32_t address)
{
	uint8_t index = (address >> 24) & 0xF;
	address &= _memMask[index];
	if (_memmap[index] == nullptr) {
		assert(_memmap[index] != nullptr); // TODO nice error handling
	}
	if (index == 6 && address > 0x17FFF) {
		assert(address <= 0x17FFF); // TODO nice error handling
	}
	return _memmap[index][address];
}

uint16_t Memory::Read16(uint32_t address)
{
	uint8_t index = (address >> 24) & 0xF;
	address &= _memMask[index];
	if (_memmap[index] == nullptr) {
		assert(_memmap[index] != nullptr); // TODO nice error handling
	}
	if (index == 6 && address > 0x17FFF) {
		assert(address <= 0x17FFF); // TODO nice error handling
	}
	return *((uint16_t *)&_memmap[index][address]);
}

uint32_t Memory::Read32(uint32_t address)
{
	uint8_t index = (address >> 24) & 0xF;
	address &= _memMask[index];
	if (_memmap[index] == nullptr) {
		assert(_memmap[index] != nullptr); // TODO nice error handling
	}
	if (index == 6 && address > 0x17FFF) {
		assert(address <= 0x17FFF); // TODO nice error handling
	}
	return *((uint32_t *)&_memmap[index][address]);
}

void Memory::Write8(uint32_t address, uint8_t value)
{
	uint8_t index = (address >> 24) & 0xF;
	address &= _memMask[index];
	if (_memmap[index] == nullptr) {
		assert(_memmap[index] != nullptr); // TODO nice error handling
	}
	if (index == 6 && address > 0x17FFF) {
		assert(address <= 0x17FFF); // TODO nice error handling
	}
	_memmap[index][address] = value;
}

void Memory::Write16(uint32_t address, uint16_t value)
{
	uint8_t index = (address >> 24) & 0xF;
	address &= _memMask[index];
	if (_memmap[index] == nullptr) {
		assert(_memmap[index] != nullptr); // TODO nice error handling
	}
	if (index == 6 && address > 0x17FFF) {
		assert(address <= 0x17FFF); // TODO nice error handling
	}
	*((uint16_t *)&_memmap[index][address]) = value;
}

void Memory::Write32(uint32_t address, uint32_t value)
{
	uint8_t index = (address >> 24) & 0xF;
	address &= _memMask[index];
	if (_memmap[index] == nullptr) {
		assert(_memmap[index] != nullptr); // TODO nice error handling
	}
	if (index == 6 && address > 0x17FFF) {
		assert(address <= 0x17FFF); // TODO nice error handling
	}
	*((uint32_t *)&_memmap[index][address]) = value;
}

uint32_t Memory::GetRomSize()
{
	return _romLength;
}

uint32_t Memory::GetSRAMSize()
{
	return _sramLength;
}

uint8_t Memory::ReadBios8(uint32_t address)
{
	assert(address < BIOSSIZE);
	return _bios[address];
}

uint8_t Memory::ReadWRAM8(uint32_t address)
{
	assert(address < WRAMSIZE);
	return _wram[address];
}

uint8_t Memory::ReadChipWRAM8(uint32_t address)
{
	assert(address < WRAMCHIPSIZE);
	return _chipWram[address];
}

uint8_t Memory::ReadRegister8(uint32_t address)
{
	assert(address < IOREGISTERSSIZE);
	return _ioRegisters[address];
}

uint8_t Memory::ReadPRAM8(uint32_t address)
{
	assert(address < PRAMSIZE);
	return _pram[address];
}

uint8_t Memory::ReadVRAM8(uint32_t address)
{
	assert(address < VRAMSIZE);
	return _vram[address];
}

uint8_t Memory::ReadORAM8(uint32_t address)
{
	assert(address < ORAMSIZE);
	return _oram[address];
}

uint8_t Memory::ReadSRAM8(uint32_t address)
{
	assert(address < _sramLength);
	return _sram[address];
}

uint8_t Memory::ReadROM8(uint32_t address)
{
	assert(address < _romLength);
	return _rom[address];
}
