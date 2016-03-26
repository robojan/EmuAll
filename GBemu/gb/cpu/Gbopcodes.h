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
		_cycles = 4;
		break;
	case 0x01: // LD BC, d16
		_cycles = 12;
		_regC = GetOP();
		_regB = GetOP();
		break;
	case 0x02: // LD (BC), A
		_cycles = 8;
		this->_gb->_mem->write(_regBC, _regA);
		break;
	case 0x03: // INC BC
		_cycles = 8;
		_regBC++;
		break;
	case 0x04: // INC B
		_cycles = 4;
		t = ((_regB & 0xF)+1)&0x10; // halfcarry
		_regB++;
		_regF = (_regF & 0x1F) | ((_regB == 0)<<7) | (t << 1);
		break;
	case 0x05: // DEC B
		_cycles = 4;
		t = ((_regB & 0xF)-1)&0x10; // halfcarry
		_regB--;
		_regF = (_regF & 0x1F) | ((_regB == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x06: // LD B, d8
		_cycles = 8;
		_regB = GetOP();
		break;
	case 0x07: // RLCA
		_cycles = 4;
		c = _regA;
		_regF = (_regF &0x0F) | (_regA & 0x80) >> 3;
		_regA = (_regA<<1) | ((c&0x80) >> 7);
		break;
	case 0x08: // LD (a16), SP
		_cycles = 20;
		t = GetOP(); t |= GetOP()<<8;
		this->_gb->_mem->write(t, _regSP&0xFF);
		this->_gb->_mem->write(t+1, _regSP>>8); 
		break;
	case 0x09: // ADD HL, BC
		_cycles = 8;
		t = ((_regHL&0xFFF) + (_regBC & 0xFFF)) & 0x1000;
		t2 = (_regHL + _regBC) & 0x10000;
		_regF = (_regF & 0x8F) | (t >> 7) | (t2 >> 12 );
		_regHL += _regBC;
		break;
	case 0x0A: // LD A, (BC)
		_cycles = 8;
		_regA = this->_gb->_mem->read(_regBC);
		break;
	case 0x0B: // DEC BC
		_cycles = 8;
		_regBC--;
		break;
	case 0x0C: // INC C
		_cycles = 4;
		t = ((_regC & 0xF)+1)&0x10; // halfcarry
		_regC++;
		_regF = (_regF & 0x1F) | ((_regC == 0)<<7) | (t << 1);
		break;
	case 0x0D: // DEC C
		_cycles = 4;
		t = ((_regC & 0xF)-1)&0x10; // halfcarry
		_regC--;
		_regF = (_regF & 0x1F) | ((_regC == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x0E: // LD C, d8
		_cycles = 8;
		_regC = GetOP();
		break;
	case 0x0F: // RRCA
		_cycles = 4;
		c = _regA;
		_regF = (_regF &0x0F) | (_regA & 0x01) << 4;
		_regA = (_regA>>1) | ((c&0x01)<<7);
		break;
#pragma endregion
#pragma region 0x10

	case 0x10: // STOP 0
		_cycles = 0;
		GetOP();
		_stopped = true;
		break;
	case 0x11: // LD DE, d16
		_cycles = 12;
		_regE = GetOP();
		_regD = GetOP();
		break;
	case 0x12: // LD (DE), A
		_cycles = 8;
		this->_gb->_mem->write(_regDE, _regA);
		break;
	case 0x13: // INC DE
		_cycles = 8;
		_regDE++;
		break;
	case 0x14: // INC D
		_cycles = 4;
		t = ((_regD & 0xF)+1)&0x10; // halfcarry
		_regD++;
		_regF = (_regF & 0x1F) | ((_regD == 0)<<7) | (t << 1);
		break;
	case 0x15: // DEC D
		_cycles = 4;
		t = ((_regD & 0xF)-1)&0x10; // halfcarry
		_regD--;
		_regF = (_regF & 0x1F) | ((_regD == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x16: // LD D, d8
		_cycles = 8;
		_regD = GetOP();
		break;
	case 0x17: // RLA
		_cycles = 4;
		t = _regF;
		_regF = (_regF &0x0F) | (_regA & 0x80) >> 3;
		_regA = (_regA<<1) | ((t&0x10) >> 4);
		break;
	case 0x18: // JR r8
		_cycles = 12;
		_regPC += (signed char)GetOP();
		break;
	case 0x19: // ADD HL, DE
		_cycles = 8;
		t = ((_regHL&0xFFF) + (_regDE & 0xFFF)) & 0x1000;
		t2 = (_regHL + _regDE) & 0x10000;
		_regF = (_regF & 0x8F) | (t >> 7) | (t2 >> 12 );
		_regHL += _regDE;
		break;
	case 0x1A: // LD A, (DE)
		_cycles = 8;
		_regA = this->_gb->_mem->read(_regDE);
		break;
	case 0x1B: // DEC DE
		_cycles = 8;
		_regDE--;
		break;
	case 0x1C: // INC E
		_cycles = 4;
		t = ((_regE & 0xF)+1)&0x10; // halfcarry
		_regE++;
		_regF = (_regF & 0x1F) | ((_regE == 0)<<7) | (t << 1);
		break;
	case 0x1D: // DEC E
		_cycles = 4;
		t = ((_regE & 0xF)-1)&0x10; // halfcarry
		_regE--;
		_regF = (_regF & 0x1F) | ((_regE == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x1E: // LD E, d8
		_cycles = 8;
		_regE = GetOP();
		break;
	case 0x1F: // RRA
		_cycles = 4;
		t = _regF;
		_regF = (_regF &0x0F) | (_regA & 0x01) << 4;
		_regA = (_regA>>1) | ((t&0x10) << 3);
		break;
#pragma endregion
#pragma region 0x20

	case 0x20: // JR NZ, r8
		_cycles = 8;
		c = GetOP();
		if(_regF & F_Z) break;
		_cycles += 4;
		_regPC += (signed char)c;
		break;
	case 0x21: // LD HL, d16
		_cycles = 12;
		_regL = GetOP();
		_regH = GetOP();
		break;
	case 0x22: // LDI (HL), A
		_cycles = 8;
		this->_gb->_mem->write(_regHL++, _regA);
		break;
	case 0x23: // INC HL
		_cycles = 8;
		_regHL++;
		break;
	case 0x24: // INC H
		_cycles = 4;
		t = ((_regH & 0xF)+1)&0x10; // halfcarry
		_regH++;
		_regF = (_regF & 0x1F) | ((_regH == 0)<<7) | (t << 1);
		break;
	case 0x25: // DEC H
		_cycles = 4;
		t = ((_regH & 0xF)-1)&0x10; // halfcarry
		_regH--;
		_regF = (_regF & 0x1F) | ((_regH == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x26: // LD H, d8
		_cycles = 8;
		_regH = GetOP();
		break;
	case 0x27: // DAA
		_cycles = 4;
		// Finaly it works:D
		t = _regA;
		if(_regF & F_N)
		{
			if(_regF & F_H)
			{
				t = (t-0x06) &0xFF;
			}
			if(_regF & F_C)
			{
				t -= 0x60;
			}
		}
		else {
			if((t & 0x0F) > 0x9 || (_regF&F_H))
			{
				t+=0x06;
			}
			if(t > 0x9F || _regF & F_C)
			{
				t+=0x60;
			}			
		}
		_regF &= ~(F_H | F_Z);
		if(t &0x100) _regF |= F_C;
		t &= 0xFF;
		if(t==0) _regF |= F_Z;

		_regA = (unsigned char) t;
		break;
	case 0x28: // JR Z, r8
		_cycles = 8;
		c = GetOP();
		if(!(_regF & F_Z)) break;
		_cycles += 4;
		_regPC += (signed char)c;
		break;
	case 0x29: // ADD HL, HL
		_cycles = 8;
		t = ((_regHL&0xFFF) + (_regHL & 0xFFF)) & 0x1000;
		t2 = (_regHL + _regHL) & 0x10000;
		_regF = (_regF & 0x8F) | (t >> 7) | (t2 >> 12 );
		_regHL += _regHL;
		break;
	case 0x2A: // LDI A, (HL)
		_cycles = 8;
		_regA = this->_gb->_mem->read(_regHL++);
		break;
	case 0x2B: // DEC HL
		_cycles = 8;
		_regHL--;
		break;
	case 0x2C: // INC L
		_cycles = 4;
		t = ((_regL & 0xF)+1)&0x10; // halfcarry
		_regL++;
		_regF = (_regF & 0x1F) | ((_regL == 0)<<7) | (t << 1);
		break;
	case 0x2D: // DEC L
		_cycles = 4;
		t = ((_regL & 0xF)-1)&0x10; // halfcarry
		_regL--;
		_regF = (_regF & 0x1F) | ((_regL == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x2E: // LD L, d8
		_cycles = 8;
		_regL = GetOP();
		break;
	case 0x2F: // CPL
		_cycles = 4;
		_regF = _regF | 0x60;
		_regA = ~_regA;
		break;
#pragma endregion
#pragma region 0x30

	case 0x30: // JR NC, r8
		_cycles = 8;
		c = GetOP();
		if(_regF & F_C) break;
		_cycles += 4;
		_regPC += (signed char)c;
		break;
	case 0x31: // LD SP, d16
		_cycles = 12;
		_regSP = GetNN();
		break;
	case 0x32: // LDD (HL), A
		_cycles = 8;
		this->_gb->_mem->write(_regHL--, _regA);
		break;
	case 0x33: // INC SP
		_cycles = 8;
		_regSP++;
		break;
	case 0x34: // INC (HL)
		_cycles = 12;
		t2 = this->_gb->_mem->read(_regHL);
		t = ((t2 & 0xF)+1)&0x10; // halfcarry
		this->_gb->_mem->write(_regHL, (t2+1)&0xFF);
		t2 = (t2+1)&0xFF;
		_regF = (_regF & 0x1F) | ((t2 == 0)<<7) | (t << 1);
		break;
	case 0x35: // DEC (HL)
		_cycles = 12;
		t2 = this->_gb->_mem->read(_regHL);
		t = ((t2 & 0xF)-1)&0x10; // halfcarry
		this->_gb->_mem->write(_regHL, t2-1);
		_regF = (_regF & 0x1F) | ((t2-1 == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x36: // LD (HL), d8
		_cycles = 12;
		this->_gb->_mem->write(_regHL, GetOP());
		break;
	case 0x37: // SCF
		_cycles = 4;
		_regF = (_regF & 0x8F) | F_C;
		break;
	case 0x38: // JR C, r8
		_cycles = 8;
		c = GetOP();
		if(!(_regF & F_C)) break;
		_cycles += 4;
		_regPC += (signed char)c;
		break;
	case 0x39: // ADD HL, SP
		_cycles = 8;
		t = ((_regHL&0xFFF) + (_regSP & 0xFFF)) & 0x1000;
		t2 = (_regHL + _regSP) & 0x10000;
		_regF = (_regF & 0x8F) | (t >> 7) | (t2 >> 12 );
		_regHL += _regSP;
		break;
	case 0x3A: // LDD A, (HL)
		_cycles = 8;
		_regA = this->_gb->_mem->read(_regHL--);
		break;
	case 0x3B: // DEC SP
		_cycles = 8;
		_regSP--;
		break;
	case 0x3C: // INC A
		_cycles = 4;
		t = ((_regA & 0xF)+1)&0x10; // halfcarry
		_regA++;
		_regF = (_regF & 0x1F) | ((_regA == 0)<<7) | (t << 1);
		break;
	case 0x3D: // DEC A
		_cycles = 4;
		t = ((_regA & 0xF)-1)&0x10; // halfcarry
		_regA--;
		_regF = (_regF & 0x1F) | ((_regA == 0)<<7) | (1<<6) | (t<<1);
		break;
	case 0x3E: // LD A, d8
		_cycles = 8;
		_regA = GetOP();
		break;
	case 0x3F: // CCF
		_cycles = 4;
		_regF = _regF & 0x9F;
		_regF ^= F_C;
		break;
#pragma endregion
#pragma region 0x40

	case 0x40: // LD B, B
		_cycles = 4;
		_regB = _regB;
		break;
	case 0x41: // LD B, C
		_cycles = 4;
		_regB = _regC;
		break;
	case 0x42: // LD B, D
		_cycles = 4;
		_regB = _regD;
		break;
	case 0x43: // LD B, E
		_cycles = 4;
		_regB = _regE;
		break;
	case 0x44: // LD B, H
		_cycles = 4;
		_regB = _regH;
		break;
	case 0x45: // LD B, L
		_cycles = 4;
		_regB = _regL;
		break;
	case 0x46: // LD B, (HL)
		_cycles = 8;
		_regB = this->_gb->_mem->read(_regHL);
		break;
	case 0x47: // LD B, A
		_cycles = 4;
		_regB = _regA;
		break;
	case 0x48: // LD C, B
		_cycles = 4;
		_regC = _regB;
		break;
	case 0x49: // LD C, C
		_cycles = 4;
		_regC = _regC;
		break;
	case 0x4A: // LD C, D
		_cycles = 4;
		_regC = _regD;
		break;
	case 0x4B: // LD C, E
		_cycles = 4;
		_regC = _regE;
		break;
	case 0x4C: // LD C, H
		_cycles = 4;
		_regC = _regH;
		break;
	case 0x4D: // LD C, L
		_cycles = 4;
		_regC = _regL;
		break;
	case 0x4E: // LD C, (HL)
		_cycles = 8;
		_regC = this->_gb->_mem->read(_regHL);
		break;
	case 0x4F: // LD C, A
		_cycles = 4;
		_regC = _regA;
		break;
#pragma endregion
#pragma region 0x50

	case 0x50: // LD D, B
		_cycles = 4;
		_regD = _regB;
		break;
	case 0x51: // LD D, C
		_cycles = 4;
		_regD = _regC;
		break;
	case 0x52: // LD D, D
		_cycles = 4;
		_regD = _regD;
		break;
	case 0x53: // LD D, E
		_cycles = 4;
		_regD = _regE;
		break;
	case 0x54: // LD D, H
		_cycles = 4;
		_regD = _regH;
		break;
	case 0x55: // LD D, L
		_cycles = 4;
		_regD = _regL;
		break;
	case 0x56: // LD D, (HL)
		_cycles = 8;
		_regD = this->_gb->_mem->read(_regHL);
		break;
	case 0x57: // LD D, A
		_cycles = 4;
		_regD = _regA;
		break;
	case 0x58: // LD E, B
		_cycles = 4;
		_regE = _regB;
		break;
	case 0x59: // LD E, C
		_cycles = 4;
		_regE = _regC;
		break;
	case 0x5A: // LD E, D
		_cycles = 4;
		_regE = _regD;
		break;
	case 0x5B: // LD E, E
		_cycles = 4;
		_regE = _regE;
		break;
	case 0x5C: // LD E, H
		_cycles = 4;
		_regE = _regH;
		break;
	case 0x5D: // LD E, L
		_cycles = 4;
		_regE = _regL;
		break;
	case 0x5E: // LD E, (HL)
		_cycles = 8;
		_regE = this->_gb->_mem->read(_regHL);
		break;
	case 0x5F: // LD E, A
		_cycles = 4;
		_regE = _regA;
		break;
#pragma endregion
#pragma region 0x60

	case 0x60: // LD H, B
		_cycles = 4;
		_regH = _regB;
		break;
	case 0x61: // LD H, C
		_cycles = 4;
		_regH = _regC;
		break;
	case 0x62: // LD H, D
		_cycles = 4;
		_regH = _regD;
		break;
	case 0x63: // LD H, E
		_cycles = 4;
		_regH = _regE;
		break;
	case 0x64: // LD H, H
		_cycles = 4;
		_regH = _regH;
		break;
	case 0x65: // LD H, L
		_cycles = 4;
		_regH = _regL;
		break;
	case 0x66: // LD H, (HL)
		_cycles = 8;
		_regH = this->_gb->_mem->read(_regHL);
		break;
	case 0x67: // LD H, A
		_cycles = 4;
		_regH = _regA;
		break;
	case 0x68: // LD L, B
		_cycles = 4;
		_regL = _regB;
		break;
	case 0x69: // LD L, C
		_cycles = 4;
		_regL = _regC;
		break;
	case 0x6A: // LD L, D
		_cycles = 4;
		_regL = _regD;
		break;
	case 0x6B: // LD L, E
		_cycles = 4;
		_regL = _regE;
		break;
	case 0x6C: // LD L, H
		_cycles = 4;
		_regL = _regH;
		break;
	case 0x6D: // LD L, L
		_cycles = 4;
		_regL = _regL;
		break;
	case 0x6E: // LD L, (HL)
		_cycles = 8;
		_regL = this->_gb->_mem->read(_regHL);
		break;
	case 0x6F: // LD L, A
		_cycles = 4;
		_regL = _regA;
		break;
#pragma endregion
#pragma region 0x70

	case 0x70: // LD (HL), B
		_cycles = 8;
		this->_gb->_mem->write(_regHL, _regB);
		break;
	case 0x71: // LD (HL), C
		_cycles = 8;
		this->_gb->_mem->write(_regHL, _regC);
		break;
	case 0x72: // LD (HL), D
		_cycles = 8;
		this->_gb->_mem->write(_regHL, _regD);
		break;
	case 0x73: // LD (HL), E
		_cycles = 8;
		this->_gb->_mem->write(_regHL, _regE);
		break;
	case 0x74: // LD (HL), H
		_cycles = 8;
		this->_gb->_mem->write(_regHL, _regH);
		break;
	case 0x75: // LD (HL), L
		_cycles = 8;
		this->_gb->_mem->write(_regHL, _regL);
		break;
	case 0x76: // HALT
		_cycles = 0;
		_halted = true;
		break;
	case 0x77: // LD (HL), A
		_cycles = 8;
		this->_gb->_mem->write(_regHL, _regA);
		break;
	case 0x78: // LD A, B
		_cycles = 4;
		_regA = _regB;
		break;
	case 0x79: // LD A, C
		_cycles = 4;
		_regA = _regC;
		break;
	case 0x7A: // LD A, D
		_cycles = 4;
		_regA = _regD;
		break;
	case 0x7B: // LD A, E
		_cycles = 4;
		_regA = _regE;
		break;
	case 0x7C: // LD A, H
		_cycles = 4;
		_regA = _regH;
		break;
	case 0x7D: // LD A, L
		_cycles = 4;
		_regA = _regL;
		break;
	case 0x7E: // LD A, (HL)
		_cycles = 8;
		_regA = this->_gb->_mem->read(_regHL);
		break;
	case 0x7F: // LD A, A
		_cycles = 4;
		_regA = _regA;
		break;
#pragma endregion
#pragma region 0x80
	case 0x80: // ADD A, B
		_cycles = 4;
		t = ((_regA & 0xF)+(_regB & 0xF))&0x10; // halfcarry
		t2 = (_regA + _regB)& 0x100; // carry
		_regA = _regA + _regB;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x81: // ADD A, C
		_cycles = 4;
		t = ((_regA & 0xF)+(_regC & 0xF))&0x10; // halfcarry
		t2 = (_regA + _regC)& 0x100; // carry
		_regA = _regA + _regC;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x82: // ADD A, D
		_cycles = 4;
		t = ((_regA & 0xF)+(_regD & 0xF))&0x10; // halfcarry
		t2 = (_regA + _regD)& 0x100; // carry
		_regA = _regA + _regD;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x83: // ADD A, E
		_cycles = 4;
		t = ((_regA & 0xF)+(_regE & 0xF))&0x10; // halfcarry
		t2 = (_regA + _regE)& 0x100; // carry
		_regA = _regA + _regE;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x84: // ADD A, H
		_cycles = 4;
		t = ((_regA & 0xF)+(_regH & 0xF))&0x10; // halfcarry
		t2 = (_regA + _regH)& 0x100; // carry
		_regA = _regA + _regH;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x85: // ADD A, L
		_cycles = 4;
		t = ((_regA & 0xF)+(_regL & 0xF))&0x10; // halfcarry
		t2 = (_regA + _regL)& 0x100; // carry
		_regA = _regA + _regL;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x86: // ADD A, (HL)
		_cycles = 8;
		c = this->_gb->_mem->read(_regHL);
		t = ((_regA & 0xF)+(c & 0xF))&0x10; // halfcarry
		t2 = (_regA + c)& 0x100; // carry
		_regA = _regA + c;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x87: // ADD A, A
		_cycles = 4;
		t = ((_regA & 0xF)+(_regA & 0xF))&0x10; // halfcarry
		t2 = (_regA + _regA)& 0x100; // carry
		_regA = _regA + _regA;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x88: // ADC A, B
		_cycles = 4;
		t = ((_regA & 0xF)+(_regB & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + _regB+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + _regB+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x89: // ADC A, C
		_cycles = 4;
		t = ((_regA & 0xF)+(_regC & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + _regC+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + _regC+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8A: // ADC A, D
		_cycles = 4;
		t = ((_regA & 0xF)+(_regD & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + _regD+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + _regD+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8B: // ADC A, E
		_cycles = 4;
		t = ((_regA & 0xF)+(_regE & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + _regE+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + _regE+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8C: // ADC A, H
		_cycles = 4;
		t = ((_regA & 0xF)+(_regH & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + _regH+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + _regH+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8D: // ADC A, L
		_cycles = 4;
		t = ((_regA & 0xF)+(_regL & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + _regL+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + _regL+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8E: // ADC A, (HL)
		_cycles = 8;
		c = this->_gb->_mem->read(_regHL);
		t = ((_regA & 0xF)+(c & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + c+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + c+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0x8F: // ADC A, A
		_cycles = 4;
		t = ((_regA & 0xF)+(_regA & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + _regA+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + _regA+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
#pragma endregion
#pragma region 0x90
	case 0x90: // SUB A, B
		_cycles = 4;
		t = ((_regA & 0xF)-(_regB & 0xF))&0x10; // halfcarry
		t2 = (_regA - _regB)& 0x100; // carry
		_regA = _regA - _regB;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x91: // SUB A, C
		_cycles = 4;
		t = ((_regA & 0xF)-(_regC & 0xF))&0x10; // halfcarry
		t2 = (_regA - _regC)& 0x100; // carry
		_regA = _regA - _regC;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x92: // SUB A, D
		_cycles = 4;
		t = ((_regA & 0xF)-(_regD & 0xF))&0x10; // halfcarry
		t2 = (_regA - _regD)& 0x100; // carry
		_regA = _regA - _regD;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x93: // SUB A, E
		_cycles = 4;
		t = ((_regA & 0xF)-(_regE & 0xF))&0x10; // halfcarry
		t2 = (_regA - _regE)& 0x100; // carry
		_regA = _regA - _regE;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x94: // SUB A, H
		_cycles = 4;
		t = ((_regA & 0xF)-(_regH & 0xF))&0x10; // halfcarry
		t2 = (_regA - _regH)& 0x100; // carry
		_regA = _regA - _regH;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x95: // SUB A, L
		_cycles = 4;
		t = ((_regA & 0xF)-(_regL & 0xF))&0x10; // halfcarry
		t2 = (_regA - _regL)& 0x100; // carry
		_regA = _regA - _regL;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x96: // SUB A, (HL)
		_cycles = 8;
		c = this->_gb->_mem->read(_regHL);
		t = ((_regA & 0xF)-(c & 0xF))&0x10; // halfcarry
		t2 = (_regA - c)& 0x100; // carry
		_regA = _regA - c;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x97: // SUB A, A
		_cycles = 4;
		t = ((_regA & 0xF)-(_regA & 0xF))&0x10; // halfcarry
		t2 = (_regA - _regA)& 0x100; // carry
		_regA = _regA - _regA;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x98: // SBC A, B
		_cycles = 4;
		t = ((_regA & 0xF)-(_regB & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - _regB-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - _regB-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x99: // SBC A, C
		_cycles = 4;
		t = ((_regA & 0xF)-(_regC & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - _regC-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - _regC-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9A: // SBC A, D
		_cycles = 4;
		t = ((_regA & 0xF)-(_regD & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - _regD-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - _regD-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9B: // SBC A, E
		_cycles = 4;
		t = ((_regA & 0xF)-(_regE & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - _regE-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - _regE-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9C: // SBC A, H
		_cycles = 4;
		t = ((_regA & 0xF)-(_regH & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - _regH-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - _regH-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9D: // SBC A, L
		_cycles = 4;
		t = ((_regA & 0xF)-(_regL & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - _regL-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - _regL-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9E: // SBC A, (HL)
		_cycles = 8;
		c = this->_gb->_mem->read(_regHL);
		t = ((_regA & 0xF)-(c & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - c-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - c-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0x9F: // SBC A, A
		_cycles = 4;
		t = ((_regA & 0xF)-(_regA & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - _regA-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - _regA-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
#pragma endregion
#pragma region 0xA0
	case 0xA0: // AND B
		_cycles = 4;
		_regA &= _regB;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xA1: // AND C
		_cycles = 4;
		_regA &= _regC;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xA2: // AND D
		_cycles = 4;
		_regA &= _regD;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xA3: // AND E
		_cycles = 4;
		_regA &= _regE;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xA4: // AND H
		_cycles = 4;
		_regA &= _regH;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xA5: // AND L
		_cycles = 4;
		_regA &= _regL;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xA6: // AND (HL)
		_cycles = 8;
		_regA &= this->_gb->_mem->read(_regHL);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xA7: // AND A
		_cycles = 4;
		_regA &= _regA;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xA8: // XOR B
		_cycles = 4;
		_regA ^= _regB;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xA9: // XOR C
		_cycles = 4;
		_regA ^= _regC;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xAA: // XOR D
		_cycles = 4;
		_regA ^= _regD;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xAB: // XOR E
		_cycles = 4;
		_regA ^= _regE;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xAC: // XOR H
		_cycles = 4;
		_regA ^= _regH;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xAD: // XOR L
		_cycles = 4;
		_regA ^= _regL;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xAE: // XOR (HL)
		_cycles = 8;
		_regA ^= this->_gb->_mem->read(_regHL);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xAF: // XOR A
		_cycles = 4;
		_regA ^= _regA;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
#pragma endregion
#pragma region 0xB0
	case 0xB0: // OR B
		_cycles = 4;
		_regA |= _regB;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xB1: // OR C
		_cycles = 4;
		_regA |= _regC;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xB2: // OR D
		_cycles = 4;
		_regA |= _regD;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xB3: // OR E
		_cycles = 4;
		_regA |= _regE;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xB4: // OR H
		_cycles = 4;
		_regA |= _regH;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xB5: // OR L
		_cycles = 4;
		_regA |= _regL;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xB6: // OR (HL)
		_cycles = 8;
		_regA |= this->_gb->_mem->read(_regHL);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xB7: // OR A
		_cycles = 4;
		_regA |= _regA;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xB8: // CP B
		_cycles = 4;
		t = _regA - _regB;
		t2 = (_regA&0xF)  - (_regB &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xB9: // CP C
		_cycles = 4;
		t = _regA - _regC;
		t2 = (_regA&0xF)  - (_regC &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBA: // CP D
		_cycles = 4;
		t = _regA - _regD;
		t2 = (_regA&0xF)  - (_regD &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBB: // CP E
		_cycles = 4;
		t = _regA - _regE;
		t2 = (_regA&0xF)  - (_regE &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBC: // CP H
		_cycles = 4;
		t = _regA - _regH;
		t2 = (_regA&0xF)  - (_regH &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBD: // CP L
		_cycles = 4;
		t = _regA - _regL;
		t2 = (_regA&0xF)  - (_regL &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBE: // CP (HL)
		_cycles = 8;
		c = this->_gb->_mem->read(_regHL);
		t = _regA - c;
		t2 = (_regA&0xF)  - (c &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xBF: // CP A
		_cycles = 4;
		t = _regA - _regA;
		t2 = (_regA&0xF)  - (_regA &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
#pragma endregion
#pragma region 0xC0
	case 0xC0: // RET NZ
		_cycles = 8;
		if(_regF & F_Z) break;
		_cycles += 12;
		_regPC = this->_gb->_mem->read(_regSP) | this->_gb->_mem->read(_regSP+1)<<8;
		_regSP +=2;
		break;
	case 0xC1: // POP BC
		_cycles = 12;
		_regB = this->_gb->_mem->read(_regSP+1);
		_regC = this->_gb->_mem->read(_regSP);
		_regSP += 2;
		break;
	case 0xC2: // JP NZ, a16
		_cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(_regF & F_Z) break;
		_cycles += 4;
		_regPC = t;
		break;
	case 0xC3: // JP a16
		_cycles = 16;
		_regPC = GetNN();
		break;
	case 0xC4: // CALL NZ, a16
		_cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(_regF & F_Z) break;
		_cycles += 12;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = t;
		break;
	case 0xC5: // PUSH BC
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regB);
		this->_gb->_mem->write(_regSP-2, _regC);
		_regSP-=2;
		break;
	case 0xC6: // ADD A, d8
		_cycles = 8;
		c = GetOP();
		t = ((_regA & 0xF)+(c & 0xF))&0x10; // halfcarry
		t2 = (_regA + c)& 0x100; // carry
		_regA = _regA + c;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0xC7: // RST 00H
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = 0;
		break;
	case 0xC8: // RET Z
		_cycles = 8;
		if(!(_regF & F_Z)) break;
		_cycles += 12;
		_regPC = this->_gb->_mem->read(_regSP) | this->_gb->_mem->read(_regSP+1)<<8;
		_regSP +=2;
		break;
	case 0xC9: // RET 
		_cycles = 16;
		_regPC = this->_gb->_mem->read(_regSP) | this->_gb->_mem->read(_regSP+1)<<8;
		_regSP +=2;
		break;
	case 0xCA: // JP Z, a16
		_cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(!(_regF & F_Z)) break;
		_cycles += 4;
		_regPC = t;
		break;
#pragma endregion
#pragma region 0xCB
	case 0xCB: // prefix
		opcode = GetOP();
		switch(opcode)
		{
		case 0x00: //RLC B
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regB == 0) << 7 | (_regB & 0x80) >> 3;
			_regB = (_regB<<1) | ((_regF&0x10) >> 4);
			break;
		case 0x01: //RLC C
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regC == 0) << 7 | (_regC & 0x80) >> 3;
			_regC = (_regC<<1) | ((_regF&0x10) >> 4);
			break;
		case 0x02: //RLC D
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regD == 0) << 7 | (_regD & 0x80) >> 3;
			_regD = (_regD<<1) | ((_regF&0x10) >> 4);
			break;
		case 0x03: //RLC E
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regE == 0) << 7 | (_regE & 0x80) >> 3;
			_regE = (_regE<<1) | ((_regF&0x10) >> 4);
			break;
		case 0x04: //RLC H
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regH == 0) << 7 | (_regH & 0x80) >> 3;
			_regH = (_regH<<1) | ((_regF&0x10) >> 4);
			break;
		case 0x05: //RLC L
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regL == 0) << 7 | (_regL & 0x80) >> 3;
			_regL = (_regL<<1) | ((_regF&0x10) >> 4);
			break;
		case 0x06: //RLC (HL)
			_cycles = 16;
			c = this->_gb->_mem->read(_regHL);
			_regF = (_regF &0x0F) | (c == 0) << 7 | (c & 0x80) >> 3;
			this->_gb->_mem->write(_regHL, (c<<1) | ((_regF&0x10) >> 4));
			break;
		case 0x07: //RLC A
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regA == 0) << 7 | (_regA & 0x80) >> 3;
			_regA = (_regA<<1) | ((_regF&0x10) >> 4);
			break;
		case 0x08: // RRC B
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regB == 0) << 7 | (_regB & 0x01) << 4;
			_regB = (_regB>>1) | ((_regF&0x10) << 3);
			break;
		case 0x09: // RRC C
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regC == 0) << 7 | (_regC & 0x01) << 4;
			_regC = (_regC>>1) | ((_regF&0x10) << 3);
			break;
		case 0x0A: // RRC D
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regD == 0) << 7 | (_regD & 0x01) << 4;
			_regD = (_regD>>1) | ((_regF&0x10) << 3);
			break;
		case 0x0B: // RRC E
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regE == 0) << 7 | (_regE & 0x01) << 4;
			_regE = (_regE>>1) | ((_regF&0x10) << 3);
			break;
		case 0x0C: // RRC H
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regH == 0) << 7 | (_regH & 0x01) << 4;
			_regH = (_regH>>1) | ((_regF&0x10) << 3);
			break;
		case 0x0D: // RRC L
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regL == 0) << 7 | (_regL & 0x01) << 4;
			_regL = (_regL>>1) | ((_regF&0x10) << 3);
			break;
		case 0x0E: // RRC (HL)
			_cycles = 16;
			c = this->_gb->_mem->read(_regHL);
			t = c;
			c = (c>>1)|((t&0x01)<<7);
			_regF = (_regF &0x0F) | (c == 0) << 7 | (t & 0x01) << 4;
			this->_gb->_mem->write(_regHL,c);
			break;
		case 0x0F: // RRC A
			_cycles = 8;
			_regF = (_regF &0x0F) | (_regA == 0) << 7 | (_regA & 0x01) << 4;
			_regA = (_regA>>1) | ((_regF&0x10) << 3);
			break;
		case 0x10: // RL B
			_cycles = 8;
			t = _regB;
			_regB = (_regB<<1) | ((_regF&0x10) >> 4);
			_regF = (_regF &0x0F) | (_regB == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x11: // RL C
			_cycles = 8;
			t = _regC;
			_regC = (_regC<<1) | ((_regF&0x10) >> 4);
			_regF = (_regF &0x0F) | (_regC == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x12: // RL D
			_cycles = 8;
			t = _regD;
			_regD = (_regD<<1) | ((_regF&0x10) >> 4);
			_regF = (_regF &0x0F) | (_regD == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x13: // RL E
			_cycles = 8;
			t = _regE;
			_regE = (_regE<<1) | ((_regF&0x10) >> 4);
			_regF = (_regF &0x0F) | (_regE == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x14: // RL H
			_cycles = 8;
			t = _regH;
			_regH = (_regH<<1) | ((_regF&0x10) >> 4);
			_regF = (_regF &0x0F) | (_regH == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x15: // RL L
			_cycles = 8;
			t = _regL;
			_regL = (_regL<<1) | ((_regF&0x10) >> 4);
			_regF = (_regF &0x0F) | (_regL == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x16: // RL (HL)
			_cycles = 16;
			c = this->_gb->_mem->read(_regHL);
			t = c;
			c =  (c<<1) | ((_regF&0x10) >> 4);
			_regF = (_regF &0x0F) | (c == 0) << 7 | (t & 0x80) >> 3;
			this->_gb->_mem->write(_regHL, c);
			break;
		case 0x17: // RL A
			_cycles = 8;
			t = _regA;
			_regA = (_regA<<1) | ((_regF&0x10) >> 4);
			_regF = (_regF &0x0F) | (_regA == 0) << 7 | (t & 0x80) >> 3;
			break;
		case 0x18: // RR B
			_cycles = 8;
			t = _regB;
			_regB = (_regB>>1) | ((_regF&0x10) << 3);
			_regF = (_regF &0x0F) | (_regB == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x19: // RR C
			_cycles = 8;
			t = _regC;
			_regC = (_regC>>1) | ((_regF&0x10) << 3);
			_regF = (_regF &0x0F) | (_regC == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1A: // RR D
			_cycles = 8;
			t = _regD;
			_regD = (_regD>>1) | ((_regF&0x10) << 3);
			_regF = (_regF &0x0F) | (_regD == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1B: // RR E
			_cycles = 8;
			t = _regE;
			_regE = (_regE>>1) | ((_regF&0x10) << 3);
			_regF = (_regF &0x0F) | (_regE == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1C: // RR H
			_cycles = 8;
			t = _regH;
			_regH = (_regH>>1) | ((_regF&0x10) << 3);
			_regF = (_regF &0x0F) | (_regH == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1D: // RR L
			_cycles = 8;
			t = _regL;
			_regL = (_regL>>1) | ((_regF&0x10) << 3);
			_regF = (_regF &0x0F) | (_regL == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x1E: // RR (HL)
			_cycles = 16;
			c = this->_gb->_mem->read(_regHL);
			t = c;
			c = (c>>1) | ((_regF&0x10) << 3);
			_regF = (_regF &0x0F) | (c == 0) << 7 | (t & 0x01) << 4;
			this->_gb->_mem->write(_regHL, c);
			break;
		case 0x1F: // RR A
			_cycles = 8;
			t = _regA;
			_regA = (_regA>>1) | ((_regF&0x10) << 3);
			_regF = (_regF &0x0F) | (_regA == 0) << 7 | (t & 0x01) << 4;
			break;
		case 0x20: // SLA B
			_cycles = 8;
			c = _regB;
			_regB <<= 1;
			_regF = (_regF &0x0F) | (_regB == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x21: // SLA C
			_cycles = 8;
			c = _regC;
			_regC <<= 1;
			_regF = (_regF &0x0F) | (_regC == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x22: // SLA D
			_cycles = 8;
			c = _regD;
			_regD <<= 1;
			_regF = (_regF &0x0F) | (_regD == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x23: // SLA E
			_cycles = 8;
			c = _regE;
			_regE <<= 1;
			_regF = (_regF &0x0F) | (_regE == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x24: // SLA H
			_cycles = 8;
			c = _regH;
			_regH <<= 1;
			_regF = (_regF &0x0F) | (_regH == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x25: // SLA L
			_cycles = 8;
			c = _regL;
			_regL <<= 1;
			_regF = (_regF &0x0F) | (_regL == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x26: // SLA (HL)
			_cycles = 16;
			c = this->_gb->_mem->read(_regHL);
			t= c;
			c = c<<1;
			_regF = (_regF &0x0F) | (c == 0) << 7 | (t & 0x80) >> 3;
			this->_gb->_mem->write(_regHL, c);
			break;
		case 0x27: // SLA A
			_cycles = 8;
			c = _regA;
			_regA <<= 1;
			_regF = (_regF &0x0F) | (_regA == 0) << 7 | (c & 0x80) >> 3;
			break;
		case 0x28: // SRA B
			_cycles = 8;
			c = _regB & 0x01;
			_regB >>= 1;
			_regB |= (_regB<<1)&0x80;
			_regF= (_regF & 0x0F) | (_regB == 0) << 7 | (c<<4);
			break;
		case 0x29: // SRA C
			_cycles = 8;
			c = _regC & 0x01;
			_regC >>= 1;
			_regC |= (_regC<<1)&0x80;
			_regF= (_regF & 0x0F) | (_regC == 0) << 7 | (c<<4);
			break;
		case 0x2A: // SRA D
			_cycles = 8;
			c = _regD & 0x01;
			_regD >>= 1;
			_regD |= (_regD<<1)&0x80;
			_regF= (_regF & 0x0F) | (_regD == 0) << 7 | (c<<4);
			break;
		case 0x2B: // SRA E
			_cycles = 8;
			c = _regE & 0x01;
			_regE >>= 1;
			_regE |= (_regE<<1)&0x80;
			_regF= (_regF & 0x0F) | (_regE == 0) << 7 | (c<<4);
			break;
		case 0x2C: // SRA H
			_cycles = 8;
			c = _regH & 0x01;
			_regH >>= 1;
			_regH |= (_regH<<1)&0x80;
			_regF= (_regF & 0x0F) | (_regH == 0) << 7 | (c<<4);
			break;
		case 0x2D: // SRA L
			_cycles = 8;
			c = _regL & 0x01;
			_regL >>= 1;
			_regL |= (_regL<<1)&0x80;
			_regF= (_regF & 0x0F) | (_regL == 0) << 7 | (c<<4);
			break;
		case 0x2E: // SRA (HL)
			_cycles = 16;
			c = this->_gb->_mem->read(_regHL);
			t = c;
			c = c >> 1;
			c |= t&0x80;
			this->_gb->_mem->write(_regHL, c);
			_regF= (_regF & 0x0F) | (c == 0) << 7 | ((t&0x01)<<4);
			break;
		case 0x2F: // SRA A
			_cycles = 8;
			c = _regA & 0x01;
			_regA >>= 1;
			_regA |= (_regA<<1)&0x80;
			_regF= (_regF & 0x0F) | (_regA == 0) << 7 | (c<<4);
			break;
		case 0x30: // SWAP B
			_cycles = 8;
			c = _regB;
			_regB = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			_regF= (_regF & 0x0F) | (_regB == 0) << 7;
			break;
		case 0x31: // SWAP C
			_cycles = 8;
			c = _regC;
			_regC = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			_regF= (_regF & 0x0F) | (_regC == 0) << 7;
			break;
		case 0x32: // SWAP D
			_cycles = 8;
			c = _regD;
			_regD = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			_regF = (_regF & 0x0F) | (_regD == 0) << 7;
			break;
		case 0x33: // SWAP E
			_cycles = 8;
			c = _regE;
			_regE = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			_regF= (_regF & 0x0F) | (_regE == 0) << 7;
			break;
		case 0x34: // SWAP H
			_cycles = 8;
			c = _regH;
			_regH = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			_regF= (_regF & 0x0F) | (_regH == 0) << 7;
			break;
		case 0x35: // SWAP L
			_cycles = 8;
			c = _regL;
			_regL = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			_regF = (_regF & 0x0F) | (_regL == 0) << 7;
			break;
		case 0x36: // SWAP (HL)
			_cycles = 16;
			c = this->_gb->_mem->read(_regHL);
			this->_gb->_mem->write(_regHL, ((c & 0xF)<<4) | ((c & 0xF0) >> 4));
			_regF= (_regF & 0x0F) | (c == 0) << 7;
			break;
		case 0x37: // SWAP A
			_cycles = 8;
			c = _regA;
			_regA = ((c & 0xF)<<4) | ((c & 0xF0) >> 4);
			_regF= (_regF & 0x0F) | (_regA == 0) << 7;
			break;
		case 0x38: // SRL B
			_cycles = 8;
			c = _regB&1;
			_regB >>= 1;
			_regF= (_regF & 0x0F) | (_regB == 0) << 7 | (c<<4);
			break;
		case 0x39: // SRL C
			_cycles = 8;
			c = _regC &1;
			_regC >>= 1;
			_regF= (_regF & 0x0F) | (_regC == 0) << 7 | (c<<4);
			break;
		case 0x3A: // SRL D
			_cycles = 8;
			c = _regD &1;
			_regD >>= 1;
			_regF= (_regF & 0x0F) | (_regD == 0) << 7 | (c<<4);
			break;
		case 0x3B: // SRL E
			_cycles = 8;
			c = _regE &1;
			_regE >>= 1;
			_regF= (_regF & 0x0F) | (_regE == 0) << 7 | (c<<4);
			break;
		case 0x3C: // SRL H
			_cycles = 8;
			c = _regH &1;
			_regH >>= 1;
			_regF= (_regF & 0x0F) | (_regH == 0) << 7 | (c<<4);
			break;
		case 0x3D: // SRL L
			_cycles = 8;
			c = _regL &1;
			_regL >>= 1;
			_regF= (_regF & 0x0F) | (_regL == 0) << 7 | (c<<4);
			break;
		case 0x3E: // SRL (HL)
			_cycles = 16;
			c = this->_gb->_mem->read(_regHL);
			t = c&1;
			this->_gb->_mem->write(_regHL, c >> 1);
			_regF= (_regF & 0x0F) | ((c>>1) == 0) << 7 | (t<<4);
			break;
		case 0x3F: // SRL A
			_cycles = 8;
			c = _regA &1;
			_regA >>= 1;
			_regF= (_regF & 0x0F) | (_regA == 0) << 7 | (c<<4);
			break;

		case 0x40: // BIT 0, B
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regB & (1<<0)) == 0) << 7;
			break;
		case 0x41: // BIT 0, C
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regC & (1<<0)) == 0) << 7;
			break;
		case 0x42: // BIT 0, D
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regD & (1<<0)) == 0) << 7;
			break;
		case 0x43: // BIT 0, E
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regE & (1<<0)) == 0) << 7;
			break;
		case 0x44: // BIT 0, H
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regH & (1<<0)) == 0) << 7;
			break;
		case 0x45: // BIT 0, L
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regL & (1<<0)) == 0) << 7;
			break;
		case 0x46: // BIT 0, (HL)
			_cycles = 12;
			_regF = (_regF & 0x1F) | F_H | ((this->_gb->_mem->read(_regHL) & (1<<0)) == 0) << 7;
			break;
		case 0x47: // BIT 0, A
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regA & (1<<0)) == 0) << 7;
			break;
		case 0x48: // BIT 1, B
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regB & (1<<1)) == 0) << 7;
			break;
		case 0x49: // BIT 1, C
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regC & (1<<1)) == 0) << 7;
			break;
		case 0x4A: // BIT 1, D
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regD & (1<<1)) == 0) << 7;
			break;
		case 0x4B: // BIT 1, E
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regE & (1<<1)) == 0) << 7;
			break;
		case 0x4C: // BIT 1, H
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regH & (1<<1)) == 0) << 7;
			break;
		case 0x4D: // BIT 1, L
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regL & (1<<1)) == 0) << 7;
			break;
		case 0x4E: // BIT 1, (HL)
			_cycles = 12;
			_regF = (_regF & 0x1F) | F_H | ((this->_gb->_mem->read(_regHL) & (1<<1)) == 0) << 7;
			break;
		case 0x4F: // BIT 1, A
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regA & (1<<1)) == 0) << 7;
			break;

			
		case 0x50: // BIT 2, B
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regB & (1<<2)) == 0) << 7;
			break;
		case 0x51: // BIT 2, C
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regC & (1<<2)) == 0) << 7;
			break;
		case 0x52: // BIT 2, D
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regD & (1<<2)) == 0) << 7;
			break;
		case 0x53: // BIT 2, E
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regE & (1<<2)) == 0) << 7;
			break;
		case 0x54: // BIT 2, H
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regH & (1<<2)) == 0) << 7;
			break;
		case 0x55: // BIT 2, L
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regL & (1<<2)) == 0) << 7;
			break;
		case 0x56: // BIT 2, (HL)
			_cycles = 12;
			_regF = (_regF & 0x1F) | F_H | ((this->_gb->_mem->read(_regHL) & (1<<2)) == 0) << 7;
			break;
		case 0x57: // BIT 2, A
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regA & (1<<2)) == 0) << 7;
			break;
		case 0x58: // BIT 3, B
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regB & (1<<3)) == 0) << 7;
			break;
		case 0x59: // BIT 3, C
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regC & (1<<3)) == 0) << 7;
			break;
		case 0x5A: // BIT 3, D
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regD & (1<<3)) == 0) << 7;
			break;
		case 0x5B: // BIT 3, E
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regE & (1<<3)) == 0) << 7;
			break;
		case 0x5C: // BIT 3, H
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regH & (1<<3)) == 0) << 7;
			break;
		case 0x5D: // BIT 3, L
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regL & (1<<3)) == 0) << 7;
			break;
		case 0x5E: // BIT 3, (HL)
			_cycles = 12;
			_regF = (_regF & 0x1F) | F_H | ((this->_gb->_mem->read(_regHL) & (1<<3)) == 0) << 7;
			break;
		case 0x5F: // BIT 3, A
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regA & (1<<3)) == 0) << 7;
			break;

		case 0x60: // BIT 4, B
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regB & (1<<4)) == 0) << 7;
			break;
		case 0x61: // BIT 4, C
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regC & (1<<4)) == 0) << 7;
			break;
		case 0x62: // BIT 4, D
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regD & (1<<4)) == 0) << 7;
			break;
		case 0x63: // BIT 4, E
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regE & (1<<4)) == 0) << 7;
			break;
		case 0x64: // BIT 4, H
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regH & (1<<4)) == 0) << 7;
			break;
		case 0x65: // BIT 4, L
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regL & (1<<4)) == 0) << 7;
			break;
		case 0x66: // BIT 4, (HL)
			_cycles = 12;
			_regF = (_regF & 0x1F) | F_H | ((this->_gb->_mem->read(_regHL) & (1<<4)) == 0) << 7;
			break;
		case 0x67: // BIT 4, A
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regA & (1<<4)) == 0) << 7;
			break;
		case 0x68: // BIT 5, B
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regB & (1<<5)) == 0) << 7;
			break;
		case 0x69: // BIT 5, C
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regC & (1<<5)) == 0) << 7;
			break;
		case 0x6A: // BIT 5, D
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regD & (1<<5)) == 0) << 7;
			break;
		case 0x6B: // BIT 5, E
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regE & (1<<5)) == 0) << 7;
			break;
		case 0x6C: // BIT 5, H
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regH & (1<<5)) == 0) << 7;
			break;
		case 0x6D: // BIT 5, L
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regL & (1<<5)) == 0) << 7;
			break;
		case 0x6E: // BIT 5, (HL)
			_cycles = 12;
			_regF = (_regF & 0x1F) | F_H | ((this->_gb->_mem->read(_regHL) & (1<<5)) == 0) << 7;
			break;
		case 0x6F: // BIT 5, A
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regA & (1<<5)) == 0) << 7;
			break;

			
		case 0x70: // BIT 6, B
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regB & (1<<6)) == 0) << 7;
			break;
		case 0x71: // BIT 6, C
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regC & (1<<6)) == 0) << 7;
			break;
		case 0x72: // BIT 6, D
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regD & (1<<6)) == 0) << 7;
			break;
		case 0x73: // BIT 6, E
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regE & (1<<6)) == 0) << 7;
			break;
		case 0x74: // BIT 6, H
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regH & (1<<6)) == 0) << 7;
			break;
		case 0x75: // BIT 6, L
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regL & (1<<6)) == 0) << 7;
			break;
		case 0x76: // BIT 6, (HL)
			_cycles = 12;
			_regF = (_regF & 0x1F) | F_H | ((this->_gb->_mem->read(_regHL) & (1<<6)) == 0) << 7;
			break;
		case 0x77: // BIT 6, A
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regA & (1<<6)) == 0) << 7;
			break;
		case 0x78: // BIT 7, B
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regB & (1<<7)) == 0) << 7;
			break;
		case 0x79: // BIT 7, C
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regC & (1<<7)) == 0) << 7;
			break;
		case 0x7A: // BIT 7, D
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regD & (1<<7)) == 0) << 7;
			break;
		case 0x7B: // BIT 7, E
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regE & (1<<7)) == 0) << 7;
			break;
		case 0x7C: // BIT 7, H
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regH & (1<<7)) == 0) << 7;
			break;
		case 0x7D: // BIT 7, L
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regL & (1<<7)) == 0) << 7;
			break;
		case 0x7E: // BIT 7, (HL)
			_cycles = 12;
			_regF = (_regF & 0x1F) | F_H | ((this->_gb->_mem->read(_regHL) & (1<<7)) == 0) << 7;
			break;
		case 0x7F: // BIT 7, A
			_cycles = 8;
			_regF = (_regF & 0x1F) | F_H | ((_regA & (1<<7)) == 0) << 7;
			break;

		case 0x80: // RES 0, B
			_cycles = 8;
			_regB &= ~(1<<0);
			break;
		case 0x81: // RES 0, C
			_cycles = 8;
			_regC &= ~(1<<0);
			break;
		case 0x82: // RES 0, D
			_cycles = 8;
			_regD &= ~(1<<0);
			break;
		case 0x83: // RES 0, E
			_cycles = 8;
			_regE &= ~(1<<0);
			break;
		case 0x84: // RES 0, H
			_cycles = 8;
			_regH &= ~(1<<0);
			break;
		case 0x85: // RES 0, L
			_cycles = 8;
			_regL &= ~(1<<0);
			break;
		case 0x86: // RES 0, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) & ~(1<<0));
			break;
		case 0x87: // RES 0, A
			_cycles = 8;
			_regA &= ~(1<<0);
			break;
		case 0x88: // RES 1, B
			_cycles = 8;
			_regB &= ~(1<<1);
			break;
		case 0x89: // RES 1, C
			_cycles = 8;
			_regC &= ~(1<<1);
			break;
		case 0x8A: // RES 1, D
			_cycles = 8;
			_regD &= ~(1<<1);
			break;
		case 0x8B: // RES 1, E
			_cycles = 8;
			_regE &= ~(1<<1);
			break;
		case 0x8C: // RES 1, H
			_cycles = 8;
			_regH &= ~(1<<1);
			break;
		case 0x8D: // RES 1, L
			_cycles = 8;
			_regL &= ~(1<<1);
			break;
		case 0x8E: // RES 1, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) & ~(1<<1));
			break;
		case 0x8F: // RES 1, A
			_cycles = 8;
			_regA &= ~(1<<1);
			break;

		case 0x90: // RES 2, B
			_cycles = 8;
			_regB &= ~(1<<2);
			break;
		case 0x91: // RES 2, C
			_cycles = 8;
			_regC &= ~(1<<2);
			break;
		case 0x92: // RES 2, D
			_cycles = 8;
			_regD &= ~(1<<2);
			break;
		case 0x93: // RES 2, E
			_cycles = 8;
			_regE &= ~(1<<2);
			break;
		case 0x94: // RES 2, H
			_cycles = 8;
			_regH &= ~(1<<2);
			break;
		case 0x95: // RES 2, L
			_cycles = 8;
			_regL &= ~(1<<2);
			break;
		case 0x96: // RES 2, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) & ~(1<<2));
			break;
		case 0x97: // RES 2, A
			_cycles = 8;
			_regA &= ~(1<<2);
			break;
		case 0x98: // RES 3, B
			_cycles = 8;
			_regB &= ~(1<<3);
			break;
		case 0x99: // RES 3, C
			_cycles = 8;
			_regC &= ~(1<<3);
			break;
		case 0x9A: // RES 3, D
			_cycles = 8;
			_regD &= ~(1<<3);
			break;
		case 0x9B: // RES 3, E
			_cycles = 8;
			_regE &= ~(1<<3);
			break;
		case 0x9C: // RES 3, H
			_cycles = 8;
			_regH &= ~(1<<3);
			break;
		case 0x9D: // RES 3, L
			_cycles = 8;
			_regL &= ~(1<<3);
			break;
		case 0x9E: // RES 3, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) & ~(1<<3));
			break;
		case 0x9F: // RES 3, A
			_cycles = 8;
			_regA &= ~(1<<3);
			break;

		case 0xA0: // RES 4, B
			_cycles = 8;
			_regB &= ~(1<<4);
			break;
		case 0xA1: // RES 4, C
			_cycles = 8;
			_regC &= ~(1<<4);
			break;
		case 0xA2: // RES 4, D
			_cycles = 8;
			_regD &= ~(1<<4);
			break;
		case 0xA3: // RES 4, E
			_cycles = 8;
			_regE &= ~(1<<4);
			break;
		case 0xA4: // RES 4, H
			_cycles = 8;
			_regH &= ~(1<<4);
			break;
		case 0xA5: // RES 4, L
			_cycles = 8;
			_regL &= ~(1<<4);
			break;
		case 0xA6: // RES 4, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) & ~(1<<4));
			break;
		case 0xA7: // RES 4, A
			_cycles = 8;
			_regA &= ~(1<<4);
			break;
		case 0xA8: // RES 5, B
			_cycles = 8;
			_regB &= ~(1<<5);
			break;
		case 0xA9: // RES 5, C
			_cycles = 8;
			_regC &= ~(1<<5);
			break;
		case 0xAA: // RES 5, D
			_cycles = 8;
			_regD &= ~(1<<5);
			break;
		case 0xAB: // RES 5, E
			_cycles = 8;
			_regE &= ~(1<<5);
			break;
		case 0xAC: // RES 5, H
			_cycles = 8;
			_regH &= ~(1<<5);
			break;
		case 0xAD: // RES 5, L
			_cycles = 8;
			_regL &= ~(1<<5);
			break;
		case 0xAE: // RES 5, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) & ~(1<<5));
			break;
		case 0xAF: // RES 5, A
			_cycles = 8;
			_regA &= ~(1<<5);
			break;

		case 0xB0: // RES 6, B
			_cycles = 8;
			_regB &= ~(1<<6);
			break;
		case 0xB1: // RES 6, C
			_cycles = 8;
			_regC &= ~(1<<6);
			break;
		case 0xB2: // RES 6, D
			_cycles = 8;
			_regD &= ~(1<<6);
			break;
		case 0xB3: // RES 6, E
			_cycles = 8;
			_regE &= ~(1<<6);
			break;
		case 0xB4: // RES 6, H
			_cycles = 8;
			_regH &= ~(1<<6);
			break;
		case 0xB5: // RES 6, L
			_cycles = 8;
			_regL &= ~(1<<6);
			break;
		case 0xB6: // RES 6, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) & ~(1<<6));
			break;
		case 0xB7: // RES 6, A
			_cycles = 8;
			_regA &= ~(1<<6);
			break;
		case 0xB8: // RES 7, B
			_cycles = 8;
			_regB &= ~(1<<7);
			break;
		case 0xB9: // RES 7, C
			_cycles = 8;
			_regC &= ~(1<<7);
			break;
		case 0xBA: // RES 7, D
			_cycles = 8;
			_regD &= ~(1<<7);
			break;
		case 0xBB: // RES 7, E
			_cycles = 8;
			_regE &= ~(1<<7);
			break;
		case 0xBC: // RES 7, H
			_cycles = 8;
			_regH &= ~(1<<7);
			break;
		case 0xBD: // RES 7, L
			_cycles = 8;
			_regL &= ~(1<<7);
			break;
		case 0xBE: // RES 7, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) & ~(1<<7));
			break;
		case 0xBF: // RES 7, A
			_cycles = 8;
			_regA &= ~(1<<7);
			break;
			
		case 0xC0: // SET 0, B
			_cycles = 8;
			_regB |=(1<<0);
			break;
		case 0xC1: // SET 0, C
			_cycles = 8;
			_regC |=(1<<0);
			break;
		case 0xC2: // SET 0, D
			_cycles = 8;
			_regD |=(1<<0);
			break;
		case 0xC3: // SET 0, E
			_cycles = 8;
			_regE |=(1<<0);
			break;
		case 0xC4: // SET 0, H
			_cycles = 8;
			_regH |=(1<<0);
			break;
		case 0xC5: // SET 0, L
			_cycles = 8;
			_regL |=(1<<0);
			break;
		case 0xC6: // SET 0, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) |(1<<0));
			break;
		case 0xC7: // SET 0, A
			_cycles = 8;
			_regA |=(1<<0);
			break;
		case 0xC8: // SET 1, B
			_cycles = 8;
			_regB |=(1<<1);
			break;
		case 0xC9: // SET 1, C
			_cycles = 8;
			_regC |=(1<<1);
			break;
		case 0xCA: // SET 1, D
			_cycles = 8;
			_regD |=(1<<1);
			break;
		case 0xCB: // SET 1, E
			_cycles = 8;
			_regE |=(1<<1);
			break;
		case 0xCC: // SET 1, H
			_cycles = 8;
			_regH |=(1<<1);
			break;
		case 0xCD: // SET 1, L
			_cycles = 8;
			_regL |=(1<<1);
			break;
		case 0xCE: // SET 1, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) |(1<<1));
			break;
		case 0xCF: // SET 1, A
			_cycles = 8;
			_regA |=(1<<1);
			break;

		case 0xD0: // SET 2, B
			_cycles = 8;
			_regB |=(1<<2);
			break;
		case 0xD1: // SET 2, C
			_cycles = 8;
			_regC |=(1<<2);
			break;
		case 0xD2: // SET 2, D
			_cycles = 8;
			_regD |=(1<<2);
			break;
		case 0xD3: // SET 2, E
			_cycles = 8;
			_regE |=(1<<2);
			break;
		case 0xD4: // SET 2, H
			_cycles = 8;
			_regH |=(1<<2);
			break;
		case 0xD5: // SET 2, L
			_cycles = 8;
			_regL |=(1<<2);
			break;
		case 0xD6: // SET 2, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) |(1<<2));
			break;
		case 0xD7: // SET 2, A
			_cycles = 8;
			_regA |=(1<<2);
			break;
		case 0xD8: // SET 3, B
			_cycles = 8;
			_regB |=(1<<3);
			break;
		case 0xD9: // SET 3, C
			_cycles = 8;
			_regC |=(1<<3);
			break;
		case 0xDA: // SET 3, D
			_cycles = 8;
			_regD |=(1<<3);
			break;
		case 0xDB: // SET 3, E
			_cycles = 8;
			_regE |=(1<<3);
			break;
		case 0xDC: // SET 3, H
			_cycles = 8;
			_regH |=(1<<3);
			break;
		case 0xDD: // SET 3, L
			_cycles = 8;
			_regL |=(1<<3);
			break;
		case 0xDE: // SET 3, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) |(1<<3));
			break;
		case 0xDF: // SET 3, A
			_cycles = 8;
			_regA |=(1<<3);
			break;

		case 0xE0: // SET 4, B
			_cycles = 8;
			_regB |=(1<<4);
			break;
		case 0xE1: // SET 4, C
			_cycles = 8;
			_regC |=(1<<4);
			break;
		case 0xE2: // SET 4, D
			_cycles = 8;
			_regD |=(1<<4);
			break;
		case 0xE3: // SET 4, E
			_cycles = 8;
			_regE |=(1<<4);
			break;
		case 0xE4: // SET 4, H
			_cycles = 8;
			_regH |=(1<<4);
			break;
		case 0xE5: // SET 4, L
			_cycles = 8;
			_regL |=(1<<4);
			break;
		case 0xE6: // SET 4, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) |(1<<4));
			break;
		case 0xE7: // SET 4, A
			_cycles = 8;
			_regA |=(1<<4);
			break;
		case 0xE8: // SET 5, B
			_cycles = 8;
			_regB |=(1<<5);
			break;
		case 0xE9: // SET 5, C
			_cycles = 8;
			_regC |=(1<<5);
			break;
		case 0xEA: // SET 5, D
			_cycles = 8;
			_regD |=(1<<5);
			break;
		case 0xEB: // SET 5, E
			_cycles = 8;
			_regE |=(1<<5);
			break;
		case 0xEC: // SET 5, H
			_cycles = 8;
			_regH |=(1<<5);
			break;
		case 0xED: // SET 5, L
			_cycles = 8;
			_regL |=(1<<5);
			break;
		case 0xEE: // SET 5, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) |(1<<5));
			break;
		case 0xEF: // SET 5, A
			_cycles = 8;
			_regA |=(1<<5);
			break;

		case 0xF0: // SET 6, B
			_cycles = 8;
			_regB |=(1<<6);
			break;
		case 0xF1: // SET 6, C
			_cycles = 8;
			_regC |=(1<<6);
			break;
		case 0xF2: // SET 6, D
			_cycles = 8;
			_regD |=(1<<6);
			break;
		case 0xF3: // SET 6, E
			_cycles = 8;
			_regE |=(1<<6);
			break;
		case 0xF4: // SET 6, H
			_cycles = 8;
			_regH |=(1<<6);
			break;
		case 0xF5: // SET 6, L
			_cycles = 8;
			_regL |=(1<<6);
			break;
		case 0xF6: // SET 6, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) |(1<<6));
			break;
		case 0xF7: // SET 6, A
			_cycles = 8;
			_regA |=(1<<6);
			break;
		case 0xF8: // SET 7, B
			_cycles = 8;
			_regB |=(1<<7);
			break;
		case 0xF9: // SET 7, C
			_cycles = 8;
			_regC |=(1<<7);
			break;
		case 0xFA: // SET 7, D
			_cycles = 8;
			_regD |=(1<<7);
			break;
		case 0xFB: // SET 7, E
			_cycles = 8;
			_regE |=(1<<7);
			break;
		case 0xFC: // SET 7, H
			_cycles = 8;
			_regH |=(1<<7);
			break;
		case 0xFD: // SET 7, L
			_cycles = 8;
			_regL |=(1<<7);
			break;
		case 0xFE: // SET 7, (HL)
			_cycles = 16;
			this->_gb->_mem->write(_regHL, this->_gb->_mem->read(_regHL) |(1<<7));
			break;
		case 0xFF: // SET 7, A
			_cycles = 8;
			_regA |=(1<<7);
			break;
		default:
			Log(Error, "Unknown extended opcode %2x", opcode);
			break;
		}
		break;
