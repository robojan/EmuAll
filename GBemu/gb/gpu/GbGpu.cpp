#include "../../util/memDbg.h"


#include "GbGpu.h"
#include <stdlib.h>

#include <GL/glew.h>
#include <gl/wglew.h>

#include <algorithm>
#include <string>

GbGpu::GbGpu(Gameboy *master)
{
	_gb = master;
	_enabled = false;
	_clocks = 0;
	_lineCounter = LINE_CLKS;
	_mode = 0;

	_screen_buffer1 = new gbColor[SCREEN_HEIGHT * SCREEN_WIDTH];
	_screen_buffer2 = new gbColor[SCREEN_HEIGHT * SCREEN_WIDTH];
	_screen = _screen_buffer1;

	_debugDrawBuffer = new gbColor[512*512];

	memset(_screen_buffer1, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(gbColor));
	memset(_screen_buffer2, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(gbColor));
	memset(_debugDrawBuffer, 0, 512 * 512 * sizeof(gbColor));
	memset(_bgPaletteData, 0, sizeof(_bgPaletteData));
	memset(_oPaletteData, 0, sizeof(_oPaletteData));
	memset(&_BGDebug, 0, sizeof(_BGDebug));
	memset(&_TiDebug, 0, sizeof(_TiDebug));
	_BGDebug.mapMode = 2;
	_BGDebug.dataMode = 2;

}

GbGpu::~GbGpu(void)
{
	delete[] _screen_buffer1;
	delete[] _screen_buffer2;
	delete[] _debugDrawBuffer;
}

void GbGpu::registerEvents()
{
	_gb->_mem->registerEvent(BCPS, this);
	_gb->_mem->registerEvent(BCPD, this);
	_gb->_mem->registerEvent(OCPS, this);
	_gb->_mem->registerEvent(OCPD, this);
	_gb->_mem->registerEvent(LCDC, this);
	_gb->_mem->registerEvent(LY, this);
}

void GbGpu::UpdateBackgroundDebug()
{
	if (unsigned(_BGDebug.mouseX) < 256 && unsigned(_BGDebug.mouseY) < 256)
	{
		int tx = _BGDebug.mouseX / 8;
		int ty = _BGDebug.mouseY / 8;
		bool bgd = _BGDebug.dataMode == 2 ? (_gb->_mem->read(LCDC) & LCDC_BS) != 0 : _BGDebug.dataMode == 1;
		bool bgm = _BGDebug.mapMode == 2 ? (_gb->_mem->read(LCDC) & LCDC_BTS) != 0 : _BGDebug.mapMode  == 1;
		gbBgMap tile = _gb->_gpu->getTileData(bgd, bgm, tx + (ty * 32));
		if (bgm)
		{
			_BGDebug.number = _gb->_mem->_vram[0][BGMAP_VRAM_OFFSET1 + tx + (ty * 32)];
		}
		else {
			_BGDebug.number = _gb->_mem->_vram[0][BGMAP_VRAM_OFFSET0 + tx + (ty * 32)];
		}
		_BGDebug.palette = (tile.attributes & BGMAP_PALETTE) != 0;
		_BGDebug.hflip = (tile.attributes & BGMAP_ATTR_HFLIP) != 0;
		_BGDebug.vflip = (tile.attributes & BGMAP_ATTR_VFLIP) != 0;
		_BGDebug.priority = (tile.attributes & BGMAP_ATTR_PRIO) != 0;
	}
}

void GbGpu::UpdateTilesDebug()
{
	if (unsigned(_TiDebug.mouseY) < 260 && unsigned(_TiDebug.mouseX) < 346)
	{
		int x = _TiDebug.mouseX * 289 / (346 * 9);
		int y = _TiDebug.mouseY * 217 / (260 * 9);
		_TiDebug.vram = x < (16) ? 0x00 : 0x01;
		x %= 16;
		_TiDebug.address = (x + y * 16) * 0x10 + 0x8000;
		_TiDebug.number = y < 12 ? (x + y * 16) : (x + (y - 12) * 16 - 64);
	}
}

void GbGpu::UpdateOAMDebug()
{
	if (unsigned(_OAMDebug.mouseY) < 326 && unsigned(_OAMDebug.mouseX) < 265)
	{
		int x = _OAMDebug.mouseX * 73 / (265 * 9);
		int y = _OAMDebug.mouseY * 86 / (326 * 17);
		int objectnr = x + y * 8;
		_OAMDebug.y = _gb->_mem->_mem[0xF][OAM_OFFSET + objectnr * 4 + 0];
		_OAMDebug.x = _gb->_mem->_mem[0xF][OAM_OFFSET + objectnr * 4 + 1];
		_OAMDebug.tile = _gb->_mem->_mem[0xF][OAM_OFFSET + objectnr * 4 + 2];
		int attr = _gb->_mem->_mem[0xF][OAM_OFFSET + objectnr * 4 + 3];
		if (_gb->IsGBC())
		{
			_OAMDebug.palette = "PAL";
			_OAMDebug.palette.append(std::to_string(attr & 0x7));
		}
		else {
			_OAMDebug.palette = "OBJ";
			_OAMDebug.palette.append((attr & OAM_PALETTENR) ? "1" : "0");
		}
		_OAMDebug.priority = (attr & OAM_PRIO) != 0;
		_OAMDebug.hflip = (attr & OAM_XFLIP) != 0;
		_OAMDebug.vflip = (attr & OAM_YFLIP) != 0;
		_OAMDebug.vram = (attr & OAM_VRAM) != 0;
	}
}

