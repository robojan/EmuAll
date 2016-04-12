
#include <GBAemu/gba.h>
#include <GBAemu/defines.h>
#include <GBAemu/cpu/cpu.h>
#include <GBAemu/util/log.h>
#include <GBAemu/cpu/hostInstructions.h>

Cpu::Cpu(Gba &gba) :
	_system(gba)
{

}

Cpu::~Cpu()
{

}

void Cpu::Reset()
{
	for (int i = 0; i < 32; i++) {
		_registers[i] = 0;
	}
}

void Cpu::Tick()
{
	// Read instruction
	uint32_t instruction = _system._memory.Read32(_registers[REGPC] & 0xFFFFFFFC);
	_registers[REGPC] += 4;
	uint8_t cond = instruction >> 28;
	switch (cond) {
	case 0: // Equal
		if ((_cpsr & CPSR_Z_MASK) == 0) return;
		break;
	case 1: // Not equal
		if ((_cpsr & CPSR_Z_MASK) != 0) return;
		break;
	case 2: // Carry set
		if ((_cpsr & CPSR_C_MASK) == 0) return;
		break;
	case 3: // Carry clear
		if ((_cpsr & CPSR_C_MASK) != 0) return;
		break;
	case 4: // negative
		if ((_cpsr & CPSR_N_MASK) == 0) return;
		break;
	case 5: // positive
		if ((_cpsr & CPSR_N_MASK) != 0) return;
		break;
	case 6: // overflow
		if ((_cpsr & CPSR_V_MASK) == 0) return;
		break;
	case 7: // no overflow
		if ((_cpsr & CPSR_V_MASK) != 0) return;
		break;
	case 8: // Unsigned higher
		if ((_cpsr & (CPSR_Z_MASK | CPSR_C_MASK)) != CPSR_C_MASK) return;
		break;
	case 9: // Unsigned lower or same
		if ((_cpsr & CPSR_Z_MASK) != CPSR_Z_MASK &&
			(_cpsr & CPSR_C_MASK) == CPSR_C_MASK) return;
		break;
	case 10: // Signed greater than or equal
		// N == V
		if ((((_cpsr << 3) ^ _cpsr) & CPSR_N_MASK) != 0) return;
		break;
	case 11: // signed less than or equal
		// N != V
		if ((((_cpsr << 3) ^ _cpsr) & CPSR_N_MASK) == 0) return;
		break;
	case 12: // signed greater than
		// !Z & (N==V)
		if ((((_cpsr << 3) ^ _cpsr) & CPSR_N_MASK) != 0 || 
			(_cpsr & CPSR_Z_MASK) != 0) return;
		break;
	case 13: // signed less than or equal
		// Z | (N!=V)
		if ((((_cpsr << 3) ^ _cpsr) & CPSR_N_MASK) == 0 &&
			(_cpsr & CPSR_Z_MASK) == 0) return;
		break;
	case 14: // always
		break;
	case 15: // never
		return;
	}
	uint8_t opcode = (instruction >> 20) & 0x1F;
	uint8_t format = (instruction >> 25) & 7;
	switch (format) {
	case 0x0: { // Multiply, Multiply long, Single data swap, branch and exchange,
				// half word data transfer register offset, half word data transfer immediate offset
		uint8_t rm = instruction & 0xF;
		uint8_t rn = instruction >> 12;
		uint8_t rd = rn & 0xF;
		rn >>= 4;
		uint8_t extra0 = instruction & 0x90;
		switch (extra0) {
		case 0x00:
		case 0x80: {
			if ((opcode & 0x19) == 0x10) {
				// Misc instruction
#include "miscInstructions.inl"
			}
			else {
				// Data processing immediate shift
				uint8_t shift_amount = (instruction >> 7) & 0x1F;
				uint8_t shift = (instruction >> 5) & 0x3;
				uint32_t operand2;
				switch (shift) {
				case 0: 
					// TODO: shifter carry out
					operand2 = _registers[rm] << shift_amount;
					break;
				case 1:
					// TODO: shifter carry out
					operand2 = _registers[rm] >> shift_amount;
					break;
				case 2:
					// TODO: shifter carry out
					operand2 = ASR(_registers[rm], shift_amount);
					break;
				case 3:
					// TODO: shifter carry out
					operand2 = ROR(_registers[rm], shift_amount);
				}
//#include "opcodeExecution.inl"
			}
			break;
		}
		case 0x10: {
			if ((opcode & 0x19) == 0x10) {
				// Misc instruction
#include "miscInstructions.inl"
			}
			else {
				// data processing register shift
				uint8_t rs = (instruction >> 8) & 0xF;
				uint8_t shift = (instruction >> 5) & 0x3;
				uint32_t operand2 = 0;
//#include "opcodeExecution.inl"
			}
			break;
		}
		case 0x90: {
			// Multiplies extra load/stores
			break;
		}
		}
		break;
	}
	case 0x1: { // Data processing and FSR transfer
		uint16_t operand2 = instruction & 0x3FF;
		uint8_t rn = instruction >> 12;
		uint8_t rd = rn & 0xF;
		rn >>= 4;

		AND_FLAGS(_registers[rn], operand2, _registers[rd], _cpsr);
		
		break;
	}
	case 0x3: { // Single data transfer
		uint16_t offset = instruction & 0xFFF;
		uint8_t rn = instruction >> 12;
		uint8_t rd = rn & 0xF;
		rn >>= 4;
		break;
	}
	case 0x4: { // Block data transfer
		uint16_t registerList = instruction;
		uint8_t rn = (instruction >> 16) & 0xF;
		break;
	}
	case 0x5: { // branch
		int32_t offset = instruction & 0xFFFFFF;
		if ((offset & 0x800000) != 0)
			offset |= 0xFF000000;
		offset <<= 2;
		_registers[REGPC] += 4 + offset;
		break;
	}
	case 0x6: { // coprocessor data transfer
		uint8_t offset = instruction;
		uint8_t rd = instruction >> 8;
		uint8_t cpn = rd & 0xF;
		rd >>= 4;
		uint8_t rn = (instruction >> 16) & 0xF;
		break;
	}
	case 0x7: { // coprocessor data operation, coprocessor register transfer
		uint8_t crm = instruction & 0xF;
		uint8_t cp = (instruction >> 5);
		uint8_t cpn = (cp >> 3) & 0xF;
		cp &= 0x7;
		uint8_t rd = instruction >> 12;
		uint8_t crn = rd >> 4;
		rd &= 0xF;
		break;
	}
	default:
		Log(Warn, "Unknown ARM instruction 0x%08x", instruction);
		break;
	}
}
