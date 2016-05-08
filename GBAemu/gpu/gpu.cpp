
#include <GL/glew.h>
#include <GL/wglew.h>

#include <GBAemu/defines.h>
#include <GBAemu/gpu/gpu.h>
#include <GBAemu/gba.h>
#include <GBAemu/util/preprocessor.h>
#include <GBAemu/util/log.h>

#include <emuall/graphics/graphicsException.h>

#include "../resources/resources.h"


Gpu::Gpu(Gba &system) :
	_debugTiles8BitDepth{ false, false, false },
	_debugTilesGridEnabled{ true, true, true },
	_system(system), _vcount(0), _hcount(0),
	_paletteInitialized{false, false}, _paletteVao{nullptr, nullptr},
	_tilesInitialized{false, false, false}, _tilesVao{nullptr, nullptr, nullptr},
	_oamInitialized(false), _oamVao(nullptr),
	_bgInitialized{ false, false, false, false}, _bgVao{nullptr, nullptr, nullptr, nullptr}
{
	RegisterEvents();
}

Gpu::~Gpu()
{
	DestroyGL(3000);
	DestroyGL(3001);
	DestroyGL(3002);
	DestroyGL(3003);
	DestroyGL(3004);
	DestroyGL(3005);
	DestroyGL(3006);
	DestroyGL(3007);
	DestroyGL(3008);
	DestroyGL(3009);
}

void Gpu::Tick()
{
	if (_hcount == 308 * 4 - 1) {
		uint8_t *registers = _system.GetMemory().GetRegisters();
		_hcount = 0;
		uint16_t dispstat = IOREG16(registers, DISPSTAT);
		dispstat &= ~2;
		if (_vcount == 227) {
			_vcount = 0;
			dispstat &= ~1;
		}
		else {
			_vcount++;
			if (_vcount == 160) {
				dispstat |= 1;
				_system.GetMemory().DMAStartVBlank();
				if ((dispstat & 0x8) != 0) {
					_system.RequestIRQ(IRQ_VBLANK);
				}
			}
			if (_vcount == dispstat >> 8) {
				dispstat |= 4;
				if ((dispstat & 0x20) != 0) {
					_system.RequestIRQ(IRQ_VMATCH);
				}
			}
			else {
				dispstat &= ~4;
			}
		}
		IOREG16(registers, DISPSTAT) = dispstat;
		IOREG16(registers, VCOUNT) = _vcount;
	}
	else {
		_hcount++;
		if (_hcount == 1006) {
			uint8_t *registers = _system.GetMemory().GetRegisters();
			uint16_t dispstat = IOREG16(registers, DISPSTAT);
			dispstat |= 1;
			_system.GetMemory().DMAStartHBlank();
			if ((dispstat & 0x10) != 0) {
				_system.RequestIRQ(IRQ_HBLANK);
			}
		}
	}
}

void Gpu::HandleEvent(uint32_t address, int size)
{
	uint8_t *registers = _system.GetMemory().GetRegisters();
	if (address <= DISPSTAT + 1 && DISPSTAT < address + size) {
		uint8_t flags = 0;
		if (_vcount >= 160 && _vcount <= 226) flags |= 0x1;
		if (_hcount >= 1006 && _hcount <= 1232) flags |= 0x2;
		if (_vcount == registers[(DISPSTAT&IOREGISTERSMASK) + 1]) flags |= 0x4;
		IOREG16(registers, DISPSTAT) = (IOREG16(registers, DISPSTAT) & ~0x00C7) | flags;
	}
	if (address <= VCOUNT + 1 && VCOUNT < address + size) {
		IOREG16(registers, VCOUNT) = _vcount;
	}
}

