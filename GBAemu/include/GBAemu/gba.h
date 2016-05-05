#pragma once

#include <emu.h>

#include <GBAemu/memory/memory.h>
#include <GBAemu/cpu/cpu.h>
#include <GBAemu/cpu/disassembler.h>
#include <GBAemu/gpu/gpu.h>

class Gba : public MemoryEventHandler {
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
	
	// access functions
	Memory &GetMemory() { return _memory; }
	Cpu &GetCpu() { return _cpu; }
	Gpu &GetGpu() { return _gpu; }
	Disassembler &GetDisassembler() { return _disassembler; }

	// execution functions
	void RequestIRQ(int mask);
	virtual void HandleEvent(uint32_t address, int size) override;

private:
	void InitRegisters();
	void PowerModeStop();
	void PowerModeHalt();

	Memory _memory;
	Cpu _cpu;
	Gpu _gpu;
	
	uint16_t _if;

	bool _running;
	bool _halted;
	bool _stopped;
	Disassembler _disassembler;
};