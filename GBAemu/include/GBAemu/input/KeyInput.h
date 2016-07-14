#ifndef INPUT_H_
#define INPUT_H_

#include <GBAemu/memory/memory.h>
#include <stdint.h>

class Gba;

class KeyInput : public MemoryEventHandler {
public:
	KeyInput(Gba &gba);
	~KeyInput();

	virtual void HandleEvent(uint32_t address, int size) override;

	void OnKeyPressed(int id, bool pressed);
	bool IsIRQ();

private:
	Gba &_system;
	uint16_t _state;
};

#endif
