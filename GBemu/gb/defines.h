#ifndef _DEFINES_GB_H
#define _DEFINES_GB_H

#define REG_BC				0
#define REG_DE				1
#define REG_HL				2
#define REG_SP				3
#define REG_PC				4

#define REG_B				0
#define REG_C				1
#define REG_D				2
#define REG_E				3
#define REG_H				4
#define REG_L				5
#define REG_F				6
#define REG_A				7

#define INPUT_A				1
#define INPUT_B				2
#define INPUT_U				3
#define INPUT_D				4
#define INPUT_L				5
#define INPUT_R				6
#define INPUT_START			7
#define INPUT_SELECT		8

#define FCPU				4194304UL		// clocks per second
#define RESET_ADDRESS		0x0100			// The address where the program starts

#define VRAM_BANKS			2				// The amount of video ram banks
#define VRAM_BANK_SIZE		(8*1024)		// 8 kb per bank and 2 banks
#define WRAM_BANKS			8				// The amount of ram banks
#define WRAM_BANK_SIZE		(4*1024)		// 4 kb per bank and 8 banks
#define ERAM_BANK_SIZE		(4*1024)		// 8 kb per bank
#define REGS_BANK_SIZE		0x1000			// The size of the register bank (0xF000 - 0xFFFF)
#define ROM_BANK_SIZE		0x4000			// The size of a Bank

#define KEY1				0xFF4D			// The cpu speed switch
#define KEY1_CURSPEED		(1<<7)			// The bit that indicates double speed
#define KEY1_PREP			(1<<0)			// When this bit is set, the cpu switches speed on the next halt instruction

#define FDIV				16384
#define DIV					0xFF04
#define TIMA				0xFF05
#define TMA					0xFF06
#define TAC					0xFF07
#define TAC_EN				(1<<2)
#define TAC_CLK				3

#define TIMER_CLK0_CLKS		FCPU/4096
#define TIMER_CLK1_CLKS		FCPU/262144
#define TIMER_CLK2_CLKS		FCPU/65536
#define TIMER_CLK3_CLKS		FCPU/16384

#define INT_VBLANK_ADDR		0x0040
#define INT_LCDSTAT_ADDR	0x0048
#define INT_TIMER_ADDR		0x0050
#define INT_SERIAL_ADDR		0x0058
#define INT_JOYPAD_ADDR		0x0060

#define F_Z					0x80
#define F_N					0x40
#define F_H					0x20
#define F_C					0x10

#define NIN_LOGO			0x0104 // Nintendo logo
#define NIN_SIZE			0x30
#define TITLE				0x0134 // title
#define TITLE_SIZE			16
#define TITLE_CGB_SIZE		11
#define MANU_CODE			0x013F // Manufacturer code in GBC
#define CGB					0x0143 // gameboycolor flag
#define NLC					0x0144 // New license code
#define SGB					0x0146 // Super gameboy flag

#define CARTRIDGE_TYPE		0x0147
#define ROM_ONLY			0x00
#define MBC1				0x01
#define MBC1_RAM			0x02
#define MBC1_RAM_BAT		0x03
#define MBC2				0x05
#define MBC2_BAT			0x06
#define ROM_RAM				0x08
#define ROM_RAM_BAT			0x09
#define MMM01				0x0B
#define MMM01_RAM			0x0c
#define MMM01_RAM_BAT		0x0d
#define MBC3_TIM_BAT		0x0F
#define MBC3_TIM_RAM_BAT	0x10
#define MBC3				0x11
#define MBC3_RAM			0x12
#define MBC3_RAM_BAT		0x13
#define MBC4				0x15
#define MBC4_RAM			0x16
#define MBC4_RAM_BAT		0x17
#define MBC5				0x19
#define MBC5_RAM			0x1A
#define MBC5_RAM_BAT		0x1B
#define MBC5_RUM			0x1C
#define MBC5_RUM_RAM		0x1D
#define MBC5_RUM_RAM_BAT	0x1E
#define POCKET_CAM			0xFC
#define BANDAI				0xFD
#define HUC3				0xFE
#define HUC1_RAM_BAT		0xFF

#define ROM_SIZE			0x148
#define SIO_32K				0x00
#define SIO_64K				0x01
#define SIO_128K			0x02
#define SIO_256K			0x03
#define SIO_512K			0x04
#define SIO_1M				0x05
#define SIO_2M				0x06
#define SIO_4M				0x07
#define SIO_11M				0x52
#define SIO_12M				0x53
#define SIO_15M				0x54

#define RAM_SIZE			0x149
#define SIA_NONE			0x00
#define SIA_2KB				0x01
#define SIA_8KB				0x02
#define SIA_32KB			0x03

#define DEST_CODE			0x014A
#define OLC					0x014B
#define USE_NLC				0x33
#define ROM_VERSION			0x014C
#define HEADER_CHECKSUM		0x014D
#define GLOBAL_CHECKSUM		0x014E

