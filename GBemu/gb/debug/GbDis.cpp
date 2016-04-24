#include "../../util/memDbg.h"

#include "gbDis.h"
#include "../mem/GbMem.h"
#include <cassert>
#include <sstream>
#include <emuall/util/string.h>

#define MAX_OPCODESIZE 3

typedef struct {
	unsigned char opcode[MAX_OPCODESIZE];
	unsigned char mask[MAX_OPCODESIZE];
	char size;
	const char *instr;
} DisassembleInfo_t;

// 0b00111000 r0
// 0b00000111 r1
// 0b00000000 0b00111000 r2
// 0b00000000 0b00000111 r3
// 0b00111000 cc0
// 0b00111000 t0
// 0b00000000 0b00111000 b0
// 0b00110000 dd0
// 0b00110000 ss0
// 0b00000000 0b11111111 e0
// 0b00000000 0b11111111 n0
// 0b00000000 0b11111111 0b11111111 nn0

static const char *namesR[] = { "B", "C", "D", "E", "H", "L", "(HL)", "A" };
static const char *namesCC[] = { "NZ", "Z", "NC", "C", "PO", "PE", "P", "M" };
static const char *namesT[] = { "0x00", "0x08", "0x10", "0x18", "0x20", "0x28", "0x30", "0x38" };
static const char *namesB[] = { "0", "1", "2", "3", "4", "5", "6", "7" };
static const char *namesDD[] = { "BC", "DE", "HL", "SP" };
static const char *namesSS[] = { "BC", "DE", "HL", "AF" };

