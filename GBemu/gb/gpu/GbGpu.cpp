#include "../../util/memDbg.h"


#include "GbGpu.h"
#include <stdlib.h>

#include <GL/glew.h>
#include <gl/wglew.h>

#include <algorithm>
#include <string>

GbGpu::GbGpu(Gameboy *master)
{
	m_gb = master;
	m_enabled = false;
	m_clocks = 0;
	m_lineCounter = LINE_CLKS;
	m_mode = 0;

	m_screen_buffer1 = new gbColor[SCREEN_HEIGHT * SCREEN_WIDTH];
	m_screen_buffer2 = new gbColor[SCREEN_HEIGHT * SCREEN_WIDTH];
	m_screen = m_screen_buffer1;

	mDebugDrawBuffer = new gbColor[512*512];

	memset(m_screen_buffer1, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(gbColor));
	memset(m_screen_buffer2, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(gbColor));
	memset(mDebugDrawBuffer, 0, 512 * 512 * sizeof(gbColor));
	memset(m_bgPaletteData, 0, sizeof(m_bgPaletteData));
	memset(m_oPaletteData, 0, sizeof(m_oPaletteData));
	memset(&mBGDebug, 0, sizeof(mBGDebug));
	memset(&mTiDebug, 0, sizeof(mTiDebug));
	mBGDebug.mapMode = 2;
	mBGDebug.dataMode = 2;

	// Initialize GLEW
	glewInit();
}

GbGpu::~GbGpu(void)
{
	delete[] m_screen_buffer1;
	delete[] m_screen_buffer2;
	delete[] mDebugDrawBuffer;
}

void GbGpu::registerEvents()
{
	m_gb->_mem->registerEvent(BCPS, this);
	m_gb->_mem->registerEvent(BCPD, this);
	m_gb->_mem->registerEvent(OCPS, this);
	m_gb->_mem->registerEvent(OCPD, this);
	m_gb->_mem->registerEvent(LCDC, this);
	m_gb->_mem->registerEvent(LY, this);
}

void GbGpu::UpdateBackgroundDebug()
{
	if (unsigned(mBGDebug.mouseX) < 256 && unsigned(mBGDebug.mouseY) < 256)
	{
		int tx = mBGDebug.mouseX / 8;
		int ty = mBGDebug.mouseY / 8;
		bool bgd = mBGDebug.dataMode == 2 ? (m_gb->_mem->read(LCDC) & LCDC_BS) != 0 : mBGDebug.dataMode == 1;
		bool bgm = mBGDebug.mapMode == 2 ? (m_gb->_mem->read(LCDC) & LCDC_BTS) != 0 : mBGDebug.mapMode  == 1;
		gbBgMap tile = m_gb->_gpu->getTileData(bgd, bgm, tx + (ty * 32));
		if (bgm)
		{
			mBGDebug.number = m_gb->_mem->vram[0][BGMAP_VRAM_OFFSET1 + tx + (ty * 32)];
		}
		else {
			mBGDebug.number = m_gb->_mem->vram[0][BGMAP_VRAM_OFFSET0 + tx + (ty * 32)];
		}
		mBGDebug.palette = (tile.attributes & BGMAP_PALETTE) != 0;
		mBGDebug.hflip = (tile.attributes & BGMAP_ATTR_HFLIP) != 0;
		mBGDebug.vflip = (tile.attributes & BGMAP_ATTR_VFLIP) != 0;
		mBGDebug.priority = (tile.attributes & BGMAP_ATTR_PRIO) != 0;
	}
}

void GbGpu::UpdateTilesDebug()
{
	if (unsigned(mTiDebug.mouseY) < 260 && unsigned(mTiDebug.mouseX) < 346)
	{
		int x = mTiDebug.mouseX * 289 / (346 * 9);
		int y = mTiDebug.mouseY * 217 / (260 * 9);
		mTiDebug.vram = x < (16) ? 0x00 : 0x01;
		x %= 16;
		mTiDebug.address = (x + y * 16) * 0x10 + 0x8000;
		mTiDebug.number = y < 12 ? (x + y * 16) : (x + (y - 12) * 16 - 64);
	}
}

