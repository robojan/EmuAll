
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
	_paletteShader{nullptr, nullptr}, _paletteData{nullptr, nullptr}, 
	_paletteVertexData{nullptr, nullptr}, _paletteVao{nullptr, nullptr},
	_tilesShader{nullptr, nullptr, nullptr}, _tilesPaletteData{nullptr, nullptr, nullptr},
	_tilesData{nullptr, nullptr, nullptr}, _tilesVertexData{nullptr, nullptr, nullptr},
	_tilesVao{nullptr, nullptr, nullptr}
{
	uint8_t *registers = _system.GetMemory().GetRegisters();

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
	case 3004: return InitPalettesGL(true);
	case 3005: return InitPalettesGL(false);
	case 3006: return InitTilesGL(0);
	case 3007: return InitTilesGL(1);
	case 3008: return InitTilesGL(2);
	}
	return false;
}

void Gpu::DestroyGL(int id)
{
	switch (id) {
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
	}
}

void Gpu::Reshape(int id, int width, int height, bool keepAspect)
{

}

void Gpu::Draw(int id)
{
	switch (id) {
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
		DestroyPalettesGL(background);
		// Clear
		GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

		// Create Vertex buffer object
		_paletteVertexData[index] = new BufferObject(BufferObject::Type::Array);
		_paletteVertexData[index]->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);

		// Create buffer for palette data
		_paletteData[index] = new Texture(16, 16, nullptr, Texture::RGBA);
		_paletteData[index]->Bind();
		_paletteData[index]->SetFilter(Texture::Nearest, Texture::Nearest);
		_paletteData[index]->SetWrap(Texture::Repeat, Texture::Repeat);
		_paletteData[index]->UnBind();

		// Create Vertex array object
		_paletteVao[index] = new VertexArrayObject;
		_paletteVao[index]->Begin();
		_paletteVao[index]->BindBuffer(0, *_paletteVertexData[index], 2,
			VertexArrayObject::Float, false, 4 * sizeof(float), 0 * sizeof(float));
		_paletteVao[index]->BindBuffer(1, *_paletteVertexData[index], 2,
			VertexArrayObject::Float, false, 4 * sizeof(float), 2 * sizeof(float));
		_paletteVao[index]->End();

		// Create Shader program
		_paletteShader[index] = new ShaderProgram;
		if (!_paletteShader[index]->AddShader(ShaderProgram::Vertex, (char *)resource_palette_vert_glsl,
			resource_palette_vert_glsl_len)) {
			Log(Error, "Could not Compile palette vertex shader\n");
			throw GraphicsException(0, _paletteShader[index]->GetLog());
		}
		if (!_paletteShader[index]->AddShader(ShaderProgram::Fragment, (char *)resource_palette_frag_glsl,
			resource_palette_frag_glsl_len)) {
			Log(Error, "Could not Compile palette fragment shader\n");
			throw GraphicsException(0, _paletteShader[index]->GetLog());
		}
		if (!_paletteShader[index]->Link()) {
			Log(Error, "Could not Link palette shader program\n");
			throw GraphicsException(0, _paletteShader[index]->GetLog());
		}
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while creating palette graphics objects: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		DestroyPalettesGL(background);
		return false;
	}
	return true;
}

void Gpu::DestroyPalettesGL(bool background)
{
	int index = background ? 0 : 1;
	if (_paletteShader[index] != nullptr) {
		delete _paletteShader[index];
		_paletteShader[index] = nullptr;
	}
	if (_paletteVao[index] != nullptr) {
		delete _paletteVao[index];
		_paletteVao[index] = nullptr;
	}
	if (_paletteData[index] != nullptr) {
		delete _paletteData[index];
		_paletteData[index] = nullptr;
	}
	if (_paletteVertexData[index] != nullptr) {
		delete _paletteVertexData[index];
		_paletteVertexData[index] = nullptr;
	}
}