bool Gpu::InitGL(int id)
{
	switch (id) {
	case 3000: return InitBGGL(0);
	case 3001: return InitBGGL(1);
	case 3002: return InitBGGL(2);
	case 3003: return InitBGGL(3);	
	case 3004: return InitPalettesGL(true);
	case 3005: return InitPalettesGL(false);
	case 3006: return InitTilesGL(0);
	case 3007: return InitTilesGL(1);
	case 3008: return InitTilesGL(2);
	case 3009: return InitOAMGL();
	}
	return false;
}

void Gpu::DestroyGL(int id)
{
	switch (id) {
	case 3000: 
		DestroyBGGL(0);
		break;
	case 3001:
		DestroyBGGL(1);
		break;
	case 3002:
		DestroyBGGL(2);
		break;
	case 3003:
		DestroyBGGL(3);
		break;
	case 3004: 
		DestroyPalettesGL(true);
		break;
	case 3005: 
		DestroyPalettesGL(false);
		break;
	case 3006:
		DestroyTilesGL(0);
		break;
	case 3007:
		DestroyTilesGL(1);
		break;
	case 3008:
		DestroyTilesGL(2);
		break;
	case 3009:
		DestroyOAMGL();
		break;
	}
}

void Gpu::Reshape(int id, int width, int height, bool keepAspect)
{

}

void Gpu::Draw(int id)
{
	switch (id) {
	case 3000:
		DrawBG(0);
		break;
	case 3001:
		DrawBG(1);
		break;
	case 3002:
		DrawBG(2);
		break;
	case 3003:
		DrawBG(3);
		break;
	case 3004:
		DrawPalettes(true);
		break;
	case 3005:
		DrawPalettes(false);
		break;
	case 3006:
		DrawTiles(0);
		break;
	case 3007:
		DrawTiles(1);
		break;
	case 3008:
		DrawTiles(2);
		break;
	case 3009:
		DrawOAM();
		break;
	}
}

void Gpu::RegisterEvents()
{
	//_system.GetMemory().RegisterEvent(DISPCNT, this);
	_system.GetMemory().RegisterEvent(DISPSTAT, this);
	//_system.GetMemory().RegisterEvent(BG0CNT, this);
	//_system.GetMemory().RegisterEvent(BG2CNT, this);
	//_system.GetMemory().RegisterEvent(BG0HOFS, this);
	//_system.GetMemory().RegisterEvent(BG1HOFS, this);
	//_system.GetMemory().RegisterEvent(BG2HOFS, this);
	//_system.GetMemory().RegisterEvent(BG3HOFS, this);

}

bool Gpu::InitPalettesGL(bool background)
{
	const float vertexData[] = {
		// x     y    u    v
		-1.0,  1.0, 0.0, 0.0,
		 1.0,  1.0, 1.0, 0.0,
		-1.0, -1.0, 0.0, 1.0,
		 1.0, -1.0, 1.0, 1.0
	};
	int index = background ? 0 : 1;
	try {
		// Clear
		GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

		if (_paletteInitialized[index]) {
			DestroyPalettesGL(background);
		}

		// Create Shader program
		_paletteShader.IncRef();
		if (_paletteShader.GetRefCount() == 1) {
			if (!_paletteShader->AddShader(ShaderProgram::Vertex, (char *)resource_palette_vert_glsl,
				resource_palette_vert_glsl_len)) {
				Log(Error, "Could not Compile palette vertex shader\n");
				throw GraphicsException(0, _paletteShader->GetLog());
			}
			if (!_paletteShader->AddShader(ShaderProgram::Fragment, (char *)resource_palette_frag_glsl,
				resource_palette_frag_glsl_len)) {
				Log(Error, "Could not Compile palette fragment shader\n");
				throw GraphicsException(0, _paletteShader->GetLog());
			}
			if (!_paletteShader->Link()) {
				Log(Error, "Could not Link palette shader program\n");
				throw GraphicsException(0, _paletteShader->GetLog());
			}
		}

		// Create Vertex buffer object
		_paletteVertexData.IncRef(BufferObject::Type::Array);
		if (_paletteVertexData.GetRefCount() == 1) {
			_paletteVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
		}

		// Create buffer for palette data
		InitPaletteDataGL();

		// Create Vertex array object
		_paletteVao[index] = new VertexArrayObject;
		_paletteVao[index]->Begin();
		_paletteVao[index]->BindBuffer(0, *_paletteVertexData, 2,
			VertexArrayObject::Float, false, 4 * sizeof(float), 0 * sizeof(float));
		_paletteVao[index]->BindBuffer(1, *_paletteVertexData, 2,
			VertexArrayObject::Float, false, 4 * sizeof(float), 2 * sizeof(float));
		_paletteVao[index]->End();
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while creating palette graphics objects: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		DestroyPalettesGL(background);
		return false;
	}
	_paletteInitialized[index] = true;
	return true;
}

