#pragma once

#include <emu.h>

#include <GBAemu/memory/memory.h>
#include <GBAemu/cpu/cpu.h>
#include <GBAemu/cpu/disassembler.h>

class Gba {
	friend class Cpu;
public:
	Gba();
	~Gba();

	int Init();
	int Load(const SaveData_t *data);

	// Running functions
	void Step();
	int Tick(unsigned int time);
	void Run(bool run = true);
	bool IsRunning() const;

	// Graphics functions
	void Draw(int id);
	
	// access functions
	Memory &GetMemory() { return _memory; }
	Cpu &GetCpu() { return _cpu; }
	Disassembler &GetDisassembler() { return _disassembler; }

private:
	Memory _memory;
	Cpu _cpu;

	bool _running;
	Disassembler _disassembler;
};
