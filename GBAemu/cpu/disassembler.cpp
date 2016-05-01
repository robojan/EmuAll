
#include <GBAemu/gba.h>
#include <GBAemu/cpu/disassembler.h>
#include <GBAemu/cpu/hostInstructions.h>
#include <GBAemu/util/preprocessor.h>

#include <emuall/util/string.h>
#include <cassert>
#include <cstring>
#include <cstdlib>

static const char *registerNames[16] = { "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "R12", "SP", "LR", "PC" };
static const char *condNames[16] = {"EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC", "HI", "LS", "GE", "LT", "GT", "LE", "", "NV"};

Disassembler::Disassembler(Gba &system) :
	_system(system)
{

}

Disassembler::~Disassembler()
{

}

int Disassembler::Disassemble(uint32_t address, const char **raw, const char **instr)
{
	uint32_t opcode;
	if (_system.GetCpu().IsBreakpoint(address)) {
		opcode = _system.GetCpu().GetBreakpointInstruction(address);
	}
	else {
		opcode = _system.GetMemory().Read32(address);
	}
	if (_system.GetCpu().IsInThumbMode()) {
		opcode &= 0xFFFF;
		if (raw != nullptr) {
			*raw = HexToString(opcode, 4);
		}
		if (instr != nullptr) {
			*instr = DisassembleThumb(address, opcode);
		}
		return 2;
	}
	else {
		if (raw != nullptr) {
			*raw = HexToString(opcode, 8);
		}
		if (instr != nullptr) {
			*instr = DisassembleArm(address, opcode);
		}
		return 4;
	}
}

const char * Disassembler::DisassembleArm(uint32_t address, uint32_t instruction)
{
	char tempBuffer[64];
	tempBuffer[0] = '0';
	tempBuffer[1] = 'x';
	const char *formatStr = GetARMFormatString(instruction);
	int pos = 0;
	int formatPos = 0;
	while (pos < 255 && formatStr[formatPos] != '\0') {
		if (formatStr[formatPos] == '<') {
			// Find len
			int argLen = 1;
			formatPos += 1;
			while (formatStr[formatPos + argLen] != '>') {
				assert(formatStr[formatPos + argLen] != '\0');
				argLen += 1;
			}
			const char *dataStr = nullptr;
			if (strncmp(formatStr + formatPos, "cond", argLen) == 0) {
				uint8_t cond = (instruction >> 28) & 0xF;
				dataStr = condNames[cond];
			}
			else if (strncmp(formatStr + formatPos, "Rd", argLen) == 0) {
				uint8_t rd = (instruction >> 12) & 0xF;
				dataStr = registerNames[rd];
			}
			else if (strncmp(formatStr + formatPos, "Rn", argLen) == 0) {
				uint8_t rn = (instruction >> 16) & 0xF;
				dataStr = registerNames[rn];
			}
			else if (strncmp(formatStr + formatPos, "Rm", argLen) == 0) {
				uint8_t rm = (instruction >> 0) & 0xF;
				dataStr = registerNames[rm];
			}
			else if (strncmp(formatStr + formatPos, "Rs", argLen) == 0) {
				uint8_t rs = (instruction >> 8) & 0xF;
				dataStr = registerNames[rs];
			}
			else if (strncmp(formatStr + formatPos, "shift_imm", argLen) == 0) {
				uint8_t shift_imm = (instruction >> 7) & 0x1F;
				DecToString(tempBuffer + 2, shift_imm);
				dataStr = tempBuffer + 2;
			}
			else if (strncmp(formatStr + formatPos, "immediate", argLen) == 0) {
				uint32_t immed_8 = instruction & 0xFF;
				uint8_t rotate_imm = ((instruction >> 8) & 0xF) * 2;
				ROR(immed_8, rotate_imm, immed_8);
				HexToString(tempBuffer + 2, immed_8);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "signed_immed_24", argLen) == 0) {
				int32_t imm = instruction & 0xFFFFFF;
				if ((imm & 0x800000) != 0) {
					imm |= 0xFF000000;
				}
				imm <<= 2;
				uint32_t b = address + 8 + imm;
				HexToString(tempBuffer + 2, b, 8);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "registers", argLen) == 0) {
				uint16_t registers = instruction & 0xFFFF;
				tempBuffer[2] = '{'; 
				int tempPos = 3;
				for (int i = 0; i < 16; i++) {
					if ((registers & (1 << i)) != 0) {
						const char *reg = registerNames[i];
						strcpy(tempBuffer + tempPos, reg);
						tempPos += strlen(reg);
						tempBuffer[tempPos++] = ',';
					}
				}
				tempBuffer[tempPos - 1] = '}';
				tempBuffer[tempPos] = '\0';
				dataStr = tempBuffer + 2;
			}
			else if (strncmp(formatStr + formatPos, "offset_12", argLen) == 0) {
				uint16_t offset = instruction & 0xFFF;
				HexToString(tempBuffer + 2, offset);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "offset_8", argLen) == 0) {
				uint16_t offset = (instruction & 0xF) | ((instruction >> 4) & 0xF0);
				HexToString(tempBuffer + 2, offset);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "fields", argLen) == 0) {
				uint8_t fields = ((instruction >> 16) & 0xF);
				int tempPos = 2;
				if ((fields & 0x1) != 0) tempBuffer[tempPos++] = 'c';
				if ((fields & 0x2) != 0) tempBuffer[tempPos++] = 'x';
				if ((fields & 0x4) != 0) tempBuffer[tempPos++] = 's';
				if ((fields & 0x8) != 0) tempBuffer[tempPos++] = 'f';
				tempBuffer[tempPos] = '\0';
				dataStr = tempBuffer + 2;
			}
			else if (strncmp(formatStr + formatPos, "imm_24", argLen) == 0) {
				uint32_t offset = instruction & 0xFFFFFF;
				HexToString(tempBuffer + 2, offset);
				dataStr = tempBuffer;
			}
			else {
				dataStr = "<Unk>";
			}

			// Copy the argument to the result
			for (int dataPos = 0; dataStr[dataPos] != '\0'; dataPos++, pos++) {
				assert(pos < 255);
				_buffer[pos] = dataStr[dataPos];
			}

			formatPos += argLen + 1;
		}
		else {
			_buffer[pos] = formatStr[formatPos];
			formatPos++;
			pos++;
		}
	}
	_buffer[pos] = '\0';
	return _buffer;
}

