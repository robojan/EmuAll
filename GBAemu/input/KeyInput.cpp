#include <GBAemu/input/KeyInput.h>
#include <GBAemu/gba.h>
#include <GBAemu/defines.h>


KeyInput::KeyInput(Gba &gba) : _system(gba), _state(0xFFFF)
{
	_system.GetMemory().RegisterEvent(KEYINPUT, this);
	IOREG16(_system.GetMemory().GetRegisters(), KEYINPUT) = _state;
}

KeyInput::~KeyInput()
{

}

void KeyInput::HandleEvent(uint32_t address, int size)
{
	if (address <= KEYINPUT + 1 && KEYINPUT < address + size) {
		IOREG16(_system.GetMemory().GetRegisters(), KEYINPUT) = _state;
	}
	if (address <= KEYCNT + 1 && KEYCNT < address + size) {
		IOREG16(_system.GetMemory().GetRegisters(), KEYINPUT) = _state;
	}
}

void KeyInput::OnKeyPressed(int id, bool pressed)
{
	int button = (1 << id);
	int buttonState = (pressed ? 0 : 1) << id;
	uint16_t newState = (_state & ~button) | buttonState;
	newState |= 0xFC00;
	_state = newState;
	if (IsIRQ()) {
		_system.RequestIRQ(IRQ_KEYPAD);
	}
	IOREG16(_system.GetMemory().GetRegisters(), KEYINPUT) = _state;
}

bool KeyInput::IsIRQ()
{
	uint16_t keycnt = IOREG16(_system.GetMemory().GetRegisters(), KEYCNT);
	if ((keycnt & (1 << 14)) == 0) return false;
	if ((keycnt &(1 << 15)) != 0) {
		// AND
		return ((~_state) & keycnt & 0x3FF) == (keycnt & 0x3FF);
	}
	else {
		// OR
		return ((~_state) & keycnt & 0x3FF) != 0;
	}
}