void Gpu::DestroyPalettesGL(bool background)
{
	int index = background ? 0 : 1;
	if (_paletteInitialized[index] == false) return;
	_paletteInitialized[index] = false;
	_paletteShader.DecRef();
	if (_paletteVao[index] != nullptr) {
		delete _paletteVao[index];
		_paletteVao[index] = nullptr;
	}
	_paletteData.DecRef();
	_paletteVertexData.DecRef();
}

void Gpu::DrawPalettes(bool background)
{
	int index = background ? 0 : 1;
	try {
		if (!_paletteInitialized[index]) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw palette without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 512, 256));

		uint8_t *palettes = _system.GetMemory().GetPalettes();
		_paletteData->UpdateData(0, 0, 16, 32, (char *)palettes, Texture::USHORT_1_5_5_5);

		_paletteShader->Begin();
		_paletteVao[index]->Begin();
		_paletteShader->SetUniform("paletteData", 0, *_paletteData);
		_paletteShader->SetUniform("windowSize", 512.0f, 256.0f);
		_paletteShader->SetUniform("objPalette", !background);

		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

		_paletteVao[index]->End();
		_paletteShader->End();
	}
	catch (GraphicsException  &e) {
		Log(Error, "Graphics exception in drawing palettes: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
	}
}

bool Gpu::InitTilesGL(int idx)
{
	const float vertexData[] = {
		// x     y    u    v
		-1.0,  1.0, 0.0, 0.0,
		1.0,  1.0, 1.0, 0.0,
		-1.0, -1.0, 0.0, 1.0,
		1.0, -1.0, 1.0, 1.0
	};
	try {
		if (_tilesInitialized[idx]) {
			DestroyTilesGL(idx);
		}
		// Create Shader program
		_tilesShader.IncRef();
		if (_tilesShader.GetRefCount() == 1) {
			if (!_tilesShader->AddShader(ShaderProgram::Vertex, (char *)resource_tiles_vert_glsl,
				resource_tiles_vert_glsl_len)) {
				Log(Error, "Could not Compile tiles vertex shader\n");
				throw GraphicsException(0, _tilesShader->GetLog());
			}
			if (!_tilesShader->AddShader(ShaderProgram::Fragment, (char *)resource_tiles_frag_glsl,
				resource_tiles_frag_glsl_len)) {
				Log(Error, "Could not Compile tiles fragment shader\n");
				throw GraphicsException(0, _tilesShader->GetLog());
			}
			if (!_tilesShader->Link()) {
				Log(Error, "Could not Link tiles shader program\n");
				throw GraphicsException(0, _tilesShader->GetLog());
			}
		}

		// Create vertex data buffer
		_tilesVertexData.IncRef(BufferObject::Type::Array);
		if (_tilesVertexData.GetRefCount() == 1) {
			_tilesVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
		}

		// create VAO
		_tilesVao[idx] = new VertexArrayObject();
		_tilesVao[idx]->Begin();
		_tilesVao[idx]->BindBuffer(0, *_tilesVertexData, 2, VertexArrayObject::Float,
			false, 4 * sizeof(float), 0 * sizeof(float));
		_tilesVao[idx]->BindBuffer(1, *_tilesVertexData, 2, VertexArrayObject::Float,
			false, 4 * sizeof(float), 2 * sizeof(float));
		_tilesVao[idx]->End();

		// Create tile buffer data
		InitVRAMDataGL();		
		InitPaletteDataGL();
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while creating graphics tiles objects: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		return false;
	}
	_tilesInitialized[idx] = true;
	return true;
}