const char * Disassembler::DisassembleThumb(uint32_t address, uint32_t instruction)
{
	char tempBuffer[64];
	tempBuffer[0] = '0';
	tempBuffer[1] = 'x';
	const char *formatStr = GetThumbFormatString(instruction);
	int pos = 0;
	int formatPos = 0;
	while (pos < 255 && formatStr[formatPos] != '\0') {
		if (formatStr[formatPos] == '<') {
			// Find len
			int argLen = 1;
			formatPos += 1;
			while (formatStr[formatPos + argLen] != '>') {
				assert(formatStr[formatPos + argLen] != '\0');
				argLen += 1;
			}
			const char *dataStr = nullptr;
			if (formatStr[formatPos] == 'R') {
				if (formatStr[formatPos+1] <= '9') {
					int offset = formatStr[formatPos + 1] - '0';
					uint8_t reg = (instruction >> offset) & 0x7;
					dataStr = registerNames[reg];
				}
				else if(formatStr[formatPos + 1] == 'd' || formatStr[formatPos + 1] == 'n') {
					uint8_t reg = (instruction & 0x7) | ((instruction >> 4) & 0x8);
					dataStr = registerNames[reg];
				}
				else if (formatStr[formatPos + 1] == 'm') {
					uint8_t reg = ((instruction >> 3) & 0xF);
					dataStr = registerNames[reg];
				}
				else {
					dataStr = "<Unk>";
				}
			}
			else if (strncmp(formatStr + formatPos, "cond", argLen) == 0) {
				uint8_t cond = (instruction >> 8) & 0xF;
				dataStr = condNames[cond];
			}
			else if (strncmp(formatStr + formatPos, "immed_8", argLen) == 0) {
				uint8_t immed_8 = instruction & 0xFF;
				HexToString(tempBuffer + 2, immed_8);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "immed_3", argLen) == 0) {
				uint8_t immed_3 = (instruction >> 6) & 0x7;
				HexToString(tempBuffer + 2, immed_3);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "immed_7", argLen) == 0) {
				uint8_t immed_7 = instruction & 0x7F;
				HexToString(tempBuffer + 2, immed_7);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "immed_5", argLen) == 0) {
				uint8_t immed_5 = (instruction >> 6) & 0x1F;
				HexToString(tempBuffer + 2, immed_5);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "signed_immed_11", argLen) == 0) {
				int32_t imm = instruction & 0x7FF;
				if ((imm & 0x400) != 0) {
					imm |= 0xFFFFF800;
				}
				imm <<= 1;
				uint32_t b = address + 4 + imm;
				HexToString(tempBuffer + 2, b, 8);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "signed_immed_8", argLen) == 0) {
				int32_t imm = instruction & 0xFF;
				if ((imm & 0x80) != 0) {
					imm |= 0xFFFFFF00;
				}
				imm <<= 1;
				uint32_t b = address + 4 + imm;
				HexToString(tempBuffer + 2, b, 8);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "offset_11", argLen) == 0) {
				uint16_t offset = instruction & 0x7FF;
				HexToString(tempBuffer + 2, offset);
				dataStr = tempBuffer;
			}
			else if (strncmp(formatStr + formatPos, "registers", argLen) == 0) {
				uint16_t registers = instruction & 0xFF;
				tempBuffer[2] = '{';
				int tempPos = 3;
				for (int i = 0; i < 8; i++) {
					if ((registers & (1 << i)) != 0) {
						const char *reg = registerNames[i];
						strcpy(tempBuffer + tempPos, reg);
						tempPos += strlen(reg);
						tempBuffer[tempPos++] = ',';
					}
				}
				tempBuffer[tempPos - 1] = '}';
				tempBuffer[tempPos] = '\0';
				dataStr = tempBuffer + 2;
			}
			else if (strncmp(formatStr + formatPos, "registers_pc", argLen) == 0) {
				uint16_t registers = instruction & 0x1FF;
				tempBuffer[2] = '{';
				int tempPos = 3;
				for (int i = 0; i < 8; i++) {
					if ((registers & (1 << i)) != 0) {
						const char *reg = registerNames[i];
						strcpy(tempBuffer + tempPos, reg);
						tempPos += strlen(reg);
						tempBuffer[tempPos++] = ',';
					}
				}
				if ((registers & (1 << 8)) != 0) {
					const char *reg = registerNames[15];
					strcpy(tempBuffer + tempPos, reg);
					tempPos += strlen(reg);
					tempBuffer[tempPos++] = ',';
				}
				tempBuffer[tempPos - 1] = '}';
				tempBuffer[tempPos] = '\0';
				dataStr = tempBuffer + 2;
			}
			else if (strncmp(formatStr + formatPos, "registers_lr", argLen) == 0) {
				uint16_t registers = instruction & 0x1FF;
				tempBuffer[2] = '{';
				int tempPos = 3;
				for (int i = 0; i < 8; i++) {
					if ((registers & (1 << i)) != 0) {
						const char *reg = registerNames[i];
						strcpy(tempBuffer + tempPos, reg);
						tempPos += strlen(reg);
						tempBuffer[tempPos++] = ',';
					}
				}
				if ((registers & (1 << 8)) != 0) {
					const char *reg = registerNames[14];
					strcpy(tempBuffer + tempPos, reg);
					tempPos += strlen(reg);
					tempBuffer[tempPos++] = ',';
				}
				tempBuffer[tempPos - 1] = '}';
				tempBuffer[tempPos] = '\0';
				dataStr = tempBuffer + 2;
			}
			else {
				dataStr = "<Unk>";
			}

			// Copy the argument to the result
			for (int dataPos = 0; dataStr[dataPos] != '\0'; dataPos++, pos++) {
				assert(pos < 255);
				_buffer[pos] = dataStr[dataPos];
			}

			formatPos += argLen + 1;
		}
		else {
			_buffer[pos] = formatStr[formatPos];
			formatPos++;
			pos++;
		}
	}
	_buffer[pos] = '\0';
	return _buffer;
}

