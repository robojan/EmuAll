
#include <cstring>
#include <cassert>
#include <cstdio>

#include <Emuall/math/math.h>

#include <GBAemu/cpu/armException.h>
#include <GBAemu/memory/memory.h>
#include <GBAemu/util/log.h>
#include <GBAemu/defines.h>
#include <GBAemu/gba.h>
#include <GBAemu/memory/cartridgeStorage.h>


Memory::Memory(Gba &gba) :
	_system(gba), _events(),
	_dma{{false, false, 0, 0},{ false, false, 0, 0 },{ false, false, 0, 0 } ,{ false, false, 0, 0 } },
	_bios(nullptr), _wram(nullptr), _chipWram(nullptr), _ioRegisters(nullptr),
	_pram(nullptr), _vram(nullptr), _oram(nullptr),
	_rom(nullptr), _romLength(0), _cartridge(nullptr)
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
	if (biosFile == nullptr) {
		fopen_s(&biosFile, "C:\\Dropbox\\Visual studio\\Projects\\emuall\\rom\\gba\\GBA.BIOS", "rb");
	}
	fread(_bios, 16 * 1024, 1, biosFile);
	fclose(biosFile);

	SetBiosProtected(0xE129F000);

	// Init registers
	InitRegisters();

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
	if (_cartridge != nullptr) {
		delete _cartridge;
		_cartridge = nullptr;
	}
}


void Memory::SetBiosProtected(uint32_t word)
{
	_biosProtected = word;
}

int Memory::Load(const SaveData_t *data)
{
	// Initialize ROM
	if (_rom != nullptr) {
		delete[] _rom;
		_rom = nullptr;
		_romLength = 0;
	}
	if (_cartridge != nullptr) {
		delete _cartridge;
		_cartridge = nullptr;
	}

	// Copy the rom 
	_romLength = data->romDataLen;
	Log(Debug, "RomLength: %dkB", _romLength / 1024);
	if (_romLength > 0x2000000) {
		Log(Error, "Rom is too big. GBA roms can be maximally 32MB");
		_romLength = 0x2000000;
	}
	if (_romLength > 0) {
		uint32_t romMemSize = _romLength > 0x1000000 ? 0x2000000 : 0x1000000;
		_rom = new uint8_t[romMemSize];
		memcpy(_rom, data->romData, _romLength);
		// Fill unused space
		for (uint32_t address = _romLength; address < romMemSize; address += 2) {
			*(uint16_t *)(_rom + address) = (address & 0x1FFFE) >> 1;
		}

		_memmap[0x8] = _rom;
		_memmap[0xA] = _rom;
		_memmap[0xC] = _rom;
		_memMask[0x8] = 0xFFFFFF;
		_memMask[0xA] = 0xFFFFFF;
		_memMask[0xC] = 0xFFFFFF;
		_memMask[0x9] = 0xFFFFFF;
		_memMask[0xB] = 0xFFFFFF;
		_memMask[0xD] = 0xFFFFFF;
		if (_romLength > 0x1000000) {
			_memmap[0x9] = _rom + 0x1000000;
			_memmap[0xB] = _rom + 0x1000000;
			_memmap[0xD] = _rom + 0x1000000;
		}
	}

	_cartridge = DetectStorageChip();
	
	return 1;
}