static const DisassembleInfo_t disassembleInfo[] =
{
	{ { 0x76 }, { 0xFF }, 1, "HALT" },
	{ { 0x40 }, { 0xc0 }, 1, "LD %r0, %r1" },
	{ { 0x06 }, { 0xC7 }, 2, "LD %r0, %n0" },
	{ { 0x36 }, { 0xFF }, 2, "LD (HL), %n0" },
	{ { 0x0A }, { 0xFF }, 1, "LD A, (BC)" },
	{ { 0x1A }, { 0xFF }, 1, "LD A, (DE)" },
	{ { 0x3A }, { 0xFF }, 1, "LD A, (HL-)" },
	{ { 0x02 }, { 0xFF }, 1, "LD (BC), A" },
	{ { 0x12 }, { 0xFF }, 1, "LD (DE), A" },
	{ { 0x32 }, { 0xFF }, 1, "LD (HL-), A" },
	{ { 0x08 }, { 0xFF }, 3, "LD (%nn0), SP" },
	{ { 0x01 }, { 0xCF }, 3, "LD %dd0, %nn0" },
	{ { 0x2A }, { 0xFF }, 1, "LD A, (HL+)" },
	{ { 0x22 }, { 0xFF }, 1, "LD (HL+), A" },
	{ { 0xF9 }, { 0xFF }, 1, "LD SP, HL" },
	{ { 0xE0 }, { 0xFF }, 2, "LD (0xFF00+%n0), A" },
	{ { 0xE2 }, { 0xFF }, 2, "LD (0xFF00+C), A" },
	{ { 0xE8 }, { 0xFF }, 2, "ADD SP, %n0" },
	{ { 0xEA }, { 0xFF }, 3, "LD (%nn0),A" },
	{ { 0xF0 }, { 0xFF }, 2, "LD A, (0xFF00+%n0)" },
	{ { 0xF2 }, { 0xFF }, 1, "LD A, (0xFF00+C)" },
	{ { 0xF8 }, { 0xFF }, 2, "LD HL, SP+%n0" },
	{ { 0xFA }, { 0xFF }, 3, "LD A, (%nn0)" },
	{ { 0xC5 }, { 0xCF }, 1, "PUSH %ss0" },
	{ { 0xC1 }, { 0xCF }, 1, "POP %ss0" },
	{ { 0x80 }, { 0xF8 }, 1, "ADD A, %r1" },
	{ { 0xC6 }, { 0xFF }, 2, "ADD A, %n0" },
	{ { 0x88 }, { 0xF8 }, 1, "ADC A, %r1" },
	{ { 0xCE }, { 0xFF }, 2, "ADC A, %n0" },
	{ { 0x90 }, { 0xF8 }, 1, "SUB %r1" },
	{ { 0xD6 }, { 0xFF }, 2, "SUB %n0" },
	{ { 0x98 }, { 0xF8 }, 1, "SBC A, %r1" },
	{ { 0xDE }, { 0xFF }, 2, "SBC A, %n0" },
	{ { 0xA0 }, { 0xF8 }, 1, "AND %r1" },
	{ { 0xE6 }, { 0xFF }, 2, "AND %n0" },
	{ { 0xB0 }, { 0xF8 }, 1, "OR %r1" },
	{ { 0xF6 }, { 0xFF }, 2, "OR %n0" },
	{ { 0xA8 }, { 0xF8 }, 1, "XOR %r1" },
	{ { 0xEE }, { 0xFF }, 2, "XOR %n0" },
	{ { 0xB8 }, { 0xF8 }, 1, "CP %r1" },
	{ { 0xFE }, { 0xFF }, 2, "CP %n0" },
	{ { 0x04 }, { 0xC7 }, 1, "INC %r0" },
	{ { 0x05 }, { 0xC7 }, 1, "DEC %r0" },
	{ { 0x27 }, { 0xFF }, 1, "DAA" },
	{ { 0x2F }, { 0xFF }, 1, "CPL" },
	{ { 0x3F }, { 0xFF }, 1, "CCF" },
	{ { 0x37 }, { 0xFF }, 1, "SCF" }, 
	{ { 0x00 }, { 0xFF }, 1, "NOP" },
	{ { 0x76 }, { 0xFF }, 1, "HALT" },
	{ { 0xF3 }, { 0xFF }, 1, "DI" },
	{ { 0xFB }, { 0xFF }, 1, "EI" },
	{ { 0x09 }, { 0xCF }, 1, "ADD HL, %dd0" },
	{ { 0x03 }, { 0xCF }, 1, "INC %dd0" },
	{ { 0x0B }, { 0xCF }, 1, "DEC %dd0" },
	{ { 0x07 }, { 0xFF }, 1, "RLCA" },
	{ { 0x17 }, { 0xFF }, 1, "RLA" },
	{ { 0x0F }, { 0xFF }, 1, "RRCA" },
	{ { 0x1F }, { 0xFF }, 1, "RRA" },
	{ { 0xCB, 0x00 }, { 0xFF, 0xF8 }, 2, "RLC %r3" },
	{ { 0xCB, 0x10 }, { 0xFF, 0xF8 }, 2, "RL %r3" },
	{ { 0xCB, 0x08 }, { 0xFF, 0xF8 }, 2, "RRC %r3" },
	{ { 0xCB, 0x18 }, { 0xFF, 0xF8 }, 2, "RR %r3" },
	{ { 0xCB, 0x20 }, { 0xFF, 0xF8 }, 2, "SLA %r3" },
	{ { 0xCB, 0x28 }, { 0xFF, 0xF8 }, 2, "SRA %r3" },
	{ { 0xCB, 0x30 }, { 0xFF, 0xF8 }, 2, "SWAP %r3" },
	{ { 0xCB, 0x38 }, { 0xFF, 0xF8 }, 2, "SRL %r3" },
	{ { 0xCB, 0x40 }, { 0xFF, 0xC0 }, 2, "BIT %b0, %r3" },
	{ { 0xCB, 0xC0 }, { 0xFF, 0xC0 }, 2, "SET %b0, %r3" },
	{ { 0xCB, 0x80 }, { 0xFF, 0xC0 }, 2, "RES %b0, %r3" },
	{ { 0xC3 }, { 0xFF }, 3, "JP %nn0"},
	{ { 0xC2 }, { 0xC7 }, 3, "JP %cc0, %nn0" },
	{ { 0x18 }, { 0xFF }, 2, "JR %e0" },
	{ { 0x38 }, { 0xFF }, 2, "JR C, %e0" },
	{ { 0x30 }, { 0xFF }, 2, "JR NC, %e0" },
	{ { 0x28 }, { 0xFF }, 2, "JR Z, %e0" },
	{ { 0x20 }, { 0xFF }, 2, "JR NZ, %e0" },
	{ { 0xE9 }, { 0xFF }, 1, "JP (HL)" },
	{ { 0x10 }, { 0xFF }, 2, "STOP" },
	{ { 0xCD }, { 0xFF }, 3, "CALL %nn0" },
	{ { 0xC4 }, { 0xC7 }, 3, "CALL %cc0, %nn0" },
	{ { 0xC9 }, { 0xFF }, 1, "RET" },
	{ { 0xC0 }, { 0xC7 }, 1, "RET %cc0" },
	{ { 0xC7 }, { 0xC7 }, 1, "RST %t0" },
	{ { 0xD9 }, { 0xFF }, 1, "RETI" }
};


GbDis::GbDis(Gameboy *master)
{
	_master = master;
}

GbDis::~GbDis()
{

}

char GbDis::Disassemble(const char **rawStr, const char **instrStr, unsigned int address)
{
	if (_master->_mem == NULL)
		return 0;
	// Get mem data
	gbByte data[5];
	for (int i = 0; i < (sizeof(data) / sizeof(data[0])); ++i)
	{
		uint32_t unbankedAddress = _master->_mem->GetUnbankedAddress(address+i);
		if (_master->IsBreakpoint(unbankedAddress))
		{
			data[i] = _master->_breakpoints[unbankedAddress].org;
		}
		else {
			data[i] = _master->_mem->read(address + i);
		}
	}
	// Disassemble memory
	return Disassemble(rawStr, instrStr, data);
}