#pragma endregion
#pragma region 0xCC		
	case 0xCC: // CALL Z, a16
		_cycles = 12;
		t = GetOP();
		t |= GetOP()<<8;
		if(!(_regF & F_Z)) break;
		_cycles += 12;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = t;
		break;
	case 0xCD: // CALL a16
		_cycles = 24;
		t = GetOP();
		t |= GetOP()<<8;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = t;
		break;	
	case 0xCE: // ADC A, d8
		_cycles = 8;
		c = GetOP();
		t = ((_regA & 0xF)+(c & 0xF)+((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA + c+((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA + c+((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | (t << 1) | (t2>>4);
		break;
	case 0xCF: // RST 08H
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = 0x08;
		break;
#pragma endregion
#pragma region 0xD0
	case 0xD0: // RET NC
		_cycles = 8;
		if(_regF & F_C) break;
		_cycles += 12;
		_regPC = this->_gb->_mem->read(_regSP) | this->_gb->_mem->read(_regSP+1)<<8;
		_regSP +=2;
		break;
	case 0xD1: // POP DE
		_cycles = 12;
		_regD = this->_gb->_mem->read(_regSP+1);
		_regE = this->_gb->_mem->read(_regSP);
		_regSP += 2;
		break;
	case 0xD2: // JP NC, a16
		_cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(_regF & F_C) break;
		_cycles += 4;
		_regPC = t;
		break;
	case 0xD4: // CALL NC, a16
		_cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(_regF & F_C) break;
		_cycles += 12;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = t;
		break;
	case 0xD5: // PUSH DE
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regD);
		this->_gb->_mem->write(_regSP-2, _regE);
		_regSP-=2;
		break;
	case 0xD6: // SUB A, d8
		_cycles = 8;
		c = GetOP();
		t = ((_regA & 0xF)-(c & 0xF))&0x10; // halfcarry
		t2 = (_regA - c)& 0x100; // carry
		_regA = _regA - c;
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0xD7: // RST 10H
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = 0x10;
		break;
	case 0xD8: // RET C
		_cycles = 8;
		if(!(_regF & F_C)) break;
		_cycles += 12;
		_regPC = this->_gb->_mem->read(_regSP) | this->_gb->_mem->read(_regSP+1)<<8;
		_regSP +=2;
		break;
	case 0xD9: // RETI
		_cycles = 16;
		_regPC = this->_gb->_mem->read(_regSP) | this->_gb->_mem->read(_regSP+1)<<8;
		_regSP +=2;
		_ime = true;
		break;
	case 0xDA: // JP C, a16
		_cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(!(_regF & F_C)) break;
		_cycles += 4;
		_regPC = t;
		break;
	case 0xDC: // CALL C, a16
		_cycles = 12;
		t = GetOP(); t |= GetOP()<<8;
		if(!(_regF & F_C)) break;
		_cycles += 12;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = t;
		break;
	case 0xDE: // SBC A, d8
		_cycles = 8;
		c = GetOP();
		t = ((_regA & 0xF)-(c & 0xF)-((_regF & F_C) != 0))&0x10; // halfcarry
		t2 = (_regA - c-((_regF & F_C) != 0))& 0x100; // carry
		_regA = _regA - c-((_regF & F_C) != 0);
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_N | (t << 1) | (t2>>4);
		break;
	case 0xDF: // RST 18H
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = 0x18;
		break;
#pragma endregion
#pragma region 0xE0
	case 0xE0: // LD (FF00 + n), A
		_cycles = 12;
		this->_gb->_mem->write(0xFF00 + GetOP(), _regA);
		break;
	case 0xE1: // POP HL
		_cycles = 12;
		_regH = this->_gb->_mem->read(_regSP+1);
		_regL = this->_gb->_mem->read(_regSP);
		_regSP += 2;
		break;
	case 0xE2: // LD (FF00 + C), A
		_cycles = 8;
		this->_gb->_mem->write(0xFF00 + _regC, _regA);
		break;
	case 0xE5: // PUSH HL
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regH);
		this->_gb->_mem->write(_regSP-2, _regL);
		_regSP-=2;
		break;
	case 0xE6: // AND d8
		_cycles = 8;
		_regA &= GetOP();
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7) | F_H;
		break;
	case 0xE7: // RST 20H
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = 0x20;
		break;
	case 0xE8: // ADD SP, r8
		_cycles = 16;
		c = GetOP();
		t = ((_regSP&0xF) + ((signed char)c & 0xF)) & 0x10;
		t2 = ((_regSP&0xFF) + ((signed char)c &0xFF)) & 0x100;
		_regF = (_regF & 0x0F) | (t << 1) | (t2 >> 4 );
		_regSP += (signed char)c;
		break;
	case 0xE9: // JP (HL)
		_cycles = 4;
		_regPC = _regHL;
		break;
	case 0xEA: // LD (a16), A
		_cycles = 16;
		this->_gb->_mem->write(GetNN(), _regA);
		break;
	case 0xEE: // XOR d8
		_cycles = 8;
		_regA ^= GetOP();
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xEF: // RST 28H
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = 0x28;
		break;
#pragma endregion
#pragma region 0xF0
	case 0xF0: // LD A, (FF00 + n)
		_cycles = 12;
		_regA = this->_gb->_mem->read(0xFF00 + GetOP());
		break;
	case 0xF1: // POP AF
		_cycles = 12;
		_regA = this->_gb->_mem->read(_regSP+1);
		_regF = this->_gb->_mem->read(_regSP)&0xF0;
		_regSP += 2;
		break;
	case 0xF2: // LD A, FF00 + C)
		_cycles = 8;
		_regA = this->_gb->_mem->read(0xFF00 + _regC);
		break;
	case 0xF3: // DI
		_cycles = 4;
		_ime = false;
		break;
	case 0xF5: // PUSH AF
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regA);
		this->_gb->_mem->write(_regSP-2, _regF);
		_regSP-=2;
		break;
	case 0xF6: // OR d8
		_cycles = 8;
		_regA |= GetOP();
		_regF = (_regF & 0x0F) | ((_regA == 0)<<7);
		break;
	case 0xF7: // RST 30H
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = 0x30;
		break;
	case 0xF8: // LD HL, SP + r8
		_cycles = 12;
		c = GetOP();
		t = ((_regSP&0xF) + ((signed char)c & 0xF)) & 0x10;
		t2 = ((_regSP&0xFF) + ((signed char)c &0xFF)) & 0x100;
		_regF = (_regF & 0x0F) | (t << 1) | (t2 >> 4 );
		_regHL = _regSP + (signed char)c;
		break;
	case 0xF9: // LD SP, HL
		_cycles = 8;
		_regSP = _regHL;
		break;
	case 0xFA: // LD A, (a16)
		_cycles = 16;
		_regA = this->_gb->_mem->read(GetNN());
		break;
	case 0xFB: // EI
		_cycles = 4;
		_ime = true;
		break;
	case 0xFC: // ContinuePoint!!!
		_regPC--;
		t = _regPC;
		_gb->RemoveBreakpoint(t); // Temporary remove breakpoint
		handleInstruction();
		_gb->AddBreakpoint(t);
		break;
	case 0xFD: // Breakpoint!!!
		_cycles = 0;
		_regPC--;
		throw BreakPointException(_regPC);
		break;
	case 0xFE: // CP d8
		_cycles = 8;
		c =  GetOP();
		t = _regA - c;
		t2 = (_regA&0xF)  - (c &0xF);
		_regF = (_regF & 0x0F) | F_N | (((t&0xFF) == 0)<<7) | (((t2 & 0x10)!=0)<<5) | (((t & 0x100)!=0)<<4);
		break;
	case 0xFF: // RST 38H
		_cycles = 16;
		this->_gb->_mem->write(_regSP-1, _regPC>>8);
		this->_gb->_mem->write(_regSP-2, _regPC&0xFF);
		_regSP-= 2;
		_regPC = 0x38;
		break;
#pragma endregion
	default:
		Log(Error,"Unknown opcode %2x", opcode);
		break;
	}
}

#endif