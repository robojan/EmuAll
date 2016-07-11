#pragma once

#include <stdint.h>
#include <emu.h>
#include <emuall/util/tree.h>

#define IOREG32(registers, addr) (*(uint32_t *)(((uint8_t *)registers) + (addr & IOREGISTERSMASK)))
#define IOREG16(registers, addr) (*(uint16_t *)(((uint8_t *)registers) + (addr & IOREGISTERSMASK)))
#define IOREG8(registers, addr) (registers[addr & IOREGISTERSMASK])

class Gba;
class CartridgeStorage;

struct DMAInternalInfo {
	bool active;
	bool hasRead;
	uint16_t wc;
	uint32_t src;
	uint32_t dst;
	uint32_t ctrl;
	uint32_t value;
	uint32_t activeSrc;
	uint32_t activeDst;
};

class MemoryEventHandler {
public:
	virtual void HandleEvent(uint32_t address, int size) = 0;
};

class Memory : public MemoryEventHandler {
public:
	Memory(Gba &gba);
	~Memory();

	void SetBiosProtected(uint32_t word);
	int Load(const SaveData_t *data);

	void Tick();
	
	uint8_t Read8(uint32_t address);
	uint16_t Read16(uint32_t address);
	uint32_t Read32(uint32_t address);
	void Write8(uint32_t address, uint8_t value, bool event = true);
	void Write16(uint32_t address, uint16_t value, bool event = true);
	void Write32(uint32_t address, uint32_t value, bool event = true);
	void ManagedWrite32(uint32_t address, uint32_t value);

	uint32_t GetRomSize();
	uint8_t ReadBios8(uint32_t address);
	uint8_t ReadWRAM8(uint32_t address);
	uint8_t ReadChipWRAM8(uint32_t address);
	uint8_t ReadRegister8(uint32_t address);
	uint8_t ReadPRAM8(uint32_t address);
	uint8_t ReadVRAM8(uint32_t address);
	uint8_t ReadORAM8(uint32_t address);
	uint8_t ReadROM8(uint32_t address);

	void RegisterEvent(uint32_t address, MemoryEventHandler *evt);

	uint8_t *GetRegisters();
	uint8_t *GetPalettes();
	uint8_t *GetVRAM();
	uint8_t *GetOAM();

	virtual void HandleEvent(uint32_t address, int size);

	bool IsDMAActive() const;
	void DMAStartVBlank();
	void DMAStartHBlank();
	void DMAStartSoundFIFO();
private:
	void InitRegisters();
	CartridgeStorage *DetectStorageChip();

	// Memory map Should be first in the class memory
	uint8_t *_memmap[16]; 
	// Mask for invalid addresses
	uint32_t _memMask[16];
	// Event callbacks
	AATree<uint32_t, MemoryEventHandler *> _events;
	// DMA
	struct DMAInternalInfo _dma[4];
	// system
	Gba &_system;
	// Protected bios word
	uint32_t _biosProtected;
	// 16 kB bios
	uint8_t *_bios;
	// 256kB on board wram
	uint8_t *_wram;
	// 32kB on chip wram
	uint8_t *_chipWram;
	// IO registers
	uint8_t *_ioRegisters;
	// 1kB palette RAM
	uint8_t *_pram;
	// 96kB vram
	uint8_t *_vram;
	// 1kB OAM, OBJ attributes
	uint8_t *_oram;
	// Gamepack rom
	uint8_t *_rom;
	uint32_t _romLength;
	CartridgeStorage *_cartridge;
};