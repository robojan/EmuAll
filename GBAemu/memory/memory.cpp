
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
	_memInvMask[0x0] = (~BIOSMASK) & 0xF0FFFFFF;
	_memInvMask[0x1] = 0xFFFFFFFF;
	_memInvMask[0x2] = (~WRAMMASK) & 0xF0FFFFFF;
	_memInvMask[0x3] = (~WRAMCHIPMASK) & 0xF0FFFFFF;
	_memInvMask[0x4] = (~IOREGISTERSMASK) & 0xF0FFFFFF;
	_memInvMask[0x5] = (~PRAMMASK) & 0xF0FFFFFF;
	_memInvMask[0x6] = (~VRAMMASK) & 0xF0FFFFFF;
	_memInvMask[0x7] = (~ORAMMASK) & 0xF0FFFFFF;
	_memInvMask[0x8] = 0xF0FFFFFF;
	_memInvMask[0x9] = 0xF0FFFFFF;
	_memInvMask[0xA] = 0xF0FFFFFF;
	_memInvMask[0xB] = 0xF0FFFFFF;
	_memInvMask[0xC] = 0xF0FFFFFF;
	_memInvMask[0xD] = 0xF0FFFFFF;
	_memInvMask[0xE] = 0xF0FFFFFF;
	_memInvMask[0xF] = 0xF0FFFFFF;

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
			_memInvMask[0x8] = 0xF0000000;
			_memInvMask[0xA] = 0xF0000000;
			_memInvMask[0xC] = 0xF0000000;
		}
		else {
			if(!isPowerOf2(_romLength)) {
				Log(Error, "Rom length is not a power of 2");
				return 0;
			}
			uint32_t romMask = _romLength - 1;
			_memInvMask[0x8] = (~romMask) & 0xF0FFFFFF;
			_memInvMask[0xA] = (~romMask) & 0xF0FFFFFF;
			_memInvMask[0xC] = (~romMask) & 0xF0FFFFFF;
		}
	}
	if (_romLength > 0x1000000) {
		_memmap[0x9] = _rom + 0x1000000;
		_memmap[0xB] = _rom + 0x1000000;
		_memmap[0xD] = _rom + 0x1000000;
		if (_romLength == 0x2000000) {
			_memInvMask[0x9] = 0xF0000000;
			_memInvMask[0xB] = 0xF0000000;
			_memInvMask[0xD] = 0xF0000000;
		}
		else {
			if (!isPowerOf2(_romLength-0x1000000)) {
				Log(Error, "Rom length is not a power of 2");
				return 0;
			}
			uint32_t romMask = _romLength - 0x1000001;
			_memInvMask[0x9] = (~romMask) & 0xF0FFFFFF;
			_memInvMask[0xB] = (~romMask) & 0xF0FFFFFF;
			_memInvMask[0xD] = (~romMask) & 0xF0FFFFFF;
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
	if ((address & _memInvMask[index]) != 0) {
		assert((address & _memInvMask[index]) == 0); // TODO nice error handling
	}
	if (index == 6 && (address & 0x1FFFF) > 0x17FFF) {
		assert((address & 0x1FFFF) <= 0x17FFF); // TODO nice error handling
	}
	return _memmap[index][address & 0xFFFFFF];
}

uint16_t Memory::Read16(uint32_t address)
{
	uint8_t index = (address >> 24) & 0xF;
	if ((address & _memInvMask[index]) != 0) {
		assert((address & _memInvMask[index]) == 0); // TODO nice error handling
	}
	if (index == 6 && (address & 0x1FFFF) > 0x17FFF) {
		assert((address & 0x1FFFF) <= 0x17FFF); // TODO nice error handling
	}
	return *((uint16_t *)&_memmap[index][address & 0xFFFFFF]);
}

uint32_t Memory::Read32(uint32_t address)
{
	uint8_t index = (address >> 24) & 0xF;
	if ((address & _memInvMask[index]) != 0) {
		assert((address & _memInvMask[index]) == 0); // TODO nice error handling
	}
	if (index == 6 && (address & 0x1FFFF) > 0x17FFF) {
		assert((address & 0x1FFFF) <= 0x17FFF); // TODO nice error handling
	}
	return *((uint32_t *)&_memmap[index][address & 0xFFFFFF]);
}