#define SVBK				0xFF70 // wram bank
#define IF					0xFF0F	// Interrupt flag
#define IE					0xFFFF	// Interrupt enable
#define INT_VBLANK			(1<<0)
#define INT_LCDSTAT			(1<<1)
#define INT_TIMER			(1<<2)
#define INT_SERIAL			(1<<3)
#define INT_JOYPAD			(1<<4)

#define DMA					0xFF46

#define HDMA1				0xFF51 // dma source H
#define HDMA2				0xFF52 // dma source L
#define HDMA3				0xFF53 // dma dest H
#define HDMA4				0xFF54 // dma dest L
#define HDMA5				0xFF55 // dma options
#define HDMA5_M				(1<<7) // mode
#define HDMA5_L				~HDMA5_M // length mask


#define LCDC				0xFF40
#define LCDC_EN				(1<<7)
#define LCDC_TS				(1<<6)
#define LCDC_WE				(1<<5)
#define LCDC_BS				(1<<4)
#define LCDC_BTS			(1<<3)
#define LCDC_OS				(1<<2)
#define LCDC_OE				(1<<1)
#define LCDC_BD				(1<<0)

#define LCDS				0xFF41
#define LCDS_CI				(1<<6)
#define LCDS_OI				(1<<5)
#define LCDS_VI				(1<<4)
#define LCDS_HI				(1<<3)
#define LCDS_CF				(1<<2)
#define LCDS_M_H			0
#define LCDS_M_V			1
#define LCDS_M_S			2
#define LCDS_M_T			3
#define LCDS_M_M			3

#define SCY					0xFF42
#define SCX					0xFF43

#define LY					0xFF44
#define LYC					0xFF45

#define WY					0xFF4A
#define WX					0xFF4B

#define BGP					0xFF47
#define WHITE				0
#define LGRAY				1
#define DGRAY				2
#define BLACK				3

#define OBP0				0xFF48
#define OBP1				0xFF49

#define BCPS				0xFF68
#define BCPD				0xFF69
#define OCPS				0xFF6A
#define OCPD				0xFF6B
#define PAL_INC				(1<<7)

#define VBK					0xFF4F
#define VBK_BANK			(1<<0)

#define REFRESH_CLKS		70224	// every 70244 clks there is a complete screen refresh
#define VBLANK_CLKS			4560	// A vblank has a duration of 4560 clks
#define LINE_CLKS			456		// A line takes 456 clks to complete
#define HBLANK_CLKS			204		// A hblank has a duration of 201-207 clocks
#define OAM_CLKS			80		// The OAM access time is 77-83 clks
#define VRAM_CLKS			172		// The gpu is accessing the memory for 169-174 clks

#define SCREEN_HEIGHT		144		// The number of lines in the screen
#define SCREEN_WIDTH		160		// The width of the screen in pixels

#define PALETTE_SIZE		0x40	// The size of the palette data

#define BGMAP_VRAM_OFFSET0	0x1800	// The offset for the vram background map information 0
#define BGMAP_VRAM_OFFSET1	0x1C00	// The offset for the vram background map information 1
#define BGDATA_VRAM_OFFSET0 0x1000	// The offset for the vram background data 0
#define BGDATA_VRAM_OFFSET1	0x0000	// The offset for the vram background data 1
#define OAM_OFFSET			0xE00	// The offset for the oam memory

#define BGMAP_PALETTE		0x7		// Mask for background palette
#define BGMAP_ATTR_BANK		(1<<3)	// Wich bank to use for background data
#define BGMAP_ATTR_HFLIP	(1<<5)	// Horizontal flip attribute
#define BGMAP_ATTR_VFLIP	(1<<6)	// Vertical flip attribute
#define BGMAP_ATTR_PRIO		(1<<7)	// BG-to_OAM priority attribute background is always in the foreground

#define TILE_SIZE			0x10	// The size of one chunk of tile data, 2 bytes per line and 8 lines
#define OAM_VRAM			(1<<3)	// Vram bank
#define OAM_PALETTENR		(1<<4)	// palette number NON GBC
#define SPRITE_OFFSET		0x0000	// offset in videoram
#define OAM_PALETTE			0x7		// OAM palette mask
#define OAM_XFLIP			(1<<5)	// Flip in the x axis
#define OAM_YFLIP			(1<<6)	// Flip in the y axis
#define OAM_PRIO			(1<<7)	// OAM priority

#define PRIO_BG				(1<<7)	// Has the background priority
#define PRIO_WI				(1<<6)	// Pixel shows window

#define JOYP				0xFF00	// Joypad register
#define BUTTON_KEYS			(1<<5)	// Select button keys
#define DIR_KEYS			(1<<4)	// Select direction keys
#define J_START				(1<<3)	// Start key
#define J_SELECT			(1<<2)	// Select key
#define J_B					(1<<1)	// Button B
#define J_A					(1<<0)	// Button A
#define J_DOWN				(1<<3)	// Down key
#define J_UP				(1<<2)	// Up key
#define J_LEFT				(1<<1)	// Left key
#define J_RIGHT				(1<<0)	// Right key

