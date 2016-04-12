#pragma once

#include <stdint.h>

class Gba;

class Cpu {
public:
	Cpu(Gba &gba);
	~Cpu();

	void Reset();

	void Tick();

private:
	Gba &_system;
	uint32_t _cpsr;
	uint32_t _hostFlags;
	uint32_t _registers[16];
};