const char * Disassembler::GetARMFormatString(uint32_t instruction)
{
	uint16_t code = ((instruction >> 4) & 0xF) | (((instruction >> 20) & 0xFF) << 4);
	switch (code) {
	case 0x0A8:
	case 0x0A0: return "ADC<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x0A1: return "ADC<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x0AA:
	case 0x0A2: return "ADC<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x0A3: return "ADC<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x0AC:
	case 0x0A4: return "ADC<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x0A5: return "ADC<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x0AE:
	case 0x0A6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "ADC<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "ADC<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x0A7: return "ADC<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x0B8:
	case 0x0B0: return "ADC<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x0B1: return "ADC<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x0BA:
	case 0x0B2: return "ADC<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x0B3: return "ADC<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x0BC:
	case 0x0B4: return "ADC<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x0B5: return "ADC<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x0BE:
	case 0x0B6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "ADC<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "ADC<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0xB7: return "ADC<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x2A0) return "ADC<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x2B0) return "ADC<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x088:
	case 0x080: return "ADD<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x081: return "ADD<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x08A:
	case 0x082: return "ADD<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x083: return "ADD<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x08C:
	case 0x084: return "ADD<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x085: return "ADD<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x08E:
	case 0x086: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "ADD<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "ADD<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x087: return "ADD<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x098:
	case 0x090: return "ADD<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x091: return "ADD<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x09A:
	case 0x092: return "ADD<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x093: return "ADD<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x09C:
	case 0x094: return "ADD<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x095: return "ADD<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x09E:
	case 0x096: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "ADD<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "ADD<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x97: return "ADD<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x280) return "ADD<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x290) return "ADD<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x008:
	case 0x000: return "AND<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x001: return "AND<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x00A:
	case 0x002: return "AND<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x003: return "AND<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x00C:
	case 0x004: return "AND<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x005: return "AND<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x00E:
	case 0x006: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "AND<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "AND<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x007: return "AND<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x018:
	case 0x010: return "AND<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x011: return "AND<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x01A:
	case 0x012: return "AND<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x013: return "AND<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x01C:
	case 0x014: return "AND<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x015: return "AND<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x01E:
	case 0x016: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "AND<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "AND<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x17: return "AND<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x200) return "AND<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x210) return "AND<cond>S <Rd>, <Rn>, #<immediate>";
	CASE_RANGE256(0xA00) return "B<cond> <signed_immed_24>";
	CASE_RANGE256(0xB00) return "BL<cond> <signed_immed_24>";
	case 0x1C8:
	case 0x1C0: return "BIC<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x1C1: return "BIC<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x1CA:
	case 0x1C2: return "BIC<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x1C3: return "BIC<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x1CC:
	case 0x1C4: return "BIC<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x1C5: return "BIC<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x1CE:
	case 0x1C6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "BIC<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "BIC<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x1C7: return "BIC<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x1D8:
	case 0x1D0: return "BIC<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x1D1: return "BIC<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x1DA:
	case 0x1D2: return "BIC<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x1D3: return "BIC<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x1DC:
	case 0x1D4: return "BIC<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x1D5: return "BIC<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x1DE:
	case 0x1D6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "BIC<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "BIC<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x1D7: return "BIC<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x3C0) return "BIC<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x3D0) return "BIC<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x127: return "BKPT<cond> <imm_16>";
	case 0x121: return "BX<cond> <Rm>";
	case 0x178:
	case 0x170: return "CMN<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x171: return "CMN<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x17A:
	case 0x172: return "CMN<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x173: return "CMN<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x17C:
	case 0x174: return "CMN<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x175: return "CMN<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x17E:
	case 0x176: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "CMN<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "CMN<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x177: return "CMN<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x370) return "CMN<cond> <Rd>, <Rn>, #<immediate>";
	case 0x158:
	case 0x150: return "CMP<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x151: return "CMP<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x15A:
	case 0x152: return "CMP<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x153: return "CMP<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x15C:
	case 0x154: return "CMP<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x155: return "CMP<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x15E:
	case 0x156: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "CMP<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "CMP<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x157: return "CMP<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x350) return "CMP<cond> <Rd>, <Rn>, #<immediate>";
	case 0x028:
	case 0x020: return "EOR<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x021: return "EOR<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x02A:
	case 0x022: return "EOR<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x023: return "EOR<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x02C:
	case 0x024: return "EOR<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x025: return "EOR<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x02E:
	case 0x026: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "EOR<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "EOR<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x027: return "EOR<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x038:
	case 0x030: return "EOR<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x031: return "EOR<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x03A:
	case 0x032: return "EOR<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x033: return "EOR<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x03C:
	case 0x034: return "EOR<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x035: return "EOR<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x03E:
	case 0x036: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "EOR<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "EOR<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x037: return "EOR<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x220) return "EOR<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x230) return "EOR<cond>S <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x810) return "LDM<cond>DA <Rn>, <registers>";
	CASE_RANGE16(0x830) return "LDM<cond>DA <Rn>!, <registers>";
	CASE_RANGE16(0x890) return "LDM<cond>IA <Rn>, <registers>";
	CASE_RANGE16(0x8B0) return "LDM<cond>IA <Rn>!, <registers>";
	CASE_RANGE16(0x910) return "LDM<cond>DB <Rn>, <registers>";
	CASE_RANGE16(0x930) return "LDM<cond>DB <Rn>!, <registers>";
	CASE_RANGE16(0x990) return "LDM<cond>IB <Rn>, <registers>";
	CASE_RANGE16(0x9B0) return "LDM<cond>IB <Rn>!, <registers>";
	CASE_RANGE16(0x850) return "LDM<cond>DA <Rn>, <registers>^";
	CASE_RANGE16(0x8D0) return "LDM<cond>IA <Rn>, <registers>^";
	CASE_RANGE16(0x950) return "LDM<cond>DB <Rn>, <registers>^";
	CASE_RANGE16(0x9D0) return "LDM<cond>IB <Rn>, <registers>^";
	CASE_RANGE16(0x870) return "LDM<cond>DA <Rn>!, <registers>^";
	CASE_RANGE16(0x8F0) return "LDM<cond>IA <Rn>!, <registers>^";
	CASE_RANGE16(0x970) return "LDM<cond>DB <Rn>!, <registers>^";
	CASE_RANGE16(0x9F0) return "LDM<cond>IB <Rn>!, <registers>^";
	CASE_RANGE16(0x410) return "LDR<cond> <Rd>, [<Rn>], #-<offset_12>";
	CASE_RANGE16(0x490) return "LDR<cond> <Rd>, [<Rn>], #+<offset_12>";
	CASE_RANGE16(0x510) return "LDR<cond> <Rd>, [<Rn>, #-<offset_12>]";
	CASE_RANGE16(0x530) return "LDR<cond> <Rd>, [<Rn>, #-<offset_12>]!";
	CASE_RANGE16(0x590) return "LDR<cond> <Rd>, [<Rn>, #+<offset_12>]";
	CASE_RANGE16(0x5B0) return "LDR<cond> <Rd>, [<Rn>, #+<offset_12>]!";
	case 0x610:
	case 0x618: return "LDR<cond> <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>";
	case 0x612:
	case 0x61A: return "LDR<cond> <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>";
	case 0x614:
	case 0x61C: return "LDR<cond> <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>";
	case 0x616:
	case 0x61E: return "LDR<cond> <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>";
	case 0x690:
	case 0x698: return "LDR<cond> <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>";
	case 0x692:
	case 0x69A: return "LDR<cond> <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>";
	case 0x694:
	case 0x69C: return "LDR<cond> <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>";
	case 0x696:
	case 0x69E: return "LDR<cond> <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>";
	case 0x710:
	case 0x718: return "LDR<cond> <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]";
	case 0x712:
	case 0x71A: return "LDR<cond> <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]";
	case 0x714:
	case 0x71C: return "LDR<cond> <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]";
	case 0x716:
	case 0x71E: return "LDR<cond> <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]";
	case 0x790:
	case 0x798: return "LDR<cond> <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]";
	case 0x792:
	case 0x79A: return "LDR<cond> <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]";
	case 0x794:
	case 0x79C: return "LDR<cond> <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]";
	case 0x796:
	case 0x79E: return "LDR<cond> <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]";
	case 0x730:
	case 0x738: return "LDR<cond> <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]!";
	case 0x732:
	case 0x73A: return "LDR<cond> <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]!";
	case 0x734:
	case 0x73C: return "LDR<cond> <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]!";
	case 0x736:
	case 0x73E: return "LDR<cond> <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]!";
	case 0x7B0:
	case 0x7B8: return "LDR<cond> <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]!";
	case 0x7B2:
	case 0x7BA: return "LDR<cond> <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]!";
	case 0x7B4:
	case 0x7BC: return "LDR<cond> <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]!";
	case 0x7B6:
	case 0x7BE: return "LDR<cond> <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]!";
	CASE_RANGE16(0x450) return "LDR<cond>B <Rd>, [<Rn>], #-<offset_12>";
	CASE_RANGE16(0x4D0) return "LDR<cond>B <Rd>, [<Rn>], #+<offset_12>";
	CASE_RANGE16(0x550) return "LDR<cond>B <Rd>, [<Rn>, #-<offset_12>]";
	CASE_RANGE16(0x570) return "LDR<cond>B <Rd>, [<Rn>, #-<offset_12>]!";
	CASE_RANGE16(0x5D0) return "LDR<cond>B <Rd>, [<Rn>, #+<offset_12>]";
	CASE_RANGE16(0x5F0) return "LDR<cond>B <Rd>, [<Rn>, #+<offset_12>]!";
	case 0x650:
	case 0x658: return "LDR<cond>B <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>";
	case 0x652:
	case 0x65A: return "LDR<cond>B <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>";
	case 0x654:
	case 0x65C: return "LDR<cond>B <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>";
	case 0x656:
	case 0x65E: return "LDR<cond>B <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>";
	case 0x6D0:
	case 0x6D8: return "LDR<cond>B <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>";
	case 0x6D2:
	case 0x6DA: return "LDR<cond>B <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>";
	case 0x6D4:
	case 0x6DC: return "LDR<cond>B <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>";
	case 0x6D6:
	case 0x6DE: return "LDR<cond>B <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>";
	case 0x750:
	case 0x758: return "LDR<cond>B <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]";
	case 0x752:
	case 0x75A: return "LDR<cond>B <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]";
	case 0x754:
	case 0x75C: return "LDR<cond>B <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]";
	case 0x756:
	case 0x75E: return "LDR<cond>B <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]";
	case 0x7D0:
	case 0x7D8: return "LDR<cond>B <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]";
	case 0x7D2:
	case 0x7DA: return "LDR<cond>B <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]";
	case 0x7D4:
	case 0x7DC: return "LDR<cond>B <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]";
	case 0x7D6:
	case 0x7DE: return "LDR<cond>B <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]";
	case 0x770:
	case 0x778: return "LDR<cond>B <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]!";
	case 0x772:
	case 0x77A: return "LDR<cond>B <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]!";
	case 0x774:
	case 0x77C: return "LDR<cond>B <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]!";
	case 0x776:
	case 0x77E: return "LDR<cond>B <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]!";
	case 0x7F0:
	case 0x7F8: return "LDR<cond>B <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]!";
	case 0x7F2:
	case 0x7FA: return "LDR<cond>B <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]!";
	case 0x7F4:
	case 0x7FC: return "LDR<cond>B <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]!";
	case 0x7F6:
	case 0x7FE: return "LDR<cond>B <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]!";
	CASE_RANGE16(0x470) return "LDR<cond>BT <Rd>, [<Rn>], #-<offset_12>";
	CASE_RANGE16(0x4F0) return "LDR<cond>BT <Rd>, [<Rn>], #+<offset_12>";
	case 0x670:
	case 0x678: return "LDR<cond>BT <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>";
	case 0x672:
	case 0x67A: return "LDR<cond>BT <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>";
	case 0x674:
	case 0x67C: return "LDR<cond>BT <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>";
	case 0x676:
	case 0x67E: return "LDR<cond>BT <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>";
	case 0x6F0:
	case 0x6F8: return "LDR<cond>BT <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>";
	case 0x6F2:
	case 0x6FA: return "LDR<cond>BT <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>";
	case 0x6F4:
	case 0x6FC: return "LDR<cond>BT <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>";
	case 0x6F6:
	case 0x6FE: return "LDR<cond>BT <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>";
	case 0x01B: return "LDR<cond>H <Rd>, [<Rn>], -<Rm>";
	case 0x09B: return "LDR<cond>H <Rd>, [<Rn>], +<Rm>";
	case 0x05B: return "LDR<cond>H <Rd>, [<Rn>], #-<offset_8>";
	case 0x0DB: return "LDR<cond>H <Rd>, [<Rn>], #+<offset_8>";
	case 0x11B: return "LDR<cond>H <Rd>, [<Rn>, -<Rm>]";
	case 0x19B: return "LDR<cond>H <Rd>, [<Rn>, +<Rm>]";
	case 0x15B: return "LDR<cond>H <Rd>, [<Rn>, #-<offset_8>]";
	case 0x1DB: return "LDR<cond>H <Rd>, [<Rn>, #+<offset_8>]";
	case 0x13B: return "LDR<cond>H <Rd>, [<Rn>, -<Rm>]!";
	case 0x1BB: return "LDR<cond>H <Rd>, [<Rn>, +<Rm>]!";
	case 0x17B: return "LDR<cond>H <Rd>, [<Rn>, #-<offset_8>]!";
	case 0x1FB: return "LDR<cond>H <Rd>, [<Rn>, #+<offset_8>]!";
	case 0x01D: return "LDR<cond>SB <Rd>, [<Rn>], -<Rm>";
	case 0x09D: return "LDR<cond>SB <Rd>, [<Rn>], +<Rm>";
	case 0x05D: return "LDR<cond>SB <Rd>, [<Rn>], #-<offset_8>";
	case 0x0DD: return "LDR<cond>SB <Rd>, [<Rn>], #+<offset_8>";
	case 0x11D: return "LDR<cond>SB <Rd>, [<Rn>, -<Rm>]";
	case 0x19D: return "LDR<cond>SB <Rd>, [<Rn>, +<Rm>]";
	case 0x15D: return "LDR<cond>SB <Rd>, [<Rn>, #-<offset_8>]";
	case 0x1DD: return "LDR<cond>SB <Rd>, [<Rn>, #+<offset_8>]";
	case 0x13D: return "LDR<cond>SB <Rd>, [<Rn>, -<Rm>]!";
	case 0x1BD: return "LDR<cond>SB <Rd>, [<Rn>, +<Rm>]!";
	case 0x17D: return "LDR<cond>SB <Rd>, [<Rn>, #-<offset_8>]!";
	case 0x1FD: return "LDR<cond>SB <Rd>, [<Rn>, #+<offset_8>]!";
	case 0x01F: return "LDR<cond>SH <Rd>, [<Rn>], -<Rm>";
	case 0x09F: return "LDR<cond>SH <Rd>, [<Rn>], +<Rm>";
	case 0x05F: return "LDR<cond>SH <Rd>, [<Rn>], #-<offset_8>";
	case 0x0DF: return "LDR<cond>SH <Rd>, [<Rn>], #+<offset_8>";
	case 0x11F: return "LDR<cond>SH <Rd>, [<Rn>, -<Rm>]";
	case 0x19F: return "LDR<cond>SH <Rd>, [<Rn>, +<Rm>]";
	case 0x15F: return "LDR<cond>SH <Rd>, [<Rn>, #-<offset_8>]";
	case 0x1DF: return "LDR<cond>SH <Rd>, [<Rn>, #+<offset_8>]";
	case 0x13F: return "LDR<cond>SH <Rd>, [<Rn>, -<Rm>]!";
	case 0x1BF: return "LDR<cond>SH <Rd>, [<Rn>, +<Rm>]!";
	case 0x17F: return "LDR<cond>SH <Rd>, [<Rn>, #-<offset_8>]!";
	case 0x1FF: return "LDR<cond>SH <Rd>, [<Rn>, #+<offset_8>]!";
	CASE_RANGE16(0x430) return "LDR<cond>T <Rd>, [<Rn>], #-<offset_12>";
	CASE_RANGE16(0x4B0) return "LDR<cond>T <Rd>, [<Rn>], #+<offset_12>";
	case 0x630: 
	case 0x638: return "LDR<cond>T <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>";
	case 0x632:
	case 0x63A: return "LDR<cond>T <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>";
	case 0x634:
	case 0x63C: return "LDR<cond>T <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>";
	case 0x63E:
	case 0x636: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "LDR<cond>T <Rd>, [<Rn>], -<Rm>, RRX";
		}
		else {
			return "LDR<cond>T <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>";
		}
	}
	case 0x6B0:
	case 0x6B8: return "LDR<cond>T <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>";
	case 0x6B2:
	case 0x6BA: return "LDR<cond>T <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>";
	case 0x6B4:
	case 0x6BC: return "LDR<cond>T <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>";
	case 0x6BE:
	case 0x6B6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "LDR<cond>T <Rd>, [<Rn>], +<Rm>, RRX";
		}
		else {
			return "LDR<cond>T <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>";
		}
	}
	case 0x029: return "MLA<cond> <Rd>, <Rm>, <Rs>, <Rn>";
	case 0x039: return "MLA<cond>S <Rd>, <Rm>, <Rs>, <Rn>";
	case 0x1A8:
	case 0x1A0: return "MOV<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x1A1: return "MOV<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x1AA:
	case 0x1A2: return "MOV<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x1A3: return "MOV<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x1AC:
	case 0x1A4: return "MOV<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x1A5: return "MOV<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x1AE:
	case 0x1A6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "MOV<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "MOV<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x1A7: return "MOV<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x1B8:
	case 0x1B0: return "MOV<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x1B1: return "MOV<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x1BA:
	case 0x1B2: return "MOV<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x1B3: return "MOV<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x1BC:
	case 0x1B4: return "MOV<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x1B5: return "MOV<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x1BE:
	case 0x1B6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "MOV<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "MOV<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x1B7: return "MOV<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x3A0) return "MOV<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x3B0) return "MOV<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x100: return "MRS<cond> <Rd>, CPSR";
	case 0x140: return "MRS<cond> <Rd>, SPSR";
	CASE_RANGE16(0x320) return "MSR<cond> CPSR_<fields>, #<immediate>";
	CASE_RANGE16(0x360) return "MSR<cond> SPSR_<fields>, #<immediate>";
	case 0x120: return "MSR<cond> CPSR_<fields>, <Rm>";
	case 0x160: return "MSR<cond> SPSR_<fields>, <Rm>";
	case 0x009: return "MUL<cond> <Rd>, <Rm>, <Rs>";
	case 0x019: return "MUL<cond>S <Rd>, <Rm>, <Rs>";
	case 0x1E8:
	case 0x1E0: return "MVN<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x1E1: return "MVN<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x1EA:
	case 0x1E2: return "MVN<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x1E3: return "MVN<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x1EC:
	case 0x1E4: return "MVN<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x1E5: return "MVN<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x1EE:
	case 0x1E6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "MVN<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "MVN<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x1E7: return "MVN<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x1F8:
	case 0x1F0: return "MVN<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x1F1: return "MVN<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x1FA:
	case 0x1F2: return "MVN<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x1F3: return "MVN<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x1FC:
	case 0x1F4: return "MVN<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x1F5: return "MVN<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x1FE:
	case 0x1F6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "MVN<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "MVN<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x1F7: return "MVN<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x3E0) return "MVN<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x3F0) return "MVN<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x188:
	case 0x180: return "ORR<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x181: return "ORR<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x18A:
	case 0x182: return "ORR<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x183: return "ORR<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x18C:
	case 0x184: return "ORR<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x185: return "ORR<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x18E:
	case 0x186: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "ORR<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "ORR<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x187: return "ORR<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x198:
	case 0x190: return "ORR<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x191: return "ORR<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x19A:
	case 0x192: return "ORR<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x193: return "ORR<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x19C:
	case 0x194: return "ORR<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x195: return "ORR<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x19E:
	case 0x196: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "ORR<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "ORR<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x197: return "ORR<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x380) return "ORR<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x390) return "ORR<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x068:
	case 0x060: return "RSB<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x061: return "RSB<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x06A:
	case 0x062: return "RSB<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x063: return "RSB<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x06C:
	case 0x064: return "RSB<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x065: return "RSB<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x06E:
	case 0x066: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "RSB<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "RSB<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x067: return "RSB<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x078:
	case 0x070: return "RSB<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x071: return "RSB<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x07A:
	case 0x072: return "RSB<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x073: return "RSB<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x07C:
	case 0x074: return "RSB<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x075: return "RSB<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x07E:
	case 0x076: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "RSB<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "RSB<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x077: return "RSB<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x260) return "RSB<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x270) return "RSB<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x0E8:
	case 0x0E0: return "RSC<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x0E1: return "RSC<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x0EA:
	case 0x0E2: return "RSC<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x0E3: return "RSC<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x0EC:
	case 0x0E4: return "RSC<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x0E5: return "RSC<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x0EE:
	case 0x0E6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "RSC<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "RSC<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x0E7: return "RSC<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x0F8:
	case 0x0F0: return "RSC<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x0F1: return "RSC<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x0FA:
	case 0x0F2: return "RSC<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x0F3: return "RSC<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x0FC:
	case 0x0F4: return "RSC<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x0F5: return "RSC<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x0FE:
	case 0x0F6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "RSC<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "RSC<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x0F7: return "RSC<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x2E0) return "RSC<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x2F0) return "RSC<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x0C8:
	case 0x0C0: return "SBC<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x0C1: return "SBC<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x0CA:
	case 0x0C2: return "SBC<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x0C3: return "SBC<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x0CC:
	case 0x0C4: return "SBC<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x0C5: return "SBC<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x0CE:
	case 0x0C6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "SBC<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "SBC<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x0C7: return "SBC<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x0D8:
	case 0x0D0: return "SBC<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x0D1: return "SBC<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x0DA:
	case 0x0D2: return "SBC<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x0D3: return "SBC<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x0DC:
	case 0x0D4: return "SBC<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x0D5: return "SBC<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x0DE:
	case 0x0D6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "SBC<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "SBC<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x0D7: return "SBC<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x2C0) return "SBC<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x2D0) return "SBC<cond>S <Rd>, <Rn>, #<immediate>";
	case 0x0E9: return "SMLAL<cond> <RdLo>, <RdHi>, <Rm>, <Rs>";
	case 0x0F9: return "SMLAL<cond>S <RdLo>, <RdHi>, <Rm>, <Rs>";
	case 0x0C9: return "SMULL<cond> <RdLo>, <RdHi>, <Rm>, <Rs>";
	case 0x0D9: return "SMULL<cond>S <RdLo>, <RdHi>, <Rm>, <Rs>";
	CASE_RANGE16(0x800) return "STM<cond>DA <Rn>, <registers>";
	CASE_RANGE16(0x820) return "STM<cond>DA <Rn>!, <registers>";
	CASE_RANGE16(0x880) return "STM<cond>IA <Rn>, <registers>";
	CASE_RANGE16(0x8A0) return "STM<cond>IA <Rn>!, <registers>";
	CASE_RANGE16(0x900) return "STM<cond>DB <Rn>, <registers>";
	CASE_RANGE16(0x920) return "STM<cond>DB <Rn>!, <registers>";
	CASE_RANGE16(0x980) return "STM<cond>IB <Rn>, <registers>";
	CASE_RANGE16(0x9A0) return "STM<cond>IB <Rn>!, <registers>";
	CASE_RANGE16(0x840) return "STM<cond>DA <Rn>, <registers>^";
	CASE_RANGE16(0x860) return "STM<cond>DA <Rn>!, <registers>^";
	CASE_RANGE16(0x8C0) return "STM<cond>IA <Rn>, <registers>^";
	CASE_RANGE16(0x8E0) return "STM<cond>IA <Rn>!, <registers>^";
	CASE_RANGE16(0x940) return "STM<cond>DB <Rn>, <registers>^";
	CASE_RANGE16(0x960) return "STM<cond>DB <Rn>!, <registers>^";
	CASE_RANGE16(0x9C0) return "STM<cond>IB <Rn>, <registers>^";
	CASE_RANGE16(0x9E0) return "STM<cond>IB <Rn>!, <registers>^";
	CASE_RANGE16(0x400) return "STR<cond> <Rd>, [<Rn>], #-<offset_12>";
	CASE_RANGE16(0x480) return "STR<cond> <Rd>, [<Rn>], #+<offset_12>";
	CASE_RANGE16(0x500) return "STR<cond> <Rd>, [<Rn>, #-<offset_12>]";
	CASE_RANGE16(0x520) return "STR<cond> <Rd>, [<Rn>, #-<offset_12>]!";
	CASE_RANGE16(0x580) return "STR<cond> <Rd>, [<Rn>, #+<offset_12>]";
	CASE_RANGE16(0x5A0) return "STR<cond> <Rd>, [<Rn>, #+<offset_12>]!";
	case 0x600:
	case 0x608: return "STR<cond> <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>";
	case 0x602:
	case 0x60A: return "STR<cond> <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>";
	case 0x604:
	case 0x60C: return "STR<cond> <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>";
	case 0x606:
	case 0x60E: return "STR<cond> <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>";
	case 0x680:
	case 0x688: return "STR<cond> <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>";
	case 0x682:
	case 0x68A: return "STR<cond> <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>";
	case 0x684:
	case 0x68C: return "STR<cond> <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>";
	case 0x686:
	case 0x68E: return "STR<cond> <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>";
	case 0x700:
	case 0x708: return "STR<cond> <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]";
	case 0x702:
	case 0x70A: return "STR<cond> <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]";
	case 0x704:
	case 0x70C: return "STR<cond> <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]";
	case 0x706:
	case 0x70E: return "STR<cond> <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]";
	case 0x780:
	case 0x788: return "STR<cond> <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]";
	case 0x782:
	case 0x78A: return "STR<cond> <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]";
	case 0x784:
	case 0x78C: return "STR<cond> <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]";
	case 0x786:
	case 0x78E: return "STR<cond> <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]";
	case 0x720:
	case 0x728: return "STR<cond> <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]!";
	case 0x722:
	case 0x72A: return "STR<cond> <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]!";
	case 0x724:
	case 0x72C: return "STR<cond> <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]!";
	case 0x726:
	case 0x72E: return "STR<cond> <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]!";
	case 0x7A0:
	case 0x7A8: return "STR<cond> <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]!";
	case 0x7A2:
	case 0x7AA: return "STR<cond> <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]!";
	case 0x7A4:
	case 0x7AC: return "STR<cond> <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]!";
	case 0x7A6:
	case 0x7AE: return "STR<cond> <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]!";
	CASE_RANGE16(0x440) return "STR<cond>B <Rd>, [<Rn>], #-<offset_12>";
	CASE_RANGE16(0x4C0) return "STR<cond>B <Rd>, [<Rn>], #+<offset_12>";
	CASE_RANGE16(0x540) return "STR<cond>B <Rd>, [<Rn>, #-<offset_12>]";
	CASE_RANGE16(0x560) return "STR<cond>B <Rd>, [<Rn>, #-<offset_12>]!";
	CASE_RANGE16(0x5C0) return "STR<cond>B <Rd>, [<Rn>, #+<offset_12>]";
	CASE_RANGE16(0x5E0) return "STR<cond>B <Rd>, [<Rn>, #+<offset_12>]!";
	case 0x640:
	case 0x648: return "STR<cond>B <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>";
	case 0x642:
	case 0x64A: return "STR<cond>B <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>";
	case 0x644:
	case 0x64C: return "STR<cond>B <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>";
	case 0x646:
	case 0x64E: return "STR<cond>B <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>";
	case 0x6C0:
	case 0x6C8: return "STR<cond>B <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>";
	case 0x6C2:
	case 0x6CA: return "STR<cond>B <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>";
	case 0x6C4:
	case 0x6CC: return "STR<cond>B <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>";
	case 0x6C6:
	case 0x6CE: return "STR<cond>B <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>";
	case 0x740:
	case 0x748: return "STR<cond>B <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]";
	case 0x742:
	case 0x74A: return "STR<cond>B <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]";
	case 0x744:
	case 0x74C: return "STR<cond>B <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]";
	case 0x746:
	case 0x74E: return "STR<cond>B <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]";
	case 0x7C0:
	case 0x7C8: return "STR<cond>B <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]";
	case 0x7C2:
	case 0x7CA: return "STR<cond>B <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]";
	case 0x7C4:
	case 0x7CC: return "STR<cond>B <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]";
	case 0x7C6:
	case 0x7CE: return "STR<cond>B <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]";
	case 0x760:
	case 0x768: return "STR<cond>B <Rd>, [<Rn>, -<Rm>, LSL #<shift_imm>]!";
	case 0x762:
	case 0x76A: return "STR<cond>B <Rd>, [<Rn>, -<Rm>, LSR #<shift_imm>]!";
	case 0x764:
	case 0x76C: return "STR<cond>B <Rd>, [<Rn>, -<Rm>, ASR #<shift_imm>]!";
	case 0x766:
	case 0x76E: return "STR<cond>B <Rd>, [<Rn>, -<Rm>, ROR #<shift_imm>]!";
	case 0x7E0:
	case 0x7E8: return "STR<cond>B <Rd>, [<Rn>, +<Rm>, LSL #<shift_imm>]!";
	case 0x7E2:
	case 0x7EA: return "STR<cond>B <Rd>, [<Rn>, +<Rm>, LSR #<shift_imm>]!";
	case 0x7E4:
	case 0x7EC: return "STR<cond>B <Rd>, [<Rn>, +<Rm>, ASR #<shift_imm>]!";
	case 0x7E6:
	case 0x7EE: return "STR<cond>B <Rd>, [<Rn>, +<Rm>, ROR #<shift_imm>]!";
	CASE_RANGE16(0x460) return "STR<cond>BT <Rd>, [<Rn>], #-<offset_12>";
	CASE_RANGE16(0x4E0) return "STR<cond>BT <Rd>, [<Rn>], #+<offset_12>";
	case 0x660:
	case 0x668: return "STR<cond>BT <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>";
	case 0x662:
	case 0x66A: return "STR<cond>BT <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>";
	case 0x664:
	case 0x66C: return "STR<cond>BT <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>";
	case 0x666:
	case 0x66E: return "STR<cond>BT <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>";
	case 0x6E0:
	case 0x6E8: return "STR<cond>BT <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>";
	case 0x6E2:
	case 0x6EA: return "STR<cond>BT <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>";
	case 0x6E4:
	case 0x6EC: return "STR<cond>BT <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>";
	case 0x6E6:
	case 0x6EE: return "STR<cond>BT <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>";
	case 0x00B: return "STR<cond>H <Rd>, [<Rn>], -<Rm>";
	case 0x08B: return "STR<cond>H <Rd>, [<Rn>], +<Rm>";
	case 0x04B: return "STR<cond>H <Rd>, [<Rn>], #-<offset_8>";
	case 0x0CB: return "STR<cond>H <Rd>, [<Rn>], #+<offset_8>";
	case 0x10B: return "STR<cond>H <Rd>, [<Rn>, -<Rm>]";
	case 0x18B: return "STR<cond>H <Rd>, [<Rn>, +<Rm>]";
	case 0x14B: return "STR<cond>H <Rd>, [<Rn>, #-<offset_8>]";
	case 0x1CB: return "STR<cond>H <Rd>, [<Rn>, #+<offset_8>]";
	case 0x12B: return "STR<cond>H <Rd>, [<Rn>, -<Rm>]!";
	case 0x1AB: return "STR<cond>H <Rd>, [<Rn>, +<Rm>]!";
	case 0x16B: return "STR<cond>H <Rd>, [<Rn>, #-<offset_8>]!";
	case 0x1EB: return "STR<cond>H <Rd>, [<Rn>, #+<offset_8>]!";
	CASE_RANGE16(0x420) return "STR<cond>T <Rd>, [<Rn>], #-<offset_12>";
	CASE_RANGE16(0x4A0) return "STR<cond>T <Rd>, [<Rn>], #+<offset_12>";
	case 0x620:
	case 0x628: return "STR<cond>T <Rd>, [<Rn>], -<Rm>, LSL #<shift_imm>";
	case 0x622:
	case 0x62A: return "STR<cond>T <Rd>, [<Rn>], -<Rm>, LSR #<shift_imm>";
	case 0x624:
	case 0x62C: return "STR<cond>T <Rd>, [<Rn>], -<Rm>, ASR #<shift_imm>";
	case 0x62E:
	case 0x626: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "STR<cond>T <Rd>, [<Rn>], -<Rm>, RRX";
		}
		else {
			return "STR<cond>T <Rd>, [<Rn>], -<Rm>, ROR #<shift_imm>";
		}
	}
	case 0x6A0:
	case 0x6A8: return "STR<cond>T <Rd>, [<Rn>], +<Rm>, LSL #<shift_imm>";
	case 0x6A2:
	case 0x6AA: return "STR<cond>T <Rd>, [<Rn>], +<Rm>, LSR #<shift_imm>";
	case 0x6A4:
	case 0x6AC: return "STR<cond>T <Rd>, [<Rn>], +<Rm>, ASR #<shift_imm>";
	case 0x6AE:
	case 0x6A6: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "STR<cond>T <Rd>, [<Rn>], +<Rm>, RRX";
		}
		else {
			return "STR<cond>T <Rd>, [<Rn>], +<Rm>, ROR #<shift_imm>";
		}
	}
	case 0x048:
	case 0x040: return "SUB<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x041: return "SUB<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x04A:
	case 0x042: return "SUB<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x043: return "SUB<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x04C:
	case 0x044: return "SUB<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x045: return "SUB<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x04E:
	case 0x046: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "SUB<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "SUB<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x047: return "SUB<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	case 0x058:
	case 0x050: return "SUB<cond>S <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x051: return "SUB<cond>S <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x05A:
	case 0x052: return "SUB<cond>S <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x053: return "SUB<cond>S <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x05C:
	case 0x054: return "SUB<cond>S <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x055: return "SUB<cond>S <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x05E:
	case 0x056: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "SUB<cond>S <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "SUB<cond>S <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x057: return "SUB<cond>S <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x240) return "SUB<cond> <Rd>, <Rn>, #<immediate>";
	CASE_RANGE16(0x250) return "SUB<cond>S <Rd>, <Rn>, #<immediate>";
	CASE_RANGE256(0xF00) return "SWI<cond> <immed_24>";
	case 0x109: return "SWP<cond> <Rd>, <Rm>, [<Rn>]";
	case 0x149: return "SWP<cond>B <Rd>, <Rm>, [<Rn>]";
	case 0x138:
	case 0x130: return "TEQ<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x131: return "TEQ<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x13A:
	case 0x132: return "TEQ<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x133: return "TEQ<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x13C:
	case 0x134: return "TEQ<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x135: return "TEQ<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x13E:
	case 0x136: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "TEQ<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "TEQ<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x137: return "TEQ<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x330) return "TEQ<cond> <Rd>, <Rn>, #<immediate>";
	case 0x118:
	case 0x110: return "TST<cond> <Rd>, <Rn>, <Rm>, LSL #<shift_imm>";
	case 0x111: return "TST<cond> <Rd>, <Rn>, <Rm>, LSL <Rs>";
	case 0x11A:
	case 0x112: return "TST<cond> <Rd>, <Rn>, <Rm>, LSR #<shift_imm>";
	case 0x113: return "TST<cond> <Rd>, <Rn>, <Rm>, LSR <Rs>";
	case 0x11C:
	case 0x114: return "TST<cond> <Rd>, <Rn>, <Rm>, ASR #<shift_imm>";
	case 0x115: return "TST<cond> <Rd>, <Rn>, <Rm>, ASR <Rs>";
	case 0x11E:
	case 0x116: {
		if (((instruction >> 7) & 0x1F) == 0) {
			return "TST<cond> <Rd>, <Rn>, <Rm>, RRX";
		}
		else {
			return "TST<cond> <Rd>, <Rn>, <Rm>, ROR #<shift_imm>";
		}
	}
	case 0x117: return "TST<cond> <Rd>, <Rn>, <Rm>, ROR <Rs>";
	CASE_RANGE16(0x310) return "TST<cond> <Rd>, <Rn>, #<immediate>";
	case 0x0A9: return "UMLAL<cond> <RdLo>, <RdHi>, <Rm>, <Rs>";
	case 0x0B9: return "UMLAL<cond>S <RdLo>, <RdHi>, <Rm>, <Rs>";
	case 0x089: return "UMULL<cond> <RdLo>, <RdHi>, <Rm>, <Rs>";
	case 0x099: return "UMULL<cond>S <RdLo>, <RdHi>, <Rm>, <Rs>";
	default:
		return "Unknown instruction";
	}
}

