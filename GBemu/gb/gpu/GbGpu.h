#ifndef _GBGPU_H
#define _GBGPU_H
#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

#include "../mem/GbMem.h"
#include "../Gameboy.h"
#include "../defines.h"
#include "../../util/log.h"
#include <stdint.h>

typedef union
{
	uint32_t color;
	struct{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t p;
	};
}gbColor;

typedef gbByte gbPallete;

typedef struct 
{
	gbByte *tile;
	gbByte attributes;
} gbBgMap;

typedef struct
{
	gbByte	x;
	gbByte	y;
	gbByte	nr;
	gbByte	attr;
	gbByte	loc;
} gbSprite;

typedef struct
{
// From GPU to debugger
	int number;
	int palette;
	bool hflip;
	bool vflip;
	bool priority;
// From debugger to GPU
	bool grid;
	int mapMode;
	int dataMode;
	int mouseX;
	int mouseY;
} BackgroundDebuggerData;

typedef struct
{
// From GPU to debugger
	int vram;
	unsigned int address;
	unsigned int number;
// From debugger to GPU
	int mouseX;
	int mouseY;
} TilesDebuggerData;

typedef struct
{
// From GPU to debugger
	int x;
	int y;
	int tile;
	std::string palette;
	bool hflip;
	bool vflip;
	bool priority;
	bool vram;
// From debugger to GPU
	int mouseX;
	int mouseY;
} OAMDebuggerData;

class GbGpu : public GbMemEvent
{
	friend class GbGpuDbg;
public:
	GbGpu(Gameboy *info);
	~GbGpu(void);
	void registerEvents();
	void drawGL(int user);
	bool InitGL(int user);
	void DestroyGL(int user);
	void Reshape(int width, int height, bool keepAspect);

	bool IsEnabled() const { return m_enabled; }

	// Debugging
	BackgroundDebuggerData *GetBackgroundDebugInfo() { return &mBGDebug; }
	void UpdateBackgroundDebug();
	TilesDebuggerData *GetTilesDebugInfo() { return &mTiDebug; }
	void UpdateTilesDebug();
	OAMDebuggerData *GetOAMDebugInfo() { return &mOAMDebug; }
	void UpdateOAMDebug();
	unsigned int GetPalette(int index);

	inline void tick(void);
private:
	void vblank(void);
	void hblank(gbByte ly);
	void oam(gbByte ly);
	void drawing(gbByte ly);
	void MemEvent(address_t address, gbByte val);
	void sortOam(int *sprite);
	bool compSprite(int a, int b);
	inline gbBgMap getTileData(bool bgd, bool bgm, int map) const;
	inline gbColor convertPaletteData(gbByte data, gbByte palette, const gbByte *paletteData) const;
	inline gbColor convertPaletteData(gbByte data, gbByte palette) const;
	inline gbByte getTilePixel(gbByte *data, gbByte x, gbByte y) const;
	void DrawDebugBackground();
	void DrawDebugTiles();
	void DrawDebugOAM();

	Gameboy		*m_gb;
	bool		m_enabled;
	int			m_clocks;
	int			m_lineCounter;
	gbByte		m_mode;
	int			m_lineSprites[40];
	int			m_ly;
	gbPallete	m_bgPaletteData[PALETTE_SIZE];
	gbPallete	m_oPaletteData[PALETTE_SIZE];

	// Main screen
	unsigned int m_texture;
	unsigned int m_surfaceVBO;
	unsigned int m_surfaceUVBO;
	unsigned int m_vao;
	unsigned int m_shaderProgram;
	gbColor		*m_screen;
	gbColor		*m_screen_buffer1;
	gbColor		*m_screen_buffer2;
	gbColor		*mDebugDrawBuffer;
	
	// Background
	BackgroundDebuggerData mBGDebug;
	unsigned int mBGTexture;
	unsigned int mBGVBO;
	unsigned int mBGUVBO;
	unsigned int mBGVAO;
	unsigned int mBGShaderProgram;

	// Tiles
	TilesDebuggerData mTiDebug;
	unsigned int mTiTexture;
	unsigned int mTiVBO;
	unsigned int mTiUVBO;
	unsigned int mTiVAO;
	unsigned int mTiShaderProgram;

	// OAM
	OAMDebuggerData mOAMDebug;
	unsigned int mOAMTexture;
	unsigned int mOAMVBO;
	unsigned int mOAMUVBO;
	unsigned int mOAMVAO;
	unsigned int mOAMShaderProgram;


};