void Gpu::DrawPalettes(bool background)
{
	int index = background ? 0 : 1;
	try {
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 512, 256));

		uint8_t *palettes = _system.GetMemory().GetPalettes();
		if (!background) palettes += 0x200;
		_paletteData[index]->UpdateData(0, 0, 16, 16, (char *)palettes, Texture::USHORT_1_5_5_5);

		_paletteShader[index]->Begin();
		_paletteVao[index]->Begin();
		_paletteShader[index]->SetUniform("paletteData", 0, *_paletteData[index]);
		_paletteShader[index]->SetUniform("windowSize", 512.0f, 256.0f);

		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

		_paletteVao[index]->End();
		_paletteShader[index]->End();
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
		DestroyTilesGL(idx);
		// Create Shader program
		_tilesShader[idx] = new ShaderProgram;
		if (!_tilesShader[idx]->AddShader(ShaderProgram::Vertex, (char *)resource_tiles_vert_glsl,
			resource_tiles_vert_glsl_len)) {
			Log(Error, "Could not Compile tiles vertex shader\n");
			throw GraphicsException(0, _tilesShader[idx]->GetLog());
		}
		if (!_tilesShader[idx]->AddShader(ShaderProgram::Fragment, (char *)resource_tiles_frag_glsl,
			resource_tiles_frag_glsl_len)) {
			Log(Error, "Could not Compile tiles fragment shader\n");
			throw GraphicsException(0, _tilesShader[idx]->GetLog());
		}
		if (!_tilesShader[idx]->Link()) {
			Log(Error, "Could not Link tiles shader program\n");
			throw GraphicsException(0, _tilesShader[idx]->GetLog());
		}
		// Create palette texture
		_tilesPaletteData[idx] = new Texture(16, 32, nullptr, Texture::RGBA);
		_tilesPaletteData[idx]->Bind();
		_tilesPaletteData[idx]->SetFilter(Texture::Nearest, Texture::Nearest);
		_tilesPaletteData[idx]->SetWrap(Texture::Repeat, Texture::Repeat);
		_tilesPaletteData[idx]->UnBind();

		// Create vertex data buffer
		_tilesVertexData[idx] = new BufferObject(BufferObject::Type::Array);
		_tilesVertexData[idx]->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);

		// Create tile buffer data
		_tilesData[idx] = new BufferTexture(0x8000, nullptr, BufferTexture::Format::R8UI, 
			BufferObject::Usage::StreamDraw);
		
		// create VAO
		_tilesVao[idx] = new VertexArrayObject();
		_tilesVao[idx]->Begin();
		_tilesVao[idx]->BindBuffer(0, *_tilesVertexData[idx], 2, VertexArrayObject::Float, 
			false, 4 * sizeof(float), 0 * sizeof(float));
		_tilesVao[idx]->BindBuffer(1, *_tilesVertexData[idx], 2, VertexArrayObject::Float, 
			false, 4 * sizeof(float), 2 * sizeof(float));
		_tilesVao[idx]->End();
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while creating graphics tiles objects: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		DestroyTilesGL(idx);
		return false;
	}
	return true;
}

void Gpu::DestroyTilesGL(int idx)
{
	if (_tilesShader[idx] != nullptr) {
		delete _tilesShader[idx];
		_tilesShader[idx] = nullptr;
	}
	if (_tilesPaletteData[idx] != nullptr) {
		delete _tilesPaletteData[idx];
		_tilesPaletteData[idx] = nullptr;
	}
	if (_tilesData[idx] != nullptr) {
		delete _tilesData[idx];
		_tilesData[idx] = nullptr;
	}
	if (_tilesVertexData[idx] != nullptr) {
		delete _tilesVertexData[idx];
		_tilesVertexData[idx] = nullptr;
	}
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
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 512, 512));

		uint8_t *palettes = _system.GetMemory().GetPalettes();
		_tilesPaletteData[idx]->UpdateData(0, 0, 16, 32, (char *)palettes, Texture::USHORT_1_5_5_5);

		uint8_t *vram = _system.GetMemory().GetVRAM() + address;
		_tilesData[idx]->GetBufferObject().BufferSubData(0, 0x8000, vram);

		int paletteId = 0;
		if (idx == 2) paletteId |= 0x10;

		_tilesShader[idx]->Begin();
		_tilesVao[idx]->Begin();
		_tilesShader[idx]->SetUniform("paletteId", paletteId); // TODO: Detect palette
		_tilesShader[idx]->SetUniform("depth8bit", _debugTiles8BitDepth[idx]);
		_tilesShader[idx]->SetUniform("paletteData", 1, *_tilesPaletteData[idx]);
		_tilesShader[idx]->SetUniform("tileData", 0, *_tilesData[idx]);
		_tilesShader[idx]->SetUniform("windowSize", 512.0f, 512.0f);
		_tilesShader[idx]->SetUniform("grid", _debugTilesGridEnabled[idx]);

		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

		_tilesVao[idx]->End();
		_tilesShader[idx]->End();
	}
	catch (GraphicsException  &e) {
		Log(Error, "Graphics exception in drawing palettes: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
	}
}

