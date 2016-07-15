#pragma once

#include <emu.h>

#include <GBAemu/memory/memory.h>
#include <GBAemu/cpu/cpu.h>
#include <GBAemu/cpu/disassembler.h>
#include <GBAemu/gpu/gpu.h>
#include <GBAemu/input/KeyInput.h>

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
	KeyInput &GetInput() { return _input; }
	Disassembler &GetDisassembler() { return _disassembler; }

	// execution functions
	void RequestIRQ(int mask);
	virtual void HandleEvent(uint32_t address, int size) override;

	// input functions
	void OnKey(int id, bool pressed);

private:
	void InitRegisters();
	void PowerModeStop();
	void PowerModeHalt();

	void TimerTick();

	Memory _memory;
	Cpu _cpu;
	Gpu _gpu;
	KeyInput _input;
	
	uint16_t _if;

	bool _running;
	bool _halted;
	bool _stopped;
	Disassembler _disassembler;

	struct {
		uint_fast16_t prescaler;
		uint_fast16_t cnt;
		uint_fast16_t prescalerMatch;
		uint_fast16_t val;
		uint_fast16_t reload;
	} _timerInfo[4];

};