unsigned int GbGpu::GetPalette(int index)
{
	if (index < 8 * 4)
	{
		// BG palette
		return convertPaletteData(index % 4, index / 4, _bgPaletteData).color;
	}
	else if (index >= 8 * 4 && index < 16 * 4) {
		// Obj palette
		index -= 8 * 4;
		return convertPaletteData(index % 4, index / 4, _oPaletteData).color;
	}
	else if (index >= 16 * 4 && index < 17 * 4) {
		// BGP
		index -= 16 * 4;
		return convertPaletteData(index %4, _gb->_mem->read(BGP)).color;
	}
	else if (index >= 17 * 4 && index < 18 * 4) {
		// OBP0
		index -= 17 * 4;
		return convertPaletteData(index % 4, _gb->_mem->read(OBP0)).color;
	}
	else if (index >= 18 * 4 && index < 19 * 4) {
		// OBP1
		index -= 18 * 4;
		return convertPaletteData(index % 4, _gb->_mem->read(OBP1)).color;
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
		_gb->_mem->write(BCPD, _bgPaletteData[val & 0x3F], false);
		break;
	case BCPD:
		_bgPaletteData[_gb->_mem->read(BCPS) & 0x3F] = val;
		if (_gb->_mem->read(BCPS) & PAL_INC)
		{
			_gb->_mem->write(BCPS, (_gb->_mem->read(BCPS) & PAL_INC) | (_gb->_mem->read(BCPS) + 1) & 0x3F);
		}
		break;
	case OCPS:
		_gb->_mem->write(OCPD, _oPaletteData[val & 0x3F], false);
		break;
	case OCPD:
		_oPaletteData[_gb->_mem->read(OCPS) & 0x3F] = val;
		if (_gb->_mem->read(OCPS) & PAL_INC)
		{
			_gb->_mem->write(OCPS, (_gb->_mem->read(OCPS) & PAL_INC) | (_gb->_mem->read(OCPS) + 1) & 0x3F);
		}
		break;
	case LCDC:
		if(_enabled == false && (val & LCDC_EN))
		{
			_enabled = true;
			_clocks = 5 ;
			_lineCounter = LINE_CLKS-5;
			_ly = 0;
			_gb->_mem->write(LY, 0x00, false);
		} 
		if(!(val & LCDC_EN))
		{
			_enabled = false;
			_gb->_mem->write(LCDS, (_gb->_mem->read(LCDS) & ~(LCDS_M_M | LCDS_CF)) | LCDS_M_V, false);
		}
		break;
	case LY:
		_gb->_mem->write(LY, 0, false);
		_clocks = 0;
		_lineCounter = LINE_CLKS;
		_ly = 0;
		break;
	default:
		return;
	}
}

void GbGpu::vblank(void)
{
	_gb->_mem->setAccessable(true, true);
	if (_gb->_mem->read(LCDS) & LCDS_VI) // If the Vblank interrupt is enabled, set the status interrupt active
	{
		_gb->_mem->write(IF, _gb->_mem->read(IF) | INT_LCDSTAT);
	}
	_gb->_mem->write(IF, _gb->_mem->read(IF) | INT_VBLANK);
	_gb->_mem->write(LCDS, (_gb->_mem->read(LCDS) & ~LCDS_M_M) | LCDS_M_V);

	if (_screen == _screen_buffer1)
	{
		_screen = _screen_buffer2;
	}
	else {
		_screen = _screen_buffer1;
	}
}

void GbGpu::hblank(gbByte ly)
{
	_gb->_mem->setAccessable(true, true);
	// Hblank
	if (_gb->_mem->read(LCDS) & LCDS_HI) // If the HBlank interrupt is enabled, set the status interrupt active
	{
		_gb->_mem->write(IF, _gb->_mem->read(IF) | INT_LCDSTAT);
	}
	_gb->_mem->write(LCDS, (_gb->_mem->read(LCDS) & ~LCDS_M_M) | LCDS_M_H);

	// if the current line is equal to the compare line and the interrupt is enabled call the interrupt
	bool lyc = _gb->_mem->read(LY) == _gb->_mem->read(LYC);
	_gb->_mem->write(LCDS, (_gb->_mem->read(LCDS) & ~LCDS_CF) | (lyc ? LCDS_CF : 0)); // set the LY == LYC flag
	if (lyc && (_gb->_mem->read(LCDS) & LCDS_CI))
	{
		_gb->_mem->write(IF, _gb->_mem->read(IF) | INT_LCDSTAT);
	}

	// HDMA
	gbByte hdma5 = _gb->_mem->read(HDMA5);
	if(hdma5 != 0xff)
	{
		int i;
		gbByte hdma1 = _gb->_mem->read(HDMA1);
		gbByte hdma2 = _gb->_mem->read(HDMA2);
		gbByte hdma3 = _gb->_mem->read(HDMA3);
		gbByte hdma4 = _gb->_mem->read(HDMA4);
		int source = ((hdma1<<8) | hdma2)&0xFFF0;
		int destination = (((hdma3<<8) | hdma4)&0x1FF0)|0x8000;
		if((hdma5 & HDMA5_M)){
			for(i = 0; i<0x10; i++)
			{
				_gb->_mem->write(destination++, _gb->_mem->read(source++));
			}
			_gb->_mem->write(HDMA1, source >> 8);
			_gb->_mem->write(HDMA2, source & 0xFF);
			_gb->_mem->write(HDMA3, destination >> 8);
			_gb->_mem->write(HDMA4, destination & 0xFF);
			_gb->_mem->write(HDMA5, hdma5 - 1);
		} 
	}
}