void Memory::Tick()
{
	for (int i = 0; i < 4; i++) {
		if (_dma[i].active) {
			bool transfer32 = (_dma[i].ctrl & (1 << 26)) != 0;
			if (_dma[i].hasRead) {
				if (transfer32) {
					Write32(_dma[i].activeDst, _dma[i].value);
					switch ((_dma[i].ctrl >> 21) & 0x3) {
					case 0: _dma[i].activeDst += 4; break;
					case 1: _dma[i].activeDst -= 4; break;
					case 2: break;
					case 3: _dma[i].activeDst += 4; break;
					}
				}
				else {
					Write16(_dma[i].activeDst, _dma[i].value);
					switch ((_dma[i].ctrl >> 21) & 0x3) {
					case 0: _dma[i].activeDst += 2; break;
					case 1: _dma[i].activeDst -= 2; break;
					case 2: break;
					case 3: _dma[i].activeDst += 2; break;
					}
				}
				_dma[i].hasRead = false;
				_dma[i].wc--;
			}
			else {
				if (transfer32) {
					_dma[i].value = Read32(_dma[i].activeSrc);
					switch ((_dma[i].ctrl >> 23) & 0x3) {
					case 0: _dma[i].activeSrc += 4; break;
					case 1: _dma[i].activeSrc -= 4; break;
					case 2: break;
					case 3: break;
					}
				}
				else {
					_dma[i].value = Read16(_dma[i].activeSrc);
					switch ((_dma[i].ctrl >> 23) & 0x3) {
					case 0: _dma[i].activeSrc += 2; break;
					case 1: _dma[i].activeSrc -= 2; break;
					case 2: break;
					case 3: break;
					}
				}
				_dma[i].hasRead = true;
			}
			if (_dma[i].wc == 0) {

				_dma[i].active = false;
				bool repeat = (_dma[i].ctrl & (1 << 25)) != 0;
				bool irq = (_dma[i].ctrl & (1 << 30)) != 0;
				if (!repeat) {
					_dma[i].ctrl &= ~(1 << 31);
				}
				else {
					if (i == 3) {
						_dma[i].wc = 0xFFFF;
					}
					else {
						_dma[i].wc = 0x7FFF;
					}
					_dma[i].activeSrc = _dma[i].src;
					if ((_dma[i].ctrl & (3 << 21)) == (3 << 21)) {
						_dma[i].activeDst = _dma[i].dst;
					}
				}
				if (irq) {
					_system.RequestIRQ(1 << (IRQ_DMA0_NO + i));
				}
			}
		}
	}
}

bool Memory::IsDMAActive() const
{
	return _dma[0].active || _dma[1].active || _dma[2].active || _dma[3].active;
}

uint8_t Memory::Read8(uint32_t address)
{
	uint8_t index = (address >> 24) & 0xF;
	if (index >= 0xD) {
		return _cartridge->Read8(address);
	}
	if (index == 0 && !_system.GetCpu().IsInBios()) {
		return (_biosProtected >> (8 * (address & 3))) & 0xFF;
	}
	if (_memmap[index] == nullptr) {
		// Comment from gbxemu source
		// reads beyond the ROM data seem to be filled with:
		// 0xx00000: 0000 0001 0002 .. FFFF: 0xx1FFFE
		// 0xx20000: 0000 0001 0002 .. FFFF: 0xx3FFFE
		// (Incrementing 16 bit values, mirrored each 128 KiB)
		// [Verified on EZ-Flash IV]
		if (index >= 8) {
			// cartridge 			
			int value = (address & 0x1FFFE) >> 1;
			if ((address & 1) != 0) {
				return (value >> 8) & 0xFF;
			}
			else {
				return value & 0xFF;
			}
		}
		throw DataAbortARMException(address);
	}
	return _memmap[index][address & _memMask[index]];
}

uint16_t Memory::Read16(uint32_t address)
{
	uint8_t index = (address >> 24) & 0xF;
	if (index >= 0xD) {
		return _cartridge->Read16(address);
	}
	if (index == 0 && !_system.GetCpu().IsInBios()) {
		return (_biosProtected >> (16 * (address & 1))) & 0xFFFF;
	}
	if (_memmap[index] == nullptr) {
		if (index >= 8) {
			// cartridge 
			return (address & 0x1FFFE) >> 1;
		}
		throw DataAbortARMException(address);
	}
	return *((uint16_t *)&_memmap[index][address & _memMask[index]]);
}

uint32_t Memory::Read32(uint32_t address)
{
	uint8_t index = (address >> 24) & 0xF;
	if (index >= 0xD) {
		return _cartridge->Read32(address);
	}
	if (index == 0 && !_system.GetCpu().IsInBios()) {
		return _biosProtected;
	}
	if (_memmap[index] == nullptr) {
		// cartridge 
		if (index >= 8) {
			// cartridge 
			uint32_t value = (address & 0x1FFFE) >> 1;
			value |= value << 16;
			return value;
		}
		throw DataAbortARMException(address);
	}
	return *((uint32_t *)&_memmap[index][address & _memMask[index]]);
}

void Memory::Write8(uint32_t address, uint8_t value, bool event)
{
	uint8_t index = (address >> 24) & 0xF;
	if (index >= 0xD) {
		_cartridge->Write8(address, value);
	}
	else {
		if (_memmap[index] == nullptr || index == 0) {
			Log(Debug, "Illegal write to address 0x%08x, val 0x%08x", address, value);
			return;
		}
		_memmap[index][address & _memMask[index]] = value;
	}
	if (event) {
		auto &it = _events.Find(address & ~3);
		while (it != _events.End()) {
			it->HandleEvent(address, 1);
			it++;
		}
	}
}

