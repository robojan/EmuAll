
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sprites.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void videoInitMode0() {
	VBlankIntrWait();
	
	// Create palette
	BG_COLORS[0] = RGB8(0xFF, 0xFF, 0xFF);
	BG_COLORS[1] = RGB8(0x00, 0x00, 0x00);
	BG_COLORS[2] = RGB8(0xFF, 0x00, 0x00);
	BG_COLORS[3] = RGB8(0x00, 0xFF, 0x00);
	BG_COLORS[4] = RGB8(0x00, 0x00, 0xFF);
	BG_COLORS[5] = RGB8(0xFF, 0x00, 0xFF);
	BG_COLORS[6] = RGB8(0xFF, 0xFF, 0x00);
	// Create tiles
	for(int i = 0; i< 64; i+=4) {
		*((s32 *)(TILE_BASE_ADR(0)+64*0+i)) = 0x00000000;
		*((s32 *)(TILE_BASE_ADR(0)+64*1+i)) = 0x01010101;
		*((s32 *)(TILE_BASE_ADR(0)+64*2+i)) = 0x02020202;
		*((s32 *)(TILE_BASE_ADR(0)+64*3+i)) = 0x03030303;
		*((s32 *)(TILE_BASE_ADR(0)+64*4+i)) = 0x04040404;
		*((s32 *)(TILE_BASE_ADR(0)+64*5+i)) = 0x05050505;
		*((s32 *)(TILE_BASE_ADR(0)+64*6+i)) = 0x06060606;
	}
	// create maps
	for(int y = 0; y < 32; y++) {
		for(int x = 0; x < 32; x++) {		
			// Checkerboard
			if(((x ^ y) & 1) != 0) {
				MAP[8][y][x] = 0x0002;
			} else {
				MAP[8][y][x] = 0x0001;
			}
			// 32x32 block
			if(x < 4 && y < 4) {
				MAP[12][y][x] = 0x0004;
			} else {
				MAP[12][y][x] = 0x0000;
			}
		}
	}
	
	REG_BG3CNT = BG_256_COLOR | BG_SIZE_0 | BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY(2);
	REG_BG2CNT = BG_256_COLOR | BG_SIZE_0 | BG_TILE_BASE(0) | BG_MAP_BASE(12) | BG_PRIORITY(1);
	
	REG_BG3HOFS = 0;
	REG_BG3VOFS = 0;
	REG_BG2HOFS = 0;
	REG_BG2VOFS = 0;
	
	REG_DISPCNT = MODE_0 | BG2_ON | BG3_ON;
	
}

void hblankHandler() {
	int currentLine = REG_VCOUNT;
	REG_BG2HOFS = -currentLine;
	REG_BG2VOFS = -currentLine;
	if((currentLine & 1) != 0) {
		BG_COLORS[4] = RGB8(0x00, 0x00, 0xFF);
	} else {
		BG_COLORS[4] = RGB8(0x00, 0xff, 0x00);
	}
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	RegisterRamReset(RESET_PALETTE | RESET_VRAM | RESET_OAM);

	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_HBLANK, hblankHandler);
	

	videoInitMode0();
	

	while (1) {
		VBlankIntrWait();
	}
}


