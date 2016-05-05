#pragma once

#include <stdint.h>
#include <emu.h>
#include <emuall/util/tree.h>

#define IOREG32(registers, addr) (*(uint32_t *)(((uint8_t *)registers) + (addr & IOREGISTERSMASK)))
#define IOREG16(registers, addr) (*(uint16_t *)(((uint8_t *)registers) + (addr & IOREGISTERSMASK)))
#define IOREG8(registers, addr) (registers[addr & IOREGISTERSMASK])

class Gba;

class MemoryEventHandler {
public:
	virtual void HandleEvent(uint32_t address, int size) = 0;
};

class Memory {
public:
	Memory(Gba &gba);
	~Memory();

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
	uint32_t GetSRAMSize();
	uint8_t ReadBios8(uint32_t address);
	uint8_t ReadWRAM8(uint32_t address);
	uint8_t ReadChipWRAM8(uint32_t address);
	uint8_t ReadRegister8(uint32_t address);
	uint8_t ReadPRAM8(uint32_t address);
	uint8_t ReadVRAM8(uint32_t address);
	uint8_t ReadORAM8(uint32_t address);
	uint8_t ReadSRAM8(uint32_t address);
	uint8_t ReadROM8(uint32_t address);

	void RegisterEvent(uint32_t address, MemoryEventHandler *evt);

	uint8_t *GetRegisters();
	uint8_t *GetPalettes();
	uint8_t *GetVRAM();

private:

	// Memory map Should be first in the class memory
	uint8_t *_memmap[16]; 
	// Mask for invalid addresses
	uint32_t _memMask[16];
	// Event callbacks
	AATree<uint32_t, MemoryEventHandler *> _events;
	// system
	Gba &_system;
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
	// external sram
	uint8_t *_sram;
	uint32_t _sramLength;
	// Gamepack rom
	uint8_t *_rom;
	uint32_t _romLength;
};