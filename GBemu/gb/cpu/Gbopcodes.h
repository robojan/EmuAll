#ifndef _GBOPCODES_H
#define _GBOPCODES_H

inline void GbCpu::handleInstruction(void)
{
	unsigned char opcode = GetOP();
	unsigned int t, t2;
	unsigned char c;

	switch(opcode)
	{
#pragma region 0x00
	case 0x00: // NOP
		cycles = 4;
		break;
	case 0x01: // LD BC, d16
		cycles = 12;
		regC = GetOP();
		regB = GetOP();
		break;
	case 0x02: // LD (BC), A
		cycles = 8;
		this->gb->_mem->write(regBC, regA);
		break;
	case 0x03: // INC BC
		cycles = 8;
		regBC++;
		break;
	case 0x04: // INC B
		cycles = 4;
		t = ((regB & 0xF)+1)&0x10; // halfcarry
		regB++;
		regF = (regF & 0x1F) | ((regB == 0)<<7) | (t << 1);
		break;
	case 0x05: // DEC B
		cycles = 4;
		t = ((regB & 0xF)-1)&0x10; // halfcarry
		regB--;
		regF = (regF & 0x1F) | ((regB == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x06: // LD B, d8
		cycles = 8;
		regB = GetOP();
		break;
	case 0x07: // RLCA
		cycles = 4;
		c = regA;
		regF = (regF &0x0F) | (regA & 0x80) >> 3;
		regA = (regA<<1) | ((c&0x80) >> 7);
		break;
	case 0x08: // LD (a16), SP
		cycles = 20;
		t = GetOP(); t |= GetOP()<<8;
		this->gb->_mem->write(t, regSP&0xFF);
		this->gb->_mem->write(t+1, regSP>>8); 
		break;
	case 0x09: // ADD HL, BC
		cycles = 8;
		t = ((regHL&0xFFF) + (regBC & 0xFFF)) & 0x1000;
		t2 = (regHL + regBC) & 0x10000;
		regF = (regF & 0x8F) | (t >> 7) | (t2 >> 12 );
		regHL += regBC;
		break;
	case 0x0A: // LD A, (BC)
		cycles = 8;
		regA = this->gb->_mem->read(regBC);
		break;
	case 0x0B: // DEC BC
		cycles = 8;
		regBC--;
		break;
	case 0x0C: // INC C
		cycles = 4;
		t = ((regC & 0xF)+1)&0x10; // halfcarry
		regC++;
		regF = (regF & 0x1F) | ((regC == 0)<<7) | (t << 1);
		break;
	case 0x0D: // DEC C
		cycles = 4;
		t = ((regC & 0xF)-1)&0x10; // halfcarry
		regC--;
		regF = (regF & 0x1F) | ((regC == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x0E: // LD C, d8
		cycles = 8;
		regC = GetOP();
		break;
	case 0x0F: // RRCA
		cycles = 4;
		c = regA;
		regF = (regF &0x0F) | (regA & 0x01) << 4;
		regA = (regA>>1) | ((c&0x01)<<7);
		break;
#pragma endregion
#pragma region 0x10

	case 0x10: // STOP 0
		cycles = 0;
		GetOP();
		stopped = true;
		break;
	case 0x11: // LD DE, d16
		cycles = 12;
		regE = GetOP();
		regD = GetOP();
		break;
	case 0x12: // LD (DE), A
		cycles = 8;
		this->gb->_mem->write(regDE, regA);
		break;
	case 0x13: // INC DE
		cycles = 8;
		regDE++;
		break;
	case 0x14: // INC D
		cycles = 4;
		t = ((regD & 0xF)+1)&0x10; // halfcarry
		regD++;
		regF = (regF & 0x1F) | ((regD == 0)<<7) | (t << 1);
		break;
	case 0x15: // DEC D
		cycles = 4;
		t = ((regD & 0xF)-1)&0x10; // halfcarry
		regD--;
		regF = (regF & 0x1F) | ((regD == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x16: // LD D, d8
		cycles = 8;
		regD = GetOP();
		break;
	case 0x17: // RLA
		cycles = 4;
		t = regF;
		regF = (regF &0x0F) | (regA & 0x80) >> 3;
		regA = (regA<<1) | ((t&0x10) >> 4);
		break;
	case 0x18: // JR r8
		cycles = 12;
		regPC += (signed char)GetOP();
		break;
	case 0x19: // ADD HL, DE
		cycles = 8;
		t = ((regHL&0xFFF) + (regDE & 0xFFF)) & 0x1000;
		t2 = (regHL + regDE) & 0x10000;
		regF = (regF & 0x8F) | (t >> 7) | (t2 >> 12 );
		regHL += regDE;
		break;
	case 0x1A: // LD A, (DE)
		cycles = 8;
		regA = this->gb->_mem->read(regDE);
		break;
	case 0x1B: // DEC DE
		cycles = 8;
		regDE--;
		break;
	case 0x1C: // INC E
		cycles = 4;
		t = ((regE & 0xF)+1)&0x10; // halfcarry
		regE++;
		regF = (regF & 0x1F) | ((regE == 0)<<7) | (t << 1);
		break;
	case 0x1D: // DEC E
		cycles = 4;
		t = ((regE & 0xF)-1)&0x10; // halfcarry
		regE--;
		regF = (regF & 0x1F) | ((regE == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x1E: // LD E, d8
		cycles = 8;
		regE = GetOP();
		break;
	case 0x1F: // RRA
		cycles = 4;
		t = regF;
		regF = (regF &0x0F) | (regA & 0x01) << 4;
		regA = (regA>>1) | ((t&0x10) << 3);
		break;
#pragma endregion
#pragma region 0x20

	case 0x20: // JR NZ, r8
		cycles = 8;
		c = GetOP();
		if(regF & F_Z) break;
		cycles += 4;
		regPC += (signed char)c;
		break;
	case 0x21: // LD HL, d16
		cycles = 12;
		regL = GetOP();
		regH = GetOP();
		break;
	case 0x22: // LDI (HL), A
		cycles = 8;
		this->gb->_mem->write(regHL++, regA);
		break;
	case 0x23: // INC HL
		cycles = 8;
		regHL++;
		break;
	case 0x24: // INC H
		cycles = 4;
		t = ((regH & 0xF)+1)&0x10; // halfcarry
		regH++;
		regF = (regF & 0x1F) | ((regH == 0)<<7) | (t << 1);
		break;
	case 0x25: // DEC H
		cycles = 4;
		t = ((regH & 0xF)-1)&0x10; // halfcarry
		regH--;
		regF = (regF & 0x1F) | ((regH == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x26: // LD H, d8
		cycles = 8;
		regH = GetOP();
		break;
	case 0x27: // DAA
		cycles = 4;
		// Finaly it works:D
		t = regA;
		if(regF & F_N)
		{
			if(regF & F_H)
			{
				t = (t-0x06) &0xFF;
			}
			if(regF & F_C)
			{
				t -= 0x60;
			}
		}
		else {
			if((t & 0x0F) > 0x9 || (regF&F_H))
			{
				t+=0x06;
			}
			if(t > 0x9F || regF & F_C)
			{
				t+=0x60;
			}			
		}
		regF &= ~(F_H | F_Z);
		if(t &0x100) regF |= F_C;
		t &= 0xFF;
		if(t==0) regF |= F_Z;

		regA = (unsigned char) t;
		break;
	case 0x28: // JR Z, r8
		cycles = 8;
		c = GetOP();
		if(!(regF & F_Z)) break;
		cycles += 4;
		regPC += (signed char)c;
		break;
	case 0x29: // ADD HL, HL
		cycles = 8;
		t = ((regHL&0xFFF) + (regHL & 0xFFF)) & 0x1000;
		t2 = (regHL + regHL) & 0x10000;
		regF = (regF & 0x8F) | (t >> 7) | (t2 >> 12 );
		regHL += regHL;
		break;
	case 0x2A: // LDI A, (HL)
		cycles = 8;
		regA = this->gb->_mem->read(regHL++);
		break;
	case 0x2B: // DEC HL
		cycles = 8;
		regHL--;
		break;
	case 0x2C: // INC L
		cycles = 4;
		t = ((regL & 0xF)+1)&0x10; // halfcarry
		regL++;
		regF = (regF & 0x1F) | ((regL == 0)<<7) | (t << 1);
		break;
	case 0x2D: // DEC L
		cycles = 4;
		t = ((regL & 0xF)-1)&0x10; // halfcarry
		regL--;
		regF = (regF & 0x1F) | ((regL == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x2E: // LD L, d8
		cycles = 8;
		regL = GetOP();
		break;
	case 0x2F: // CPL
		cycles = 4;
		regF = regF | 0x60;
		regA = ~regA;
		break;
#pragma endregion
#pragma region 0x30

	case 0x30: // JR NC, r8
		cycles = 8;
		c = GetOP();
		if(regF & F_C) break;
		cycles += 4;
		regPC += (signed char)c;
		break;
	case 0x31: // LD SP, d16
		cycles = 12;
		regSP = GetNN();
		break;
	case 0x32: // LDD (HL), A
		cycles = 8;
		this->gb->_mem->write(regHL--, regA);
		break;
	case 0x33: // INC SP
		cycles = 8;
		regSP++;
		break;
	case 0x34: // INC (HL)
		cycles = 12;
		t2 = this->gb->_mem->read(regHL);
		t = ((t2 & 0xF)+1)&0x10; // halfcarry
		this->gb->_mem->write(regHL, (t2+1)&0xFF);
		t2 = (t2+1)&0xFF;
		regF = (regF & 0x1F) | ((t2 == 0)<<7) | (t << 1);
		break;
	case 0x35: // DEC (HL)
		cycles = 12;
		t2 = this->gb->_mem->read(regHL);
		t = ((t2 & 0xF)-1)&0x10; // halfcarry
		this->gb->_mem->write(regHL, t2-1);
		regF = (regF & 0x1F) | ((t2-1 == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x36: // LD (HL), d8
		cycles = 12;
		this->gb->_mem->write(regHL, GetOP());
		break;
	case 0x37: // SCF
		cycles = 4;
		regF = (regF & 0x8F) | F_C;
		break;
	case 0x38: // JR C, r8
		cycles = 8;
		c = GetOP();
		if(!(regF & F_C)) break;
		cycles += 4;
		regPC += (signed char)c;
		break;
	case 0x39: // ADD HL, SP
		cycles = 8;
		t = ((regHL&0xFFF) + (regSP & 0xFFF)) & 0x1000;
		t2 = (regHL + regSP) & 0x10000;
		regF = (regF & 0x8F) | (t >> 7) | (t2 >> 12 );
		regHL += regSP;
		break;
	case 0x3A: // LDD A, (HL)
		cycles = 8;
		regA = this->gb->_mem->read(regHL--);
		break;
	case 0x3B: // DEC SP
		cycles = 8;
		regSP--;
		break;
	case 0x3C: // INC A
		cycles = 4;
		t = ((regA & 0xF)+1)&0x10; // halfcarry
		regA++;
		regF = (regF & 0x1F) | ((regA == 0)<<7) | (t << 1);
		break;
	case 0x3D: // DEC A
		cycles = 4;
		t = ((regA & 0xF)-1)&0x10; // halfcarry
		regA--;
		regF = (regF & 0x1F) | ((regA == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x3E: // LD A, d8
		cycles = 8;
		regA = GetOP();
		break;
	case 0x3F: // CCF
		cycles = 4;
		regF = regF & 0x9F;
		regF ^= F_C;
		break;
#pragma endregion
#pragma region 0x40

	case 0x40: // LD B, B
		cycles = 4;
		regB = regB;
		break;
	case 0x41: // LD B, C
		cycles = 4;
		regB = regC;
		break;
	case 0x42: // LD B, D
		cycles = 4;
		regB = regD;
		break;
	case 0x43: // LD B, E
		cycles = 4;
		regB = regE;
		break;
	case 0x44: // LD B, H
		cycles = 4;
		regB = regH;
		break;
	case 0x45: // LD B, L
		cycles = 4;
		regB = regL;
		break;
	case 0x46: // LD B, (HL)
		cycles = 8;
		regB = this->gb->_mem->read(regHL);
		break;
	case 0x47: // LD B, A
		cycles = 4;
		regB = regA;
		break;
	case 0x48: // LD C, B
		cycles = 4;
		regC = regB;
		break;
	case 0x49: // LD C, C
		cycles = 4;
		regC = regC;
		break;
	case 0x4A: // LD C, D
		cycles = 4;
		regC = regD;
		break;
	case 0x4B: // LD C, E
		cycles = 4;
		regC = regE;
		break;
	case 0x4C: // LD C, H
		cycles = 4;
		regC = regH;
		break;
	case 0x4D: // LD C, L
		cycles = 4;
		regC = regL;
		break;
	case 0x4E: // LD C, (HL)
		cycles = 8;
		regC = this->gb->_mem->read(regHL);
		break;
	case 0x4F: // LD C, A
		cycles = 4;
		regC = regA;
		break;
#pragma endregion
#pragma region 0x50

	case 0x50: // LD D, B
		cycles = 4;
		regD = regB;
		break;
	case 0x51: // LD D, C
		cycles = 4;
		regD = regC;
		break;
	case 0x52: // LD D, D
		cycles = 4;
		regD = regD;
		break;
	case 0x53: // LD D, E
		cycles = 4;
		regD = regE;
		break;
	case 0x54: // LD D, H
		cycles = 4;
		regD = regH;
		break;
	case 0x55: // LD D, L
		cycles = 4;
		regD = regL;
		break;
	case 0x56: // LD D, (HL)
		cycles = 8;
		regD = this->gb->_mem->read(regHL);
		break;
	case 0x57: // LD D, A
		cycles = 4;
		regD = regA;
		break;
	case 0x58: // LD E, B
		cycles = 4;
		regE = regB;
		break;
	case 0x59: // LD E, C
		cycles = 4;
		regE = regC;
		break;
	case 0x5A: // LD E, D
		cycles = 4;
		regE = regD;
		break;
	case 0x5B: // LD E, E
		cycles = 4;
		regE = regE;
		break;
	case 0x5C: // LD E, H
		cycles = 4;
		regE = regH;
		break;
	case 0x5D: // LD E, L
		cycles = 4;
		regE = regL;
		break;
	case 0x5E: // LD E, (HL)
		cycles = 8;
		regE = this->gb->_mem->read(regHL);
		break;
	case 0x5F: // LD E, A
		cycles = 4;
		regE = regA;
		break;
#pragma endregion
#pragma region 0x60

	case 0x60: // LD H, B
		cycles = 4;
		regH = regB;
		break;
	case 0x61: // LD H, C
		cycles = 4;
		regH = regC;
		break;
	case 0x62: // LD H, D
		cycles = 4;
		regH = regD;
		break;
	case 0x63: // LD H, E
		cycles = 4;
		regH = regE;
		break;
	case 0x64: // LD H, H
		cycles = 4;
		regH = regH;
		break;
	case 0x65: // LD H, L
		cycles = 4;
		regH = regL;
		break;
	case 0x66: // LD H, (HL)
		cycles = 8;
		regH = this->gb->_mem->read(regHL);
		break;
	case 0x67: // LD H, A
		cycles = 4;
		regH = regA;
		break;
	case 0x68: // LD L, B
		cycles = 4;
		regL = regB;
		break;
	case 0x69: // LD L, C
		cycles = 4;
		regL = regC;
		break;
	case 0x6A: // LD L, D
		cycles = 4;
		regL = regD;
		break;
	case 0x6B: // LD L, E
		cycles = 4;
		regL = regE;
		break;
	case 0x6C: // LD L, H
		cycles = 4;
		regL = regH;
		break;
	case 0x6D: // LD L, L
		cycles = 4;
		regL = regL;
		break;
	case 0x6E: // LD L, (HL)
		cycles = 8;
		regL = this->gb->_mem->read(regHL);
		break;
	case 0x6F: // LD L, A
		cycles = 4;
		regL = regA;
		break;
#pragma endregion
#pragma region 0x70

	case 0x70: // LD (HL), B
		cycles = 8;
		this->gb->_mem->write(regHL, regB);
		break;
	case 0x71: // LD (HL), C
		cycles = 8;
		this->gb->_mem->write(regHL, regC);
		break;
	case 0x72: // LD (HL), D
		cycles = 8;
		this->gb->_mem->write(regHL, regD);
		break;
	case 0x73: // LD (HL), E
		cycles = 8;
		this->gb->_mem->write(regHL, regE);
		break;
	case 0x74: // LD (HL), H
		cycles = 8;
		this->gb->_mem->write(regHL, regH);
		break;
	case 0x75: // LD (HL), L
		cycles = 8;
		this->gb->_mem->write(regHL, regL);
		break;
	case 0x76: // HALT
		cycles = 0;
		halted = true;
		break;
	case 0x77: // LD (HL), A
		cycles = 8;
		this->gb->_mem->write(regHL, regA);
		break;
	case 0x78: // LD A, B
		cycles = 4;
		regA = regB;
		break;
	case 0x79: // LD A, C
		cycles = 4;
		regA = regC;
		break;
	case 0x7A: // LD A, D
		cycles = 4;
		regA = regD;
		break;
	case 0x7B: // LD A, E
		cycles = 4;
		regA = regE;
		break;
	case 0x7C: // LD A, H
		cycles = 4;
		regA = regH;
		break;
	case 0x7D: // LD A, L
		cycles = 4;
		regA = regL;
		break;
	case 0x7E: // LD A, (HL)
		cycles = 8;
		regA = this->gb->_mem->read(regHL);
		break;
	case 0x7F: // LD A, A
		cycles = 4;
		regA = regA;
		break;
#pragma endregion
#pragma region 0x80
	case 0x80: // ADD A, B
		cycles = 4;
		t = ((regA & 0xF)+(regB & 0xF))&0x10; // halfcarry
		t2 = (regA + regB)& 0x100; // carry
		regA = regA + regB;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x81: // ADD A, C
		cycles = 4;
		t = ((regA & 0xF)+(regC & 0xF))&0x10; // halfcarry
		t2 = (regA + regC)& 0x100; // carry
		regA = regA + regC;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x82: // ADD A, D
		cycles = 4;
		t = ((regA & 0xF)+(regD & 0xF))&0x10; // halfcarry
		t2 = (regA + regD)& 0x100; // carry
		regA = regA + regD;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x83: // ADD A, E
		cycles = 4;
		t = ((regA & 0xF)+(regE & 0xF))&0x10; // halfcarry
		t2 = (regA + regE)& 0x100; // carry
		regA = regA + regE;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x84: // ADD A, H
		cycles = 4;
		t = ((regA & 0xF)+(regH & 0xF))&0x10; // halfcarry
		t2 = (regA + regH)& 0x100; // carry
		regA = regA + regH;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x85: // ADD A, L
		cycles = 4;
		t = ((regA & 0xF)+(regL & 0xF))&0x10; // halfcarry
		t2 = (regA + regL)& 0x100; // carry
		regA = regA + regL;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x86: // ADD A, (HL)
		cycles = 8;
		c = this->gb->_mem->read(regHL);
		t = ((regA & 0xF)+(c & 0xF))&0x10; // halfcarry
		t2 = (regA + c)& 0x100; // carry
		regA = regA + c;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x87: // ADD A, A
		cycles = 4;
		t = ((regA & 0xF)+(regA & 0xF))&0x10; // halfcarry
		t2 = (regA + regA)& 0x100; // carry
		regA = regA + regA;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x88: // ADC A, B
		cycles = 4;
		t = ((regA & 0xF)+(regB & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + regB+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + regB+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x89: // ADC A, C
		cycles = 4;
		t = ((regA & 0xF)+(regC & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + regC+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + regC+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8A: // ADC A, D
		cycles = 4;
		t = ((regA & 0xF)+(regD & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + regD+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + regD+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8B: // ADC A, E
		cycles = 4;
		t = ((regA & 0xF)+(regE & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + regE+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + regE+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8C: // ADC A, H
		cycles = 4;
		t = ((regA & 0xF)+(regH & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + regH+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + regH+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8D: // ADC A, L
		cycles = 4;
		t = ((regA & 0xF)+(regL & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + regL+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + regL+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8E: // ADC A, (HL)
		cycles = 8;
		c = this->gb->_mem->read(regHL);
		t = ((regA & 0xF)+(c & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + c+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + c+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8F: // ADC A, A
		cycles = 4;
		t = ((regA & 0xF)+(regA & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + regA+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + regA+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
#pragma endregion
#pragma region 0x90
	case 0x90: // SUB A, B
		cycles = 4;
		t = ((regA & 0xF)-(regB & 0xF))&0x10; // halfcarry
		t2 = (regA - regB)& 0x100; // carry
		regA = regA - regB;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x91: // SUB A, C
		cycles = 4;
		t = ((regA & 0xF)-(regC & 0xF))&0x10; // halfcarry
		t2 = (regA - regC)& 0x100; // carry
		regA = regA - regC;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x92: // SUB A, D
		cycles = 4;
		t = ((regA & 0xF)-(regD & 0xF))&0x10; // halfcarry
		t2 = (regA - regD)& 0x100; // carry
		regA = regA - regD;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x93: // SUB A, E
		cycles = 4;
		t = ((regA & 0xF)-(regE & 0xF))&0x10; // halfcarry
		t2 = (regA - regE)& 0x100; // carry
		regA = regA - regE;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x94: // SUB A, H
		cycles = 4;
		t = ((regA & 0xF)-(regH & 0xF))&0x10; // halfcarry
		t2 = (regA - regH)& 0x100; // carry
		regA = regA - regH;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x95: // SUB A, L
		cycles = 4;
		t = ((regA & 0xF)-(regL & 0xF))&0x10; // halfcarry
		t2 = (regA - regL)& 0x100; // carry
		regA = regA - regL;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x96: // SUB A, (HL)
		cycles = 8;
		c = this->gb->_mem->read(regHL);
		t = ((regA & 0xF)-(c & 0xF))&0x10; // halfcarry
		t2 = (regA - c)& 0x100; // carry
		regA = regA - c;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x97: // SUB A, A
		cycles = 4;
		t = ((regA & 0xF)-(regA & 0xF))&0x10; // halfcarry
		t2 = (regA - regA)& 0x100; // carry
		regA = regA - regA;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x98: // SBC A, B
		cycles = 4;
		t = ((regA & 0xF)-(regB & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - regB-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - regB-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x99: // SBC A, C
		cycles = 4;
		t = ((regA & 0xF)-(regC & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - regC-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - regC-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9A: // SBC A, D
		cycles = 4;
		t = ((regA & 0xF)-(regD & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - regD-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - regD-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9B: // SBC A, E
		cycles = 4;
		t = ((regA & 0xF)-(regE & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - regE-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - regE-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9C: // SBC A, H
		cycles = 4;
		t = ((regA & 0xF)-(regH & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - regH-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - regH-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9D: // SBC A, L
		cycles = 4;
		t = ((regA & 0xF)-(regL & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - regL-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - regL-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9E: // SBC A, (HL)
		cycles = 8;
		c = this->gb->_mem->read(regHL);
		t = ((regA & 0xF)-(c & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - c-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - c-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9F: // SBC A, A
		cycles = 4;
		t = ((regA & 0xF)-(regA & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - regA-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - regA-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
#pragma endregion
#pragma region 0xA0
	case 0xA0: // AND B
		cycles = 4;
		regA &= regB;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xA1: // AND C
		cycles = 4;
		regA &= regC;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xA2: // AND D
		cycles = 4;
		regA &= regD;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xA3: // AND E
		cycles = 4;
		regA &= regE;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xA4: // AND H
		cycles = 4;
		regA &= regH;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xA5: // AND L
		cycles = 4;
		regA &= regL;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xA6: // AND (HL)
		cycles = 8;
		regA &= this->gb->_mem->read(regHL);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xA7: // AND A
		cycles = 4;
		regA &= regA;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xA8: // XOR B
		cycles = 4;
		regA ^= regB;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xA9: // XOR C
		cycles = 4;
		regA ^= regC;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xAA: // XOR D
		cycles = 4;
		regA ^= regD;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xAB: // XOR E
		cycles = 4;
		regA ^= regE;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xAC: // XOR H
		cycles = 4;
		regA ^= regH;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xAD: // XOR L
		cycles = 4;
		regA ^= regL;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xAE: // XOR (HL)
		cycles = 8;
		regA ^= this->gb->_mem->read(regHL);
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xAF: // XOR A
		cycles = 4;
		regA ^= regA;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
#pragma endregion
#pragma region 0xB0
	case 0xB0: // OR B
		cycles = 4;
		regA |= regB;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xB1: // OR C
		cycles = 4;
		regA |= regC;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xB2: // OR D
		cycles = 4;
		regA |= regD;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xB3: // OR E
		cycles = 4;
		regA |= regE;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xB4: // OR H
		cycles = 4;
		regA |= regH;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xB5: // OR L
		cycles = 4;
		regA |= regL;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xB6: // OR (HL)
		cycles = 8;
		regA |= this->gb->_mem->read(regHL);
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xB7: // OR A
		cycles = 4;
		regA |= regA;
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xB8: // CP B
		cycles = 4;
		t = regA - regB;
		t2 = (regA&0xF)  - (regB &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xB9: // CP C
		cycles = 4;
		t = regA - regC;
		t2 = (regA&0xF)  - (regC &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBA: // CP D
		cycles = 4;
		t = regA - regD;
		t2 = (regA&0xF)  - (regD &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBB: // CP E
		cycles = 4;
		t = regA - regE;
		t2 = (regA&0xF)  - (regE &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBC: // CP H
		cycles = 4;
		t = regA - regH;
		t2 = (regA&0xF)  - (regH &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBD: // CP L
		cycles = 4;
		t = regA - regL;
		t2 = (regA&0xF)  - (regL &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBE: // CP (HL)
		cycles = 8;
		c = this->gb->_mem->read(regHL);
		t = regA - c;
		t2 = (regA&0xF)  - (c &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBF: // CP A
		cycles = 4;
		t = regA - regA;
		t2 = (regA&0xF)  - (regA &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
#pragma endregion
#pragma region 0xC0
	case 0xC0: // RET NZ
		cycles = 8;
		if(regF & F_Z) break;
		cycles += 12;
		regPC = this->gb->_mem->read(regSP) | this->gb->_mem->read(regSP+1)<<8;
		regSP +=2;
		break;
	case 0xC1: // POP BC
		cycles = 12;
		regB = this->gb->_mem->read(regSP+1);
		regC = this->gb->_mem->read(regSP);
		regSP += 2;
		break;
	case 0xC2: // JP NZ, a16
		cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(regF & F_Z) break;
		cycles += 4;
		regPC = t;
		break;
	case 0xC3: // JP a16
		cycles = 16;
		regPC = GetNN();
		break;
	case 0xC4: // CALL NZ, a16
		cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(regF & F_Z) break;
		cycles += 12;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = t;
		break;
	case 0xC5: // PUSH BC
		cycles = 16;
		this->gb->_mem->write(regSP-1, regB);
		this->gb->_mem->write(regSP-2, regC);
		regSP-=2;
		break;
	case 0xC6: // ADD A, d8
		cycles = 8;
		c = GetOP();
		t = ((regA & 0xF)+(c & 0xF))&0x10; // halfcarry
		t2 = (regA + c)& 0x100; // carry
		regA = regA + c;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0xC7: // RST 00H
		cycles = 16;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = 0;
		break;
	case 0xC8: // RET Z
		cycles = 8;
		if(!(regF & F_Z)) break;
		cycles += 12;
		regPC = this->gb->_mem->read(regSP) | this->gb->_mem->read(regSP+1)<<8;
		regSP +=2;
		break;
	case 0xC9: // RET 
		cycles = 16;
		regPC = this->gb->_mem->read(regSP) | this->gb->_mem->read(regSP+1)<<8;
		regSP +=2;
		break;
	case 0xCA: // JP Z, a16
		cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(!(regF & F_Z)) break;
		cycles += 4;
		regPC = t;
		break;
#pragma endregion
#pragma region 0xCB
	case 0xCB: // prefix
		opcode = GetOP();
		switch(opcode)
		{
		case 0x00: //RLC B
			cycles = 8;
			regF = (regF &0x0F) | (regB == 0) << 7 | (regB & 0x80) >> 3;
			regB = (regB<<1) | ((regF&0x10) >> 4);
			break;
		case 0x01: //RLC C
			cycles = 8;
			regF = (regF &0x0F) | (regC == 0) << 7 | (regC & 0x80) >> 3;
			regC = (regC<<1) | ((regF&0x10) >> 4);
			break;
		case 0x02: //RLC D
			cycles = 8;
			regF = (regF &0x0F) | (regD == 0) << 7 | (regD & 0x80) >> 3;
			regD = (regD<<1) | ((regF&0x10) >> 4);
			break;
		case 0x03: //RLC E
			cycles = 8;
			regF = (regF &0x0F) | (regE == 0) << 7 | (regE & 0x80) >> 3;
			regE = (regE<<1) | ((regF&0x10) >> 4);
			break;
		case 0x04: //RLC H
			cycles = 8;
			regF = (regF &0x0F) | (regH == 0) << 7 | (regH & 0x80) >> 3;
			regH = (regH<<1) | ((regF&0x10) >> 4);
			break;
		case 0x05: //RLC L
			cycles = 8;
			regF = (regF &0x0F) | (regL == 0) << 7 | (regL & 0x80) >> 3;
			regL = (regL<<1) | ((regF&0x10) >> 4);
			break;
		case 0x06: //RLC (HL)
			cycles = 16;
			c = this->gb->_mem->read(regHL);
			regF = (regF &0x0F) | (c == 0) << 7 | (c & 0x80) >> 3;
			this->gb->_mem->write(regHL, (c<<1) | ((regF&0x10) >> 4));
			break;
		case 0x07: //RLC A
			cycles = 8;
			regF = (regF &0x0F) | (regA == 0) << 7 | (regA & 0x80) >> 3;
			regA = (regA<<1) | ((regF&0x10) >> 4);
			break;
		case 0x08: // RRC B
			cycles = 8;
			regF = (regF &0x0F) | (regB == 0) << 7 | (regB & 0x01) << 4;
			regB = (regB>>1) | ((regF&0x10) << 3);
			break;
		case 0x09: // RRC C
			cycles = 8;
			regF = (regF &0x0F) | (regC == 0) << 7 | (regC & 0x01) << 4;
			regC = (regC>>1) | ((regF&0x10) << 3);
			break;
		case 0x0A: // RRC D
			cycles = 8;
			regF = (regF &0x0F) | (regD == 0) << 7 | (regD & 0x01) << 4;
			regD = (regD>>1) | ((regF&0x10) << 3);
			break;
		case 0x0B: // RRC E
			cycles = 8;
			regF = (regF &0x0F) | (regE == 0) << 7 | (regE & 0x01) << 4;
			regE = (regE>>1) | ((regF&0x10) << 3);
			break;
		case 0x0C: // RRC H
			cycles = 8;
			regF = (regF &0x0F) | (regH == 0) << 7 | (regH & 0x01) << 4;
			regH = (regH>>1) | ((regF&0x10) << 3);
			break;
		case 0x0D: // RRC L
			cycles = 8;
			regF = (regF &0x0F) | (regL == 0) << 7 | (regL & 0x01) << 4;
			regL = (regL>>1) | ((regF&0x10) << 3);
			break;
		case 0x0E: // RRC (HL)
			cycles = 16;
			c = this->gb->_mem->read(regHL);
			t = c;
			c = (c>>1)|((t&0x01)<<7);
			regF = (regF &0x0F) | (c == 0) << 7 | (t & 0x01) << 4;
			this->gb->_mem->write(regHL,c);
			break;
		case 0x0F: // RRC A
			cycles = 8;
			regF = (regF &0x0F) | (regA == 0) << 7 | (regA & 0x01) << 4;
			regA = (regA>>1) | ((regF&0x10) << 3);
			break;
		case 0x10: // RL B
			cycles = 8;
			t = regB;
			regB = (regB<<1) | ((regF&0x10) >> 4);
			regF = (regF &0x0F) | (regB == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x11: // RL C
			cycles = 8;
			t = regC;
			regC = (regC<<1) | ((regF&0x10) >> 4);
			regF = (regF &0x0F) | (regC == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x12: // RL D
			cycles = 8;
			t = regD;
			regD = (regD<<1) | ((regF&0x10) >> 4);
			regF = (regF &0x0F) | (regD == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x13: // RL E
			cycles = 8;
			t = regE;
			regE = (regE<<1) | ((regF&0x10) >> 4);
			regF = (regF &0x0F) | (regE == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x14: // RL H
			cycles = 8;
			t = regH;
			regH = (regH<<1) | ((regF&0x10) >> 4);
			regF = (regF &0x0F) | (regH == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x15: // RL L
			cycles = 8;
			t = regL;
			regL = (regL<<1) | ((regF&0x10) >> 4);
			regF = (regF &0x0F) | (regL == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x16: // RL (HL)
			cycles = 16;
			c = this->gb->_mem->read(regHL);
			t = c;
			c =  (c<<1) | ((regF&0x10) >> 4);
			regF = (regF &0x0F) | (c == 0) << 7 | (t & 0x80) >> 3;
			this->gb->_mem->write(regHL, c);
			break;
		case 0x17: // RL A
			cycles = 8;
			t = regA;
			regA = (regA<<1) | ((regF&0x10) >> 4);
			regF = (regF &0x0F) | (regA == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x18: // RR B
			cycles = 8;
			t = regB;
			regB = (regB>>1) | ((regF&0x10) << 3);
			regF = (regF &0x0F) | (regB == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x19: // RR C
			cycles = 8;
			t = regC;
			regC = (regC>>1) | ((regF&0x10) << 3);
			regF = (regF &0x0F) | (regC == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1A: // RR D
			cycles = 8;
			t = regD;
			regD = (regD>>1) | ((regF&0x10) << 3);
			regF = (regF &0x0F) | (regD == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1B: // RR E
			cycles = 8;
			t = regE;
			regE = (regE>>1) | ((regF&0x10) << 3);
			regF = (regF &0x0F) | (regE == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1C: // RR H
			cycles = 8;
			t = regH;
			regH = (regH>>1) | ((regF&0x10) << 3);
			regF = (regF &0x0F) | (regH == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1D: // RR L
			cycles = 8;
			t = regL;
			regL = (regL>>1) | ((regF&0x10) << 3);
			regF = (regF &0x0F) | (regL == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1E: // RR (HL)
			cycles = 16;
			c = this->gb->_mem->read(regHL);
			t = c;
			c = (c>>1) | ((regF&0x10) << 3);
			regF = (regF &0x0F) | (c == 0) << 7 | (t & 0x01) << 4;
			this->gb->_mem->write(regHL, c);
			break;
		case 0x1F: // RR A
			cycles = 8;
			t = regA;
			regA = (regA>>1) | ((regF&0x10) << 3);
			regF = (regF &0x0F) | (regA == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x20: // SLA B
			cycles = 8;
			c = regB;
			regB <<= 1;
			regF = (regF &0x0F) | (regB == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x21: // SLA C
			cycles = 8;
			c = regC;
			regC <<= 1;
			regF = (regF &0x0F) | (regC == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x22: // SLA D
			cycles = 8;
			c = regD;
			regD <<= 1;
			regF = (regF &0x0F) | (regD == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x23: // SLA E
			cycles = 8;
			c = regE;
			regE <<= 1;
			regF = (regF &0x0F) | (regE == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x24: // SLA H
			cycles = 8;
			c = regH;
			regH <<= 1;
			regF = (regF &0x0F) | (regH == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x25: // SLA L
			cycles = 8;
			c = regL;
			regL <<= 1;
			regF = (regF &0x0F) | (regL == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x26: // SLA (HL)
			cycles = 16;
			c = this->gb->_mem->read(regHL);
			t= c;
			c = c<<1;
			regF = (regF &0x0F) | (c == 0) << 7 | (t & 0x80) >> 3;
			this->gb->_mem->write(regHL, c);
			break;
		case 0x27: // SLA A
			cycles = 8;
			c = regA;
			regA <<= 1;
			regF = (regF &0x0F) | (regA == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x28: // SRA B
			cycles = 8;
			c = regB & 0x01;
			regB >>= 1;
			regB |= (regB<<1)&0x80;
			regF= (regF & 0x0F) | (regB == 0) << 7 | (c<<4);
			break;
		case 0x29: // SRA C
			cycles = 8;
			c = regC & 0x01;
			regC >>= 1;
			regC |= (regC<<1)&0x80;
			regF= (regF & 0x0F) | (regC == 0) << 7 | (c<<4);
			break;
		case 0x2A: // SRA D
			cycles = 8;
			c = regD & 0x01;
			regD >>= 1;
			regD |= (regD<<1)&0x80;
			regF= (regF & 0x0F) | (regD == 0) << 7 | (c<<4);
			break;
		case 0x2B: // SRA E
			cycles = 8;
			c = regE & 0x01;
			regE >>= 1;
			regE |= (regE<<1)&0x80;
			regF= (regF & 0x0F) | (regE == 0) << 7 | (c<<4);
			break;
		case 0x2C: // SRA H
			cycles = 8;
			c = regH & 0x01;
			regH >>= 1;
			regH |= (regH<<1)&0x80;
			regF= (regF & 0x0F) | (regH == 0) << 7 | (c<<4);
			break;
		case 0x2D: // SRA L
			cycles = 8;
			c = regL & 0x01;
			regL >>= 1;
			regL |= (regL<<1)&0x80;
			regF= (regF & 0x0F) | (regL == 0) << 7 | (c<<4);
			break;
		case 0x2E: // SRA (HL)
			cycles = 16;
			c = this->gb->_mem->read(regHL);
			t = c;
			c = c >> 1;
			c |= t&0x80;
			this->gb->_mem->write(regHL, c);
			regF= (regF & 0x0F) | (c == 0) << 7 | ((t&0x01)<<4);
			break;
		case 0x2F: // SRA A
			cycles = 8;
			c = regA & 0x01;
			regA >>= 1;
			regA |= (regA<<1)&0x80;
			regF= (regF & 0x0F) | (regA == 0) << 7 | (c<<4);
			break;
		case 0x30: // SWAP B
			cycles = 8;
			c = regB;
			regB = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			regF= (regF & 0x0F) | (regB == 0) << 7;
			break;
		case 0x31: // SWAP C
			cycles = 8;
			c = regC;
			regC = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			regF= (regF & 0x0F) | (regC == 0) << 7;
			break;
		case 0x32: // SWAP D
			cycles = 8;
			c = regD;
			regD = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			regF = (regF & 0x0F) | (regD == 0) << 7;
			break;
		case 0x33: // SWAP E
			cycles = 8;
			c = regE;
			regE = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			regF= (regF & 0x0F) | (regE == 0) << 7;
			break;
		case 0x34: // SWAP H
			cycles = 8;
			c = regH;
			regH = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			regF= (regF & 0x0F) | (regH == 0) << 7;
			break;
		case 0x35: // SWAP L
			cycles = 8;
			c = regL;
			regL = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			regF = (regF & 0x0F) | (regL == 0) << 7;
			break;
		case 0x36: // SWAP (HL)
			cycles = 16;
			c = this->gb->_mem->read(regHL);
			this->gb->_mem->write(regHL, ((c & 0xF)<<4) | ((c & 0xF0) >> 4));
			regF= (regF & 0x0F) | (c == 0) << 7;
			break;
		case 0x37: // SWAP A
			cycles = 8;
			c = regA;
			regA = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			regF= (regF & 0x0F) | (regA == 0) << 7;
			break;
		case 0x38: // SRL B
			cycles = 8;
			c = regB&1;
			regB >>= 1;
			regF= (regF & 0x0F) | (regB == 0) << 7 | (c<<4);
			break;
		case 0x39: // SRL C
			cycles = 8;
			c = regC &1;
			regC >>= 1;
			regF= (regF & 0x0F) | (regC == 0) << 7 | (c<<4);
			break;
		case 0x3A: // SRL D
			cycles = 8;
			c = regD &1;
			regD >>= 1;
			regF= (regF & 0x0F) | (regD == 0) << 7 | (c<<4);
			break;
		case 0x3B: // SRL E
			cycles = 8;
			c = regE &1;
			regE >>= 1;
			regF= (regF & 0x0F) | (regE == 0) << 7 | (c<<4);
			break;
		case 0x3C: // SRL H
			cycles = 8;
			c = regH &1;
			regH >>= 1;
			regF= (regF & 0x0F) | (regH == 0) << 7 | (c<<4);
			break;
		case 0x3D: // SRL L
			cycles = 8;
			c = regL &1;
			regL >>= 1;
			regF= (regF & 0x0F) | (regL == 0) << 7 | (c<<4);
			break;
		case 0x3E: // SRL (HL)
			cycles = 16;
			c = this->gb->_mem->read(regHL);
			t = c&1;
			this->gb->_mem->write(regHL, c >> 1);
			regF= (regF & 0x0F) | ((c>>1) == 0) << 7 | (t<<4);
			break;
		case 0x3F: // SRL A
			cycles = 8;
			c = regA &1;
			regA >>= 1;
			regF= (regF & 0x0F) | (regA == 0) << 7 | (c<<4);
			break;

		case 0x40: // BIT 0, B
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regB & (1<<0)) == 0) << 7;
			break;
		case 0x41: // BIT 0, C
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regC & (1<<0)) == 0) << 7;
			break;
		case 0x42: // BIT 0, D
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regD & (1<<0)) == 0) << 7;
			break;
		case 0x43: // BIT 0, E
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regE & (1<<0)) == 0) << 7;
			break;
		case 0x44: // BIT 0, H
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regH & (1<<0)) == 0) << 7;
			break;
		case 0x45: // BIT 0, L
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regL & (1<<0)) == 0) << 7;
			break;
		case 0x46: // BIT 0, (HL)
			cycles = 12;
			regF = (regF & 0x1F) | F_H | ((this->gb->_mem->read(regHL) & (1<<0)) == 0) << 7;
			break;
		case 0x47: // BIT 0, A
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regA & (1<<0)) == 0) << 7;
			break;
		case 0x48: // BIT 1, B
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regB & (1<<1)) == 0) << 7;
			break;
		case 0x49: // BIT 1, C
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regC & (1<<1)) == 0) << 7;
			break;
		case 0x4A: // BIT 1, D
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regD & (1<<1)) == 0) << 7;
			break;
		case 0x4B: // BIT 1, E
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regE & (1<<1)) == 0) << 7;
			break;
		case 0x4C: // BIT 1, H
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regH & (1<<1)) == 0) << 7;
			break;
		case 0x4D: // BIT 1, L
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regL & (1<<1)) == 0) << 7;
			break;
		case 0x4E: // BIT 1, (HL)
			cycles = 12;
			regF = (regF & 0x1F) | F_H | ((this->gb->_mem->read(regHL) & (1<<1)) == 0) << 7;
			break;
		case 0x4F: // BIT 1, A
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regA & (1<<1)) == 0) << 7;
			break;

			
		case 0x50: // BIT 2, B
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regB & (1<<2)) == 0) << 7;
			break;
		case 0x51: // BIT 2, C
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regC & (1<<2)) == 0) << 7;
			break;
		case 0x52: // BIT 2, D
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regD & (1<<2)) == 0) << 7;
			break;
		case 0x53: // BIT 2, E
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regE & (1<<2)) == 0) << 7;
			break;
		case 0x54: // BIT 2, H
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regH & (1<<2)) == 0) << 7;
			break;
		case 0x55: // BIT 2, L
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regL & (1<<2)) == 0) << 7;
			break;
		case 0x56: // BIT 2, (HL)
			cycles = 12;
			regF = (regF & 0x1F) | F_H | ((this->gb->_mem->read(regHL) & (1<<2)) == 0) << 7;
			break;
		case 0x57: // BIT 2, A
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regA & (1<<2)) == 0) << 7;
			break;
		case 0x58: // BIT 3, B
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regB & (1<<3)) == 0) << 7;
			break;
		case 0x59: // BIT 3, C
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regC & (1<<3)) == 0) << 7;
			break;
		case 0x5A: // BIT 3, D
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regD & (1<<3)) == 0) << 7;
			break;
		case 0x5B: // BIT 3, E
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regE & (1<<3)) == 0) << 7;
			break;
		case 0x5C: // BIT 3, H
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regH & (1<<3)) == 0) << 7;
			break;
		case 0x5D: // BIT 3, L
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regL & (1<<3)) == 0) << 7;
			break;
		case 0x5E: // BIT 3, (HL)
			cycles = 12;
			regF = (regF & 0x1F) | F_H | ((this->gb->_mem->read(regHL) & (1<<3)) == 0) << 7;
			break;
		case 0x5F: // BIT 3, A
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regA & (1<<3)) == 0) << 7;
			break;

		case 0x60: // BIT 4, B
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regB & (1<<4)) == 0) << 7;
			break;
		case 0x61: // BIT 4, C
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regC & (1<<4)) == 0) << 7;
			break;
		case 0x62: // BIT 4, D
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regD & (1<<4)) == 0) << 7;
			break;
		case 0x63: // BIT 4, E
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regE & (1<<4)) == 0) << 7;
			break;
		case 0x64: // BIT 4, H
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regH & (1<<4)) == 0) << 7;
			break;
		case 0x65: // BIT 4, L
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regL & (1<<4)) == 0) << 7;
			break;
		case 0x66: // BIT 4, (HL)
			cycles = 12;
			regF = (regF & 0x1F) | F_H | ((this->gb->_mem->read(regHL) & (1<<4)) == 0) << 7;
			break;
		case 0x67: // BIT 4, A
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regA & (1<<4)) == 0) << 7;
			break;
		case 0x68: // BIT 5, B
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regB & (1<<5)) == 0) << 7;
			break;
		case 0x69: // BIT 5, C
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regC & (1<<5)) == 0) << 7;
			break;
		case 0x6A: // BIT 5, D
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regD & (1<<5)) == 0) << 7;
			break;
		case 0x6B: // BIT 5, E
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regE & (1<<5)) == 0) << 7;
			break;
		case 0x6C: // BIT 5, H
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regH & (1<<5)) == 0) << 7;
			break;
		case 0x6D: // BIT 5, L
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regL & (1<<5)) == 0) << 7;
			break;
		case 0x6E: // BIT 5, (HL)
			cycles = 12;
			regF = (regF & 0x1F) | F_H | ((this->gb->_mem->read(regHL) & (1<<5)) == 0) << 7;
			break;
		case 0x6F: // BIT 5, A
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regA & (1<<5)) == 0) << 7;
			break;

			
		case 0x70: // BIT 6, B
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regB & (1<<6)) == 0) << 7;
			break;
		case 0x71: // BIT 6, C
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regC & (1<<6)) == 0) << 7;
			break;
		case 0x72: // BIT 6, D
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regD & (1<<6)) == 0) << 7;
			break;
		case 0x73: // BIT 6, E
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regE & (1<<6)) == 0) << 7;
			break;
		case 0x74: // BIT 6, H
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regH & (1<<6)) == 0) << 7;
			break;
		case 0x75: // BIT 6, L
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regL & (1<<6)) == 0) << 7;
			break;
		case 0x76: // BIT 6, (HL)
			cycles = 12;
			regF = (regF & 0x1F) | F_H | ((this->gb->_mem->read(regHL) & (1<<6)) == 0) << 7;
			break;
		case 0x77: // BIT 6, A
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regA & (1<<6)) == 0) << 7;
			break;
		case 0x78: // BIT 7, B
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regB & (1<<7)) == 0) << 7;
			break;
		case 0x79: // BIT 7, C
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regC & (1<<7)) == 0) << 7;
			break;
		case 0x7A: // BIT 7, D
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regD & (1<<7)) == 0) << 7;
			break;
		case 0x7B: // BIT 7, E
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regE & (1<<7)) == 0) << 7;
			break;
		case 0x7C: // BIT 7, H
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regH & (1<<7)) == 0) << 7;
			break;
		case 0x7D: // BIT 7, L
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regL & (1<<7)) == 0) << 7;
			break;
		case 0x7E: // BIT 7, (HL)
			cycles = 12;
			regF = (regF & 0x1F) | F_H | ((this->gb->_mem->read(regHL) & (1<<7)) == 0) << 7;
			break;
		case 0x7F: // BIT 7, A
			cycles = 8;
			regF = (regF & 0x1F) | F_H | ((regA & (1<<7)) == 0) << 7;
			break;

		case 0x80: // RES 0, B
			cycles = 8;
			regB &= ~(1<<0);
			break;
		case 0x81: // RES 0, C
			cycles = 8;
			regC &= ~(1<<0);
			break;
		case 0x82: // RES 0, D
			cycles = 8;
			regD &= ~(1<<0);
			break;
		case 0x83: // RES 0, E
			cycles = 8;
			regE &= ~(1<<0);
			break;
		case 0x84: // RES 0, H
			cycles = 8;
			regH &= ~(1<<0);
			break;
		case 0x85: // RES 0, L
			cycles = 8;
			regL &= ~(1<<0);
			break;
		case 0x86: // RES 0, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) & ~(1<<0));
			break;
		case 0x87: // RES 0, A
			cycles = 8;
			regA &= ~(1<<0);
			break;
		case 0x88: // RES 1, B
			cycles = 8;
			regB &= ~(1<<1);
			break;
		case 0x89: // RES 1, C
			cycles = 8;
			regC &= ~(1<<1);
			break;
		case 0x8A: // RES 1, D
			cycles = 8;
			regD &= ~(1<<1);
			break;
		case 0x8B: // RES 1, E
			cycles = 8;
			regE &= ~(1<<1);
			break;
		case 0x8C: // RES 1, H
			cycles = 8;
			regH &= ~(1<<1);
			break;
		case 0x8D: // RES 1, L
			cycles = 8;
			regL &= ~(1<<1);
			break;
		case 0x8E: // RES 1, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) & ~(1<<1));
			break;
		case 0x8F: // RES 1, A
			cycles = 8;
			regA &= ~(1<<1);
			break;

		case 0x90: // RES 2, B
			cycles = 8;
			regB &= ~(1<<2);
			break;
		case 0x91: // RES 2, C
			cycles = 8;
			regC &= ~(1<<2);
			break;
		case 0x92: // RES 2, D
			cycles = 8;
			regD &= ~(1<<2);
			break;
		case 0x93: // RES 2, E
			cycles = 8;
			regE &= ~(1<<2);
			break;
		case 0x94: // RES 2, H
			cycles = 8;
			regH &= ~(1<<2);
			break;
		case 0x95: // RES 2, L
			cycles = 8;
			regL &= ~(1<<2);
			break;
		case 0x96: // RES 2, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) & ~(1<<2));
			break;
		case 0x97: // RES 2, A
			cycles = 8;
			regA &= ~(1<<2);
			break;
		case 0x98: // RES 3, B
			cycles = 8;
			regB &= ~(1<<3);
			break;
		case 0x99: // RES 3, C
			cycles = 8;
			regC &= ~(1<<3);
			break;
		case 0x9A: // RES 3, D
			cycles = 8;
			regD &= ~(1<<3);
			break;
		case 0x9B: // RES 3, E
			cycles = 8;
			regE &= ~(1<<3);
			break;
		case 0x9C: // RES 3, H
			cycles = 8;
			regH &= ~(1<<3);
			break;
		case 0x9D: // RES 3, L
			cycles = 8;
			regL &= ~(1<<3);
			break;
		case 0x9E: // RES 3, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) & ~(1<<3));
			break;
		case 0x9F: // RES 3, A
			cycles = 8;
			regA &= ~(1<<3);
			break;

		case 0xA0: // RES 4, B
			cycles = 8;
			regB &= ~(1<<4);
			break;
		case 0xA1: // RES 4, C
			cycles = 8;
			regC &= ~(1<<4);
			break;
		case 0xA2: // RES 4, D
			cycles = 8;
			regD &= ~(1<<4);
			break;
		case 0xA3: // RES 4, E
			cycles = 8;
			regE &= ~(1<<4);
			break;
		case 0xA4: // RES 4, H
			cycles = 8;
			regH &= ~(1<<4);
			break;
		case 0xA5: // RES 4, L
			cycles = 8;
			regL &= ~(1<<4);
			break;
		case 0xA6: // RES 4, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) & ~(1<<4));
			break;
		case 0xA7: // RES 4, A
			cycles = 8;
			regA &= ~(1<<4);
			break;
		case 0xA8: // RES 5, B
			cycles = 8;
			regB &= ~(1<<5);
			break;
		case 0xA9: // RES 5, C
			cycles = 8;
			regC &= ~(1<<5);
			break;
		case 0xAA: // RES 5, D
			cycles = 8;
			regD &= ~(1<<5);
			break;
		case 0xAB: // RES 5, E
			cycles = 8;
			regE &= ~(1<<5);
			break;
		case 0xAC: // RES 5, H
			cycles = 8;
			regH &= ~(1<<5);
			break;
		case 0xAD: // RES 5, L
			cycles = 8;
			regL &= ~(1<<5);
			break;
		case 0xAE: // RES 5, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) & ~(1<<5));
			break;
		case 0xAF: // RES 5, A
			cycles = 8;
			regA &= ~(1<<5);
			break;

		case 0xB0: // RES 6, B
			cycles = 8;
			regB &= ~(1<<6);
			break;
		case 0xB1: // RES 6, C
			cycles = 8;
			regC &= ~(1<<6);
			break;
		case 0xB2: // RES 6, D
			cycles = 8;
			regD &= ~(1<<6);
			break;
		case 0xB3: // RES 6, E
			cycles = 8;
			regE &= ~(1<<6);
			break;
		case 0xB4: // RES 6, H
			cycles = 8;
			regH &= ~(1<<6);
			break;
		case 0xB5: // RES 6, L
			cycles = 8;
			regL &= ~(1<<6);
			break;
		case 0xB6: // RES 6, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) & ~(1<<6));
			break;
		case 0xB7: // RES 6, A
			cycles = 8;
			regA &= ~(1<<6);
			break;
		case 0xB8: // RES 7, B
			cycles = 8;
			regB &= ~(1<<7);
			break;
		case 0xB9: // RES 7, C
			cycles = 8;
			regC &= ~(1<<7);
			break;
		case 0xBA: // RES 7, D
			cycles = 8;
			regD &= ~(1<<7);
			break;
		case 0xBB: // RES 7, E
			cycles = 8;
			regE &= ~(1<<7);
			break;
		case 0xBC: // RES 7, H
			cycles = 8;
			regH &= ~(1<<7);
			break;
		case 0xBD: // RES 7, L
			cycles = 8;
			regL &= ~(1<<7);
			break;
		case 0xBE: // RES 7, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) & ~(1<<7));
			break;
		case 0xBF: // RES 7, A
			cycles = 8;
			regA &= ~(1<<7);
			break;
			
		case 0xC0: // SET 0, B
			cycles = 8;
			regB |=(1<<0);
			break;
		case 0xC1: // SET 0, C
			cycles = 8;
			regC |=(1<<0);
			break;
		case 0xC2: // SET 0, D
			cycles = 8;
			regD |=(1<<0);
			break;
		case 0xC3: // SET 0, E
			cycles = 8;
			regE |=(1<<0);
			break;
		case 0xC4: // SET 0, H
			cycles = 8;
			regH |=(1<<0);
			break;
		case 0xC5: // SET 0, L
			cycles = 8;
			regL |=(1<<0);
			break;
		case 0xC6: // SET 0, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) |(1<<0));
			break;
		case 0xC7: // SET 0, A
			cycles = 8;
			regA |=(1<<0);
			break;
		case 0xC8: // SET 1, B
			cycles = 8;
			regB |=(1<<1);
			break;
		case 0xC9: // SET 1, C
			cycles = 8;
			regC |=(1<<1);
			break;
		case 0xCA: // SET 1, D
			cycles = 8;
			regD |=(1<<1);
			break;
		case 0xCB: // SET 1, E
			cycles = 8;
			regE |=(1<<1);
			break;
		case 0xCC: // SET 1, H
			cycles = 8;
			regH |=(1<<1);
			break;
		case 0xCD: // SET 1, L
			cycles = 8;
			regL |=(1<<1);
			break;
		case 0xCE: // SET 1, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) |(1<<1));
			break;
		case 0xCF: // SET 1, A
			cycles = 8;
			regA |=(1<<1);
			break;

		case 0xD0: // SET 2, B
			cycles = 8;
			regB |=(1<<2);
			break;
		case 0xD1: // SET 2, C
			cycles = 8;
			regC |=(1<<2);
			break;
		case 0xD2: // SET 2, D
			cycles = 8;
			regD |=(1<<2);
			break;
		case 0xD3: // SET 2, E
			cycles = 8;
			regE |=(1<<2);
			break;
		case 0xD4: // SET 2, H
			cycles = 8;
			regH |=(1<<2);
			break;
		case 0xD5: // SET 2, L
			cycles = 8;
			regL |=(1<<2);
			break;
		case 0xD6: // SET 2, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) |(1<<2));
			break;
		case 0xD7: // SET 2, A
			cycles = 8;
			regA |=(1<<2);
			break;
		case 0xD8: // SET 3, B
			cycles = 8;
			regB |=(1<<3);
			break;
		case 0xD9: // SET 3, C
			cycles = 8;
			regC |=(1<<3);
			break;
		case 0xDA: // SET 3, D
			cycles = 8;
			regD |=(1<<3);
			break;
		case 0xDB: // SET 3, E
			cycles = 8;
			regE |=(1<<3);
			break;
		case 0xDC: // SET 3, H
			cycles = 8;
			regH |=(1<<3);
			break;
		case 0xDD: // SET 3, L
			cycles = 8;
			regL |=(1<<3);
			break;
		case 0xDE: // SET 3, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) |(1<<3));
			break;
		case 0xDF: // SET 3, A
			cycles = 8;
			regA |=(1<<3);
			break;

		case 0xE0: // SET 4, B
			cycles = 8;
			regB |=(1<<4);
			break;
		case 0xE1: // SET 4, C
			cycles = 8;
			regC |=(1<<4);
			break;
		case 0xE2: // SET 4, D
			cycles = 8;
			regD |=(1<<4);
			break;
		case 0xE3: // SET 4, E
			cycles = 8;
			regE |=(1<<4);
			break;
		case 0xE4: // SET 4, H
			cycles = 8;
			regH |=(1<<4);
			break;
		case 0xE5: // SET 4, L
			cycles = 8;
			regL |=(1<<4);
			break;
		case 0xE6: // SET 4, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) |(1<<4));
			break;
		case 0xE7: // SET 4, A
			cycles = 8;
			regA |=(1<<4);
			break;
		case 0xE8: // SET 5, B
			cycles = 8;
			regB |=(1<<5);
			break;
		case 0xE9: // SET 5, C
			cycles = 8;
			regC |=(1<<5);
			break;
		case 0xEA: // SET 5, D
			cycles = 8;
			regD |=(1<<5);
			break;
		case 0xEB: // SET 5, E
			cycles = 8;
			regE |=(1<<5);
			break;
		case 0xEC: // SET 5, H
			cycles = 8;
			regH |=(1<<5);
			break;
		case 0xED: // SET 5, L
			cycles = 8;
			regL |=(1<<5);
			break;
		case 0xEE: // SET 5, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) |(1<<5));
			break;
		case 0xEF: // SET 5, A
			cycles = 8;
			regA |=(1<<5);
			break;

		case 0xF0: // SET 6, B
			cycles = 8;
			regB |=(1<<6);
			break;
		case 0xF1: // SET 6, C
			cycles = 8;
			regC |=(1<<6);
			break;
		case 0xF2: // SET 6, D
			cycles = 8;
			regD |=(1<<6);
			break;
		case 0xF3: // SET 6, E
			cycles = 8;
			regE |=(1<<6);
			break;
		case 0xF4: // SET 6, H
			cycles = 8;
			regH |=(1<<6);
			break;
		case 0xF5: // SET 6, L
			cycles = 8;
			regL |=(1<<6);
			break;
		case 0xF6: // SET 6, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) |(1<<6));
			break;
		case 0xF7: // SET 6, A
			cycles = 8;
			regA |=(1<<6);
			break;
		case 0xF8: // SET 7, B
			cycles = 8;
			regB |=(1<<7);
			break;
		case 0xF9: // SET 7, C
			cycles = 8;
			regC |=(1<<7);
			break;
		case 0xFA: // SET 7, D
			cycles = 8;
			regD |=(1<<7);
			break;
		case 0xFB: // SET 7, E
			cycles = 8;
			regE |=(1<<7);
			break;
		case 0xFC: // SET 7, H
			cycles = 8;
			regH |=(1<<7);
			break;
		case 0xFD: // SET 7, L
			cycles = 8;
			regL |=(1<<7);
			break;
		case 0xFE: // SET 7, (HL)
			cycles = 16;
			this->gb->_mem->write(regHL, this->gb->_mem->read(regHL) |(1<<7));
			break;
		case 0xFF: // SET 7, A
			cycles = 8;
			regA |=(1<<7);
			break;
		default:
			Log(Error, "Unknown extended opcode %2x", opcode);
			break;
		}
		break;