void GbGpu::UpdateOAMDebug()
{
	if (unsigned(mOAMDebug.mouseY) < 326 && unsigned(mOAMDebug.mouseX) < 265)
	{
		int x = mOAMDebug.mouseX * 73 / (265 * 9);
		int y = mOAMDebug.mouseY * 86 / (326 * 17);
		int objectnr = x + y * 8;
		mOAMDebug.y = m_gb->_mem->mem[0xF][OAM_OFFSET + objectnr * 4 + 0];
		mOAMDebug.x = m_gb->_mem->mem[0xF][OAM_OFFSET + objectnr * 4 + 1];
		mOAMDebug.tile = m_gb->_mem->mem[0xF][OAM_OFFSET + objectnr * 4 + 2];
		int attr = m_gb->_mem->mem[0xF][OAM_OFFSET + objectnr * 4 + 3];
		if (m_gb->IsGBC())
		{
			mOAMDebug.palette = "PAL";
			mOAMDebug.palette.append(std::to_string(attr & 0x7));
		}
		else {
			mOAMDebug.palette = "OBJ";
			mOAMDebug.palette.append((attr & OAM_PALETTENR) ? "1" : "0");
		}
		mOAMDebug.priority = (attr & OAM_PRIO) != 0;
		mOAMDebug.hflip = (attr & OAM_XFLIP) != 0;
		mOAMDebug.vflip = (attr & OAM_YFLIP) != 0;
		mOAMDebug.vram = (attr & OAM_VRAM) != 0;
	}
}

unsigned int GbGpu::GetPalette(int index)
{
	if (index < 8 * 4)
	{
		// BG palette
		return convertPaletteData(index % 4, index / 4, m_bgPaletteData).color;
	}
	else if (index >= 8 * 4 && index < 16 * 4) {
		// Obj palette
		index -= 8 * 4;
		return convertPaletteData(index % 4, index / 4, m_oPaletteData).color;
	}
	else if (index >= 16 * 4 && index < 17 * 4) {
		// BGP
		index -= 16 * 4;
		return convertPaletteData(index %4, m_gb->_mem->read(BGP)).color;
	}
	else if (index >= 17 * 4 && index < 18 * 4) {
		// OBP0
		index -= 17 * 4;
		return convertPaletteData(index % 4, m_gb->_mem->read(OBP0)).color;
	}
	else if (index >= 18 * 4 && index < 19 * 4) {
		// OBP1
		index -= 18 * 4;
		return convertPaletteData(index % 4, m_gb->_mem->read(OBP1)).color;
	}
	else {
		Log(Warn, "Invalid index in GetPalette");
		return 0;
	}
}

void GbGpu::MemEvent(address_t address, gbByte val)
{
	switch(address)
	{
	case BCPS:
		m_gb->_mem->write(BCPD, m_bgPaletteData[val & 0x3F], false);
		break;
	case BCPD:
		m_bgPaletteData[m_gb->_mem->read(BCPS) & 0x3F] = val;
		if (m_gb->_mem->read(BCPS) & PAL_INC)
		{
			m_gb->_mem->write(BCPS, (m_gb->_mem->read(BCPS) & PAL_INC) | (m_gb->_mem->read(BCPS) + 1) & 0x3F);
		}
		break;
	case OCPS:
		m_gb->_mem->write(OCPD, m_oPaletteData[val & 0x3F], false);
		break;
	case OCPD:
		m_oPaletteData[m_gb->_mem->read(OCPS) & 0x3F] = val;
		if (m_gb->_mem->read(OCPS) & PAL_INC)
		{
			m_gb->_mem->write(OCPS, (m_gb->_mem->read(OCPS) & PAL_INC) | (m_gb->_mem->read(OCPS) + 1) & 0x3F);
		}
		break;
	case LCDC:
		if(m_enabled == false && (val & LCDC_EN))
		{
			m_enabled = true;
			m_clocks = 5 ;
			m_lineCounter = LINE_CLKS-5;
			m_ly = 0;
			m_gb->_mem->write(LY, 0x00, false);
		} 
		if(!(val & LCDC_EN))
		{
			m_enabled = false;
			m_gb->_mem->write(LCDS, (m_gb->_mem->read(LCDS) & ~(LCDS_M_M | LCDS_CF)) | LCDS_M_V, false);
		}
		break;
	case LY:
		m_gb->_mem->write(LY, 0, false);
		m_clocks = 0;
		m_lineCounter = LINE_CLKS;
		m_ly = 0;
		break;
	default:
		return;
	}
}

void GbGpu::vblank(void)
{
	m_gb->_mem->setAccessable(true, true);
	if (m_gb->_mem->read(LCDS) & LCDS_VI) // If the Vblank interrupt is enabled, set the status interrupt active
	{
		m_gb->_mem->write(IF, m_gb->_mem->read(IF) | INT_LCDSTAT);
	}
	m_gb->_mem->write(IF, m_gb->_mem->read(IF) | INT_VBLANK);
	m_gb->_mem->write(LCDS, (m_gb->_mem->read(LCDS) & ~LCDS_M_M) | LCDS_M_V);

	if (m_screen == m_screen_buffer1)
	{
		m_screen = m_screen_buffer2;
	}
	else {
		m_screen = m_screen_buffer1;
	}
}

