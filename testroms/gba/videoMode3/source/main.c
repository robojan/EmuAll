
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sprites.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../gfx/testImage.h"


void videoInitMode3() {
	VBlankIntrWait();

	for(int i = 0; i < 19200; i++) {
		((u32*)VRAM)[i] = testImageBitmap[i];
	}
	
	REG_BG2X = -10 << 8;
	REG_BG2Y = -10 << 8;
	
	REG_BG2PA = 128;
	REG_BG2PB = 0;
	REG_BG2PC = 0;
	REG_BG2PD = 128;
	
	REG_BG2CNT = (1<<13);
	
	REG_DISPCNT = MODE_3 | BG2_ON ;
	
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

	videoInitMode3();	

	while (1) {
		VBlankIntrWait();
	}
}