void Memory::Write16(uint32_t address, uint16_t value, bool event)
{
	uint8_t index = (address >> 24) & 0xF;
	if (index >= 0xD) {
		_cartridge->Write16(address, value);
	}
	else {
		if (_memmap[index] == nullptr || index == 0) {
			Log(Debug, "Illegal write to address 0x%08x, val 0x%08x", address, value);
			return;
		}
		*((uint16_t *)&_memmap[index][address & _memMask[index]]) = value;
	}
	if (event) {
		auto &it = _events.Find(address & ~3);
		while (it != _events.End()) {
			it->HandleEvent(address, 2);
			it++;
		}
	}
}

void Memory::Write32(uint32_t address, uint32_t value, bool event)
{
	uint8_t index = (address >> 24) & 0xF;
	if (index >= 0xD) {
		_cartridge->Write32(address, value);
	}
	else {
		if (_memmap[index] == nullptr || index == 0) {
			Log(Debug, "Illegal write to address 0x%08x, val 0x%08x", address, value);
			return;
		}
		*((uint32_t *)&_memmap[index][address & _memMask[index]]) = value;
	}
	if (event) {
		auto &it = _events.Find(address & ~3);
		while (it != _events.End()) {
			it->HandleEvent(address, 4);
			it++;
		}
	}
}

void Memory::ManagedWrite32(uint32_t address, uint32_t value)
{
	uint8_t index = (address >> 24) & 0xF;
	address &= _memMask[index];
	if (_memmap[index] == nullptr) {
		throw DataAbortARMException(address);
	}
	*((uint32_t *)&_memmap[index][address]) = value;
}

