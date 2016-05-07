
#include <GBAemu/cpu/hostInstructions.h>
#include <GBAemu/cpu/armException.h>
#include <GBAemu/cpu/cpu.h>
#include <GBAemu/gba.h>
#include <GBAemu/util/log.h>
#include <GBAemu/defines.h>

void Cpu::TickThumb(bool step)
{
	uint16_t instruction = _pipelineInstruction;
	_pipelineInstruction = _system._memory.Read16(_registers[REGPC] & 0xFFFFFFFE);
	if (step && IsBreakpoint(_registers[REGPC] - 2)) {
		_pipelineInstruction = _breakpoints.at(_registers[REGPC] - 2);
		instruction = _pipelineInstruction & 0xFFFF;
		_pipelineInstruction >>= 16;
	}
	_registers[REGPC] += 2;

	switch (instruction >> 8) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07: { // LSL
		uint8_t rd = instruction & 0x7;
		uint8_t rm = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		LSL_FLAGS(_registers[rm], imm, _registers[rd], _hostFlags);
		break;
	}
	case 0x08:
	case 0x09:
	case 0x0A:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x0F: { // LSR
		uint8_t rd = instruction & 0x7;
		uint8_t rm = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		LSR_FLAGS(_registers[rm], imm, _registers[rd], _hostFlags);
		break;
	}
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17: { // ASR
		uint8_t rd = instruction & 0x7;
		uint8_t rm = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		ASR_FLAGS(_registers[rm], imm, _registers[rd], _hostFlags);
		break;
	}
	case 0x18:
	case 0x19: { // ADD rd, rn, rm
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		ADD_FLAGS(_registers[rn], _registers[rm], _registers[rd], _hostFlags);
		break;
	}
	case 0x1A:
	case 0x1B: { // SUB rd, rn, rm
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		SUB_FLAGS(_registers[rn], _registers[rm], _registers[rd], _hostFlags);
		break;
	}
	case 0x1C:
	case 0x1D: { // ADD rd, rn, imm3
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x7;
		ADD_FLAGS(_registers[rn], imm, _registers[rd], _hostFlags);
		break;
	}
	case 0x1E:
	case 0x1F: { // SUB rd, rn, imm3
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x7;
		SUB_FLAGS(_registers[rn], imm, _registers[rd], _hostFlags);
		break;
	}
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24:
	case 0x25:
	case 0x26:
	case 0x27: { // MOV rd, imm8
		uint8_t rd = (instruction >> 8) & 0x7;
		uint8_t imm = instruction & 0xFF;
		MOV_FLAGS(imm, _hostFlags);
		_registers[rd] = imm;
		break;
	}
	case 0x28:
	case 0x29:
	case 0x2A:
	case 0x2B:
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x2F: { // CMP rn, imm8
		uint8_t rn = (instruction >> 8) & 0x7;
		uint8_t imm = instruction & 0xFF;
		CMP(_registers[rn], imm, _hostFlags);
		break;
	}
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x37: { // ADD rd, imm8
		uint8_t rd = (instruction >> 8) & 0x7;
		uint8_t imm = instruction & 0xFF;
		ADD_FLAGS(_registers[rd], imm, _registers[rd], _hostFlags);
		break;
	}
	case 0x38:
	case 0x39:
	case 0x3A:
	case 0x3B:
	case 0x3C:
	case 0x3D:
	case 0x3E:
	case 0x3F: {
		uint8_t rd = (instruction >> 8) & 0x7;
		uint8_t imm = instruction & 0xFF;
		SUB_FLAGS(_registers[rd], imm, _registers[rd], _hostFlags);
		break;
	}
	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43: {
		uint8_t rd = instruction & 0x7;
		uint8_t rm = (instruction >> 3) & 0x7;
		switch ((instruction >> 6) & 0xF) {
		case 0: // AND rd, rm
			AND_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 1: // EOR rd, rm
			EOR_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 2: // LSL rd, rs
			LSL_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 3: // LSR rd, rs
			LSR_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 4: // ASR rd, rs
			ASR_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 5: // ADC rd, rm
			ADC_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 6: // SBC rd, rm
			SBC_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 7: // ROR rd, rs
			ROR_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 8: // TST rn, rm
			TST(_registers[rd], _registers[rm], _hostFlags);
			break;
		case 9: // NEG, rd, rm
			SUB_FLAGS(0, _registers[rm], _registers[rd], _hostFlags);
			break;
		case 10: // CMP rn, rm 
			CMP(_registers[rd], _registers[rm], _hostFlags);
			break;
		case 11: // CMN rn, rm
			CMN(_registers[rd], _registers[rm], _hostFlags);
			break;
		case 12: // ORR rd, rm
			ORR_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 13: // MUL rd, rm
			MUL_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 14: // BIC rd, rm
			BIC_FLAGS(_registers[rd], _registers[rm], _registers[rd], _hostFlags);
			break;
		case 15: // MVN rd, rm
			_registers[rd] = ~_registers[rm];
			MVN_FLAGS(_registers[rm], _hostFlags);
			break;
		}
		break;
	}
	case 0x44: { // Add rd, rm
		uint8_t rd = (instruction & 0x7) | ((instruction >> 4) & 0x8);
		uint8_t rm = (instruction >> 3) & 0xF;
		ADD(_registers[rd], _registers[rm], _registers[rd]);
		break;
	}
	case 0x45: { // CMP rn, rm
		uint8_t rd = (instruction & 0x7) | ((instruction >> 4) & 0x8);
		uint8_t rm = (instruction >> 3) & 0xF;
		CMP(_registers[rd], _registers[rm], _hostFlags);
		break;
	}
	case 0x46: { // MOV rd, rm
		uint8_t rd = (instruction & 0x7) | ((instruction >> 4) & 0x8);
		uint8_t rm = (instruction >> 3) & 0xF;
		_registers[rd] = _registers[rm];
		break;
	}
	case 0x47: { // Branch/exchange instruction set
		uint8_t rm = (instruction >> 3) & 0xF;
		uint32_t operand = _registers[rm];
		SetThumbMode((operand & 1) != 0);
		_registers[REGPC] = (operand & (~1));
		_pipelineInstruction = ARM_NOP;
		break;
	}
	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x4B:
	case 0x4C:
	case 0x4D:
	case 0x4E:
	case 0x4F: { // LDR Rd, [PC, imm * 4]
		uint8_t rd = (instruction >> 8) & 0x7;
		uint8_t imm = (instruction & 0xFF);
		_registers[rd] = _system._memory.Read32((_registers[REGPC] & 0xFFFFFFFC) + imm * 4);
		break;
	}
	case 0x50:
	case 0x51: { // STR rd, [rn, rm]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		_system._memory.Write32(_registers[rn] + _registers[rm], _registers[rd]);
		break;
	}
	case 0x52:
	case 0x53: { // STRH rd, [rn, rm]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		_system._memory.Write16(_registers[rn] + _registers[rm], _registers[rd]);
		break;
	}
	case 0x54:
	case 0x55: { // STRB rd, [rn, rm]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		_system._memory.Write8(_registers[rn] + _registers[rm], _registers[rd]);
		break;
	}
	case 0x56:
	case 0x57: { // LDRSB, rd, [rn, rm]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		_registers[rd] = _system._memory.Read8(_registers[rn] + _registers[rm]);
		if ((_registers[rd] & 0x80) != 0) _registers[rd] |= 0xFFFFFF00;
		break;
	}
	case 0x58:
	case 0x59: { // LDR, rd, [rn, rm]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		_registers[rd] = _system._memory.Read32(_registers[rn] + _registers[rm]);
		break;
	}
	case 0x5A:
	case 0x5B: { // LDRH, rd, [rn, rm]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		_registers[rd] = _system._memory.Read16(_registers[rn] + _registers[rm]);
		break;
	}
	case 0x5C:
	case 0x5D: { // LDRB, rd, [rn, rm]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		_registers[rd] = _system._memory.Read8(_registers[rn] + _registers[rm]);
		break;
	}
	case 0x5E:
	case 0x5F: { // LDRSH, rd, [rn, rm]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t rm = (instruction >> 6) & 0x7;
		_registers[rd] = _system._memory.Read16(_registers[rn] + _registers[rm]);
		if ((_registers[rd] & 0x8000) != 0) _registers[rd] |= 0xFFFF0000;
		break;
	}
	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67: { // STR rd, [rn, imm5 * 4]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		uint32_t address = _registers[rn] + imm * 4;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x68:
	case 0x69:
	case 0x6A:
	case 0x6B:
	case 0x6C:
	case 0x6D:
	case 0x6E:
	case 0x6F: { // LDR rd, [rn, imm5 * 4]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		uint32_t address = _registers[rn] + imm * 4;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0x70:
	case 0x71:
	case 0x72:
	case 0x73:
	case 0x74:
	case 0x75:
	case 0x76:
	case 0x77: { // STRB rd, [rn, imm5]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		uint32_t address = _registers[rn] + imm;
		_system._memory.Write8(address, _registers[rd]);
		break;
	}
	case 0x78:
	case 0x79:
	case 0x7A:
	case 0x7B:
	case 0x7C:
	case 0x7D:
	case 0x7E:
	case 0x7F: { // LDRB rd, [rn, imm5]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		uint32_t address = _registers[rn] + imm;
		_registers[rd] = _system._memory.Read8(address);
		break;
	}
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87: { // STRH rd, [rn, imm5 * 2]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		uint32_t address = _registers[rn] + imm * 2;
		_system._memory.Write16(address, _registers[rd]);
		break;
	}
	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x8F: { // LDRB rd, [rn, imm5]
		uint8_t rd = instruction & 0x7;
		uint8_t rn = (instruction >> 3) & 0x7;
		uint8_t imm = (instruction >> 6) & 0x1F;
		uint32_t address = _registers[rn] + imm * 2;
		_registers[rd] = _system._memory.Read16(address);
		break;
	}
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97: { // STR rd, [SP, imm8 * 4]
		uint8_t rd = (instruction >> 8) & 0x7;
		uint8_t imm = instruction & 0xFF;
		uint32_t address = _registers[REGSP] + imm * 4;
		_system._memory.Write32(address, _registers[rd]);
		break;
	}
	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:
	case 0x9C:
	case 0x9D:
	case 0x9E:
	case 0x9F: { // LDR rd, [SP, imm8 * 4]
		uint8_t rd = (instruction >> 8) & 0x7;
		uint8_t imm = instruction & 0xFF;
		uint32_t address = _registers[REGSP] + imm * 4;
		_registers[rd] = _system._memory.Read32(address);
		break;
	}
	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:
	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7: { // ADD rd, PC, imm8 * 4
		uint8_t rd = (instruction >> 8) & 0x7;
		uint8_t imm = instruction & 0xFF;
		ADD(_registers[REGPC] & 0xFFFFFFFC, imm * 4, _registers[rd]);
		break;
	}
	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:
	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF: { // ADD rd, SP, imm8 * 4
		uint8_t rd = (instruction >> 8) & 0x7;
		uint8_t imm = instruction & 0xFF;
		ADD(_registers[REGSP], imm * 4, _registers[rd]);
		break;
	}
	case 0xB0: { // Adjust stack pointer
		uint8_t imm = instruction & 0x7F;
		if ((instruction & 0x80) != 0) {
			SUB(_registers[REGSP], imm * 4, _registers[REGSP]);
		}
		else {
			ADD(_registers[REGSP], imm * 4, _registers[REGSP]);
		}
		break;
	}
	case 0xB4:
	case 0xB5: { // Push
		uint8_t registerList = instruction & 0xFF;
		if ((instruction & 0x100) != 0) {
			_registers[REGSP] -= 4;
			_system._memory.Write32(_registers[REGSP], _registers[REGLR]);
		}
		for (int i = 7; i >= 0; i--) {
			if ((registerList & (1 << i)) != 0) {
				_registers[REGSP] -= 4;
				_system._memory.Write32(_registers[REGSP], _registers[i]);
			}
		}
		break;
	}
	case 0xBC:
	case 0xBD: { // pop
		uint8_t registerList = instruction & 0xFF;
		for (int i = 0; i <= 7; i++) {
			if ((registerList & (1 << i)) != 0) {
				_registers[i] = _system._memory.Read32(_registers[REGSP]);
				_registers[REGSP] += 4;
			}
		}
		if ((instruction & 0x100) != 0) {
			_registers[REGPC] = _system._memory.Read32(_registers[REGSP]);
			_registers[REGPC] &= 0xFFFFFFFE;
			_registers[REGSP] += 4;
			_pipelineInstruction = ARM_NOP;
		}
		break;
	}
	case 0xBE: { // BKPT
		uint8_t imm = instruction & 0xFF;
		throw BreakPointARMException(imm);
	}
	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xC7: { // STMIA rn 
		uint8_t registerList = instruction & 0xFF;
		uint8_t rn = (instruction >> 8) & 0x7;
		uint32_t address = _registers[rn];
		for (int i = 0; i <= 7; i++) {
			if ((registerList & (1 << i)) != 0) {
				_system._memory.Write32(address, _registers[i]);
				address += 4;
			}
		}
		_registers[rn] = address;
		break;
	}
	case 0xC8:
	case 0xC9:
	case 0xCA:
	case 0xCB:
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF: { // LDMIA rn 
		uint8_t registerList = instruction & 0xFF;
		uint8_t rn = (instruction >> 8) & 0x7;
		uint32_t address = _registers[rn];
		for (int i = 0; i <= 7; i++) {
			if ((registerList & (1 << i)) != 0) {
				_registers[i] = _system._memory.Read32(address);
				address += 4;
			}
		}
		_registers[rn] = address;
		break;
	}
	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:
	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	case 0xDC:
	case 0xDD: { // Conditional branch
		uint8_t cond = (instruction >> 8) & 0xF;
		if (IsConditionMet(cond, _hostFlags)) {
			int32_t addr = instruction & 0xFF;
			addr <<= 1;
			if ((addr & 0x100) != 0) addr |= 0xFFFFFE00;
			_registers[REGPC] += addr;
			_pipelineInstruction = ARM_NOP;
		}
		break;
	}
	case 0xDF: { // Software interrupt
		uint8_t imm = instruction & 0xFF;
		SoftwareInterrupt(imm);
		break;
	}
	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7: { // Branch
		int16_t imm = instruction & 0x7FF;
		imm <<= 1;
		if ((imm & 0x800) != 0) imm |= 0xFFFFF000;
		_registers[REGPC] += imm;
		_pipelineInstruction = ARM_NOP;
		break;
	}
	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:
	case 0xF4:
	case 0xF5:
	case 0xF6:
	case 0xF7: { // Branch with link
		int32_t addr = instruction & 0x7FF;
		if ((addr & 0x400) != 0) addr |= 0xFFFFF800;
		addr <<= 12;
		_registers[REGLR] = _registers[REGPC] + addr;
		break;
	}
	case 0xF8:
	case 0xF9:
	case 0xFA:
	case 0xFB:
	case 0xFC:
	case 0xFD:
	case 0xFE:
	case 0xFF: { // Branch with link
		uint32_t imm = instruction & 0x7FF;
		uint32_t pc = _registers[REGPC];
		_registers[REGPC] = _registers[REGLR] + (imm << 1);
		_registers[REGLR] = (pc - 2) | 1;
		_pipelineInstruction = ARM_NOP;
		break;
	}
	}
}
