#pragma once

// General defines
#define FCPU                16780000
#define SCREEN_HEIGHT       160
#define SCREEN_WIDTH        240

// CPU defines
#define THUMB_NOP           0xB000
#define ARM_NOP             (0xF0000000 | THUMB_NOP)

// Memory sizes
#define BIOSSIZE            (16 * 1024)
#define WRAMSIZE            (256 * 1024)
#define WRAMCHIPSIZE        (32 * 1024)
#define IOREGISTERSSIZE     (1024)
#define PRAMSIZE            (1024)
#define VRAMSIZE            (128 * 1024) // 96 kB is the actual vram
#define VRAMSIZE_USED       (96 * 1024)
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
#define CPSR_I				7
#define CPSR_F				6
#define CPSR_T              5

#define CPSR_N_MASK         (1<<CPSR_N)
#define CPSR_Z_MASK         (1<<CPSR_Z)
#define CPSR_C_MASK         (1<<CPSR_C)
#define CPSR_V_MASK         (1<<CPSR_V)
#define CPSR_I_MASK         (1<<CPSR_I)
#define CPSR_F_MASK         (1<<CPSR_F)
#define CPSR_T_MASK         (1<<CPSR_T)

// IRQ bits
#define IRQ_VBLANK_NO       0
#define IRQ_HBLANK_NO       1
#define IRQ_VMATCH_NO       2
#define IRQ_TIM0_NO         3
#define IRQ_TIM1_NO         4
#define IRQ_TIM2_NO         5
#define IRQ_TIM3_NO         6
#define IRQ_SERIAL_NO       7
#define IRQ_DMA0_NO         8
#define IRQ_DMA1_NO         9
#define IRQ_DMA2_NO         10
#define IRQ_DMA3_NO         11
#define IRQ_KEYPAD_NO       12
#define IRQ_EXT_NO          13
// IRQ masks
#define IRQ_VBLANK          (1<<IRQ_VBLANK_NO )
#define IRQ_HBLANK          (1<<IRQ_HBLANK_NO )
#define IRQ_VMATCH          (1<<IRQ_VMATCH_NO )
#define IRQ_TIM0            (1<<IRQ_TIM0_NO   )
#define IRQ_TIM1            (1<<IRQ_TIM1_NO   )
#define IRQ_TIM2            (1<<IRQ_TIM2_NO   )
#define IRQ_TIM3            (1<<IRQ_TIM3_NO   )
#define IRQ_SERIAL          (1<<IRQ_SERIAL_NO )
#define IRQ_DMA0            (1<<IRQ_DMA0_NO   )
#define IRQ_DMA1            (1<<IRQ_DMA1_NO   )
#define IRQ_DMA2            (1<<IRQ_DMA2_NO   )
#define IRQ_DMA3            (1<<IRQ_DMA3_NO   )
#define IRQ_KEYPAD          (1<<IRQ_KEYPAD_NO )
#define IRQ_EXT             (1<<IRQ_EXT_NO    )
#define IRQ_MASK            0x3FFF

// Registers
// LCD
#define DISPCNT             0x4000000
#define DISPSTAT            0x4000004
#define VCOUNT              0x4000006
#define BG0CNT              0x4000008
#define BG1CNT              0x400000A
#define BG2CNT              0x400000C
#define BG3CNT              0x400000E
#define BG0HOFS             0x4000010
#define BG0VOFS             0x4000012
#define BG1HOFS             0x4000014
#define BG1VOFS             0x4000016
#define BG2HOFS             0x4000018
#define BG2VOFS             0x400001A
#define BG3HOFS             0x400001C
#define BG3VOFS             0x400001E
#define BG2PA               0x4000020
#define BG2PB               0x4000022
#define BG2PC               0x4000024
#define BG2PD               0x4000026
#define BG2X_L              0x4000028
#define BG2X_H              0x400002A
#define BG2Y_L              0x400002C
#define BG2Y_H              0x400002E
#define BG3PA               0x4000030
#define BG3PB               0x4000032
#define BG3PC               0x4000034
#define BG3PD               0x4000036
#define BG3X_L              0x4000038
#define BG3X_H              0x400003A
#define BG3Y_L              0x400003C
#define BG3Y_H              0x400003E
#define WIN0H               0x4000040
#define WIN1H               0x4000042
#define WIN0V               0x4000044
#define WIN1V               0x4000046
#define WININ               0x4000048
#define WINOUT              0x400004A
#define MOSAIC              0x400004C
#define BLDCNT              0x4000050
#define BLDALPHA            0x4000052
#define BLDY                0x4000054

// Interrupt control
#define IE                  0x4000200
#define IF                  0x4000202
#define IME                 0x4000208

// Control
#define POSTFLG             0x4000300
#define HALTCNT             0x4000301

// DMA
#define DMA0SAD             0x40000B0
#define DMA0DAD             0x40000B4
#define DMA0CNT_L           0x40000B8
#define DMA0CNT_H           0x40000BA
#define DMA1SAD             0x40000BC
#define DMA1DAD             0x40000C0
#define DMA1CNT_L           0x40000C4
#define DMA1CNT_H           0x40000C6
#define DMA2SAD             0x40000C8
#define DMA2DAD             0x40000CC
#define DMA2CNT_L           0x40000D0
#define DMA2CNT_H           0x40000D2
#define DMA3SAD             0x40000D4
#define DMA3DAD             0x40000D8
#define DMA3CNT_L           0x40000DC
#define DMA3CNT_H           0x40000DE