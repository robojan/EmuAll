#pragma once

#include <stdint.h>
#include <map>

class Gba;

class Cpu {
public:
	enum class Mode {
		User,
		FIQ, 
		IRQ, 
		Supervisor,
		Abort,
		Undefined, 
		System,
		Invalid,
	};

	Cpu(Gba &gba);
	~Cpu();

	void Reset();

	void Tick(bool step = false);

	bool IsInBios() const;
	bool InAPrivilegedMode() const;
	bool InABankedUserRegistersMode() const;
	Mode GetMode() const;
	void SetMode(Mode mode);
	bool IsInThumbMode() const;
	bool IsInArmMode() const;
	void SetThumbMode(bool thumb);
	bool AreFIQsEnabled() const;
	void EnableFIQs(bool enabled);
	bool AreIRQsEnabled() const;
	void EnableIRQs(bool enabled);
	uint32_t GetRegisterValue(int id);

	void UpdateMode();
	void SaveHostFlagsToCPSR();
	void LoadHostFlagsFromCPSR();

	void AddBreakpoint(uint32_t address);
	void RemoveBreakpoint(uint32_t address);
	bool IsBreakpoint(uint32_t address);
	uint32_t GetBreakpointInstruction(uint32_t address);
	bool IsStalled();
private:
	void TickARM(bool step);
	void TickThumb(bool step);
	uint32_t GetShifterOperandLSL(uint32_t instruction);
	uint32_t GetShifterOperandLSLReg(uint32_t instruction);
	uint32_t GetShifterOperandLSR(uint32_t instruction);
	uint32_t GetShifterOperandLSRReg(uint32_t instruction);
	uint32_t GetShifterOperandASR(uint32_t instruction);
	uint32_t GetShifterOperandASRReg(uint32_t instruction);
	uint32_t GetShifterOperandROR(uint32_t instruction);
	uint32_t GetShifterOperandRORReg(uint32_t instruction);
	uint32_t GetShifterOperandImm(uint32_t instruction);
	uint32_t GetShifterOperandLSLFlags(uint32_t instruction);
	uint32_t GetShifterOperandLSLRegFlags(uint32_t instruction);
	uint32_t GetShifterOperandLSRFlags(uint32_t instruction);
	uint32_t GetShifterOperandLSRRegFlags(uint32_t instruction);
	uint32_t GetShifterOperandASRFlags(uint32_t instruction);
	uint32_t GetShifterOperandASRRegFlags(uint32_t instruction);
	uint32_t GetShifterOperandRORFlags(uint32_t instruction);
	uint32_t GetShifterOperandRORRegFlags(uint32_t instruction);
	uint32_t GetShifterOperandImmFlags(uint32_t instruction);

	void SoftwareInterrupt(uint32_t value);
	void IRQ();
	Gba &_system;
	uint32_t _hostFlags;
	uint32_t _registers[16];
	uint32_t _cpsr;
	uint32_t _spsr;
	uint32_t _pipelineInstruction;

	Mode _activeMode;
	uint32_t _registersUser[7];
	uint32_t _registersFIQ[7];
	uint32_t _registersIRQ[2];
	uint32_t _registersABT[2];
	uint32_t _registersUND[2];
	uint32_t _registersSVC[2];
	uint32_t _spsrSVC;
	uint32_t _spsrABT;
	uint32_t _spsrUND;
	uint32_t _spsrIRQ;
	uint32_t _spsrFIQ;

	std::map<uint32_t, uint32_t> _breakpoints;
};