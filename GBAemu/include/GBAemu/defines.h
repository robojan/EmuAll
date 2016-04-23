#pragma once

// General defines
#define FCPU                16780000

// CPU defines
#define ARM_NOP             0xF0001C00
#define THUMB_NOP           0x1C00

// Memory sizes
#define BIOSSIZE            (16 * 1024)
#define WRAMSIZE            (256 * 1024)
#define WRAMCHIPSIZE        (32 * 1024)
#define IOREGISTERSSIZE     (1024)
#define PRAMSIZE            (1024)
#define VRAMSIZE            (96 * 1024)
#define ORAMSIZE            (1024)

// Memory masks
#define BIOSMASK            (0x3FFF)
#define WRAMMASK            (0x3FFFF)
#define WRAMCHIPMASK        (0x7FFF)
#define IOREGISTERSMASK     (0x3FF)
#define PRAMMASK            (0x3FF)
#define VRAMMASK            (0x1FFFF) // Warning this is 128kB not 96kB so extra check required
#define ORAMMASK            (0x3FF)

// Register names
#define REGA1               0
#define REGA2               1
#define REGA3               2
#define REGA4               3
#define REGV1               4
#define REGV2               5
#define REGV3               6
#define REGV4               7
#define REGV5               8
#define REGV6               9
#define REGV7               10
#define REGV8               11
#define REGIP               12
#define REGSP               13
#define REGLR               14
#define REGPC               15

// CPSR bits
#define CPSR_N              31
#define CPSR_Z              30
#define CPSR_C              29
#define CPSR_V              28

#define CPSR_N_MASK         (1<<CPSR_N)
#define CPSR_Z_MASK         (1<<CPSR_Z)
#define CPSR_C_MASK         (1<<CPSR_C)
#define CPSR_V_MASK         (1<<CPSR_V)