#define NR10				0xFF10	// Audio sweep register
#define NR10_SWEEP_TIME		4		// Location where sweeptime starts
#define	NR10_SWEEP_DEC		(1<<3)	// The frequency decreases
#define NR10_SWEEP_AMOUNT	0x7		// The amount with each step decreases
#define SWEEP_FREQ			128		// The frequency at Which sweep executes 
#define NR11				0xFF11	// Audio Channel 1 info
#define NR11_DUTY_MASK		(3<<6)	// Duty cycle mask
#define NR11_DUTY125		(0<<6)	// 12.5% duty cycle
#define NR11_DUTY25			(1<<6)	// 25% duty cycle
#define NR11_DUTY50			(2<<6)	// 50% duty cycle
#define NR11_DUTY75			(3<<6)	// 75% duty cycle
#define NR11_LENGTH			0x3F	// Mask for the length
#define LENGTH_FREQ			256		// The frequency at which the length is calculated
#define NR12				0xFF12	// Audio envelope register
#define NR12_VOL			0xF0	// Initial volume mask
#define NR12_VOL_POS		4		// Initial volume position
#define NR12_INC			(1<<3)	// Envelope increase
#define NR12_NUMBER			(7<<0)	// Number of envelope sweep
#define ENVELOPE_FREQ		64		// The frequency at which the envelope is updated
#define NR13				0xFF13	// Audio Frequency lsb
#define NR14				0xFF14	// Audio Frequency msb
#define NR14_FREQMSB_MASK	(0x7)	// Frequency msb mask
#define NR14_RESTART		(1<<7)	// Restart sound
#define NR14_COUNTER		(1<<6)	// Stop when counter expires
#define NR21				0xFF16	// Audio Channel 2 info
#define NR21_DUTY_MASK		NR11_DUTY_MASK
#define NR21_DUTY125		NR11_DUTY125
#define NR21_DUTY25			NR11_DUTY25
#define NR21_DUTY50			NR11_DUTY50
#define NR21_DUTY75			NR11_DUTY75
#define NR22				0xFF17	// Audio envelope register
#define NR22_VOL			NR12_VOL
#define NR22_VOL_POS		NR12_VOL_POS
#define NR22_INC			NR12_INC
#define NR22_NUMBER			NR12_NUMBER
#define NR23				0xFF18	// Audio Frequency lsb
#define NR24				0xFF19	// Audio Frequency msb
#define NR24_FREQMSB_MASK	NR14_FREQMSB_MASK
#define NR24_RESTART		NR14_RESTART
#define NR24_COUNTER		NR14_COUNTER
#define NR30				0xFF1A	// Audio Sound enable
#define NR31				0xFF1B	// Audio length
#define NR32				0xFF1C	// Audio Volume 
#define NR33				0xFF1D	// Audio Freq lsb
#define NR34				0xFF1E	// Audio Freq msb
#define WAVERAMSTART		0xFF30	// Audio wave pattern first address
#define WAVERAMSTOP			0xFF3F	// Audio wave pattern last address
#define NR41				0xFF20	// Audio sound length
#define NR42				0xFF21	// Audio envelope
#define NR43				0xFF22	// Audio Polynomial Counter
#define NR44				0xFF23	// Audio control
#define NR50				0xFF24	// Channel control
#define NR50_SO2			(1<<7)	// Vin SO2 On/Off
#define NR50_SO2_VOL		(7<<4)	// Vin SO2 Volume
#define NR50_SO1			(1<<3)	// Vin SO1 On/Off
#define NR50_SO1_VAL		(7<<0)	// Vin SO1 Volume
#define NR51				0xFF25	// Sound output
#define NR51_CH4_SO2		(1<<7)	// Channel 4 to SO2
#define NR51_CH3_SO2		(1<<6)	// Channel 3 to SO2
#define NR51_CH2_SO2		(1<<5)	// Channel 2 to SO2
#define NR51_CH1_SO2		(1<<4)	// Channel 1 to SO2
#define NR51_CH4_SO1		(1<<3)	// Channel 4 to SO1
#define NR51_CH3_SO1		(1<<2)	// Channel 3 to SO1
#define NR51_CH2_SO1		(1<<1)	// Channel 2 to SO1
#define NR51_CH1_SO1		(1<<0)	// Channel 1 to SO1
#define NR52				0xFF26	// Sound switches
#define NR52_ALL			(1<<7)	// All channels On/Off
#define NR52_CH4			(1<<3)	// Channel4 On/Off
#define NR52_CH3			(1<<2)	// Channel3 On/Off
#define NR52_CH2			(1<<1)	// Channel2 On/Off
#define NR52_CH1			(1<<0)	// Channel1 On/Off

#endif