void Gpu::DestroyTilesGL(int idx)
{
	if (_tilesInitialized[idx] == false) return;
	_tilesInitialized[idx] = false;
	_tilesShader.DecRef();
	_paletteData.DecRef();
	_vramBT.DecRef();
	_tilesVertexData.DecRef();
	if (_tilesVao[idx] != nullptr) {
		delete _tilesVao[idx];
		_tilesVao[idx] = nullptr;
	}
}

void Gpu::DrawTiles(int idx)
{
	const uint32_t tilesAddresses[3] = { 0x00000, 0x08000, 0x10000 };
	uint32_t address = tilesAddresses[idx];
	try {
		if (!_tilesInitialized[idx]) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw tiles without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 512, 512));

		uint8_t *palettes = _system.GetMemory().GetPalettes();
		_paletteData->UpdateData(0, 0, 16, 32, (char *)palettes, Texture::USHORT_1_5_5_5);

		uint8_t *vram = _system.GetMemory().GetVRAM() + address;
		_vramBT->GetBufferObject().BufferSubData(address, 0x8000, vram);

		int paletteId = 0;
		if (idx == 2) paletteId |= 0x10;

		_tilesShader->Begin();
		_tilesVao[idx]->Begin();
		_tilesShader->SetUniform("paletteId", paletteId); // TODO: Detect palette
		_tilesShader->SetUniform("depth8bit", _debugTiles8BitDepth[idx]);
		_tilesShader->SetUniform("paletteData", 1, *_paletteData);
		_tilesShader->SetUniform("vramData", 0, *_vramBT);
		_tilesShader->SetUniform("baseAddress", (int)address);
		_tilesShader->SetUniform("windowSize", 512.0f, 512.0f);
		_tilesShader->SetUniform("grid", _debugTilesGridEnabled[idx]);

		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

		_tilesVao[idx]->End();
		_tilesShader->End();
	}
	catch (GraphicsException  &e) {
		Log(Error, "Graphics exception in drawing tiles: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
	}
}

bool Gpu::InitOAMGL()
{
	const float vertexData[] = {
		// x     y    u    v
		-1.0,  1.0, 0.0, 0.0,
		1.0,  1.0, 1.0, 0.0,
		-1.0, -1.0, 0.0, 1.0,
		1.0, -1.0, 1.0, 1.0
	};
	try {
		if (_oamInitialized) {
			DestroyOAMGL();
		}
		// Create Shader program
		_oamShader.IncRef();
		if (_oamShader.GetRefCount() == 1) {
			if (!_oamShader->AddShader(ShaderProgram::Vertex, (char *)resource_oam_vert_glsl,
					resource_oam_vert_glsl_len)) {
				Log(Error, "Could not Compile oam vertex shader\n");
				throw GraphicsException(0, _oamShader->GetLog());
			}
			if (!_oamShader->AddShader(ShaderProgram::Fragment, (char *)resource_oam_frag_glsl,
					resource_oam_frag_glsl_len)) {
				Log(Error, "Could not Compile oam fragment shader\n");
				throw GraphicsException(0, _oamShader->GetLog());
			}
			if (!_oamShader->Link()) {
				Log(Error, "Could not Link oam shader program\n");
				throw GraphicsException(0, _oamShader->GetLog());
			}
		}

		// Create vertex data buffer
		_oamVertexData.IncRef(BufferObject::Type::Array);
		if (_oamVertexData.GetRefCount() == 1) {
			_oamVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
		}

		// create VAO
		_oamVao = new VertexArrayObject();
		_oamVao->Begin();
		_oamVao->BindBuffer(0, *_oamVertexData, 2, VertexArrayObject::Float,
			false, 4 * sizeof(float), 0 * sizeof(float));
		_oamVao->BindBuffer(1, *_oamVertexData, 2, VertexArrayObject::Float,
			false, 4 * sizeof(float), 2 * sizeof(float));
		_oamVao->End();

		// Create tile buffer data
		InitVRAMDataGL();
		InitPaletteDataGL();
		InitOAMAttributeDataGL();
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while creating graphics oam objects: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		return false;
	}
	_oamInitialized = true;
	return true;
}

