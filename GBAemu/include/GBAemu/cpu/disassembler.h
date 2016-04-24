#pragma once

#include <cstdint>

class Gba;

class Disassembler {
public:
	Disassembler(Gba &system);
	~Disassembler();

	int Disassemble(uint32_t address, const char **raw, const char **instr);

private:
	const char *DisassembleArm(uint32_t address, uint32_t instruction);
	const char *DisassembleThumb(uint32_t address, uint32_t instruction);

	const char *GetARMFormatString(uint32_t instruction);
	const char *GetThumbFormatString(uint16_t instruction);

	Gba &_system;
	char _buffer[256];
};