void GbGpu::oam(gbByte ly)
{
	_gb->_mem->setAccessable(true, false);
	bool gbc = (_gb->_mem->read(CGB) & 0x80) != 0; // gameboy color mode
	bool large = (_gb->_mem->read(LCDC) & LCDC_OS) != 0; // 8 by 16 sprites

	if (_gb->_mem->read(LCDS) & LCDS_OI) // If the OAM interrupt is enabled, set the status interrupt active
	{
		_gb->_mem->write(IF, _gb->_mem->read(IF) | INT_LCDSTAT);
	}
	_gb->_mem->write(LCDS, (_gb->_mem->read(LCDS) & ~LCDS_M_M) | LCDS_M_S);

	// Fill buffer with all active sprites
	gbByte *ptr = _gb->_mem->_mem[0xF] + OAM_OFFSET;
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
		_lineSprites[k] = sprite[k];
	}
	for(;j<40;j++)
	{
		_lineSprites[j] = -1;
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
	gbByte *ptr = _gb->_mem->_mem[0xF] + OAM_OFFSET;
	gbByte *ptrA = ptr+a*4;
	gbByte *ptrB = ptr+b*4;
	if(ptrA[1] == ptrB[1])
	{
		return a < b;
	}
	return ptrA[1] < ptrB[1];
}

static const uint32_t StateGPUid = 0x47505520;

// 0 - id
// 4 - size
// 8 - enabled
// 9 - mode
// 10 - clocks
// 14 - linecounter
// 18 - linesprites[40]
// 178 - ly
// 182 - bgPaletteData[64]
// 246 - oPaletteData[64]
// 310 - 

bool GbGpu::LoadState(const SaveData_t *data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	uint8_t *ptr = (uint8_t *)data->miscData;
	int miscLen = data->miscDataLen;
	// Find gpu segment
	while (miscLen >= 8) {
		uint32_t id = conv->convu32(*(uint32_t *)(ptr + 0));
		uint32_t len = conv->convu32(*(uint32_t *)(ptr + 4));
		if (id == StateGPUid && len >= 310) {
			_enabled = ptr[8] != 0;
			_mode = ptr[9];
			_clocks = conv->convi32(*(int32_t *)(ptr + 10));
			_lineCounter = conv->convi32(*(int32_t *)(ptr + 14));
			for (int i = 0; i < 40; i++) {
				_lineSprites[i]  = conv->convi32(*(int32_t *)(ptr + 18 + sizeof(int) * i));
			}
			_ly = conv->convi32(*(int32_t *)(ptr + 178));
			for (int i = 0; i < PALETTE_SIZE; i++) {
				_bgPaletteData[i] = ptr[182 + sizeof(gbPallete) * i];
			}
			for (int i = 0; i < PALETTE_SIZE; i++) {
				_oPaletteData[i] = ptr[246 + sizeof(gbPallete) * i];
			}
			return true;
		}
		ptr += len;
		miscLen -= len;
	}
	return false;
}

bool GbGpu::SaveState(std::vector<uint8_t> &data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	int dataLen = 310;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	*(uint32_t *)(ptr + 0) = conv->convu32(StateGPUid);
	*(uint32_t *)(ptr + 4) = conv->convu32(dataLen);
	ptr[8] = _enabled ? 0x01 : 0x00;
	ptr[9] = _mode;
	*(uint32_t *)(ptr + 10) = conv->convi32(_clocks);
	*(uint32_t *)(ptr + 14) = conv->convi32(_lineCounter);
	for (int i = 0; i < 40; i++) {
		*(uint32_t *)(ptr + 18 + sizeof(int) * i) = conv->convi32(_lineSprites[i]);
	}
	*(uint32_t *)(ptr + 178) = conv->convi32(_ly);
	for (int i = 0; i < PALETTE_SIZE; i++) {
		ptr[182 + sizeof(gbPallete) * i] = _bgPaletteData[i];
	}
	for (int i = 0; i < PALETTE_SIZE; i++) {
		ptr[246 + sizeof(gbPallete) * i] = _oPaletteData[i];
	}
	return true;
}