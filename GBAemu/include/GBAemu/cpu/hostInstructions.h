#pragma once

#include <cstdint>

// The flags functions modify the last four bits in the flags variable,
// NZCV, Negative, Zero, Cary, overflow

#ifdef _MSC_VER
#ifdef _M_IX86
inline void AND(uint32_t x, uint32_t y, uint32_t &result) {
	_asm {
		mov eax, x;
		mov ecx, y;
		and eax, ecx;
		mov ecx, dword ptr[result];
		mov dword ptr[ecx], eax;
	}
}

inline void AND_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	_asm {
		mov eax, x
		mov ecx, y
		and eax, ecx
		pushf
		mov ecx, dword ptr[result]
		mov dword ptr[ecx], eax
		mov ecx, dword ptr[flags]
		mov eax, dword ptr[ecx]
		and eax, 0x3FFFFFFF
		pop dx
		shl edx, 24
		and edx, 0xC0000000
		or eax, edx
		mov dword ptr[ecx], eax
	}
}

inline void XOR(uint32_t x, uint32_t y, uint32_t &result) {
	_asm {
		mov eax, x
		mov ecx, y
		xor eax, ecx
		mov ecx, dword ptr[result]
		mov dword ptr[ecx], eax
	}
}

inline void XOR_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	_asm {
		mov eax, x
		mov ecx, y
		xor eax, ecx
		pushf
		mov ecx, dword ptr[result]
		mov dword ptr[ecx], eax
		mov ecx, dword ptr[flags]
		mov eax, dword ptr[ecx]
		and eax, 0x3FFFFFFF
		pop dx
		shl edx, 24
		and edx, 0xC0000000
		or eax, edx
		mov dword ptr[ecx], eax
	}
}

inline void SUB(uint32_t x, uint32_t y, uint32_t &result) {
	_asm {
		mov eax, x
		mov ecx, y
		sub eax, ecx
		mov ecx, dword ptr[result]
		mov dword ptr[ecx], eax
	}
}

inline void SUB_FLAGS(uint32_t x, uint32_t y, uint32_t &result, uint32_t &flags) {
	_asm {
		mov eax, x
		mov ecx, y
		sub eax, ecx
		pushf
		mov ecx, dword ptr[result]
		mov dword ptr[ecx], eax
		mov ecx, dword ptr[flags]
		mov eax, dword ptr[ecx]
		and eax, 0x0FFFFFFF
		pop dx
		bt edx, 0
		jc noCarry
		or eax, 0x20000000
	noCarry:
		bt edx, 11
		jc noOverflow
		or eax, 0x10000000
	noOverflow:
		shl edx, 24
		and edx, 0xC0000000
		or eax, edx
		mov dword ptr[ecx], eax
	}
}

inline int32_t ASR(int32_t x, uint8_t amount) {
	__asm {
		mov CL, amount
		sar x, CL
	}
	return x;
}
inline uint32_t ROR(uint32_t x, uint8_t amount) {
	__asm {
		mov CL, amount
		ror x, CL
	}
}


#else 
#error Implement these instructions for your architecture
#endif

#else 

#error Define these instructions for your compiler
#endif