void Gpu::DestroyOAMGL()
{
	if (_oamInitialized == false) return;
	_oamInitialized = false;
	_oamShader.DecRef();
	_paletteData.DecRef();
	_vramBT.DecRef();
	_oamVertexData.DecRef();
	_oamBT.DecRef();
	if (_oamVao != nullptr) {
		delete _oamVao;
		_oamVao = nullptr;
	}
}

void Gpu::DrawOAM()
{
	try {
		if (!_oamInitialized) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw oam without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 768, 384));

		uint8_t *palettes = _system.GetMemory().GetPalettes();
		_paletteData->UpdateData(0, 0, 16, 32, (char *)palettes, Texture::USHORT_1_5_5_5);

		uint8_t *vram = _system.GetMemory().GetVRAM() + 0x10000;
		_vramBT->GetBufferObject().BufferSubData(0x10000, 0x8000, vram);
		
		uint8_t *oam = _system.GetMemory().GetOAM();
		_oamBT->GetBufferObject().BufferSubData(0, 0x400, oam);

		uint8_t *registers = _system.GetMemory().GetRegisters();

		_oamShader->Begin();
		_oamVao->Begin();
		_oamShader->SetUniform("oamData", 2, *_oamBT);
		_oamShader->SetUniform("paletteData", 1, *_paletteData);
		_oamShader->SetUniform("vramData", 0, *_vramBT);
		_oamShader->SetUniform("DISPCNT", IOREG32(registers, DISPCNT));
		_oamShader->SetUniform("windowSize", 768.0f, 384.0f);

		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

		_oamVao->End();
		_oamShader->End();
	}
	catch (GraphicsException  &e) {
		Log(Error, "Graphics exception in drawing oam: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
	}
}

bool Gpu::InitBGGL(int idx)
{
	const float vertexData[] = {
		// x     y    u    v
		-1.0,  1.0, 0.0, 0.0,
		1.0,  1.0, 1.0, 0.0,
		-1.0, -1.0, 0.0, 1.0,
		1.0, -1.0, 1.0, 1.0
	};
	try {
		if (_bgInitialized[idx]) {
			DestroyOAMGL();
		}
		// Create Shader program
		_bgShader.IncRef();
		if (_bgShader.GetRefCount() == 1) {
			if (!_bgShader->AddShader(ShaderProgram::Vertex, (char *)resource_bg_vert_glsl,
					resource_bg_vert_glsl_len)) {
				Log(Error, "Could not Compile bg vertex shader\n");
				throw GraphicsException(0, _bgShader->GetLog());
			}
			if (!_bgShader->AddShader(ShaderProgram::Fragment, (char *)resource_bg_frag_glsl,
					resource_bg_frag_glsl_len)) {
				Log(Error, "Could not Compile bg fragment shader\n");
				throw GraphicsException(0, _bgShader->GetLog());
			}
			if (!_bgShader->Link()) {
				Log(Error, "Could not Link bg shader program\n");
				throw GraphicsException(0, _bgShader->GetLog());
			}
		}

		// Create vertex data buffer
		_bgVertexData.IncRef(BufferObject::Type::Array);
		if (_bgVertexData.GetRefCount() == 1) {
			_bgVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
		}

		// create VAO
		_bgVao[idx] = new VertexArrayObject();
		_bgVao[idx]->Begin();
		_bgVao[idx]->BindBuffer(0, *_bgVertexData, 2, VertexArrayObject::Float,
			false, 4 * sizeof(float), 0 * sizeof(float));
		_bgVao[idx]->BindBuffer(1, *_bgVertexData, 2, VertexArrayObject::Float,
			false, 4 * sizeof(float), 2 * sizeof(float));
		_bgVao[idx]->End();

		// Create tile buffer data
		InitVRAMDataGL();
		InitPaletteDataGL();
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while creating graphics bg objects: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		return false;
	}
	_bgInitialized[idx] = true;
	return true;
}

