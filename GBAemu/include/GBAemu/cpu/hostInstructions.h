#pragma once

#include <cstdint>

// The flags functions modify the last four bits in the flags variable,
// NZCV, Negative, Zero, Cary, overflow

#ifdef _MSC_VER
#ifdef _M_IX86

inline bool IsConditionMet(uint8_t cond, uint32_t &flags) {
	switch (cond) {
	case 0: // Equal
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		je returnTrue;
		jmp returnFalse;
	}
	case 1: // Not equal
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jne returnTrue;
		jmp returnFalse;
	}
	case 2: // Carry set
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jc returnTrue;
		jmp returnFalse;
	}
	case 3: // Carry clear
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jnc returnTrue;
		jmp returnFalse;
	}
	case 4: // negative
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		js returnTrue;
		jmp returnFalse;
	}
	case 5: // positive
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jns returnTrue;
		jmp returnFalse;
	}
	case 6: // overflow
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jo returnTrue;
		jmp returnFalse;
	}
	case 7: // no overflow
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jno returnTrue;
		jmp returnFalse;
	}
	case 8: // Unsigned higher
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jnc returnFalse;
		jz returnFalse;
		jmp returnTrue;
	}
	case 9: // Unsigned lower or same
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jnc returnTrue;
		jz returnTrue;
		jmp returnFalse;
	}
	case 10: // Signed greater than or equal
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jge returnTrue;
		jmp returnFalse;
	}
	case 11: // signed less than or equal
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jl returnTrue;
		jmp returnFalse;
	}
	case 12: // signed greater than
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jg returnTrue;
		jmp returnFalse;
	}
	case 13: // signed less than or equal
		__asm {
		mov eax, dword ptr[flags];
		mov eax, dword ptr[eax];
		push ax;
		popf;
		jle returnTrue;
		jmp returnFalse;
	}
	case 14: // always
		goto returnTrue;
	case 15: // never
		goto returnFalse;
	}
returnTrue:
	return true;
returnFalse:
	return false;
}

inline void AND(uint32_t x, uint32_t y, uint32_t &result) {
	__asm {
		mov eax, x;
		mov ecx, y;
		and eax, ecx;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
	}
}

inline void AND_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		and eax, ecx;
		pushf;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void EOR(uint32_t x, uint32_t y, uint32_t &result) {
	__asm {
		mov eax, x
		mov ecx, y
		xor eax, ecx
		mov ecx, dword ptr[result]
		mov dword ptr[ecx], eax
	}
}

inline void EOR_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		xor eax, ecx;
		pushf;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void SUB(uint32_t x, uint32_t y, uint32_t &result) {
	__asm {
		mov eax, x;
		mov ecx, y;
		sub eax, ecx;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
	}
}

inline void SUB_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		sub eax, ecx;
		pushf;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFF73E;
		pop dx;
		and edx, 0x000008C1;
		or eax, edx;
		xor eax, 0x1;
		mov dword ptr[ecx], eax;
	}
}

inline void ADD(uint32_t x, uint32_t y, uint32_t &result) {
	__asm {
		mov eax, x;
		mov ecx, y;
		add eax, ecx;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
	}
}

inline void ADD_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		add eax, ecx;
		pushf;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFF73E;
		pop dx;
		and edx, 0x000008C1;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void ADC(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		push ax;
		popf;
		mov eax, x;
		mov ecx, y;
		adc eax, ecx;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
	}
}

inline void ADC_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		// Load carry information
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		push ax;
		popf;
		mov eax, x;
		mov ebx, y;
		adc eax, ebx;
		pushf;
		mov ebx, dword ptr[result];
		mov dword ptr[ebx], eax;
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFF73E;
		pop dx;
		and edx, 0x000008C1;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void SBC(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		xor eax, 1
		push ax;
		popf;
		mov eax, x;
		mov ecx, y;
		sbb eax, ecx;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
	}
}