#pragma endregion
#pragma region 0xCC		
	case 0xCC: // CALL Z, a16
		cycles = 12;
		t = GetOP();
		t |= GetOP()<<8;
		if(!(regF & F_Z)) break;
		cycles += 12;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = t;
		break;
	case 0xCD: // CALL a16
		cycles = 24;
		t = GetOP();
		t |= GetOP()<<8;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = t;
		break;	
	case 0xCE: // ADC A, d8
		cycles = 8;
		c = GetOP();
		t = ((regA & 0xF)+(c & 0xF)+((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA + c+((regF & F_C) != 0))& 0x100; // carry
		regA = regA + c+((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0xCF: // RST 08H
		cycles = 16;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = 0x08;
		break;
#pragma endregion
#pragma region 0xD0
	case 0xD0: // RET NC
		cycles = 8;
		if(regF & F_C) break;
		cycles += 12;
		regPC = this->gb->_mem->read(regSP) | this->gb->_mem->read(regSP+1)<<8;
		regSP +=2;
		break;
	case 0xD1: // POP DE
		cycles = 12;
		regD = this->gb->_mem->read(regSP+1);
		regE = this->gb->_mem->read(regSP);
		regSP += 2;
		break;
	case 0xD2: // JP NC, a16
		cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(regF & F_C) break;
		cycles += 4;
		regPC = t;
		break;
	case 0xD4: // CALL NC, a16
		cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(regF & F_C) break;
		cycles += 12;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = t;
		break;
	case 0xD5: // PUSH DE
		cycles = 16;
		this->gb->_mem->write(regSP-1, regD);
		this->gb->_mem->write(regSP-2, regE);
		regSP-=2;
		break;
	case 0xD6: // SUB A, d8
		cycles = 8;
		c = GetOP();
		t = ((regA & 0xF)-(c & 0xF))&0x10; // halfcarry
		t2 = (regA - c)& 0x100; // carry
		regA = regA - c;
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0xD7: // RST 10H
		cycles = 16;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = 0x10;
		break;
	case 0xD8: // RET C
		cycles = 8;
		if(!(regF & F_C)) break;
		cycles += 12;
		regPC = this->gb->_mem->read(regSP) | this->gb->_mem->read(regSP+1)<<8;
		regSP +=2;
		break;
	case 0xD9: // RETI
		cycles = 16;
		regPC = this->gb->_mem->read(regSP) | this->gb->_mem->read(regSP+1)<<8;
		regSP +=2;
		ime = true;
		break;
	case 0xDA: // JP C, a16
		cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(!(regF & F_C)) break;
		cycles += 4;
		regPC = t;
		break;
	case 0xDC: // CALL C, a16
		cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(!(regF & F_C)) break;
		cycles += 12;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = t;
		break;
	case 0xDE: // SBC A, d8
		cycles = 8;
		c = GetOP();
		t = ((regA & 0xF)-(c & 0xF)-((regF & F_C) != 0))&0x10; // halfcarry
		t2 = (regA - c-((regF & F_C) != 0))& 0x100; // carry
		regA = regA - c-((regF & F_C) != 0);
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0xDF: // RST 18H
		cycles = 16;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = 0x18;
		break;
#pragma endregion
#pragma region 0xE0
	case 0xE0: // LD (FF00 + n), A
		cycles = 12;
		this->gb->_mem->write(0xFF00 + GetOP(), regA);
		break;
	case 0xE1: // POP HL
		cycles = 12;
		regH = this->gb->_mem->read(regSP+1);
		regL = this->gb->_mem->read(regSP);
		regSP += 2;
		break;
	case 0xE2: // LD (FF00 + C), A
		cycles = 8;
		this->gb->_mem->write(0xFF00 + regC, regA);
		break;
	case 0xE5: // PUSH HL
		cycles = 16;
		this->gb->_mem->write(regSP-1, regH);
		this->gb->_mem->write(regSP-2, regL);
		regSP-=2;
		break;
	case 0xE6: // AND d8
		cycles = 8;
		regA &= GetOP();
		regF = (regF & 0x0F) | ((regA == 0)<<7) | F_H;
		break;
	case 0xE7: // RST 20H
		cycles = 16;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = 0x20;
		break;
	case 0xE8: // ADD SP, r8
		cycles = 16;
		c = GetOP();
		t = ((regSP&0xF) + ((signed char)c & 0xF)) & 0x10;
		t2 = ((regSP&0xFF) + ((signed char)c &0xFF)) & 0x100;
		regF = (regF & 0x0F) | (t << 1) | (t2 >> 4 );
		regSP += (signed char)c;
		break;
	case 0xE9: // JP (HL)
		cycles = 4;
		regPC = regHL;
		break;
	case 0xEA: // LD (a16), A
		cycles = 16;
		this->gb->_mem->write(GetNN(), regA);
		break;
	case 0xEE: // XOR d8
		cycles = 8;
		regA ^= GetOP();
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xEF: // RST 28H
		cycles = 16;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = 0x28;
		break;
#pragma endregion
#pragma region 0xF0
	case 0xF0: // LD A, (FF00 + n)
		cycles = 12;
		regA = this->gb->_mem->read(0xFF00 + GetOP());
		break;
	case 0xF1: // POP AF
		cycles = 12;
		regA = this->gb->_mem->read(regSP+1);
		regF = this->gb->_mem->read(regSP)&0xF0;
		regSP += 2;
		break;
	case 0xF2: // LD A, FF00 + C)
		cycles = 8;
		regA = this->gb->_mem->read(0xFF00 + regC);
		break;
	case 0xF3: // DI
		cycles = 4;
		ime = false;
		break;
	case 0xF5: // PUSH AF
		cycles = 16;
		this->gb->_mem->write(regSP-1, regA);
		this->gb->_mem->write(regSP-2, regF);
		regSP-=2;
		break;
	case 0xF6: // OR d8
		cycles = 8;
		regA |= GetOP();
		regF = (regF & 0x0F) | ((regA == 0)<<7);
		break;
	case 0xF7: // RST 30H
		cycles = 16;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = 0x30;
		break;
	case 0xF8: // LD HL, SP + r8
		cycles = 12;
		c = GetOP();
		t = ((regSP&0xF) + ((signed char)c & 0xF)) & 0x10;
		t2 = ((regSP&0xFF) + ((signed char)c &0xFF)) & 0x100;
		regF = (regF & 0x0F) | (t << 1) | (t2 >> 4 );
		regHL = regSP + (signed char)c;
		break;
	case 0xF9: // LD SP, HL
		cycles = 8;
		regSP = regHL;
		break;
	case 0xFA: // LD A, (a16)
		cycles = 16;
		regA = this->gb->_mem->read(GetNN());
		break;
	case 0xFB: // EI
		cycles = 4;
		ime = true;
		break;
	case 0xFC: // ContinuePoint!!!
		regPC--;
		t = regPC;
		gb->RemoveBreakpoint(t); // Temporary remove breakpoint
		handleInstruction();
		gb->AddBreakpoint(t);
		break;
	case 0xFD: // Breakpoint!!!
		cycles = 0;
		regPC--;
		throw BreakPointException(regPC);
		break;
	case 0xFE: // CP d8
		cycles = 8;
		c =  GetOP();
		t = regA - c;
		t2 = (regA&0xF)  - (c &0xF);
		regF = (regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xFF: // RST 38H
		cycles = 16;
		this->gb->_mem->write(regSP-1, regPC>>8);
		this->gb->_mem->write(regSP-2, regPC&0xFF);
		regSP-= 2;
		regPC = 0x38;
		break;
#pragma endregion
	default:
		Log(Error,"Unknown opcode %2x", opcode);
		break;
	}
}

#endif