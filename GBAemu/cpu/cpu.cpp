
#include <GBAemu/gba.h>
#include <GBAemu/defines.h>
#include <GBAemu/cpu/cpu.h>
#include <GBAemu/cpu/armException.h>
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
	SetMode(Mode::Supervisor);
	SetThumbMode(false);
	EnableIRQs(false);
	EnableFIQs(false);
	_pipelineInstruction = ARM_NOP;
}

void Cpu::Tick(bool step)
{
	// Read instruction
	try {
		if (AreIRQsEnabled()) {
			uint8_t *registers = _system.GetMemory().GetRegisters();
			if ((IOREG32(registers, IME) & 1) != 0 && (IOREG16(registers, IE) & IOREG16(registers, IF) & IRQ_MASK) != 0) {
				IRQ();
			}
		}

		if (IsInThumbMode()) {
			TickThumb(step);
		}
		else {
			TickARM(step);
		}
	}
	catch (BreakPointARMException &e) {
		if (IsInThumbMode()) {
			_registers[REGPC] -= 4;
		}
		else {
			_registers[REGPC] -= 8;
		}
		_pipelineInstruction = ARM_NOP;
		throw e;
	}
	catch (DataAbortARMException &e) {
		Log(Error, "Data Abort ARM Exception at 0x%08x", _registers[REGPC]);
		throw e;
	}
}

bool Cpu::InAPrivilegedMode() const
{
	return (_cpsr & 0x1F) != 0x10000;
}

bool Cpu::InABankedUserRegistersMode() const
{
	return _activeMode == Mode::FIQ;
}

Cpu::Mode Cpu::GetMode() const
{
	switch ((_cpsr & 0x1F)) {
	case 0x10: return Mode::User;
	case 0x11: return Mode::FIQ;
	case 0x12: return Mode::IRQ;
	case 0x13: return Mode::Supervisor;
	case 0x17: return Mode::Abort;
	case 0x1B: return Mode::Undefined;
	case 0x1F: return Mode::System;
	default: return Mode::Invalid;
	}
}

void Cpu::SetMode(Mode mode)
{
	_cpsr &= ~0x1F;
	switch (mode) {
	case Mode::User:
		_cpsr |= 0x10;
		break;
	case Mode::FIQ:
		_cpsr |= 0x11;
		break;
	case Mode::IRQ:
		_cpsr |= 0x12;
		break;
	case Mode::Supervisor:
		_cpsr |= 0x13;
		break;
	case Mode::Abort:
		_cpsr |= 0x17;
		break;
	case Mode::Undefined:
		_cpsr |= 0x1B;
		break;
	case Mode::System:
		_cpsr |= 0x1F;
		break;
	default:
	case Mode::Invalid:
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

uint32_t Cpu::GetRegisterValue(int id)
{
	if (id >= 0 && id < 16) {
		return _registers[id];
	}
	else if (id == 16) {
		return _cpsr;
	}
	else if (id == 17) {
		return _spsr;
	}
	return 0;
}

void Cpu::UpdateMode()
{
	Mode newMode = GetMode();
	if (newMode == _activeMode) {
		return;
	}
	switch (_activeMode) {
	default:
	case Mode::Invalid:
	case Mode::User:
	case Mode::System:
		_registersUser[5] = _registers[13];
		_registersUser[6] = _registers[14];
		break;
	case Mode::FIQ:
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
	case Mode::IRQ:
		_registersIRQ[0] = _registers[13];
		_registersIRQ[1] = _registers[14];
		_spsrIRQ = _spsr;
		break;
	case Mode::Supervisor:
		_registersSVC[0] = _registers[13];
		_registersSVC[1] = _registers[14];
		_spsrSVC = _spsr;
		break;
	case Mode::Abort:
		_registersABT[0] = _registers[13];
		_registersABT[1] = _registers[14];
		_spsrABT = _spsr;
		break;
	case Mode::Undefined:
		_registersUND[0] = _registers[13];
		_registersUND[1] = _registers[14];
		_spsrUND = _spsr;
		break;
	}
	switch (newMode) {
	default:
	case Mode::Invalid:
	case Mode::User:
	case Mode::System:
		_registers[13] = _registersUser[5];
		_registers[14] = _registersUser[6];
		break;
	case Mode::FIQ:
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
	case Mode::IRQ:
		_registers[13] = _registersIRQ[0];
		_registers[14] = _registersIRQ[1];
		_spsr = _spsrIRQ;
		break;
	case Mode::Supervisor:
		_registers[13] = _registersSVC[0];
		_registers[14] = _registersSVC[1];
		_spsr = _spsrSVC;
		break;
	case Mode::Abort:
		_registers[13] = _registersABT[0];
		_registers[14] = _registersABT[1];
		_spsr = _spsrABT;
		break;
	case Mode::Undefined:
		_registers[13] = _registersUND[0];
		_registers[14] = _registersUND[1];
		_spsr = _spsrUND;
		break;
	}
	_activeMode = newMode;
}

void Cpu::SoftwareInterrupt(uint32_t value)
{
	SaveHostFlagsToCPSR();
	_spsrSVC = _cpsr;
	uint32_t returnAddress = IsInThumbMode() ? _registers[REGPC] - 2 : _registers[REGPC] - 4;
	SetThumbMode(false);
	EnableIRQs(false);
	SetMode(Mode::Supervisor);
	_registers[REGLR] = returnAddress;
	_registers[REGPC] = 0x00000008;
	_pipelineInstruction = ARM_NOP;
	Log(Debug, "Software interrupt %d", value);
}

void Cpu::IRQ()
{
	SaveHostFlagsToCPSR();
	_spsrIRQ = _cpsr;
	uint32_t returnAddress = _registers[REGPC] + 4 + (IsInThumbMode() ? -2 : -4);
	SetThumbMode(false);
	EnableIRQs(false);
	SetMode(Mode::IRQ);
	_registers[REGLR] = returnAddress;
	_registers[REGPC] = 0x00000018;
	_pipelineInstruction = ARM_NOP;
}

uint32_t Cpu::GetBreakpointInstruction(uint32_t address)
{
	return _breakpoints.at(address);
}

bool Cpu::IsStalled()
{
	if (IsInThumbMode()) {
		return (_pipelineInstruction & 0xFFFF) == THUMB_NOP;
	}
	else {
		return _pipelineInstruction == ARM_NOP;
	}
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

void Cpu::AddBreakpoint(uint32_t address)
{
	if (IsBreakpoint(address)) {
		RemoveBreakpoint(address);
	}
	uint32_t instr = _system.GetMemory().Read32(address);
	_system.GetMemory().ManagedWrite32(address, 0xE120BE70);
	_breakpoints[address] = instr;
}

void Cpu::RemoveBreakpoint(uint32_t address)
{
	if (!IsBreakpoint(address)) {
		return;
	}
	_system.GetMemory().ManagedWrite32(address, _breakpoints[address]);
	_breakpoints.erase(address);
}

bool Cpu::IsBreakpoint(uint32_t address)
{
	return _breakpoints.find(address) != _breakpoints.end();
}

