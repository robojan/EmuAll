
#include <GBAemu/cpu/hostInstructions.h>
#include <GBAemu/cpu/cpu.h>
#include <GBAemu/cpu/armException.h>
#include <GBAemu/gba.h>
#include <GBAemu/util/log.h>
#include <GBAemu/defines.h>
#include <GBAemu/util/preprocessor.h>

// 001xxxxxxxxx
uint32_t Cpu::GetShifterOperandImm(uint32_t instruction)
{
	uint32_t operand = instruction & 0xFF;
	uint8_t rotateImm = (instruction >> 8) & 0xF;
	if (rotateImm == 0) {
		return operand;
	}
	else {
		ROR(operand, rotateImm * 2, operand);
	}
	return operand;
}

uint32_t Cpu::GetShifterOperandLSL(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t shift = (instruction >> 7) & 0x1F;
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		LSL(_registers[rm], shift, operand);
		return operand;
	}
}

uint32_t Cpu::GetShifterOperandLSLReg(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t rs = (instruction >> 8) & 0xF;
	uint32_t shift = _registers[rs];
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		LSL(_registers[rm], shift, operand);
		return operand;
	}
}

uint32_t Cpu::GetShifterOperandLSR(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t shift = (instruction >> 7) & 0x1F;
	if (shift == 0) {
		shift = 32;
	}
	uint32_t operand;
	LSR(_registers[rm], shift, operand);
	return operand;
}

uint32_t Cpu::GetShifterOperandLSRReg(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t rs = (instruction >> 8) & 0xF;
	uint8_t shift = _registers[rs];
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		LSR(_registers[rm], shift, operand);
		return operand;
	}
}

uint32_t Cpu::GetShifterOperandASR(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t shift = (instruction >> 7) & 0x1F;
	if (shift == 0) {
		shift = 32;
	}
	uint32_t operand;
	ASR(_registers[rm], shift, operand);
	return operand;
}

uint32_t Cpu::GetShifterOperandASRReg(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t rs = (instruction >> 8) & 0xF;
	uint8_t shift = _registers[rs];
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		ASR(_registers[rm], shift, operand);
		return operand;
	}
}

uint32_t Cpu::GetShifterOperandROR(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t shift = (instruction >> 7) & 0x1F;
	uint32_t operand;
	if (shift == 0) {
		RRX(_registers[rm], 1, operand, _hostFlags);
	}
	else {
		ROR(_registers[rm], shift, operand);
	}
	return operand;
}

uint32_t Cpu::GetShifterOperandRORReg(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t rs = (instruction >> 8) & 0xF;
	uint8_t shift = _registers[rs];
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		ROR(_registers[rm], shift, operand);
		return operand;
	}
}
// 001xxxxxxxxx
uint32_t Cpu::GetShifterOperandImmFlags(uint32_t instruction)
{
	uint32_t operand = instruction & 0xFF;
	uint8_t rotateImm = (instruction >> 8) & 0xF;
	if (rotateImm == 0) {
		return operand;
	}
	else {
		ROR_CFLAG(operand, rotateImm * 2, operand, _hostFlags);
	}
	return operand;
}

uint32_t Cpu::GetShifterOperandLSLFlags(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t shift = (instruction >> 7) & 0x1F;
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		LSL_CFLAG(_registers[rm], shift, operand, _hostFlags);
		return operand;
	}
}

uint32_t Cpu::GetShifterOperandLSLRegFlags(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t rs = (instruction >> 8) & 0xF;
	uint32_t shift = _registers[rs];
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		LSL_CFLAG(_registers[rm], shift, operand, _hostFlags);
		return operand;
	}
}

uint32_t Cpu::GetShifterOperandLSRFlags(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t shift = (instruction >> 7) & 0x1F;
	if (shift == 0) {
		shift = 32;
	}
	uint32_t operand;
	LSR_CFLAG(_registers[rm], shift, operand, _hostFlags);
	return operand;
}

uint32_t Cpu::GetShifterOperandLSRRegFlags(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t rs = (instruction >> 8) & 0xF;
	uint8_t shift = _registers[rs];
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		LSR_CFLAG(_registers[rm], shift, operand, _hostFlags);
		return operand;
	}
}

uint32_t Cpu::GetShifterOperandASRFlags(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t shift = (instruction >> 7) & 0x1F;
	if (shift == 0) {
		shift = 32;
	}
	uint32_t operand;
	ASR_CFLAG(_registers[rm], shift, operand, _hostFlags);
	return operand;
}

uint32_t Cpu::GetShifterOperandASRRegFlags(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t rs = (instruction >> 8) & 0xF;
	uint8_t shift = _registers[rs];
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		ASR_CFLAG(_registers[rm], shift, operand, _hostFlags);
		return operand;
	}
}

uint32_t Cpu::GetShifterOperandRORFlags(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t shift = (instruction >> 7) & 0x1F;
	uint32_t operand;
	if (shift == 0) {
		RRX_CFLAG(_registers[rm], 1, operand, _hostFlags);
	}
	else {
		ROR_CFLAG(_registers[rm], shift, operand, _hostFlags);
	}
	return operand;
}

uint32_t Cpu::GetShifterOperandRORRegFlags(uint32_t instruction)
{
	uint8_t rm = instruction & 0xF;
	uint8_t rs = (instruction >> 8) & 0xF;
	uint8_t shift = _registers[rs];
	if (shift == 0) {
		return _registers[rm];
	}
	else {
		uint32_t operand;
		ROR_CFLAG(_registers[rm], shift, operand, _hostFlags);
		return operand;
	}
}

