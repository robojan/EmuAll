
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sprites.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../gfx/testImage.h"


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
				MAP[8][y][x] = 0x0000;
			} else {
				MAP[8][y][x] = 0x0001;
			}
			// Checkerboard
			if(((x ^ y) & 1) != 0) {
				MAP[9][y][x] = 0x0000;
			} else {
				MAP[9][y][x] = 0x0004;
			}
			// Checkerboard
			if(((x ^ y) & 1) != 0) {
				MAP[10][y][x] = 0x0000;
			} else {
				MAP[10][y][x] = 0x0005;
			}
			// Checkerboard
			if(((x ^ y) & 1) != 0) {
				MAP[11][y][x] = 0x0000;
			} else {
				MAP[11][y][x] = 0x0006;
			}
			// 32x32 block
			if(x < 4 && y < 4) {
				MAP[12][y][x] = 0x0002;
			} else {
				MAP[12][y][x] = 0x0000;
			}
			// 64x64 block
			if(x < 8 && y < 8) {
				MAP[13][y][x] = 0x0003;
			} else {
				MAP[13][y][x] = 0x0000;
			}
		}
	}
	
	REG_BG2CNT = BG_256_COLOR | BG_SIZE_3 | BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY(2);
	REG_BG1CNT = BG_256_COLOR | BG_SIZE_0 | BG_TILE_BASE(0) | BG_MAP_BASE(12) | BG_PRIORITY(2);
	REG_BG0CNT = BG_256_COLOR | BG_SIZE_0 | BG_TILE_BASE(0) | BG_MAP_BASE(13) | BG_PRIORITY(2);
	
	
	//CpuFastSet(testImagePal, BG_COLORS, (testImagePalLen+3) / 4);
	//CpuFastSet(testImageTiles, TILE_BASE_ADR(0), (testImageTilesLen + 0x40) / 4);
	//memcpy(TILE_BASE_ADR(0), testImageTiles, testImageTilesLen);
	//for(int i = 0; i< 20; i++) {
	//	CpuSet(&testImageMap[i*30], &MAP[8][i][0], (30*2) / 2);
	//}
	
	//REG_BG0CNT = BG_256_COLOR | BG_SIZE_0 | BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY(1);
	//REG_BG1CNT = BG_256_COLOR | BG_SIZE_0 | BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY(1);
	//REG_BG2CNT = BG_256_COLOR | BG_SIZE_0 | BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY(1);
	//REG_BG3CNT = BG_256_COLOR | BG_SIZE_0 | BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY(1);
	
	REG_BG2HOFS = 128;
	REG_BG2VOFS = 192;
	REG_BG0HOFS = -56;
	REG_BG0VOFS = -50;
	
	REG_DISPCNT = MODE_0 | BG1_ON | BG2_ON | BG0_ON;
	
}

void videoInitObjects() {

	REG_DISPCNT |= (1<<6) | OBJ_ENABLE;

	SPRITE_PALETTE[0] = RGB8(0,0,0);
	SPRITE_PALETTE[1] = RGB8(0,255,0);
	SPRITE_PALETTE[2] = RGB8(128,128,128);
	
	// 16x16 
	for(int i = 0; i < 128; i++) {
		SPRITE_GFX[i] = 0x0202;
	}
	for(int i =0; i < 4; i++) {
		SPRITE_GFX[0 + i] = 0x0101;
		SPRITE_GFX[32 + i] = 0x0101;
		
		SPRITE_GFX[92 + i] = 0x0101;
		SPRITE_GFX[124 + i] = 0x0101;
	}
	for(int i = 1; i < 8; i++) {
		SPRITE_GFX[i * 4] = 0x0201;
		SPRITE_GFX[35 + i * 4] = 0x0102;
		SPRITE_GFX[60 + i * 4] = 0x0201;
		SPRITE_GFX[95 + i * 4] = 0x0102;
	}
	
	for(int i = 0; i < 128; i++) {
		OAM[i].attr0 = OBJ_DISABLE;
	}
	
	OAM[0].attr0 = ATTR0_SQUARE | ATTR0_NORMAL | ATTR0_COLOR_256 | OBJ_Y(0);
	OAM[0].attr1 = ATTR1_SIZE_16 | OBJ_X(0);
	OAM[0].attr2 = OBJ_CHAR(0) | OBJ_PRIORITY(0);
	OAM[1].attr0 = ATTR0_SQUARE | ATTR0_NORMAL | ATTR0_COLOR_256 | OBJ_Y(32);
	OAM[1].attr1 = ATTR1_SIZE_16 | OBJ_X(16);
	OAM[1].attr2 = OBJ_CHAR(0) | OBJ_PRIORITY(0);
	OAM[2].attr0 = ATTR0_SQUARE | OBJ_ROT_SCALE_ON | OBJ_DOUBLE | OBJ_256_COLOR | OBJ_Y(32);
	OAM[2].attr1 = ATTR1_SIZE_16 | OBJ_ROT_SCALE(0) | OBJ_X(64);
	OAM[2].attr2 = OBJ_CHAR(0) | OBJ_PRIORITY(0);
	OAM[3].attr0 = ATTR0_SQUARE | OBJ_ROT_SCALE_ON | OBJ_DOUBLE | OBJ_256_COLOR | OBJ_Y(240);
	OAM[3].attr1 = ATTR1_SIZE_16 | OBJ_ROT_SCALE(0) | OBJ_X(64);
	OAM[3].attr2 = OBJ_CHAR(0) | OBJ_PRIORITY(0);
	((OBJAFFINE *)OAM)[0].pa = 192;
	((OBJAFFINE *)OAM)[0].pb = 64;
	((OBJAFFINE *)OAM)[0].pc = 0;
	((OBJAFFINE *)OAM)[0].pd = 192;
}

void videoInitWindow() {
	REG_WIN0H = (121) | (20 << 8);
	REG_WIN0V = (93) | (16 << 8);
	REG_WIN1H = (150) | (100 << 8);
	REG_WIN1V = (129) | (48 << 8);
	
	REG_WININ = 0x0312;
	REG_WINOUT = 0x0004;
	
	REG_DISPCNT |= (1<<13) | (1<<14) | (0 << 15);
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

	videoInitMode0();
	videoInitObjects();
	videoInitWindow();

	while (1) {
		VBlankIntrWait();
	}
}


