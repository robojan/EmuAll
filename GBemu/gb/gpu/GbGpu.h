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
#include <vector>
#include <emuall/graphics/shader.h>

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

	bool IsEnabled() const { return _enabled; }

	bool LoadState(const SaveData_t *data);
	bool SaveState(std::vector<uint8_t> &data);

	// Debugging
	BackgroundDebuggerData *GetBackgroundDebugInfo() { return &_BGDebug; }
	void UpdateBackgroundDebug();
	TilesDebuggerData *GetTilesDebugInfo() { return &_TiDebug; }
	void UpdateTilesDebug();
	OAMDebuggerData *GetOAMDebugInfo() { return &_OAMDebug; }
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

	Gameboy		*_gb;
	bool		_enabled;
	int			_clocks;
	int			_lineCounter;
	gbByte		_mode;
	int			_lineSprites[40];
	int			_ly;
	gbPallete	_bgPaletteData[PALETTE_SIZE];
	gbPallete	_oPaletteData[PALETTE_SIZE];

	// Main screen
	unsigned int _texture;
	unsigned int _surfaceVBO;
	unsigned int _surfaceUVBO;
	unsigned int _vao;
	Shader		_shader;
	gbColor		*_screen;
	gbColor		*_screen_buffer1;
	gbColor		*_screen_buffer2;
	gbColor		*_debugDrawBuffer;
	
	// Background
	BackgroundDebuggerData _BGDebug;
	unsigned int _BGTexture;
	unsigned int _BGVBO;
	unsigned int _BGUVBO;
	unsigned int _BGVAO;
	Shader		_BGShader;

	// Tiles
	TilesDebuggerData _TiDebug;
	unsigned int _TiTexture;
	unsigned int _TiVBO;
	unsigned int _TiUVBO;
	unsigned int _TiVAO;
	Shader		_TiShader;

	// OAM
	OAMDebuggerData _OAMDebug;
	unsigned int _OAMTexture;
	unsigned int _OAMVBO;
	unsigned int _OAMUVBO;
	unsigned int _OAMVAO;
	Shader		_OAMShader;


};

inline void GbGpu::tick(void)
{ 
	if (_gb == NULL || _gb->_mem == NULL || _enabled == false)
	{
		return;
	}
	_clocks++;
	_lineCounter--;
	if(_clocks == REFRESH_CLKS)
	{
		_clocks = 0; // Update in which clock we are
	}
	if (_lineCounter == 0)
	{
		_lineCounter = LINE_CLKS;
		_ly = (_clocks) / LINE_CLKS;
		_gb->_mem->write(LY, (gbByte) _ly, false);
	}
	if(_clocks < (REFRESH_CLKS - VBLANK_CLKS))  // The GPU is in active mode
	{
		if (LINE_CLKS - _lineCounter < OAM_CLKS) // OAM acces mode Mode 2
		{
			if(_mode != LCDS_M_S)	//First time entering this mode
			{
				_mode = LCDS_M_S;
				oam(_ly);
			}
		}
		else if (LINE_CLKS - _lineCounter < OAM_CLKS + VRAM_CLKS) { // GPU draw mode Mode 3
			// Draw the line
			if(_mode != LCDS_M_T)
			{
				_mode = LCDS_M_T;
				drawing(_ly);
			}			
		} else { // Hblank period Mode 0
			if(_mode != LCDS_M_H)
			{
				_mode = LCDS_M_H;
				hblank(_ly);
			}
		}
	} else {					// The GPU is in VBLANK
		if(_mode != LCDS_M_V)	// First time entering this mode
		{
			_mode = LCDS_M_V;
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
		tileNr = _gb->_mem->_vram[0][BGMAP_VRAM_OFFSET1 + map];
		r.attributes = _gb->_mem->_vram[1][BGMAP_VRAM_OFFSET1 + map];
	} else {
		tileNr = _gb->_mem->_vram[0][BGMAP_VRAM_OFFSET0 + map];
		r.attributes = _gb->_mem->_vram[1][BGMAP_VRAM_OFFSET0 + map];
	}
	if(bgd) // (0=8800-97FF, 1=8000-8FFF)
	{
		r.tile = _gb->_mem->_vram[(r.attributes & BGMAP_ATTR_BANK) ? 1 : 0] + BGDATA_VRAM_OFFSET1 + tileNr*TILE_SIZE;
	} else {
		r.tile = _gb->_mem->_vram[(r.attributes & BGMAP_ATTR_BANK) ? 1 : 0] + BGDATA_VRAM_OFFSET0 + ((int8_t) tileNr)*TILE_SIZE;
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