char GbDis::Disassemble(const char **rawStr, const char **instrStr, const unsigned char *data)
{
	assert(data != NULL);
	std::ostringstream ossRaw;
	std::ostringstream ossInstr;
	// Determine instruction
	static std::string instrBuffer;
	static std::string rawBuffer;
	const DisassembleInfo_t *opcodeInfo = NULL;
	for (int i = 0; i < (sizeof(disassembleInfo) / sizeof(disassembleInfo[0])); ++i)
	{
		if (((*data)&disassembleInfo[i].mask[0]) == disassembleInfo[i].opcode[0])
		{
			// First byte match
			int j;
			for (j = 1; j < disassembleInfo[i].size; ++j)
			{
				if ((data[j] & disassembleInfo[i].mask[j]) != disassembleInfo[i].opcode[j])
					break;
			}
			if (j == disassembleInfo[i].size)
			{
				// Found instruction
				opcodeInfo = &disassembleInfo[i];
				break;
			}
		}
	}
	if (opcodeInfo == NULL)
	{
		instrBuffer = "?";
		ossRaw << HexToString(*data, 2) << " ";
		rawBuffer = ossRaw.str();
		if (rawStr != NULL)
			*rawStr = rawBuffer.c_str();
		if (instrStr != NULL)
			*instrStr = rawBuffer.c_str();
		return 1;
	}

	// Create the raw string
	if (rawStr != NULL)
	{
		for (int i = 0; i < opcodeInfo->size; ++i)
		{
			ossRaw << HexToString(data[i], 2) << " ";
		}
		rawBuffer = ossRaw.str();
		*rawStr = rawBuffer.c_str();
	}

	// Create the instruction string
	if (instrStr != NULL)
	{
		const char *ptr = opcodeInfo->instr;
		while (*ptr)
		{
			if (*ptr == '%')
			{
				int index;
				switch (ptr[1])
				{
				case 'r': // Register
					index = ptr[2] - '0';
					switch (index)
					{
					case 0:
						ossInstr << namesR[(data[0] >> 3) & 7];
						break;
					case 1:
						ossInstr << namesR[data[0] & 7];
						break;
					case 2:
						ossInstr << namesR[(data[1] >> 3) & 7];
						break;
					case 3:
						ossInstr << namesR[data[1] & 7];
						break;
					default:
						Log(Error, "Invalid register index %d", index);
						break;
					}
					ptr += 2;
					break;
				case 'c': // Flag
					if (ptr[2] == 'c' && ptr[3] == '0')
					{
						ossInstr << namesCC[(data[0] >> 3) & 7];
					}
					else {
						Log(Error, "Invalid flag");
					}
					ptr += 3;
					break;
				case 't': // Reset address
					if (ptr[2] == '0')
					{
						ossInstr << namesT[(data[0] >> 3) & 7];
					}
					else {
						Log(Error, "Invalid reset address");
					}
					ptr += 2;
					break;
				case 'b': // Bit
					if (ptr[2] == '0')
					{
						ossInstr << namesB[(data[1] >> 3) & 7];
					}
					else {
						Log(Error, "Invalid bit");
					}
					ptr += 2;
					break;
				case 'd': // RegisterPair
					if (ptr[2] == 'd' && ptr[3] == '0')
					{
						ossInstr << namesDD[(data[0] >> 4) & 3];
					}
					else {
						Log(Error, "Invalid register pair");
					}
					ptr += 3;
					break;
				case 's': // Register pair push
					if (ptr[2] == 's' && ptr[3] == '0')
					{
						ossInstr << namesSS[(data[0] >> 4) & 3];
					}
					else {
						Log(Error, "Invalid register stack pair");
					}
					ptr+=3;
					break;
				case 'e': // Offset
					if (ptr[2] == '0')
					{
						ossInstr << "0x" << HexToString(data[1] + 2);
					}
					else {
						Log(Error, "Invalid offset");
					}
					ptr += 2;
					break;
				case 'n': // number
					if (ptr[2] == '0')
					{
						ossInstr << "0x" << HexToString(data[1]);
						ptr += 1;
					}
					else if (ptr[2] == 'n' && ptr[3] == '0') {
						ossInstr << "0x" << HexToString((data[2] << 8) | data[1]);
						ptr += 2;
					}
					else {
						Log(Error, "Invalid constant");
					}
					ptr += 1;
					break;
				default:
					Log(Error, "Unknown type %c", ptr[2]);
					ptr++;
					break;
				}
			}
			else {
				ossInstr << *ptr;
			}
			ptr++;
		}
		instrBuffer = ossInstr.str();
		*instrStr = instrBuffer.c_str();
	}

	return opcodeInfo->size;	
}
