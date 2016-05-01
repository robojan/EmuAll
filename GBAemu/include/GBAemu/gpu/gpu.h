#pragma once

#include <GBAemu/memory/memory.h>

class Gba;

class Gpu : public MemoryEventHandler{
public:
	Gpu(Gba &system);
	~Gpu();

	void Tick();

	virtual void HandleEvent(uint32_t address, int size) override;

private:
	void RegisterEvents();


	Gba &_system;
	uint_fast8_t _vcount;
	uint_fast16_t _hcount;
};