inline void SBC_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		// Load carry information
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		xor eax,1
		push ax;
		popf;
		mov eax, x;
		mov ebx, y;
		sbb eax, ebx;
		pushf;
		mov ebx, dword ptr[result];
		mov dword ptr[ebx], eax;
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFF73E;
		pop dx;
		xor edx, 1;
		and edx, 0x000008C1;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void TST(uint32_t x, uint32_t y, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		and eax, ecx;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void TEQ(uint32_t x, uint32_t y, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		xor eax, ecx;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void CMP(uint32_t x, uint32_t y, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		cmp eax, ecx;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFF73E;
		pop dx;
		and edx, 0x000008C1;
		or eax, edx;
		xor eax, 0x1;
		mov dword ptr[ecx], eax;
	}
}

inline void CMN(uint32_t x, uint32_t y, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		add eax, ecx;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFF73E;
		pop dx;
		and edx, 0x000008C1;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void ORR(uint32_t x, uint32_t y, uint32_t &result) {
	__asm {
		mov eax, x;
		mov ecx, y;
		or eax, ecx;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
	}
}

inline void ORR_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		or eax, ecx;
		pushf;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void MOV_FLAGS(uint32_t x, uint32_t &flags) {
	__asm {
		mov eax, x;
		or eax, eax;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void BIC(uint32_t x, uint32_t y, uint32_t &result) {
	__asm {
		mov eax, x;
		mov ecx, y;
		not ecx;
		and eax, ecx;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
	}
}

inline void BIC_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov ecx, y;
		not ecx;
		and eax, ecx;
		pushf;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void MVN_FLAGS(uint32_t x, uint32_t &flags) {
	__asm {
		mov eax, x;
		not eax;
		or eax, eax;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void LSL_FLAGS(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov cl, amount;
		shl eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3E;
		pop dx;
		and edx, 0x000000C1;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void LSR_FLAGS(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov cl, amount;
		shr eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3E;
		pop dx;
		and edx, 0x000000C1;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void ASR_FLAGS(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov cl, amount;
		sar eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3E;
		pop dx;
		and edx, 0x000000C1;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void MUL_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov edx, y;
		mul edx;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		or eax, eax;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void SMLAL(uint32_t x, uint32_t y, uint32_t &resultLo, uint32_t &resultHi) {
	__asm {
		mov eax, x;
		mov edx, y;
		imul edx;
		mov ecx, dword ptr[resultLo];
		mov ecx, dword ptr[ecx];
		add eax, ecx;
		mov ecx, dword ptr[resultHi];
		mov ecx, ecx;
		adc edx, ecx;
		mov ecx, dword ptr[resultLo];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[resultHi];
		mov dword ptr[ecx], edx;
	}
}

inline void SMLAL_FLAGS(uint32_t x, uint32_t y, uint32_t &resultLo, uint32_t &resultHi, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov edx, y;
		imul edx;
		mov ecx, dword ptr[resultLo];
		mov ecx, dword ptr[ecx];
		add eax, ecx;
		mov ecx, dword ptr[resultHi];
		mov ecx, ecx;
		adc edx, ecx;
		mov ecx, dword ptr[resultLo];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[resultHi];
		mov dword ptr[ecx], edx;
		or eax, edx;
		pushf;
		or edx, edx;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x00000080;
		or eax, edx;
		pop dx;
		and edx, 0x00000040;
		or eax, edx;
		mov dword ptr[ecx], edx;		
	}
}

inline void UMLAL(uint32_t x, uint32_t y, uint32_t &resultLo, uint32_t &resultHi) {
	__asm {
		mov eax, x;
		mov edx, y;
		mul edx;
		mov ecx, dword ptr[resultLo];
		mov ecx, dword ptr[ecx];
		add eax, ecx;
		mov ecx, dword ptr[resultHi];
		mov ecx, ecx;
		adc edx, ecx;
		mov ecx, dword ptr[resultLo];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[resultHi];
		mov dword ptr[ecx], edx;
	}
}

inline void UMLAL_FLAGS(uint32_t x, uint32_t y, uint32_t &resultLo, uint32_t &resultHi, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov edx, y;
		mul edx;
		mov ecx, dword ptr[resultLo];
		mov ecx, dword ptr[ecx];
		add eax, ecx;
		mov ecx, dword ptr[resultHi];
		mov ecx, ecx;
		adc edx, ecx;
		mov ecx, dword ptr[resultLo];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[resultHi];
		mov dword ptr[ecx], edx;
		or eax, edx;
		pushf;
		or edx, edx;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x00000080;
		or eax, edx;
		pop dx;
		and edx, 0x00000040;
		or eax, edx;
		mov dword ptr[ecx], edx;
	}
}

inline void SMULL(uint32_t x, uint32_t y, uint32_t &resultLo, uint32_t &resultHi) {
	__asm {
		mov eax, x;
		mov edx, y;
		imul edx;
		mov ecx, dword ptr[resultLo];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[resultHi];
		mov dword ptr[ecx], edx;
	}
}

inline void SMULL_FLAGS(uint32_t x, uint32_t y, uint32_t &resultLo, uint32_t &resultHi, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov edx, y;
		imul edx;
		mov ecx, dword ptr[resultLo];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[resultHi];
		mov dword ptr[ecx], edx;
		or eax, edx;
		pushf;
		or edx, edx;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x00000080;
		or eax, edx;
		pop dx;
		and edx, 0x00000040;
		or eax, edx;
		mov dword ptr[ecx], edx;
	}
}

inline void UMULL(uint32_t x, uint32_t y, uint32_t &resultLo, uint32_t &resultHi) {
	__asm {
		mov eax, x;
		mov edx, y;
		mul edx;
		mov ecx, dword ptr[resultLo];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[resultHi];
		mov dword ptr[ecx], edx;
	}
}

inline void UMULL_FLAGS(uint32_t x, uint32_t y, uint32_t &resultLo, uint32_t &resultHi, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov edx, y;
		mul edx;
		mov ecx, dword ptr[resultLo];
		mov dword ptr[ecx], eax;
		mov ecx, dword ptr[resultHi];
		mov dword ptr[ecx], edx;
		or eax, edx;
		pushf;
		or edx, edx;
		pushf;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x00000080;
		or eax, edx;
		pop dx;
		and edx, 0x00000040;
		or eax, edx;
		mov dword ptr[ecx], edx;
	}
}

inline void MLA(uint32_t a, uint32_t b, uint32_t c, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, a;
		mov edx, b;
		mul edx;
		mov edx, c;
		add eax, edx;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
	}
}

inline void MLA_FLAGS(uint32_t a, uint32_t b, uint32_t c, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, a;
		mov edx, b;
		mul edx;
		mov edx, c;
		ADD eax, edx;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3F;
		pop dx;
		and edx, 0x000000C0;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline int32_t SAR(int32_t x, uint8_t amount) {
	__asm {
		mov CL, amount;
		mov eax, x;
		sar eax, CL;
		mov x, eax;
	}
	return x;
}

inline void ROR(uint32_t x, uint8_t amount, uint32_t &result) {
	__asm {
		mov eax, x;
		mov cl, amount;
		ror eax, cl;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
	}
}

inline void ROR_FLAGS(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov cl, amount;
		ror eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFF3E;
		pop dx;
		and edx, 0x000000C1;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void ROR_CFLAG(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov cl, amount;
		ror eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFFFE;
		pop dx;
		and edx, 0x00000001;
		or eax, edx;
		mov dword ptr[ecx], eax;
}
}

inline void RRX(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		push ax;
		mov eax, x;
		mov cl, amount;
		popf;
		rcr eax, cl;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
	}
}

inline void RRX_CFLAG(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		push ax;
		mov eax, x;
		mov cl, amount;
		popf;
		rcr eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFFFE;
		pop dx;
		and edx, 0x00000001;
		or eax, edx;
		mov dword ptr[ecx], eax;
	}
}

inline void LSL(uint32_t x, uint8_t amount, uint32_t &result) {
	__asm {
		mov eax, x;
		mov cl, amount;
		shl eax, cl;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
	}
}

inline void LSL_CFLAG(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov cl, amount;
		shl eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFFFE;
		pop dx;
		and edx, 0x00000001;
		or eax, edx;
		mov dword ptr[ecx], eax;
}
}

inline void LSR(uint32_t x, uint8_t amount, uint32_t &result) {
	__asm {
		mov eax, x;
		mov cl, amount;
		shr eax, cl;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
	}
}

inline void LSR_CFLAG(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov cl, amount;
		shr eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFFFE;
		pop dx;
		and edx, 0x00000001;
		or eax, edx;
		mov dword ptr[ecx], eax;
}
}

inline void ASR_CFLAG(uint32_t x, uint8_t amount, uint32_t &result, uint32_t &flags) {
	__asm {
		mov eax, x;
		mov cl, amount;
		sar eax, cl;
		pushf;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
		mov ecx, dword ptr[flags];
		mov eax, dword ptr[ecx];
		and eax, 0xFFFFFFFE;
		pop dx;
		and edx, 0x00000001;
		or eax, edx;
		mov dword ptr[ecx], eax;
}
}

inline void ASR(uint32_t x, uint8_t amount, uint32_t &result) {
	__asm {
		mov eax, x;
		mov cl, amount;
		sar eax, cl;
		MOV ecx, dword ptr[result];
		MOV dword ptr[ecx], eax;
	}
}


#else 
#error Implement these instructions for your architecture
#endif

#else 

#error Define these instructions for your compiler
#endif