void Cpu::TickARM(bool step) {
	uint32_t instruction = _pipelineInstruction;
	_pipelineInstruction = _system._memory.Read32(_registers[REGPC] & 0xFFFFFFFC);
	if (step && IsBreakpoint(_registers[REGPC] - 4)) {
		instruction = _breakpoints.at(_registers[REGPC] - 4);
	}
	_registers[REGPC] += 4;
	uint8_t cond = instruction >> 28;
	if (!IsConditionMet(cond, _hostFlags)) {
		return;
	}
	uint16_t code = ((instruction >> 4) & 0xF) | (((instruction >> 20) & 0xFF) << 4);
	switch (code) {
	// ADC
	case 0x0A8:
	case 0x0A0: {// ADC <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0A1: { // ADC <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0AA:
	case 0x0A2: { // ADC <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0A3: { // ADC <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0AC:
	case 0x0A4: { // ADC <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0A5: { // ADC <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0AE:
	case 0x0A6: { // ADC <Rd>, <Rn>, <Rm>, ROR #<imm>
		// ADC <Rd>, <Rn>, <Rm>, RRX
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0A7: { // ADC <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0B8:
	case 0x0B0: {// ADCS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLFlags(instruction);
		if (rd == REGPC) {
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0B1: { // ADCS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0BA:
	case 0x0B2: { // ADCS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0B3: { // ADCS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0BC:
	case 0x0B4: { // ADCS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0B5: { // ADCS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0BE:
	case 0x0B6: { // ADCS <Rd>, <Rn>, <Rm>, ROR #<imm>
				  // ADCS <Rd>, <Rn>, <Rm>, RRX
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0B7: { // ADCS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x2A0) { // ADC <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		ADC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	CASE_RANGE16(0x2B0) { // ADC <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			ADC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			ADC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	// ADD
	case 0x080:
	case 0x088: { // ADD <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x081: { // ADD <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x082:
	case 0x08A: { // ADD <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x083: { // ADD <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x084:
	case 0x08C: { // ADD <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x085: { // ADD <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x086: // ADD <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x08E: { // ADD <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x087: { // ADD <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x090:
	case 0x098: { // ADDS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x091: { // ADDS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x092:
	case 0x09A: { // ADDS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x093: { // ADDS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x094:
	case 0x09C: { // ADDS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x095: { // ADDS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x096: // ADDS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x09E: { // ADDS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x097: { // ADDS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x280) { // ADD <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		ADD(_registers[rn], operand, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x290) { // ADDS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			ADD(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			ADD_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	// AND
	case 0x000:
	case 0x008: { // AND <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x001: { // AND <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x002:
	case 0x00A: { // AND <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x003: { // AND <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x004:
	case 0x00C: { // AND <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x005: { // AND <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x006: // AND <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x00E: { // AND <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x007: { // AND <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x010:
	case 0x018: { // ANDS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x011: { // ANDS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x012:
	case 0x01A: { // ANDS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x013: { // ANDS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x014:
	case 0x01C: { // ANDS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x015: { // ANDS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x016: // ANDS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x01E: { // ANDS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x017: { // ANDS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x200) { // AND <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		AND(_registers[rn], operand, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x210) { // ANDS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			AND(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			AND_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	// B
	CASE_RANGE256(0xA00) { // B <target_address>
		uint32_t address = instruction & 0xFFFFFF;
		if ((address & 0x00800000) != 0) address |= 0xFF000000;
		address <<= 2;
		_registers[REGPC] += address;
		_pipelineInstruction = ARM_NOP;
		break;
	}
	// BL
	CASE_RANGE256(0xB00) { // BL <target_address>
		uint32_t address = instruction & 0xFFFFFF;
		if ((address & 0x00800000) != 0) address |= 0xFF000000;
		address <<= 2;
		_registers[REGLR] = _registers[REGPC] - 4;
		_registers[REGPC] += address;
		_pipelineInstruction = ARM_NOP;
		break;
	}
	// BIC
	case 0x1C0:
	case 0x1C8: { // BIC <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x1C1: { // BIC <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x1C2:
	case 0x1CA: { // BIC <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x1C3: { // BIC <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x1C4:
	case 0x1CC: { // BIC <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x1C5: { // BIC <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x1C6: // BIC <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x1CE: { // BIC <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x1C7: { // BIC <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x1D0:
	case 0x1D8: { // BICS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x1D1: { // BICS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x1D2:
	case 0x1DA: { // BICS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x1D3: { // BICS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x1D4:
	case 0x1DC: { // BICS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x1D5: { // BICS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x1D6: // BICS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x1DE: { // BICS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x1D7: { // BICS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x3C0) { // BIC <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		BIC(_registers[rn], operand, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x3D0) { // BICS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			BIC(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			BIC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	// BKPT
	case 0x127: { // BKPT <imm_16>
		uint16_t imm = instruction & 0xF;
		imm |= (instruction >> 4) & 0xFFF0;
		throw BreakPointARMException(imm);
		break;
	}
	// BX
	case 0x121: { // BX <Rm>
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rm];
		SetThumbMode((address & 0x1) != 0);
		_registers[REGPC] = address & 0xFFFFFFFE;
		break;
	}
	CASE_RANGE128_OFFSET(0xE00, 1) { // CDP <coproc>, <opcode_1>, <CRd>, <CRn>, <CRm>, <opcode_2>
		Log(Warn, "Coprocessor instruction found. Not supported.");
		__debugbreak();
		break;
	}
	// CMN
	case 0x170:
	case 0x178: { // CMN <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSLFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x171: { // CMN <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x172:
	case 0x17A: { // CMN <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSRFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x173: { // CMN <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x174:
	case 0x17C: { // CMN <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandASRFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x175: { // CMN <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandASRRegFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x176: // CMN <Rn>, <Rm>, RRX #<imm>
	case 0x17E: { // CMN <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandRORFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x177: { // CMN <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORRegFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	CASE_RANGE16(0x370) { // CMN <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandImmFlags(instruction);
		CMN(_registers[rn], operand, _hostFlags);
		break;
	}
	// CMP
	case 0x150:
	case 0x158: { // CMP <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSLFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x151: { // CMP <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x152:
	case 0x15A: { // CMP <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSRFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x153: { // CMP <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x154:
	case 0x15C: { // CMP <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandASRFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x155: { // CMP <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandASRRegFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x156: // CMP <Rn>, <Rm>, RRX #<imm>
	case 0x15E: { // CMP <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandRORFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x157: { // CMP <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandRORRegFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	CASE_RANGE16(0x350) { // CMP <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandImmFlags(instruction);
		CMP(_registers[rn], operand, _hostFlags);
		break;
	}
	// EOR
	case 0x020:
	case 0x028: { // EOR <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x021: { // EOR <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x022:
	case 0x02A: { // EOR <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x023: { // EOR <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x024:
	case 0x02C: { // EOR <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x025: { // EOR <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x026: // EOR <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x02E: { // EOR <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x027: { // EOR <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x030:
	case 0x038: { // EORS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x031: { // EORS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x032:
	case 0x03A: { // EORS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x033: { // EORS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x034:
	case 0x03C: { // EORS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x035: { // EORS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x036: // EORS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x03E: { // EORS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x037: { // EORS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x220) { // EOR <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		EOR(_registers[rn], operand, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x230) { // EORS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			EOR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			EOR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	// LDM
	CASE_RANGE16(0x810) { // LDMDA <Rn>, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if ((registerList & (1 << 15)) != 0) {
			address -= 4;
			_registers[REGPC] = _system._memory.Read32(address) & 0xFFFFFFFC;
			_pipelineInstruction = ARM_NOP;
		}
		for (int i = 14; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				address -= 4;
				_registers[i] = _system._memory.Read32(address);
			}
		}
		break;
	}
	CASE_RANGE16(0x830) { // LDMDA <Rn>!, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if ((registerList & (1 << 15)) != 0) {
			address -= 4;
			_registers[REGPC] = _system._memory.Read32(address) & 0xFFFFFFFC;
		}
		for (int i = 14; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				address -= 4;
				_registers[i] = _system._memory.Read32(address);
			}
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x890) { // LDMIA <Rn>, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 0; i <= 14; i++) {
			if ((registerList & (1 << i)) != 0) {
				address += 4;
				_registers[i] = _system._memory.Read32(address);
			}
		}
		if ((registerList & (1 << 15)) != 0) {
			address += 4;
			_registers[REGPC] = _system._memory.Read32(address) & 0xFFFFFFFC;
		}
		break;
	}
	CASE_RANGE16(0x8B0) { // LDMIA <Rn>!, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 0; i <= 14; i++) {
			if ((registerList & (1 << i)) != 0) {
				address += 4;
				_registers[i] = _system._memory.Read32(address);
			}
		}
		if ((registerList & (1 << 15)) != 0) {
			address += 4;
			_registers[REGPC] = _system._memory.Read32(address) & 0xFFFFFFFC;
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x910) { // LDMDB <Rn>, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if ((registerList & (1 << 15)) != 0) {
			_registers[REGPC] = _system._memory.Read32(address) & 0xFFFFFFFC;
			address -= 4;
		}
		for (int i = 14; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				_registers[i] = _system._memory.Read32(address);
				address -= 4;
			}
		}
		break;
	}
	CASE_RANGE16(0x930) { // LDMDB <Rn>!, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if ((registerList & (1 << 15)) != 0) {
			_registers[REGPC] = _system._memory.Read32(address) & 0xFFFFFFFC;
			address -= 4;
		}
		for (int i = 14; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				_registers[i] = _system._memory.Read32(address);
				address -= 4;
			}
		}
		_registers[rn] = address + 4;
		break;
	}
	CASE_RANGE16(0x990) { // LDMIB <Rn>, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 0; i >= 14; i++) {
			if ((registerList & (1 << i)) != 0) {
				_registers[i] = _system._memory.Read32(address);
				address += 4;
			}
		}
		if ((registerList & (1 << 15)) != 0) {
			_registers[REGPC] = _system._memory.Read32(address) & 0xFFFFFFFC;
			address += 4;
		}
		break;
	}
	CASE_RANGE16(0x9B0) { // LDMIB <Rn>!, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 0; i <= 14; i++) {
			if ((registerList & (1 << i)) != 0) {
				_registers[i] = _system._memory.Read32(address);
				address += 4;
			}
		}
		if ((registerList & (1 << 15)) != 0) {
			_registers[REGPC] = _system._memory.Read32(address) & 0xFFFFFFFC;
			address += 4;
		}
		_registers[rn] = address - 4;
		break;
	}
	CASE_RANGE16(0x850) { // LDMDA <Rn>, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if ((registerList & (1 << 15)) != 0) {
				address -= 4;
				_cpsr = _spsr;
				uint32_t pc = _system._memory.Read32(address);
				UpdateMode();
				if (IsInThumbMode()) {
					_registers[15] = pc & 0xFFFFFFFE;
				}
				else {
					_registers[15] = pc & 0xFFFFFFFC;
				}
			}
			for (int i = 14; i >= 8; i--) {
				if ((registerList & (1 << i)) != 0) {
					address -= 4;
					_registersUser[i - 8] = _system._memory.Read32(address);
				}
			}
			for (int i = 7; i >= 0; i--) {
				address -= 4;
				_registers[i] = _system._memory.Read32(address);
			}
		}
		else {
			for (int i = 15; i >= 0; i--) {
				if ((registerList & (1 << i)) != 0) {
					address -= 4;
					_registers[i] = _system._memory.Read32(address);
				}
			}
		}
		break;
	}
	CASE_RANGE16(0x870) { // LDMDA <Rn>!, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if ((registerList & (1 << 15)) != 0) {
				address -= 4;
				_cpsr = _spsr;
				uint32_t pc = _system._memory.Read32(address);
				UpdateMode();
				if (IsInThumbMode()) {
					_registers[15] = pc & 0xFFFFFFFE;
				}
				else {
					_registers[15] = pc & 0xFFFFFFFC;
				}
			}
			for (int i = 14; i >= 8; i--) {
				if ((registerList & (1 << i)) != 0) {
					address -= 4;
					_registersUser[i - 8] = _system._memory.Read32(address);
				}
			}
			for (int i = 7; i >= 0; i--) {
				address -= 4;
				_registers[i] = _system._memory.Read32(address);
			}
		}
		else {
			for (int i = 15; i >= 0; i--) {
				if ((registerList & (1 << i)) != 0) {
					address -= 4;
					_registers[i] = _system._memory.Read32(address);
				}
			}
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x8D0) { // LDMIA <Rn>!, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			for (int i = 0; i <= 7; i++) {
				address += 4;
				_registers[i] = _system._memory.Read32(address);
			}
			for (int i = 8; i <= 14; i++) {
				if ((registerList & (1 << i)) != 0) {
					address += 4;
					_registersUser[i - 8] = _system._memory.Read32(address);
				}
			}
			if ((registerList & (1 << 15)) != 0) {
				address += 4;
				_cpsr = _spsr;
				uint32_t pc = _system._memory.Read32(address);
				UpdateMode();
				if (IsInThumbMode()) {
					_registers[15] = pc & 0xFFFFFFFE;
				}
				else {
					_registers[15] = pc & 0xFFFFFFFC;
				}
			}
		}
		else {
			for (int i = 0; i <= 14; i++) {
				if ((registerList & (1 << i)) != 0) {
					address += 4;
					_registers[i] = _system._memory.Read32(address);
				}
			}
			if ((registerList & (1 << 15)) != 0) {
				address += 4;
				uint32_t pc = _system._memory.Read32(address);
				_registers[15] = pc & 0xFFFFFFFC;
			}
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x8F0) { // LDMIA <Rn>, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			for (int i = 0; i <= 7; i++) {
				address += 4;
				_registers[i] = _system._memory.Read32(address);
			}
			for (int i = 8; i <= 14; i++) {
				if ((registerList & (1 << i)) != 0) {
					address += 4;
					_registersUser[i - 8] = _system._memory.Read32(address);
				}
			}
			if ((registerList & (1 << 15)) != 0) {
				address += 4;
				_cpsr = _spsr;
				uint32_t pc = _system._memory.Read32(address);
				UpdateMode();
				if (IsInThumbMode()) {
					_registers[15] = pc & 0xFFFFFFFE;
				}
				else {
					_registers[15] = pc & 0xFFFFFFFC;
				}
			}
		}
		else {
			for (int i = 0; i <= 14; i++) {
				if ((registerList & (1 << i)) != 0) {
					address += 4;
					_registers[i] = _system._memory.Read32(address);
				}
			}
			if ((registerList & (1 << 15)) != 0) {
				address += 4;
				uint32_t pc = _system._memory.Read32(address);
				_registers[15] = pc & 0xFFFFFFFC;
			}
		}
		break;
	}
	CASE_RANGE16(0x950) { // LDMDB <Rn>, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if ((registerList & (1 << 15)) != 0) {
				_cpsr = _spsr;
				uint32_t pc = _system._memory.Read32(address);
				UpdateMode();
				if (IsInThumbMode()) {
					_registers[15] = pc & 0xFFFFFFFE;
				}
				else {
					_registers[15] = pc & 0xFFFFFFFC;
				}
				address -= 4;
			}
			for (int i = 14; i >= 8; i--) {
				if ((registerList & (1 << i)) != 0) {
					_registersUser[i - 8] = _system._memory.Read32(address);
					address -= 4;
				}
			}
			for (int i = 7; i >= 0; i--) {
				_registers[i] = _system._memory.Read32(address);
				address -= 4;
			}
		}
		else {
			if ((registerList & (1 << 15)) != 0) {
				uint32_t pc = _system._memory.Read32(address);
				_registers[15] = pc & 0xFFFFFFFC;
				address -= 4;
			}
			for (int i = 14; i >= 0; i--) {
				if ((registerList & (1 << i)) != 0) {
					_registers[i] = _system._memory.Read32(address);
					address -= 4;
				}
			}
		}
		break;
	}
	CASE_RANGE16(0x970) { // LDMDB <Rn>!, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if ((registerList & (1 << 15)) != 0) {
				_cpsr = _spsr;
				uint32_t pc = _system._memory.Read32(address);
				UpdateMode();
				if (IsInThumbMode()) {
					_registers[15] = pc & 0xFFFFFFFE;
				}
				else {
					_registers[15] = pc & 0xFFFFFFFC;
				}
				address -= 4;
			}
			for (int i = 14; i >= 8; i--) {
				if ((registerList & (1 << i)) != 0) {
					_registersUser[i - 8] = _system._memory.Read32(address);
					address -= 4;
				}
			}
			for (int i = 7; i >= 0; i--) {
				_registers[i] = _system._memory.Read32(address);
				address -= 4;
			}
		}
		else {
			if ((registerList & (1 << 15)) != 0) {
				uint32_t pc = _system._memory.Read32(address);
				_registers[15] = pc & 0xFFFFFFFC;
				address -= 4;
			}
			for (int i = 14; i >= 0; i--) {
				if ((registerList & (1 << i)) != 0) {
					_registers[i] = _system._memory.Read32(address);
					address -= 4;
				}
			}
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x9D0) { // LDMIB <Rn>, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			for (int i = 0; i <= 7; i++) {
				_registers[i] = _system._memory.Read32(address);
				address += 4;
			}
			for (int i = 8; i <= 14; i++) {
				if ((registerList & (1 << i)) != 0) {
					_registersUser[i - 8] = _system._memory.Read32(address);
					address += 4;
				}
			}
			if ((registerList & (1 << 15)) != 0) {
				_cpsr = _spsr;
				uint32_t pc = _system._memory.Read32(address);
				UpdateMode();
				if (IsInThumbMode()) {
					_registers[15] = pc & 0xFFFFFFFE;
				}
				else {
					_registers[15] = pc & 0xFFFFFFFC;
				}
				address += 4;
			}
		}
		else {
			for (int i = 0; i <= 14; i++) {
				if ((registerList & (1 << i)) != 0) {
					_registers[i] = _system._memory.Read32(address);
					address += 4;
				}
			}
			if ((registerList & (1 << 15)) != 0) {
				uint32_t pc = _system._memory.Read32(address);
				_registers[15] = pc & 0xFFFFFFFC;
				address += 4;
			}
		}
		break;
	}
	CASE_RANGE16(0x9F0) { // LDMIB <Rn>!, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			for (int i = 0; i <= 7; i++) {
				_registers[i] = _system._memory.Read32(address);
				address += 4;
			}
			for (int i = 8; i <= 14; i++) {
				if ((registerList & (1 << i)) != 0) {
					_registersUser[i - 8] = _system._memory.Read32(address);
					address += 4;
				}
			}
			if ((registerList & (1 << 15)) != 0) {
				_cpsr = _spsr;
				uint32_t pc = _system._memory.Read32(address);
				UpdateMode();
				if (IsInThumbMode()) {
					_registers[15] = pc & 0xFFFFFFFE;
				}
				else {
					_registers[15] = pc & 0xFFFFFFFC;
				}
				address += 4;
			}
		}
		else {
			for (int i = 0; i <= 14; i++) {
				if ((registerList & (1 << i)) != 0) {
					_registers[i] = _system._memory.Read32(address);
					address += 4;
				}
			}
			if ((registerList & (1 << 15)) != 0) {
				uint32_t pc = _system._memory.Read32(address);
				_registers[15] = pc & 0xFFFFFFFC;
				address += 4;
			}
		}
		_registers[rn] = address;
		break;
	}
	// LDR
	CASE_RANGE16(0x410) { // LDR <Rd>, [<Rn>], #-<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address - offset;
		break;
	}
	CASE_RANGE16(0x490) { // LDR <Rd>, [<Rn>], #+<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address + offset;
		break;
	}
	CASE_RANGE16(0x510) { // LDR <Rd>, [<Rn>, #-<offset_12>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	CASE_RANGE16(0x530) { // LDR <Rd>, [<Rn>, #-<offset_12>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x590) { // LDR <Rd>, [<Rn>, #+<offset_12>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	CASE_RANGE16(0x5B0) { // LDR <Rd>, [<Rn>, #+<offset_12>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	case 0x610: 
	case 0x618:{ // LDR <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x612:
	case 0x61A: { // LDR <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x614:
	case 0x61C: { // LDR <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x616:
	case 0x61E: { // LDR <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x690:
	case 0x698: { // LDR <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x692:
	case 0x69A: { // LDR <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x694:
	case 0x69C: { // LDR <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x696:
	case 0x69E: { // LDR <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x710:
	case 0x718: { // LDR <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x712:
	case 0x71A: { // LDR <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x714:
	case 0x71C: { // LDR <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x716:
	case 0x71E: { // LDR <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x790:
	case 0x798: { // LDR <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x792:
	case 0x79A: { // LDR <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x794:
	case 0x79C: { // LDR <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x796:
	case 0x79E: { // LDR <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x730:
	case 0x738: { // LDR <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	case 0x732:
	case 0x73A: { // LDR <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	case 0x734:
	case 0x73C: { // LDR <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	case 0x736:
	case 0x73E: { // LDR <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	case 0x7B0:
	case 0x7B8: { // LDR <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	case 0x7B2:
	case 0x7BA: { // LDR <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	case 0x7B4:
	case 0x7BC: { // LDR <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	case 0x7B6:
	case 0x7BE: { // LDR <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read32(address);
		_registers[rn] = address;
		break;
	}
	// LDRB
	CASE_RANGE16(0x450) { // LDRB <Rd>, [<Rn>], #-<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address - offset;
		break;
	}
	CASE_RANGE16(0x4D0) { // LDRB <Rd>, [<Rn>], #+<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address + offset;
		break;
	}
	CASE_RANGE16(0x550) { // LDRB <Rd>, [<Rn>, #-<offset_12>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	CASE_RANGE16(0x570) { // LDRB <Rd>, [<Rn>, #-<offset_12>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x5D0) { // LDRB <Rd>, [<Rn>, #+<offset_12>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	CASE_RANGE16(0x5F0) { // LDRB <Rd>, [<Rn>, #+<offset_12>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	case 0x650:
	case 0x658: { // LDRB <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x652:
	case 0x65A: { // LDRB <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x654:
	case 0x65C: { // LDRB <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x656:
	case 0x65E: { // LDRB <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x6D0:
	case 0x6D8: { // LDRB <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x6D2:
	case 0x6DA: { // LDRB <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x6D4:
	case 0x6DC: { // LDRB <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x6D6:
	case 0x6DE: { // LDRB <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x750:
	case 0x758: { // LDRB <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x752:
	case 0x75A: { // LDRB <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x754:
	case 0x75C: { // LDRB <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x756:
	case 0x75E: { // LDRB <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x7D0:
	case 0x7D8: { // LDRB <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x7D2:
	case 0x7DA: { // LDRB <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x7D4:
	case 0x7DC: { // LDRB <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x7D6:
	case 0x7DE: { // LDRB <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x770:
	case 0x778: { // LDRB <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	case 0x772:
	case 0x77A: { // LDRB <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	case 0x774:
	case 0x77C: { // LDRB <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	case 0x776:
	case 0x77E: { // LDRB <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	case 0x7F0:
	case 0x7F8: { // LDRB <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	case 0x7F2:
	case 0x7FA: { // LDRB <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	case 0x7F4:
	case 0x7FC: { // LDRB <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	case 0x7F6:
	case 0x7FE: { // LDRB <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read8(address);
		_registers[rn] = address;
		break;
	}
	//LDRBT
	CASE_RANGE16(0x470) { // LDRBT <Rd>, [<Rn>], #-<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	CASE_RANGE16(0x4F0) { // LDRBT <Rd>, [<Rn>], #+<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd-8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x670:
	case 0x678: { // LDRBT <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x672:
	case 0x67A: { // LDRBT <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x674:
	case 0x67C: { // LDRBT <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x676:
	case 0x67E: { // LDRBT <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x6F0:
	case 0x6F8: { // LDRBT <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6F2:
	case 0x6FA: { // LDRBT <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6F4:
	case 0x6FC: { // LDRBT <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6F6:
	case 0x6FE: { // LDRBT <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read8(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read8(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read8(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	// LDRH
	case 0x01B: { // LDRH <Rd>, [<Rn>], -<Rm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn];
		_registers[rd] = _system._memory.Read16(address);
		_registers[rn] = address - _registers[rm];
		break;
	}
	case 0x09B: { // LDRH <Rd>, [<Rn>], +<Rm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn];
		_registers[rd] = _system._memory.Read16(address);
		_registers[rn] = address + _registers[rm];
		break;
	}
	case 0x05B: { // LDRH <Rd>, [<Rn>], #-<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn];
		_registers[rd] = _system._memory.Read16(address);
		_registers[rn] = address - offset;
		break;
	}
	case 0x0DB: { // LDRH <Rd>, [<Rn>], #+<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn];
		_registers[rd] = _system._memory.Read16(address);
		_registers[rn] = address + offset;
		break;
	}
	case 0x11B: { // LDRH <Rd>, [<Rn>, -<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] - _registers[rm];
		_registers[rd] = _system._memory.Read16(address);
		break;
	}
	case 0x19B: { // LDRH <Rd>, [<Rn>, +<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] + _registers[rm];
		_registers[rd] = _system._memory.Read16(address);
		break;
	}
	case 0x15B: { // LDRH <Rd>, [<Rn>, #-<offset_8>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read16(address);
		break;
	}
	case 0x1DB: { // LDRH <Rd>, [<Rn>, #+<offset_8>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read16(address);
		break;
	}
	case 0x13B: { // LDRH <Rd>, [<Rn>, -<Rm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] - _registers[rm];
		_registers[rd] = _system._memory.Read16(address);
		_registers[rn] = address;
		break;
	}
	case 0x1BB: { // LDRH <Rd>, [<Rn>, +<Rm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] + _registers[rm];
		_registers[rd] = _system._memory.Read16(address);
		_registers[rn] = address;
		break;
	}
	case 0x17B: { // LDRH <Rd>, [<Rn>, #-<offset_8>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] - offset;
		_registers[rd] = _system._memory.Read16(address);
		_registers[rn] = address;
		break;
	}
	case 0x1FB: { // LDRH <Rd>, [<Rn>, #+<offset_8>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] + offset;
		_registers[rd] = _system._memory.Read16(address);
		_registers[rn] = address;
		break;
	}
	// LDRSB
	case 0x01D: { // LDRSB <Rd>, [<Rn>], -<Rm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn];
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		_registers[rn] = address - _registers[rm];
		break;
	}
	case 0x09D: { // LDRSB <Rd>, [<Rn>], +<Rm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn];
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		_registers[rn] = address + _registers[rm];
		break;
	}
	case 0x05D: { // LDRSB <Rd>, [<Rn>], #-<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn];
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		_registers[rn] = address - offset;
		break;
	}
	case 0x0DD: { // LDRSB <Rd>, [<Rn>], #+<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn];
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		_registers[rn] = address + offset;
		break;
	}
	case 0x11D: { // LDRSB <Rd>, [<Rn>, -<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] - _registers[rm];
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		break;
	}
	case 0x19D: { // LDRSB <Rd>, [<Rn>, +<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] + _registers[rm];
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		break;
	}
	case 0x15D: { // LDRSB <Rd>, [<Rn>], #-<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] - offset;
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		break;
	}
	case 0x1DD: { // LDRSB <Rd>, [<Rn>], #+<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] + offset;
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		break;
	}
	case 0x13D: { // LDRSB <Rd>, [<Rn>, -<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] - _registers[rm];
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		_registers[rn] = address;
		break;
	}
	case 0x1BD: { // LDRSB <Rd>, [<Rn>, +<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] + _registers[rm];
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		_registers[rn] = address;
		break;
	}
	case 0x17D: { // LDRSB <Rd>, [<Rn>, #-<offset_8>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] - offset;
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		_registers[rn] = address;
		break;
	}
	case 0x1FD: { // LDRSB <Rd>, [<Rn>, #+<offset_8>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] + offset;
		uint32_t value = _system._memory.Read8(address);
		if ((value & 0x80) != 0) value |= 0xFFFFFF00;
		_registers[rd] = value;
		_registers[rn] = address;
		break;
	}
	// LDRSH
	case 0x01F: { // LDRSH <Rd>, [<Rn>], -<Rm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn];
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		_registers[rn] = address - _registers[rm];
		break;
	}
	case 0x09F: { // LDRSH <Rd>, [<Rn>], +<Rm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn];
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		_registers[rn] = address + _registers[rm];
		break;
	}
	case 0x05F: { // LDRSH <Rd>, [<Rn>], #-<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn];
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		_registers[rn] = address - offset;
		break;
	}
	case 0x0DF: { // LDRSH <Rd>, [<Rn>], #+<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn];
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		_registers[rn] = address + offset;
		break;
	}
	case 0x11F: { // LDRSH <Rd>, [<Rn>, -<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] - _registers[rm];
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		break;
	}
	case 0x19F: { // LDRSH <Rd>, [<Rn>, +<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] + _registers[rm];
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		break;
	}
	case 0x15F: { // LDRSH <Rd>, [<Rn>], #-<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] - offset;
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		break;
	}
	case 0x1DF: { // LDRSH <Rd>, [<Rn>], #+<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] + offset;
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		break;
	}
	case 0x13F: { // LDRSH <Rd>, [<Rn>, -<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] - _registers[rm];
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		_registers[rn] = address;
		break;
	}
	case 0x1BF: { // LDRSH <Rd>, [<Rn>, +<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] + _registers[rm];
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		_registers[rn] = address;
		break;
	}
	case 0x17F: { // LDRSH <Rd>, [<Rn>, #-<offset_8>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] - offset;
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		_registers[rn] = address;
		break;
	}
	case 0x1FF: { // LDRSH <Rd>, [<Rn>, #+<offset_8>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] + offset;
		uint32_t value = _system._memory.Read16(address);
		if ((value & 0x8000) != 0) value |= 0xFFFF0000;
		_registers[rd] = value;
		_registers[rn] = address;
		break;
	}
	// LDRT
	CASE_RANGE16(0x430) { // LDRT <Rd>, [<Rn>], #-<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	CASE_RANGE16(0x4B0) { // LDRT <Rd>, [<Rn>], #+<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x630:
	case 0x638: { // LDRT <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x632:
	case 0x63A: { // LDRT <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x634:
	case 0x63C: { // LDRT <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x636:
	case 0x63E: { // LDRT <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x6B0:
	case 0x6B8: { // LDRT <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6B2:
	case 0x6BA: { // LDRT <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6B4:
	case 0x6BC: { // LDRT <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6B6:
	case 0x6BE: { // LDRT <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_registers[rd] = _system._memory.Read32(address);
			}
			else {
				_registersUser[rd - 8] = _system._memory.Read32(address);
			}
		}
		else {
			_registers[rd] = _system._memory.Read32(address);
		}
		_registers[rn] = address + offset;
		break;
	}
	// MLA
	case 0x029: { // MLA <Rd>, <Rm>, <Rs>, <Rn>
		uint8_t rd = (instruction >> 16) & 0xF;
		uint8_t rn = (instruction >> 12) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 0) & 0xF;
		MLA(_registers[rn], _registers[rs], _registers[rm], _registers[rd], _hostFlags);
		break;
	}
	case 0x039: { // MLAS <Rd>, <Rm>, <Rs>, <Rn>
		uint8_t rd = (instruction >> 16) & 0xF;
		uint8_t rn = (instruction >> 12) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 0) & 0xF;
		MLA_FLAGS(_registers[rn], _registers[rs], _registers[rm], _registers[rd], _hostFlags);
		break;
	}
	// MOV
	case 0x1A0:
	case 0x1A8: { // MOV <Rd>, <Rm>, LSL #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		_registers[rd] = operand;
		break;
	}
	case 0x1A1: { // MOV <Rd>, <Rm>, LSL <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		_registers[rd] = operand;
		break;
	}
	case 0x1A2:
	case 0x1AA: { // MOV <Rd>, <Rm>, LSR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		_registers[rd] = operand;
		break;
	}
	case 0x1A3: { // MOV <Rd>, <Rm>, LSR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		_registers[rd] = operand;
		break;
	}
	case 0x1A4:
	case 0x1AC: { // MOV <Rd>, <Rm>, ASR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		_registers[rd] = operand;
		break;
	}
	case 0x1A5: { // MOV <Rd>, <Rm>, ASR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		_registers[rd] = operand;
		break;
	}
	case 0x1A6: // MOV <Rd>, <Rm>, RRX #<imm>
	case 0x1AE: { // MOV <Rd>, <Rm>, ROR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		_registers[rd] = operand;
		break;
	}
	case 0x1A7: { // MOV <Rd>, <Rm>, ROR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		_registers[rd] = operand;
		break;
	}
	case 0x1B0:
	case 0x1B8: { // MOVS <Rd>, <Rm>, LSL #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	case 0x1B1: { // MOVS <Rd>, <Rm>, LSL <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	case 0x1B2:
	case 0x1BA: { // MOVS <Rd>, <Rm>, LSR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	case 0x1B3: { // MOVS <Rd>, <Rm>, LSR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	case 0x1B4:
	case 0x1BC: { // MOVS <Rd>, <Rm>, ASR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	case 0x1B5: { // MOVS <Rd>, <Rm>, ASR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	case 0x1B6: // MOVS <Rd>, <Rm>, RRX #<imm>
	case 0x1BE: { // MOVS <Rd>, <Rm>, ROR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	case 0x1B7: { // MOVS <Rd>, <Rm>, ROR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	CASE_RANGE16(0x3A0) { // MOV <Rd>, #<immediate>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		_registers[rd] = operand;
		break;
	}
	CASE_RANGE16(0x3B0) { // MOVS <Rd>, #<immediate>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			_registers[rd] = operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			MOV_FLAGS(operand, _hostFlags);
			_registers[rd] = operand;
		}
		break;
	}
	// MRS
	case 0x100: { // MRS <Rd>, CPSR
		uint8_t rd = (instruction >> 12) & 0xF;
		SaveHostFlagsToCPSR();
		_registers[rd] = _cpsr;
		break;
	}
	case 0x140: { // MRS <Rd>, SPSR
		uint8_t rd = (instruction >> 12) & 0xF;
		_registers[rd] = _spsr;
		break;
	}
	// MSR
	CASE_RANGE16(0x320) { // MSR CPSR_<fields>, #<immediate>
		uint8_t rotImm = (instruction >> 8) & 0xF;
		uint8_t imm = instruction & 0xFF;
		uint32_t operand;
		ROR(imm, rotImm * 2, operand);
		uint32_t mask = 0;
		SaveHostFlagsToCPSR();
		if ((instruction & (1 << 16)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF;
		}
		if ((instruction & (1 << 17)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF00;
		}
		if ((instruction & (1 << 18)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF0000;
		}
		if ((instruction & (1 << 19)) != 0) {
			mask |= 0xFF000000;
		}
		_cpsr = (_cpsr & ~mask) | (operand & mask);
		LoadHostFlagsFromCPSR();
		UpdateMode();
		break;
	}
	CASE_RANGE16(0x360) { // MSR SPSR_<fields>, #<immediate>
		uint8_t rotImm = (instruction >> 8) & 0xF;
		uint8_t imm = instruction & 0xFF;
		uint32_t operand;
		ROR(imm, rotImm * 2, operand);
		uint32_t mask = 0;
		if ((instruction & (1 << 16)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF;
		}
		if ((instruction & (1 << 17)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF00;
		}
		if ((instruction & (1 << 18)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF0000;
		}
		if ((instruction & (1 << 19)) != 0) {
			mask |= 0xFF000000;
		}
		_spsr = (_spsr & ~mask) | (operand & mask);
		break;
	}
	case 0x120: { // MSR CPSR_<fields>, <Rm>
		uint8_t rm = instruction & 0xF;
		uint32_t operand = _registers[rm];
		uint32_t mask = 0;
		SaveHostFlagsToCPSR();
		if ((instruction & (1 << 16)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF;
		}
		if ((instruction & (1 << 17)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF00;
		}
		if ((instruction & (1 << 18)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF0000;
		}
		if ((instruction & (1 << 19)) != 0) {
			mask |= 0xFF000000;
		}
		_cpsr = (_cpsr & ~mask) | (operand & mask);
		LoadHostFlagsFromCPSR();
		UpdateMode();
		break;
	}
	case 0x160: { // MSR SPSR_<fields>, <Rm>
		uint8_t rm = instruction & 0xF;
		uint32_t operand = _registers[rm];
		uint32_t mask = 0;
		if ((instruction & (1 << 16)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF;
		}
		if ((instruction & (1 << 17)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF00;
		}
		if ((instruction & (1 << 18)) != 0 && InAPrivilegedMode()) {
			mask |= 0xFF0000;
		}
		if ((instruction & (1 << 19)) != 0) {
			mask |= 0xFF000000;
			_hostFlags &= ~((1 << 0) | (1 << 6) | (1 << 7) | (1 << 11));
			if ((operand & CPSR_V_MASK) != 0) _hostFlags |= (1 << 11);
			if ((operand & CPSR_C_MASK) != 0) _hostFlags |= (1 << 0);
			if ((operand & CPSR_Z_MASK) != 0) _hostFlags |= (1 << 6);
			if ((operand & CPSR_N_MASK) != 0) _hostFlags |= (1 << 7);
		}
		_spsr = (_spsr & ~mask) | (operand & mask);
		break;
	}
	// MUL
	case 0x009: { // MUL <Rd>, <Rm>, <Rs>
		uint8_t rm = instruction & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rd = (instruction >> 16) & 0xF;
		_registers[rd] = _registers[rm] * _registers[rs];
		break;
	}
	case 0x019: { // MULS <Rd>, <Rm>, <Rs>
		uint8_t rm = instruction & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rd = (instruction >> 16) & 0xF;
		MUL_FLAGS(_registers[rm], _registers[rs], _registers[rd], _hostFlags);
		break;
	}
	// MVN
	case 0x1E0:
	case 0x1E8: { // MVN <Rd>, <Rm>, LSL #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		_registers[rd] = ~operand;
		break;
	}
	case 0x1E1: { // MVN <Rd>, <Rm>, LSL <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		_registers[rd] = ~operand;
		break;
	}
	case 0x1E2:
	case 0x1EA: { // MVN <Rd>, <Rm>, LSR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		_registers[rd] = ~operand;
		break;
	}
	case 0x1E3: { // MVN <Rd>, <Rm>, LSR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		_registers[rd] = ~operand;
		break;
	}
	case 0x1E4:
	case 0x1EC: { // MVN <Rd>, <Rm>, ASR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		_registers[rd] = ~operand;
		break;
	}
	case 0x1E5: { // MVN <Rd>, <Rm>, ASR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		_registers[rd] = ~operand;
		break;
	}
	case 0x1E6: // MVN <Rd>, <Rm>, RRX #<imm>
	case 0x1EE: { // MVN <Rd>, <Rm>, ROR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		_registers[rd] = ~operand;
		break;
	}
	case 0x1E7: { // MVN <Rd>, <Rm>, ROR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		_registers[rd] = ~operand;
		break;
	}
	case 0x1F0:
	case 0x1F8: { // MVNS <Rd>, <Rm>, LSL #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	case 0x1F1: { // MVNS <Rd>, <Rm>, LSL <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	case 0x1F2:
	case 0x1FA: { // MVNS <Rd>, <Rm>, LSR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	case 0x1F3: { // MVNS <Rd>, <Rm>, LSR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	case 0x1F4:
	case 0x1FC: { // MVNS <Rd>, <Rm>, ASR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	case 0x1F5: { // MVNS <Rd>, <Rm>, ASR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	case 0x1F6: // MVNS <Rd>, <Rm>, RRX #<imm>
	case 0x1FE: { // MVNS <Rd>, <Rm>, ROR #<imm>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	case 0x1F7: { // MVNS <Rd>, <Rm>, ROR <Rs>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	CASE_RANGE16(0x3E0) { // MVN <Rd>, #<immediate>
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		_registers[rd] = ~operand;
		break;
	}
	CASE_RANGE16(0x3F0) { // MVNS <Rd>, #<immediate>
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			_registers[rd] = ~operand;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			MVN_FLAGS(operand, _hostFlags);
			_registers[rd] = ~operand;
		}
		break;
	}
	// ORR
	case 0x180:
	case 0x188: { // ORR <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x181: { // ORR <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x182:
	case 0x18A: { // ORR <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x183: { // ORR <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x184:
	case 0x18C: { // ORR <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x185: { // ORR <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x186: // ORR <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x18E: { // ORR <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x187: { // ORR <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x190:
	case 0x198: { // ORRS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x191: { // ORRS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x192:
	case 0x19A: { // ORRS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x193: { // ORRS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x194:
	case 0x19C: { // ORRS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x195: { // ORRS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x196: // ORRS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x19E: { // ORRS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x197: { // ORRS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x380) { // ORR <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		ORR(_registers[rn], operand, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x390) { // ORRS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			ORR(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			ORR_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	// RSB
	case 0x060:
	case 0x068: { // RSB <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	case 0x061: { // RSB <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	case 0x062:
	case 0x06A: { // RSB <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	case 0x063: { // RSB <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	case 0x064:
	case 0x06C: { // RSB <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	case 0x065: { // RSB <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	case 0x066: // RSB <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x06E: { // RSB <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	case 0x067: { // RSB <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	case 0x070:
	case 0x078: { // RSBS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x071: { // RSBS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x072:
	case 0x07A: { // RSBS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x073: { // RSBS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x074:
	case 0x07C: { // RSBS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x075: { // RSBS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x076: // RSBS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x07E: { // RSBS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x077: { // RSBS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x260) { // RSB <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		SUB(operand, _registers[rn], _registers[rd]);
		break;
	}
	CASE_RANGE16(0x270) { // RSBS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			SUB(operand, _registers[rn], _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			SUB_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	// RSC
	case 0x0E0:
	case 0x0E8: { // RSC <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	case 0x0E1: { // RSC <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	case 0x0E2:
	case 0x0EA: { // RSC <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	case 0x0E3: { // RSC <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	case 0x0E4:
	case 0x0EC: { // RSC <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	case 0x0E5: { // RSC <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	case 0x0E6: // RSC <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x0EE: { // RSC <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	case 0x0E7: { // RSC <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	case 0x0F0:
	case 0x0F8: { // RSCS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);;
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0F1: { // RSCS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0F2:
	case 0x0FA: { // RSCS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0F3: { // RSCS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0F4:
	case 0x0FC: { // RSCS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0F5: { // RSCS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0F6: // RSCS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x0FE: { // RSCS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0F7: { // RSCS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x2E0) { // RSC <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		SBC(operand, _registers[rn], _registers[rd], _hostFlags);
		break;
	}
	CASE_RANGE16(0x2F0) { // RSCS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			SBC(operand, _registers[rn], _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			SBC_FLAGS(operand, _registers[rn], _registers[rd], _hostFlags);
		}
		break;
	}
	// SBC
	case 0x0C0:
	case 0x0C8: { // SBC <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0C1: { // SBC <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0C2:
	case 0x0CA: { // SBC <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0C3: { // SBC <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0C4:
	case 0x0CC: { // SBC <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0C5: { // SBC <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0C6: // SBC <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x0CE: { // SBC <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0C7: { // SBC <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	case 0x0D0:
	case 0x0D8: { // SBCS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0D1: { // SBCS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0D2:
	case 0x0DA: { // SBCS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0D3: { // SBCS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0D4:
	case 0x0DC: { // SBCS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0D5: { // SBCS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0D6: // SBCS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x0DE: { // SBCS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x0D7: { // SBCS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x2C0) { // SBC <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		SBC(_registers[rn], operand, _registers[rd], _hostFlags);
		break;
	}
	CASE_RANGE16(0x2D0) { // SBCS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			SBC(_registers[rn], operand, _registers[rd], _hostFlags);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			SBC_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	// SMLAL
	case 0x0E9: { // SMLAL <RdLo>, <RdHi>, <Rm>, <Rs>
		uint8_t rdLo = (instruction >> 12) & 0xF;
		uint8_t rdHi = (instruction >> 16) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 8) & 0xF;
		SMLAL(_registers[rm], _registers[rs], _registers[rdLo], _registers[rdHi]);
		break;
	}
	case 0x0F9: { // SMLALS <RdLo>, <RdHi>, <Rm>, <Rs>
		uint8_t rdLo = (instruction >> 12) & 0xF;
		uint8_t rdHi = (instruction >> 16) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 8) & 0xF;
		SMLAL_FLAGS(_registers[rm], _registers[rs], _registers[rdLo], _registers[rdHi], _hostFlags);
		break;
	}
	// SMULL
	case 0x0C9: { // SMULL <RdLo>, <RdHi>, <Rm>, <Rs>
		uint8_t rdLo = (instruction >> 12) & 0xF;
		uint8_t rdHi = (instruction >> 16) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 8) & 0xF;
		SMULL(_registers[rm], _registers[rs], _registers[rdLo], _registers[rdHi]);
		break;
	}
	case 0x0D9: { // SMULLS <RdLo>, <RdHi>, <Rm>, <Rs>
		uint8_t rdLo = (instruction >> 12) & 0xF;
		uint8_t rdHi = (instruction >> 16) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 8) & 0xF;
		SMULL_FLAGS(_registers[rm], _registers[rs], _registers[rdLo], _registers[rdHi], _hostFlags);
		break;
	}
	// STM
	CASE_RANGE16(0x800) { // STMDA <Rn>, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 15; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				address -= 4;
				_system._memory.Write32(address, _registers[i]);
			}
		}
		break;
	}
	CASE_RANGE16(0x820) { // STMDA <Rn>!, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 15; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				address -= 4;
				_system._memory.Write32(address, _registers[i]);
			}
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x880) { // STMIA <Rn>, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 0; i <= 15; i++) {
			if ((registerList & (1 << i)) != 0) {
				address += 4;
				_system._memory.Write32(address, _registers[i]);
			}
		}
		break;
	}
	CASE_RANGE16(0x8A0) { // STMIA <Rn>!, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 0; i <= 15; i++) {
			if ((registerList & (1 << i)) != 0) {
				address += 4;
				_system._memory.Write32(address, _registers[i]);
			}
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x900) { // STMDB <Rn>, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 15; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				_system._memory.Write32(address, _registers[i]);
				address -= 4;
			}
		}
		break;
	}
	CASE_RANGE16(0x920) { // STMDB <Rn>!, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 15; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				_system._memory.Write32(address, _registers[i]);
				address -= 4;
			}
		}
		_registers[rn] = address + 4;
		break;
	}
	CASE_RANGE16(0x980) { // STMIB <Rn>, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 0; i >= 15; i++) {
			if ((registerList & (1 << i)) != 0) {
				_system._memory.Write32(address, _registers[i]);
				address += 4;
			}
		}
		break;
	}
	CASE_RANGE16(0x9A0) { // STMIB <Rn>!, <registers>
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		for (int i = 0; i <= 15; i++) {
			if ((registerList & (1 << i)) != 0) {
				_system._memory.Write32(address, _registers[i]);
				address += 4;
			}
		}
		_registers[rn] = address - 4;
		break;
	}
	CASE_RANGE16(0x840) { // STMDA <Rn>, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if ((registerList & (1 << 15)) != 0) {
				address -= 4;
				_system._memory.Write32(address, _registers[REGPC]);
			}
			address -= 4;
			_system._memory.Write32(address, _registersUser[14-8]);
			address -= 4;
			_system._memory.Write32(address, _registersUser[13-8]);
			if (InABankedUserRegistersMode()) {
				for (int i = 12; i >= 8; i--) {
					if ((registerList & (1 << i)) != 0) {
						address -= 4;
						_system._memory.Write32(address, _registersUser[i-8]);
					}
				}
			}
			else {
				for (int i = 12; i >= 8; i--) {
					if ((registerList & (1 << i)) != 0) {
						address -= 4;
						_system._memory.Write32(address, _registers[i]);
					}
				}
			}
			for (int i = 7; i >= 0; i--) {
				address -= 4;
				_system._memory.Write32(address, _registers[i]);
			}
		}
		else {
			for (int i = 15; i >= 0; i--) {
				if ((registerList & (1 << i)) != 0) {
					address -= 4;
					_system._memory.Write32(address, _registers[i]);
				}
			}
		}
		break;
	}
	CASE_RANGE16(0x860) { // STMDA <Rn>!, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if ((registerList & (1 << 15)) != 0) {
				address -= 4;
				_system._memory.Write32(address, _registers[REGPC]);
			}
			address -= 4;
			_system._memory.Write32(address, _registersUser[14 - 8]);
			address -= 4;
			_system._memory.Write32(address, _registersUser[13 - 8]);
			if (InABankedUserRegistersMode()) {
				for (int i = 12; i >= 8; i--) {
					if ((registerList & (1 << i)) != 0) {
						address -= 4;
						_system._memory.Write32(address, _registersUser[i - 8]);
					}
				}
			}
			else {
				for (int i = 12; i >= 8; i--) {
					if ((registerList & (1 << i)) != 0) {
						address -= 4;
						_system._memory.Write32(address, _registers[i]);
					}
				}
			}
			for (int i = 7; i >= 0; i--) {
				address -= 4;
				_system._memory.Write32(address, _registers[i]);
			}
		}
		else {
			for (int i = 15; i >= 0; i--) {
				if ((registerList & (1 << i)) != 0) {
					address -= 4;
					_system._memory.Write32(address, _registers[i]);
				}
			}
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x8C0) { // STMIA <Rn>, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			for (int i = 0; i <= 7; i++) {
				address += 4;
				_system._memory.Write32(address, _registers[i]);
			}
			if (InABankedUserRegistersMode()) {
				for (int i = 8; i <= 12; i++) {
					if ((registerList & (1 << i)) != 0) {
						address += 4;
						_system._memory.Write32(address, _registersUser[i - 8]);
					}
				}
			}
			else {
				for (int i = 8; i <= 12; i++) {
					if ((registerList & (1 << i)) != 0) {
						address += 4;
						_system._memory.Write32(address, _registers[i]);
					}
				}
			}
			address += 4;
			_system._memory.Write32(address, _registersUser[13 - 8]);
			address += 4;
			_system._memory.Write32(address, _registersUser[14 - 8]);
			if ((registerList & (1 << 15)) != 0) {
				address += 4;
				_system._memory.Write32(address, _registers[REGPC]);
			}
		}
		else {
			for (int i = 0; i <= 15; i++) {
				if ((registerList & (1 << i)) != 0) {
					address += 4;
					_system._memory.Write32(address, _registers[i]);
				}
			}
		}
		break;
	}
	CASE_RANGE16(0x8E0) { // STMIA <Rn>!, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			for (int i = 0; i <= 7; i++) {
				address += 4;
				_system._memory.Write32(address, _registers[i]);
			}
			if (InABankedUserRegistersMode()) {
				for (int i = 8; i <= 12; i++) {
					if ((registerList & (1 << i)) != 0) {
						address += 4;
						_system._memory.Write32(address, _registersUser[i - 8]);
					}
				}
			}
			else {
				for (int i = 8; i <= 12; i++) {
					if ((registerList & (1 << i)) != 0) {
						address += 4;
						_system._memory.Write32(address, _registers[i]);
					}
				}
			}
			address += 4;
			_system._memory.Write32(address, _registersUser[13 - 8]);
			address += 4;
			_system._memory.Write32(address, _registersUser[14 - 8]);
			if ((registerList & (1 << 15)) != 0) {
				address += 4;
				_system._memory.Write32(address, _registers[REGPC]);
			}
		}
		else {
			for (int i = 0; i <= 15; i++) {
				if ((registerList & (1 << i)) != 0) {
					address += 4;
					_system._memory.Write32(address, _registers[i]);
				}
			}
		}
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x940) { // STMDB <Rn>, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if ((registerList & (1 << 15)) != 0) {
				_system._memory.Write32(address, _registers[REGPC]);
				address -= 4;
			}
			_system._memory.Write32(address, _registersUser[14 - 8]);
			address -= 4;
			_system._memory.Write32(address, _registersUser[13 - 8]);
			address -= 4;
			if (InABankedUserRegistersMode()) {
				for (int i = 12; i >= 8; i--) {
					if ((registerList & (1 << i)) != 0) {
						_system._memory.Write32(address, _registersUser[i - 8]);
						address -= 4;
					}
				}
			}
			else {
				for (int i = 12; i >= 8; i--) {
					if ((registerList & (1 << i)) != 0) {
						_system._memory.Write32(address, _registers[i]);
						address -= 4;
					}
				}
			}
			for (int i = 7; i >= 0; i--) {
				_system._memory.Write32(address, _registers[i]);
				address -= 4;
			}
		}
		else {
			for (int i = 15; i >= 0; i--) {
				if ((registerList & (1 << i)) != 0) {
					_system._memory.Write32(address, _registers[i]);
					address -= 4;
				}
			}
		}
		break;
	}
	CASE_RANGE16(0x960) { // STMDB <Rn>!, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if ((registerList & (1 << 15)) != 0) {
				_system._memory.Write32(address, _registers[REGPC]);
				address -= 4;
			}
			_system._memory.Write32(address, _registersUser[14 - 8]);
			address -= 4;
			_system._memory.Write32(address, _registersUser[13 - 8]);
			address -= 4;
			if (InABankedUserRegistersMode()) {
				for (int i = 12; i >= 8; i--) {
					if ((registerList & (1 << i)) != 0) {
						_system._memory.Write32(address, _registersUser[i - 8]);
						address -= 4;
					}
				}
			}
			else {
				for (int i = 12; i >= 8; i--) {
					if ((registerList & (1 << i)) != 0) {
						_system._memory.Write32(address, _registers[i]);
						address -= 4;
					}
				}
			}
			for (int i = 7; i >= 0; i--) {
				_system._memory.Write32(address, _registers[i]);
				address -= 4;
			}
		}
		else {
			for (int i = 15; i >= 0; i--) {
				if ((registerList & (1 << i)) != 0) {
					_system._memory.Write32(address, _registers[i]);
					address -= 4;
				}
			}
		}
		_registers[rn] = address+4;
		break;
	}
	CASE_RANGE16(0x9C0) { // STMIB <Rn>, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			for (int i = 0; i <= 7; i++) {
				_system._memory.Write32(address, _registers[i]);
				address += 4;
			}
			if (InABankedUserRegistersMode()) {
				for (int i = 8; i <= 12; i++) {
					if ((registerList & (1 << i)) != 0) {
						_system._memory.Write32(address, _registersUser[i - 8]);
						address += 4;
					}
				}
			}
			else {
				for (int i = 8; i <= 12; i++) {
					if ((registerList & (1 << i)) != 0) {
						_system._memory.Write32(address, _registers[i]);
						address += 4;
					}
				}
			}
			_system._memory.Write32(address, _registersUser[13 - 8]);
			address += 4;
			_system._memory.Write32(address, _registersUser[14 - 8]);
			address += 4;
			if ((registerList & (1 << 15)) != 0) {
				_system._memory.Write32(address, _registers[REGPC]);
				address += 4;
			}
		}
		else {
			for (int i = 0; i <= 15; i++) {
				if ((registerList & (1 << i)) != 0) {
					_system._memory.Write32(address, _registers[i]);
					address += 4;
				}
			}
		}
		break;
	}
	CASE_RANGE16(0x9E0) { // STMIB <Rn>!, <registers>^
		uint16_t registerList = instruction & 0xFFFF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			for (int i = 0; i <= 7; i++) {
				_system._memory.Write32(address, _registers[i]);
				address += 4;
			}
			if (InABankedUserRegistersMode()) {
				for (int i = 8; i <= 12; i++) {
					if ((registerList & (1 << i)) != 0) {
						_system._memory.Write32(address, _registersUser[i - 8]);
						address += 4;
					}
				}
			}
			else {
				for (int i = 8; i <= 12; i++) {
					if ((registerList & (1 << i)) != 0) {
						_system._memory.Write32(address, _registers[i]);
						address += 4;
					}
				}
			}
			_system._memory.Write32(address, _registersUser[13 - 8]);
			address += 4;
			_system._memory.Write32(address, _registersUser[14 - 8]);
			address += 4;
			if ((registerList & (1 << 15)) != 0) {
				_system._memory.Write32(address, _registers[REGPC]);
				address += 4;
			}
		}
		else {
			for (int i = 0; i <= 15; i++) {
				if ((registerList & (1 << i)) != 0) {
					_system._memory.Write32(address, _registers[i]);
					address += 4;
				}
			}
		}
		_registers[rn] = address - 4;
		break;
	}
	// STR
	CASE_RANGE16(0x400) { // STR <Rd>, [<Rn>], #-<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	CASE_RANGE16(0x480) { // STR <Rd>, [<Rn>], #+<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	CASE_RANGE16(0x500) { // STR <Rd>, [<Rn>, #-<offset_12>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x520) { // STR <Rd>, [<Rn>, #-<offset_12>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x580) { // STR <Rd>, [<Rn>, #+<offset_12>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x5A0) { // STR <Rd>, [<Rn>, #+<offset_12>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x600:
	case 0x608: { // STR <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x602:
	case 0x60A: { // STR <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x604:
	case 0x60C: { // STR <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x606:
	case 0x60E: { // STR <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x680:
	case 0x688: { // STR <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x682:
	case 0x68A: { // STR <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x684:
	case 0x68C: { // STR <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x686:
	case 0x68E: { // STR <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x700:
	case 0x708: { // STR <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x702:
	case 0x70A: { // STR <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x704:
	case 0x70C: { // STR <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x706:
	case 0x70E: { // STR <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x780:
	case 0x788: { // STR <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x782:
	case 0x78A: { // STR <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x784:
	case 0x78C: { // STR <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x786:
	case 0x78E: { // STR <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x720:
	case 0x728: { // STR <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x722:
	case 0x72A: { // STR <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x724:
	case 0x72C: { // STR <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x726:
	case 0x72E: { // STR <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x7A0:
	case 0x7A8: { // STR <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x7A2:
	case 0x7AA: { // STR <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x7A4:
	case 0x7AC: { // STR <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x7A6:
	case 0x7AE: { // STR <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write32(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	// STRB
	CASE_RANGE16(0x440) { // STRB <Rd>, [<Rn>], #-<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	CASE_RANGE16(0x4C0) { // STRB <Rd>, [<Rn>], #+<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	CASE_RANGE16(0x540) { // STRB <Rd>, [<Rn>, #-<offset_12>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x560) { // STRB <Rd>, [<Rn>, #-<offset_12>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	CASE_RANGE16(0x5C0) { // STRB <Rd>, [<Rn>, #+<offset_12>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x5E0) { // STRB <Rd>, [<Rn>, #+<offset_12>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x640:
	case 0x648: { // STRB <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x642:
	case 0x64A: { // STRB <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x644:
	case 0x64C: { // STRB <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x646:
	case 0x64E: { // STRB <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x6C0:
	case 0x6C8: { // STRB <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x6C2:
	case 0x6CA: { // STRB <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x6C4:
	case 0x6CC: { // STRB <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x6C6:
	case 0x6CE: { // LDRB <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x740:
	case 0x748: { // STRB <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x742:
	case 0x74A: { // STRB <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x744:
	case 0x74C: { // STRB <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x746:
	case 0x74E: { // STRB <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x7C0:
	case 0x7C8: { // STRB <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x7C2:
	case 0x7CA: { // STRB <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x7C4:
	case 0x7CC: { // LDRB <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x7C6:
	case 0x7CE: { // LDRB <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x760:
	case 0x768: { // LDRB <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x762:
	case 0x76A: { // STRB <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x764:
	case 0x76C: { // STRB <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x766:
	case 0x76E: { // STRB <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x7E0:
	case 0x7E8: { // STRB <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSL(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x7E2:
	case 0x7EA: { // STRB <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandLSR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x7E4:
	case 0x7EC: { // STRB <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandASR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x7E6:
	case 0x7EE: { // STRB <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]!
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t offset = GetShifterOperandROR(instruction);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write8(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	//STRBT
	CASE_RANGE16(0x460) { // STRBT <Rd>, [<Rn>], #-<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd-8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	CASE_RANGE16(0x4E0) { // STRBT <Rd>, [<Rn>], #+<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd-8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x660:
	case 0x668: { // STRBT <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd-8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x662:
	case 0x66A: { // STRBT <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd-8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x664:
	case 0x66C: { // STRBT <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x666:
	case 0x66E: { // STRBT <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x6E0:
	case 0x6E8: { // STRBT <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6E2:
	case 0x6EA: { // STRBT <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6E4:
	case 0x6EC: { // STRBT <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6E6:
	case 0x6EE: { // STRBT <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write8(address, _registers[rd]);
			}
			else {
				_system._memory.Write8(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write8(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	// STRH
	case 0x00B: { // STRH <Rd>, [<Rn>], -<Rm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn];
		_system._memory.Write16(address, _registers[rd]);
		_registers[rn] = address - _registers[rm];
		break;
	}
	case 0x08B: { // STRH <Rd>, [<Rn>], +<Rm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn];
		_system._memory.Write16(address, _registers[rd]);
		_registers[rn] = address + _registers[rm];
		break;
	}
	case 0x04B: { // STRH <Rd>, [<Rn>], #-<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn];
		_system._memory.Write16(address, _registers[rd]);
		_registers[rn] = address - offset;
		break;
	}
	case 0x0CB: { // STRH <Rd>, [<Rn>], #+<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn];
		_system._memory.Write16(address, _registers[rd]);
		_registers[rn] = address + offset;
		break;
	}
	case 0x10B: { // STRH <Rd>, [<Rn>, -<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] - _registers[rm];
		_system._memory.Write16(address, _registers[rd]);
		break;
	}
	case 0x18B: { // STRH <Rd>, [<Rn>, +<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] + _registers[rm];
		_system._memory.Write16(address, _registers[rd]);
		break;
	}
	case 0x14B: { // STRH <Rd>, [<Rn>], #-<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write16(address, _registers[rd]);
		break;
	}
	case 0x1CB: { // STRH <Rd>, [<Rn>], #+<offset_8>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write16(address, _registers[rd]);
		break;
	}
	case 0x12B: { // STRH <Rd>, [<Rn>, -<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] - _registers[rm];
		_system._memory.Write16(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x1AB: { // STRH <Rd>, [<Rn>, +<Rm>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rm = instruction & 0xF;
		uint32_t address = _registers[rn] + _registers[rm];
		_system._memory.Write16(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x16B: { // STRH <Rd>, [<Rn>, #-<offset_8>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] - offset;
		_system._memory.Write16(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	case 0x1EB: { // STRH <Rd>, [<Rn>, #+<offset_8>]
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t offset = (instruction & 0xF) | ((instruction >> 8) & 0xF);
		uint32_t address = _registers[rn] + offset;
		_system._memory.Write16(address, _registers[rd]);
		_registers[rn] = address;
		break;
	}
	// STRT
	CASE_RANGE16(0x420) { // STRT <Rd>, [<Rn>], #-<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	CASE_RANGE16(0x4A0) { // STRT <Rd>, [<Rn>], #+<offset_12>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint16_t offset = (instruction & 0xFFF);
		uint32_t address = _registers[rn];
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x620:
	case 0x628: { // STRT <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x622:
	case 0x62A: { // STRT <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x624:
	case 0x62C: { // STRT <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x626:
	case 0x62E: { // STRT <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address - offset;
		break;
	}
	case 0x6A0:
	case 0x6A8: { // STRT <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSL(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6A2:
	case 0x6AA: { // STRT <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandLSR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6A4:
	case 0x6AC: { // STRT <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandASR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	case 0x6A6:
	case 0x6AE: { // STRT <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t offset = GetShifterOperandROR(instruction);
		if (InAPrivilegedMode()) {
			if (rd == 15 || rd < 8 || (!InABankedUserRegistersMode() && rd < 13)) {
				_system._memory.Write32(address, _registers[rd]);
			}
			else {
				_system._memory.Write32(address, _registersUser[rd - 8]);
			}
		}
		else {
			_system._memory.Write32(address, _registers[rd]);
		}
		_registers[rn] = address + offset;
		break;
	}
	// SUB
	case 0x040:
	case 0x048: { // SUB <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSL(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x041: { // SUB <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSLReg(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x042:
	case 0x04A: { // SUB <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSR(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x043: { // SUB <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandLSRReg(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x044:
	case 0x04C: { // SUB <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASR(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x045: { // SUB <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandASRReg(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x046: // SUB <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x04E: { // SUB <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandROR(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x047: { // SUB <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandRORReg(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	case 0x050:
	case 0x058: { // SUBS <Rd>, <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSL(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x051: { // SUBS <Rd>, <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSLReg(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x052:
	case 0x05A: { // SUBS <Rd>, <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSR(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x053: { // SUBS <Rd>, <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandLSRReg(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x054:
	case 0x05C: { // SUBS <Rd>, <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASR(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x055: { // SUBS <Rd>, <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandASRReg(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandASRRegFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x056: // SUBS <Rd>, <Rn>, <Rm>, RRX #<imm>
	case 0x05E: { // SUBS <Rd>, <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandROR(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	case 0x057: { // SUBS <Rd>, <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandRORReg(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandRORRegFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	CASE_RANGE16(0x240) { // SUB <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint32_t operand = GetShifterOperandImm(instruction);
		SUB(_registers[rn], operand, _registers[rd]);
		break;
	}
	CASE_RANGE16(0x250) { // SUBS <Rd>, <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		if (rd == REGPC) {
			uint32_t operand = GetShifterOperandImm(instruction);
			SUB(_registers[rn], operand, _registers[rd]);
			_cpsr = _spsr;
		}
		else {
			uint32_t operand = GetShifterOperandImmFlags(instruction);
			SUB_FLAGS(_registers[rn], operand, _registers[rd], _hostFlags);
		}
		break;
	}
	// SWI
	CASE_RANGE256(0xF00) { // SWI <immed_24>
		uint32_t imm = instruction & 0xFFFFFF;
		SoftwareInterrupt(imm);
		break;
	}
	// SWP
	case 0x109: { // SWP <Rd>, <Rm>, [<Rn>]
		uint8_t rm = (instruction)& 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		uint32_t temp = _system._memory.Read32(address);
		ROR(temp, (address & 0x3) * 8, temp);
		_system._memory.Write32(address, _registers[rm]);
		_registers[rd] = temp;
		break;
	}
	// SWPB
	case 0x149: { // SWPB <Rd>, <Rm>, [<Rn>]
		uint8_t rm = (instruction)& 0xF;
		uint8_t rd = (instruction >> 12) & 0xF;
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t address = _registers[rn];
		uint8_t temp = _system._memory.Read8(address);
		_system._memory.Write8(address, _registers[rm]);
		_registers[rd] = temp;
		break;
	}
	// TEQ
	case 0x130:
	case 0x138: { // TEQ <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSLFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x131: { // TEQ <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x132:
	case 0x13A: { // TEQ <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSRFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x133: { // TEQ <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x134:
	case 0x13C: { // TEQ <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandASRFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x135: { // TEQ <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandASRRegFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x136: // TEQ <Rn>, <Rm>, RRX #<imm>
	case 0x13E: { // TEQ <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandRORFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x137: { // TEQ <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandRORRegFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	CASE_RANGE16(0x330) { // TEQ <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandImmFlags(instruction);
		TEQ(_registers[rn], operand, _hostFlags);
		break;
	}
	// TST
	case 0x110:
	case 0x118: { // TST <Rn>, <Rm>, LSL #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSLFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x111: { // TST <Rn>, <Rm>, LSL <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSLRegFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x112:
	case 0x11A: { // TST <Rn>, <Rm>, LSR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSRFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x113: { // TST <Rn>, <Rm>, LSR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandLSRRegFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x114:
	case 0x11C: { // TST <Rn>, <Rm>, ASR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandASRFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x115: { // TST <Rn>, <Rm>, ASR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandASRRegFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x116: // TST <Rn>, <Rm>, RRX #<imm>
	case 0x11E: { // TST <Rn>, <Rm>, ROR #<imm>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandRORFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	case 0x117: { // TST <Rn>, <Rm>, ROR <Rs>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandRORRegFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	CASE_RANGE16(0x310) { // TST <Rn>, #<immediate>
		uint8_t rn = (instruction >> 16) & 0xF;
		uint32_t operand = GetShifterOperandImmFlags(instruction);
		TST(_registers[rn], operand, _hostFlags);
		break;
	}
	// UMLAL
	case 0x0A9: { // UMLAL <RdLo>, <RdHi>, <Rm>, <Rs>
		uint8_t rdLo = (instruction >> 12) & 0xF;
		uint8_t rdHi = (instruction >> 16) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 8) & 0xF;
		UMLAL(_registers[rm], _registers[rs], _registers[rdLo], _registers[rdHi]);
		break;
	}
	case 0x0B9: { // UMLALS <RdLo>, <RdHi>, <Rm>, <Rs>
		uint8_t rdLo = (instruction >> 12) & 0xF;
		uint8_t rdHi = (instruction >> 16) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 8) & 0xF;
		UMLAL_FLAGS(_registers[rm], _registers[rs], _registers[rdLo], _registers[rdHi], _hostFlags);
		break;
	}
	// UMULL
	case 0x089: { // UMULL <RdLo>, <RdHi>, <Rm>, <Rs>
		uint8_t rdLo = (instruction >> 12) & 0xF;
		uint8_t rdHi = (instruction >> 16) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 8) & 0xF;
		UMULL(_registers[rm], _registers[rs], _registers[rdLo], _registers[rdHi]);
		break;
	}
	case 0x099: { // UMULLS <RdLo>, <RdHi>, <Rm>, <Rs>
		uint8_t rdLo = (instruction >> 12) & 0xF;
		uint8_t rdHi = (instruction >> 16) & 0xF;
		uint8_t rs = (instruction >> 8) & 0xF;
		uint8_t rm = (instruction >> 8) & 0xF;
		UMULL_FLAGS(_registers[rm], _registers[rs], _registers[rdLo], _registers[rdHi], _hostFlags);
		break;
	}
	default: {
		Log(Error, "Unknown ARM instruction found: 0x%08x at PC=0x%08x", instruction, _registers[REGPC] - 8);
		__debugbreak();
		break;
	}
	}
}