inline void GbGpu::tick(void)
{ 
	if (m_gb == NULL || m_gb->_mem == NULL || m_enabled == false)
	{
		return;
	}
	m_clocks++;
	m_lineCounter--;
	if(m_clocks == REFRESH_CLKS)
	{
		m_clocks = 0; // Update in which clock we are
	}
	if (m_lineCounter == 0)
	{
		m_lineCounter = LINE_CLKS;
		m_ly = (m_clocks) / LINE_CLKS;
		m_gb->_mem->write(LY, (gbByte) m_ly, false);
	}
	if(m_clocks < (REFRESH_CLKS - VBLANK_CLKS))  // The GPU is in active mode
	{
		if (LINE_CLKS - m_lineCounter < OAM_CLKS) // OAM acces mode Mode 2
		{
			if(m_mode != LCDS_M_S)	//First time entering this mode
			{
				m_mode = LCDS_M_S;
				oam(m_ly);
			}
		}
		else if (LINE_CLKS - m_lineCounter < OAM_CLKS + VRAM_CLKS) { // GPU draw mode Mode 3
			// Draw the line
			if(m_mode != LCDS_M_T)
			{
				m_mode = LCDS_M_T;
				drawing(m_ly);
			}			
		} else { // Hblank period Mode 0
			if(m_mode != LCDS_M_H)
			{
				m_mode = LCDS_M_H;
				hblank(m_ly);
			}
		}
	} else {					// The GPU is in VBLANK
		if(m_mode != LCDS_M_V)	// First time entering this mode
		{
			m_mode = LCDS_M_V;
			vblank();
		}
	}
}

inline gbBgMap GbGpu::getTileData(bool bgd, bool bgm, int map) const
{
	gbByte tileNr;
	gbBgMap r;
	if(bgm) // (0=9800-9BFF, 1=9C00-9FFF)
	{
		tileNr = m_gb->_mem->vram[0][BGMAP_VRAM_OFFSET1 + map];
		r.attributes = m_gb->_mem->vram[1][BGMAP_VRAM_OFFSET1 + map];
	} else {
		tileNr = m_gb->_mem->vram[0][BGMAP_VRAM_OFFSET0 + map];
		r.attributes = m_gb->_mem->vram[1][BGMAP_VRAM_OFFSET0 + map];
	}
	if(bgd) // (0=8800-97FF, 1=8000-8FFF)
	{
		r.tile = m_gb->_mem->vram[(r.attributes & BGMAP_ATTR_BANK) ? 1 : 0] + BGDATA_VRAM_OFFSET1 + tileNr*TILE_SIZE;
	} else {
		r.tile = m_gb->_mem->vram[(r.attributes & BGMAP_ATTR_BANK) ? 1 : 0] + BGDATA_VRAM_OFFSET0 + ((int8_t) tileNr)*TILE_SIZE;
	}
	return r;
}

inline gbColor GbGpu::convertPaletteData(gbByte data, gbByte palette, const gbByte *paletteData) const
{
	gbByte *ptr = (unsigned char *)paletteData + palette * 8 + data * 2;
	gbColor r;
	r.r = (ptr[0] & 0x1F)<<3;
	r.g = (((ptr[0] & 0xE0) >> 5)|((ptr[1] & 0x3)<<3))<<3;
	r.b = ((ptr[1]&0x7C)>>2)<<3;
	return r;
}

inline gbColor GbGpu::convertPaletteData(gbByte data, gbByte palette) const
{
	data = (palette >> (data*2))&0x3;
	gbColor r;
	switch(data)
	{
	case 0:
		r.r = 0xFF;
		r.g = 0xFF;
		r.b = 0xFF;
		break; 
	case 1:
		r.r = 0xD3;
		r.g = 0xD3;
		r.b = 0xD3;
		break;
	case 2:
		r.r = 0xA9;
		r.g = 0xA9;
		r.b = 0xA9;
		break;
	case 3:
		r.r = 0x00;
		r.g = 0x00;
		r.b = 0x00;
		break;
	default:
		r.color = 0;
		break;
	}
	return r;
}

inline gbByte GbGpu::getTilePixel(gbByte *data, gbByte x, gbByte y) const 
{
	data += y*2;
	x = 7-x;
	return ((data[0] & (1<<x)) ? 0x1 : 0x0 )|((data[1] & (1<<x)) ? 0x2 : 0x0 );
}

#endif