const char * Disassembler::GetThumbFormatString(uint16_t instruction)
{
	switch ((instruction >> 6) & 0x3FF) {
	case 0x105: return "ADC <R0>, <R3>";
	CASE_RANGE8(0x070) return "ADD <R0>, <R3>, #<immed_3>";
	CASE_RANGE32(0x0C0) return "ADD <R8>, #<immed_8>";
	CASE_RANGE8(0x060) return "ADD <R0>, <R3>, <R6>";
	CASE_RANGE4(0x110) return "ADD <Rd>, <Rm>";
	CASE_RANGE32(0x280) return "ADD <Rd>, PC, #<immed_8> * 4";
	CASE_RANGE32(0x2A0) return "ADD <Rd>, SP, #<immed_8> * 4";
	CASE_RANGE2(0x2C0) return "ADD SP, #<immed_7> * 4";
	case 0x100: return "AND <R0>, <R3>";
	CASE_RANGE32(0x040) return "ASR <R0>, <R3>, #<immed_5>";
	case 0x104: return "ASR <R0>, <R3>";
	CASE_RANGE64(0x340) {
		if ((instruction & 0xF00) == 0xF00) {
			return "SWI <immed_8>";
		}
		else {
			return "B<cond> <signed_immed_8>";
		}
	}
	CASE_RANGE32(0x380) return "B <signed_immed_11>";
	case 0x10E: return "BIC <R0>, <R3>";
	CASE_RANGE4(0x2F8) return "BKPT <immed_8>";
	CASE_RANGE32(0x3C0) return "BL <offset_11>";
	CASE_RANGE32(0x3E0) return "BL <offset_11>";
	CASE_RANGE2(0x11C) return "BX <Rm>";
	case 0x10B: return "CMN <R0>, <R3>";
	CASE_RANGE32(0x0A0) return "CMP <immed_8>";
	case 0x10A: return "CMP <R0>, <R3>";
	CASE_RANGE4(0x114) return "CMP <Rn>, <Rm>";
	case 0x101: return "EOR <R0>, <R3>";
	CASE_RANGE32(0x320) return "LDMIA <R8>!, <registers>";
	CASE_RANGE32(0x1A0) return "LDR <R0>, [<R3>, #<immed_5> * 4]";
	CASE_RANGE8(0x160) return "LDR <R0>, [<R3>, <R6>]";
	CASE_RANGE32(0x120) return "LDR <R8>, [PC, #<immed_8> * 4]";
	CASE_RANGE32(0x260) return "LDR <R8>, [SP, #<immed_8> * 4]";
	CASE_RANGE32(0x1E0) return "LDRB <R0>, [<R3>, #<immed_5>]";
	CASE_RANGE8(0x170) return "LDRB <R0>, [<R3>, <R6>]";
	CASE_RANGE32(0x220) return "LDRH <R0>, [<R3>, #<immed_5> * 2]";
	CASE_RANGE8(0x168) return "LDRH <R0>, [<R3>, <R6>]";
	CASE_RANGE8(0x158) return "LDRSB <R0>, [<R3>, <R6>]";
	CASE_RANGE8(0x178) return "LDRSH <R0>, [<R3>, <R6>]";
	CASE_RANGE32(0x000) return "LSL <R0>, <R3>, #<immed_5>";
	case 0x102: return "LSL <R0>, <R3>";
	CASE_RANGE32(0x020) return "LSR <R0>, <R3>, #<immed_5>";
	case 0x103: return "LSR <R0>, <R3>";
	CASE_RANGE32(0x080) return "MOV <R8>, #<immed_8>";
	CASE_RANGE4(0x118) return "MOV <Rd>, <Rm>";
	case 0x10D: return "MUL <R0>, <R3>";
	case 0x10F: return "MVN <R0>, <R3>";
	case 0x109: return "NEG <R0>, <R3>";
	case 0x10C: return "ORR <R0>, <R3>";
	CASE_RANGE8(0x2F0) return "POP <registers_pc>";
	CASE_RANGE8(0x2D0) return "PUSH <registers_lr>";
	case 0x107: return "ROR <R0>, <R3>";
	case 0x106: return "SBC <R0>, <R3>";
	CASE_RANGE32(0x300) return "STMIA <R8>!, <registers>";
	CASE_RANGE32(0x180) return "STR <R0>, [<R3>, #<immed_5> * 4]";
	CASE_RANGE8(0x140) return "STR <R0>, [<R3>, <R6>]";
	CASE_RANGE32(0x240) return "STR <R8>, [SP, #<immed_8> * 4]";
	CASE_RANGE32(0x1C0) return "STRB <R0>, [<R3>, #<immed_5>]";
	CASE_RANGE8(0x150) return "STRB <R0>, [<R3>, <R6>]";
	CASE_RANGE32(0x200) return "STRH <R0>, [<R3>, #<immed_5> * 2]";
	CASE_RANGE8(0x148) return "STRH <R0>, [<R3>, <R6>]";
	CASE_RANGE8(0x078) return "SUB <R0>, <R3>, #<immed_3>";
	CASE_RANGE32(0x0E0) return "SUB <R8>, #<immed_8>";
	CASE_RANGE8(0x068) return "SUB <R0>, <R3>, <R6>";
	CASE_RANGE2(0x2C2) return "SUB SP, #<immed_7> * 4";
	case 0x108: return "TST <R0>, <R3>";
	default:
		return "Unknown instruction";
	}
}