void GbGpu::hblank(gbByte ly)
{
	m_gb->_mem->setAccessable(true, true);
	// Hblank
	if (m_gb->_mem->read(LCDS) & LCDS_HI) // If the HBlank interrupt is enabled, set the status interrupt active
	{
		m_gb->_mem->write(IF, m_gb->_mem->read(IF) | INT_LCDSTAT);
	}
	m_gb->_mem->write(LCDS, (m_gb->_mem->read(LCDS) & ~LCDS_M_M) | LCDS_M_H);

	// if the current line is equal to the compare line and the interrupt is enabled call the interrupt
	bool lyc = m_gb->_mem->read(LY) == m_gb->_mem->read(LYC);
	m_gb->_mem->write(LCDS, (m_gb->_mem->read(LCDS) & ~LCDS_CF) | (lyc ? LCDS_CF : 0)); // set the LY == LYC flag
	if (lyc && (m_gb->_mem->read(LCDS) & LCDS_CI))
	{
		m_gb->_mem->write(IF, m_gb->_mem->read(IF) | INT_LCDSTAT);
	}

	// HDMA
	gbByte hdma5 = m_gb->_mem->read(HDMA5);
	if(hdma5 != 0xff)
	{
		int i;
		gbByte hdma1 = m_gb->_mem->read(HDMA1);
		gbByte hdma2 = m_gb->_mem->read(HDMA2);
		gbByte hdma3 = m_gb->_mem->read(HDMA3);
		gbByte hdma4 = m_gb->_mem->read(HDMA4);
		int source = ((hdma1<<8) | hdma2)&0xFFF0;
		int destination = (((hdma3<<8) | hdma4)&0x1FF0)|0x8000;
		if((hdma5 & HDMA5_M)){
			for(i = 0; i<0x10; i++)
			{
				m_gb->_mem->write(destination++, m_gb->_mem->read(source++));
			}
			m_gb->_mem->write(HDMA1, source >> 8);
			m_gb->_mem->write(HDMA2, source & 0xFF);
			m_gb->_mem->write(HDMA3, destination >> 8);
			m_gb->_mem->write(HDMA4, destination & 0xFF);
			m_gb->_mem->write(HDMA5, hdma5 - 1);
		} 
	}
}

void GbGpu::oam(gbByte ly)
{
	m_gb->_mem->setAccessable(true, false);
	bool gbc = (m_gb->_mem->read(CGB) & 0x80) != 0; // gameboy color mode
	bool large = (m_gb->_mem->read(LCDC) & LCDC_OS) != 0; // 8 by 16 sprites

	if (m_gb->_mem->read(LCDS) & LCDS_OI) // If the OAM interrupt is enabled, set the status interrupt active
	{
		m_gb->_mem->write(IF, m_gb->_mem->read(IF) | INT_LCDSTAT);
	}
	m_gb->_mem->write(LCDS, (m_gb->_mem->read(LCDS) & ~LCDS_M_M) | LCDS_M_S);

	// Fill buffer with all active sprites
	gbByte *ptr = m_gb->_mem->mem[0xF] + OAM_OFFSET;
	int j = 0;
	int sprite[40];
	memset(sprite, -1, sizeof(sprite));
	for(int i = 0; i < 40; i++)
	{
		gbByte *data = ptr+i*4;
		gbByte iny = ly - (data[0]-16);
		if(!(iny>=0 && iny < 16))
		{
			continue;
		}
		sprite[j] = i;
		j++;
	}
	if(gbc)
	{
		std::sort(sprite, &sprite[j]);
	} else {
		//sortOam(sprite);
		//TODO: add sorting
	}
	for(int k = 0; k<j; k++)
	{
		m_lineSprites[k] = sprite[k];
	}
	for(;j<40;j++)
	{
		m_lineSprites[j] = -1;
	}

}

void GbGpu::sortOam(int *sprite)
{
	for(int i = 0; i<40; i++)
	{
		if(sprite[i] == -1)
		{
			break;
		}
		if(compSprite(sprite[i+1], sprite[i]))
		{
			int t = sprite[i+1];
			sprite[i+1] = sprite[i];
			sprite[i] = t;
			i-=2;
			if(i < 0 ) i = 0;
		}
	}
}

bool GbGpu::compSprite(int a, int b)
{
	gbByte *ptr = m_gb->_mem->mem[0xF] + OAM_OFFSET;
	gbByte *ptrA = ptr+a*4;
	gbByte *ptrB = ptr+b*4;
	if(ptrA[1] == ptrB[1])
	{
		return a < b;
	}
	return ptrA[1] < ptrB[1];
}
