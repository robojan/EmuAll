#pragma once

#include <stdint.h>
#include <emu.h>
class Gba;

class Memory {
public:
	Memory(Gba &gba);
	~Memory();

	int Load(const SaveData_t *data);

	void Tick();

	uint8_t Read8(uint32_t address);
	uint16_t Read16(uint32_t address);
	uint32_t Read32(uint32_t address);
	void Write8(uint32_t address, uint8_t value);
	void Write16(uint32_t address, uint16_t value);
	void Write32(uint32_t address, uint32_t value);

private:
	// Memory map Should be first in the class memory
	uint8_t *_memmap[16]; 
	// Mask for invalid addresses
	uint32_t _memMask[16];
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