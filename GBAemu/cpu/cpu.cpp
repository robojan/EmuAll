
#include <GBAemu/gba.h>
#include <GBAemu/defines.h>
#include <GBAemu/cpu/cpu.h>
#include <GBAemu/util/log.h>
#include <GBAemu/cpu/hostInstructions.h>

Cpu::Cpu(Gba &gba) :
	_system(gba), _cpsr(0), _spsr(0)
{
	Reset();
}

Cpu::~Cpu()
{

}

void Cpu::Reset()
{
	for (int i = 0; i < 16; i++) {
		_registers[i] = 0;
	}
	SetMode(Supervisor);
	SetThumbMode(false);
	EnableIRQs(false);
	EnableFIQs(false);
	_pipelineInstruction = 0xF0000000;
}

void Cpu::Tick()
{
	// Read instruction
	if (IsInThumbMode()) {
		TickThumb();
	}
	else {
		TickARM();
	}
}

bool Cpu::InAPrivilegedMode() const
{
	return (_cpsr & 0x1F) != 0x10000;
}

bool Cpu::InABankedUserRegistersMode() const
{
	return _activeMode == FIQ;
}

Cpu::Mode Cpu::GetMode() const
{
	switch ((_cpsr & 0x1F)) {
	case 0x10: return User;
	case 0x11: return FIQ;
	case 0x12: return IRQ;
	case 0x13: return Supervisor;
	case 0x17: return Abort;
	case 0x1B: return Undefined;
	case 0x1F: return System;
	default: return Invalid;
	}
}

void Cpu::SetMode(Mode mode)
{
	_cpsr &= ~0x1F;
	switch (mode) {
	case User:
		_cpsr |= 0x10;
		break;
	case FIQ:
		_cpsr |= 0x11;
		break;
	case IRQ:
		_cpsr |= 0x12;
		break;
	case Supervisor:
		_cpsr |= 0x13;
		break;
	case Abort:
		_cpsr |= 0x17;
		break;
	case Undefined:
		_cpsr |= 0x1B;
		break;
	case System:
		_cpsr |= 0x1F;
		break;
	default:
	case Invalid:
		Log(Error, "Tried setting the cpu to an invallid mode");
		_cpsr |= 0x10;
		break;	
	}
	UpdateMode();
}

bool Cpu::IsInThumbMode() const
{
	return (_cpsr & 0x20) != 0;
}

bool Cpu::IsInArmMode() const
{
	return (_cpsr & 0x20) == 0;
}

void Cpu::SetThumbMode(bool thumb)
{
	_cpsr = (_cpsr & ~0x20) | (thumb ? 0x20 : 0x00);
}

bool Cpu::AreFIQsEnabled() const
{
	return (_cpsr & 0x40) == 0;
}

void Cpu::EnableFIQs(bool enabled)
{
	_cpsr = (_cpsr & ~0x40) | (enabled ? 0x00 : 0x40);
}

bool Cpu::AreIRQsEnabled() const
{
	return (_cpsr & 0x80) == 0;
}

void Cpu::EnableIRQs(bool enabled)
{
	_cpsr = (_cpsr & ~0x80) | (enabled ? 0x00 : 0x80);
}

void Cpu::UpdateMode()
{
	Mode newMode = GetMode();
	if (newMode == _activeMode) {
		return;
	}
	switch (_activeMode) {
	default:
	case Invalid:
	case User:
	case System:
		_registersUser[5] = _registers[13];
		_registersUser[6] = _registers[14];
		break;
	case FIQ:
		_registersFIQ[0] = _registers[8];
		_registersFIQ[1] = _registers[9];
		_registersFIQ[2] = _registers[10];
		_registersFIQ[3] = _registers[11];
		_registersFIQ[4] = _registers[12];
		_registersFIQ[5] = _registers[13];
		_registersFIQ[6] = _registers[14];
		for (int i = 8; i <= 12; i++) {
			_registers[i] = _registersUser[i - 8];
		}
		_spsrFIQ = _spsr;
		break;
	case IRQ:
		_registersIRQ[0] = _registers[13];
		_registersIRQ[1] = _registers[14];
		_spsrIRQ = _spsr;
		break;
	case Supervisor:
		_registersSVC[0] = _registers[13];
		_registersSVC[1] = _registers[14];
		_spsrSVC = _spsr;
		break;
	case Abort:
		_registersABT[0] = _registers[13];
		_registersABT[1] = _registers[14];
		_spsrABT = _spsr;
		break;
	case Undefined:
		_registersUND[0] = _registers[13];
		_registersUND[1] = _registers[14];
		_spsrUND = _spsr;
		break;
	}
	switch (newMode) {
	default:
	case Invalid:
	case User:
	case System:
		_registers[13] = _registersUser[5];
		_registers[14] = _registersUser[6];
		break;
	case FIQ:
		_registersUser[0] = _registers[8];
		_registersUser[1] = _registers[9];
		_registersUser[2] = _registers[10];
		_registersUser[3] = _registers[11];
		_registersUser[4] = _registers[12];
		for (int i = 8; i <= 14; i++) {
			_registers[i] = _registersFIQ[i - 8];
		}
		_spsr = _spsrFIQ;
		break;
	case IRQ:
		_registers[13] = _registersIRQ[0];
		_registers[14] = _registersIRQ[1];
		_spsr = _spsrIRQ;
		break;
	case Supervisor:
		_registers[13] = _registersSVC[0];
		_registers[14] = _registersSVC[1];
		_spsr = _spsrSVC;
		break;
	case Abort:
		_registers[13] = _registersABT[0];
		_registers[14] = _registersABT[1];
		_spsr = _spsrABT;
		break;
	case Undefined:
		_registers[13] = _registersUND[0];
		_registers[14] = _registersUND[1];
		_spsr = _spsrUND;
		break;
	}
	_activeMode = newMode;
}

void Cpu::SoftwareInterrupt(uint32_t value)
{
	Log(Debug, "Software interrupt");
}

void Cpu::SaveHostFlagsToCPSR()
{
	_cpsr &= ~(CPSR_C_MASK | CPSR_N_MASK | CPSR_V_MASK | CPSR_Z_MASK);
	if ((_hostFlags & (1 << 0)) != 0) _cpsr |= CPSR_C_MASK;
	if ((_hostFlags & (1 << 6)) != 0) _cpsr |= CPSR_Z_MASK;
	if ((_hostFlags & (1 << 7)) != 0) _cpsr |= CPSR_N_MASK;
	if ((_hostFlags & (1 << 11)) != 0) _cpsr |= CPSR_V_MASK;
}

void Cpu::LoadHostFlagsFromCPSR()
{
	_hostFlags &= ~((1 << 0) | (1 << 6) | (1 << 7) | (1 << 11));
	if ((_cpsr & CPSR_V_MASK) != 0) _hostFlags |= (1 << 11);
	if ((_cpsr & CPSR_C_MASK) != 0) _hostFlags |= (1 << 0);
	if ((_cpsr & CPSR_Z_MASK) != 0) _hostFlags |= (1 << 6);
	if ((_cpsr & CPSR_N_MASK) != 0) _hostFlags |= (1 << 7);
}

