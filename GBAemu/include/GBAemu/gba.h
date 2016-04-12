#pragma once

#include <emu.h>

#include <GBAemu/memory/memory.h>
#include <GBAemu/cpu/cpu.h>

class Gba {
	friend class Cpu;
public:
	Gba();
	~Gba();

	int Init();
	int Load(const SaveData_t *data);

	// Running functions
	int Tick(unsigned int time);
	void Run(bool run = true);
	bool IsRunning() const;

	// Graphics functions
	void Draw(int id);
private:
	Memory _memory;
	Cpu _cpu;

	bool _running;
};