void Gpu::DestroyBGGL(int idx)
{
	if (_bgInitialized[idx] == false) return;
	_bgInitialized[idx] = false;
	_bgShader.DecRef();
	_paletteData.DecRef();
	_vramBT.DecRef();
	_bgVertexData.DecRef();
	if (_bgVao[idx] != nullptr) {
		delete _bgVao[idx];
		_bgVao[idx] = nullptr;
	}
}

void Gpu::DrawBG(int idx)
{
	try {
		if (!_bgInitialized[idx]) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw bg without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 512, 512));

		uint8_t *palettes = _system.GetMemory().GetPalettes();
		_paletteData->UpdateData(0, 0, 16, 32, (char *)palettes, Texture::USHORT_1_5_5_5);

		uint8_t *vram = _system.GetMemory().GetVRAM();
		_vramBT->GetBufferObject().BufferSubData(0, 0x14000, vram);

		uint8_t *registers = _system.GetMemory().GetRegisters();

		_bgShader->Begin();
		_bgVao[idx]->Begin();
		_bgShader->SetUniform("paletteData", 1, *_paletteData);
		_bgShader->SetUniform("vramData", 0, *_vramBT);
		_bgShader->SetUniform("DISPCNT", IOREG32(registers, DISPCNT));
		_bgShader->SetUniform("BGCNT", (uint32_t)IOREG16(registers, BG0CNT + 2 * idx));
		_bgShader->SetUniform("BGHOFS", (uint32_t)IOREG16(registers, BG0HOFS + 4 * idx));
		_bgShader->SetUniform("BGVOFS", (uint32_t)IOREG16(registers, BG0VOFS + 4 * idx));
		_bgShader->SetUniform("BGX", ((uint32_t)IOREG16(registers, BG2X_H + 8 * (idx - 2)) << 16) | IOREG16(registers, BG2X_L + 8 * (idx - 2)));
		_bgShader->SetUniform("BGY", ((uint32_t)IOREG16(registers, BG2Y_H + 8 * (idx - 2)) << 16) | IOREG16(registers, BG2Y_L + 8 * (idx - 2)));
		_bgShader->SetUniform("backgroundId", idx);
		_bgShader->SetUniform("windowSize", 512.0f, 512.0f);
		_bgShader->SetUniform("grid", true);

		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

		_bgVao[idx]->End();
		_bgShader->End();
	}
	catch (GraphicsException  &e) {
		Log(Error, "Graphics exception in drawing bg: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
	}
}

void Gpu::InitPaletteDataGL()
{
	_paletteData.IncRef(16, 32, nullptr, Texture::RGBA);
	if (_paletteData.GetRefCount() == 1) {
		_paletteData->Bind();
		_paletteData->SetFilter(Texture::Nearest, Texture::Nearest);
		_paletteData->SetWrap(Texture::Repeat, Texture::Repeat);
		_paletteData->UnBind();
	}
}

void Gpu::InitVRAMDataGL()
{
	_vramBT.IncRef(0x18000, nullptr, BufferTexture::Format::R8UI, BufferObject::Usage::StreamDraw);
}

void Gpu::InitOAMAttributeDataGL()
{
	_oamBT.IncRef(0x400, nullptr, BufferTexture::Format::R16UI, BufferObject::Usage::StreamDraw);
}