uint32_t Memory::GetRomSize()
{
	return _romLength;
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

uint8_t Memory::ReadROM8(uint32_t address)
{
	assert(address < _romLength);
	return _rom[address];
}

void Memory::RegisterEvent(uint32_t address, MemoryEventHandler *evt)
{
	assert((address & 3) == 0);
	_events.Insert(address, evt);
}
uint8_t * Memory::GetRegisters()
{
	return _ioRegisters;
}

uint8_t * Memory::GetPalettes()
{
	return _pram;
}

uint8_t * Memory::GetVRAM()
{
	return _vram;
}

uint8_t * Memory::GetOAM()
{
	return _oram;
}

void Memory::InitRegisters()
{
	IOREG32(_ioRegisters, DMA0SAD) = 0xFFFFFFFF;
	IOREG32(_ioRegisters, DMA1SAD) = 0xFFFFFFFF;
	IOREG32(_ioRegisters, DMA2SAD) = 0xFFFFFFFF;
	IOREG32(_ioRegisters, DMA3SAD) = 0xFFFFFFFF;
	IOREG32(_ioRegisters, DMA0DAD) = 0xFFFFFFFF;
	IOREG32(_ioRegisters, DMA1DAD) = 0xFFFFFFFF;
	IOREG32(_ioRegisters, DMA2DAD) = 0xFFFFFFFF;
	IOREG32(_ioRegisters, DMA3DAD) = 0xFFFFFFFF;
	IOREG32(_ioRegisters, DMA0CNT_L) = 0;
	IOREG32(_ioRegisters, DMA1CNT_L) = 0;
	IOREG32(_ioRegisters, DMA2CNT_L) = 0;
	IOREG32(_ioRegisters, DMA3CNT_L) = 0;
	RegisterEvent(DMA0SAD, this);
	RegisterEvent(DMA0DAD, this);
	RegisterEvent(DMA0CNT_L, this);
	RegisterEvent(DMA1SAD, this);
	RegisterEvent(DMA1DAD, this);
	RegisterEvent(DMA1CNT_L, this);
	RegisterEvent(DMA2SAD, this);
	RegisterEvent(DMA2DAD, this);
	RegisterEvent(DMA2CNT_L, this);
	RegisterEvent(DMA3SAD, this);
	RegisterEvent(DMA3DAD, this);
	RegisterEvent(DMA3CNT_L, this);
}


CartridgeStorage *Memory::DetectStorageChip()
{
	Log(Message, "Detecting storage chip");
	// Detect storage type from nintendo api id's 
	// EEPROM_Vnnn
	// SRAM_Vnnn
	// FLASH_Vnnn
	// FLASH512_Vnnn
	// FLASH1M_Vnnn
	int memorySize = 0;
	for (unsigned int i = 0; i < _romLength; i++) {
		char c = (char)_rom[i];
		if (c == 'E' || c == 'S' || c == 'F') {
			// check remaining
			if (memcmp(&_rom[i + 1], "EPROM_V", 7) == 0) {
				memorySize = 8 * 1024;
				Log(Message, "EEPROM storage of 8KB assumed found");
				break;
			}
			else if (memcmp(&_rom[i + 1], "RAM_V", 5) == 0) {
				memorySize = 32 * 1024;
				Log(Message, "SRAM storage of 32KB found");
				break;
			}
			else if (memcmp(&_rom[i + 1], "LASH_V", 6) == 0) {
				memorySize = 64 * 1024;
				Log(Message, "FLASH storage of 64KB found");
				break;
			}
			else if (memcmp(&_rom[i + 1], "LASH512_V", 9) == 0) {
				memorySize = 64 * 1024;
				Log(Message, "FLASH storage of 64KB found");
				break;
			}
			else if (memcmp(&_rom[i + 1], "LASH1M_V", 8) == 0) {
				memorySize = 128 * 1024;
				Log(Message, "FLASH storage of 128KB found");
				break;
			}
		}
	}
	return new CartridgeStorage();
}

void Memory::HandleEvent(uint32_t address, int size)
{
	if (address <= DMA0SAD + 3 && DMA0SAD < address + size) {
		_dma[0].src = IOREG32(_ioRegisters, DMA0SAD) & 0x07FFFFFF;
		IOREG32(_ioRegisters, DMA0SAD) = 0xFFFFFFFF;
	}
	if (address <= DMA1SAD + 3 && DMA1SAD < address + size) {
		_dma[1].src = IOREG32(_ioRegisters, DMA1SAD) & 0x07FFFFFF;
		IOREG32(_ioRegisters, DMA1SAD) = 0xFFFFFFFF;
	}
	if (address <= DMA2SAD + 3 && DMA2SAD < address + size) {
		_dma[2].src = IOREG32(_ioRegisters, DMA2SAD) & 0x07FFFFFF;
		IOREG32(_ioRegisters, DMA2SAD) = 0xFFFFFFFF;
	}
	if (address <= DMA3SAD + 3 && DMA3SAD < address + size) {
		_dma[3].src = IOREG32(_ioRegisters, DMA3SAD) & 0x0FFFFFFF;
		IOREG32(_ioRegisters, DMA3SAD) = 0xFFFFFFFF;
	}
	if (address <= DMA0DAD + 3 && DMA0DAD < address + size) {
		_dma[0].dst = IOREG32(_ioRegisters, DMA0DAD) & 0x07FFFFFF;
		IOREG32(_ioRegisters, DMA0DAD) = 0xFFFFFFFF;
	}
	if (address <= DMA1DAD + 3 && DMA1DAD < address + size) {
		_dma[1].dst = IOREG32(_ioRegisters, DMA1DAD) & 0x07FFFFFF;
		IOREG32(_ioRegisters, DMA1DAD) = 0xFFFFFFFF;
	}
	if (address <= DMA2DAD + 3 && DMA2DAD < address + size) {
		_dma[2].dst = IOREG32(_ioRegisters, DMA2DAD) & 0x07FFFFFF;
		IOREG32(_ioRegisters, DMA2DAD) = 0xFFFFFFFF;
	}
	if (address <= DMA3DAD + 3 && DMA3DAD < address + size) {
		_dma[3].dst = IOREG32(_ioRegisters, DMA3DAD) & 0x0FFFFFFF;
		IOREG32(_ioRegisters, DMA3DAD) = 0xFFFFFFFF;
	}
	if (address <= DMA0CNT_L + 3 && DMA0CNT_L < address + size) {
		uint32_t newCtrl = IOREG32(_ioRegisters, DMA0CNT_L);
		_dma[0].active = (((_dma[0].ctrl ^ newCtrl) & newCtrl) & (1 << 31)) != 0;
		_dma[0].activeDst = _dma[0].dst;
		_dma[0].activeSrc = _dma[0].src;
		_dma[0].hasRead = false;
		if (_dma[0].active) {
			_dma[0].wc = newCtrl & 0x3FFF;
			Log(Debug, "DMA %d src: 0x%08x dst: 0x%08x, length: %d", 0, _dma[0].activeSrc, _dma[0].activeDst, _dma[0].wc);
		}
		else {
			_dma[0].wc = 0;
		}
		_dma[0].ctrl = newCtrl;
	}
	if (address <= DMA1CNT_L + 3 && DMA1CNT_L < address + size) {
		uint32_t newCtrl = IOREG32(_ioRegisters, DMA0CNT_L);
		_dma[1].active = (((_dma[1].ctrl ^ newCtrl) & newCtrl) & (1 << 31)) != 0;
		_dma[1].activeDst = _dma[1].dst;
		_dma[1].activeSrc = _dma[1].src;
		_dma[1].hasRead = false;
		if (_dma[1].active) {
			_dma[1].wc = newCtrl & 0x3FFF;
			Log(Debug, "DMA %d src: 0x%08x dst: 0x%08x, length: %d", 1, _dma[1].activeSrc, _dma[1].activeDst, _dma[1].wc);
		}
		else {
			_dma[1].wc = 0;
		}
		_dma[1].ctrl = newCtrl;
	}
	if (address <= DMA2CNT_L + 3 && DMA2CNT_L < address + size) {
		uint32_t newCtrl = IOREG32(_ioRegisters, DMA0CNT_L);
		_dma[2].active = (((_dma[2].ctrl ^ newCtrl) & newCtrl) & (1 << 31)) != 0;
		_dma[2].activeDst = _dma[2].dst;
		_dma[2].activeSrc = _dma[2].src;
		_dma[2].hasRead = false;
		if (_dma[2].active) {
			_dma[2].wc = newCtrl & 0x3FFF;
			Log(Debug, "DMA %d src: 0x%08x dst: 0x%08x, length: %d", 2, _dma[2].activeSrc, _dma[2].activeDst, _dma[2].wc);
		}
		else {
			_dma[2].wc = 0;
		}
		_dma[2].ctrl = newCtrl;
	}
	if (address <= DMA3CNT_L + 3 && DMA3CNT_L < address + size) {
		uint32_t newCtrl = IOREG32(_ioRegisters, DMA3CNT_L);
		_dma[3].active = (((_dma[3].ctrl ^ newCtrl) & newCtrl) & (1 << 31)) != 0;
		_dma[3].activeDst = _dma[3].dst;
		_dma[3].activeSrc = _dma[3].src;
		_dma[3].hasRead = false;
		if (_dma[3].active) {
			_dma[3].wc = newCtrl & 0xFFFF;
			Log(Debug, "DMA %d src: 0x%08x dst: 0x%08x, length: %d", 3, _dma[3].activeSrc, _dma[3].activeDst, _dma[3].wc);
		}
		else {
			_dma[3].wc = 0;
		}
		_dma[3].ctrl = newCtrl;
	}
}

void Memory::DMAStartVBlank() {
	for (int i = 0; i < 4; i++) {
		if ((_dma[i].ctrl & (1 << 31)) != 0 && 
			!_dma[i].active && 
			(_dma[i].ctrl & (3 << 12)) == (1 << 12)) {
			_dma[i].wc = _dma[i].ctrl & (i == 3 ? 0xFFFF : 0x7FFF);
			_dma[i].activeDst = _dma[i].dst;
			_dma[i].activeSrc = _dma[i].src;
			_dma[i].active = true;
			_dma[i].hasRead = false;
			Log(Debug, "DMA %d src: 0x%08x dst: 0x%08x, length: %d", i, _dma[i].activeSrc, _dma[i].activeDst, _dma[i].wc);
		}
	}
}

void Memory::DMAStartHBlank() {
	for (int i = 0; i < 4; i++) {
		if ((_dma[i].ctrl & (1 << 31)) != 0 &&
			!_dma[i].active &&
			(_dma[i].ctrl & (3 << 12)) == (2 << 12)) {
			_dma[i].wc = _dma[i].ctrl & (i == 3 ? 0xFFFF : 0x7FFF);
			_dma[i].activeDst = _dma[i].dst;
			_dma[i].activeSrc = _dma[i].src;
			_dma[i].active = true;
			_dma[i].hasRead = false;
			Log(Debug, "DMA %d src: 0x%08x dst: 0x%08x, length: %d", i, _dma[i].activeSrc, _dma[i].activeDst, _dma[i].wc);
		}
	}
}

void Memory::DMAStartSoundFIFO() {
	for (int i = 1; i < 3; i++) {
		if ((_dma[i].ctrl & (1 << 31)) != 0 &&
			!_dma[i].active &&
			(_dma[i].ctrl & (3 << 12)) == (3 << 12)) {
			_dma[i].wc = _dma[i].ctrl & 0xFFFF;
			_dma[i].activeDst = _dma[i].dst;
			_dma[i].activeSrc = _dma[i].src;
			_dma[i].active = true;
			_dma[i].hasRead = false;
			Log(Debug, "DMA %d src: 0x%08x dst: 0x%08x, length: %d", i, _dma[i].activeSrc, _dma[i].activeDst, _dma[i].wc);
		}
	}
}