
#include <GL/glew.h>
#include <GL/wglew.h>

#include <GBAemu/defines.h>
#include <GBAemu/gpu/gpu.h>
#include <GBAemu/gba.h>
#include <GBAemu/util/preprocessor.h>
#include <GBAemu/util/log.h>
#include <GBAemu/util/FixedPointGBA.h>
#include <GBAemu/emuallApi.h>

#include <emuall/graphics/graphicsException.h>

#include "../resources/resources.h"


Gpu::Gpu(Gba &system) :
	_debugTiles8BitDepth{ false, false, false },
	_debugTilesGridEnabled{ true, true, true },
	_system(system), _vcount(0), _hcount(0),
	_mainInitialized(false), _mainMixVao{ nullptr }, _mainBGVao{ nullptr }, _mainObjVao{ nullptr },
	_paletteInitialized{false, false}, _paletteVao{nullptr, nullptr},
	_tilesInitialized{false, false, false}, _tilesVao{nullptr, nullptr, nullptr},
	_oamInitialized(false), _oamVao(nullptr),
	_bgInitialized{ false, false, false, false }, _bgVao{ nullptr, nullptr, nullptr, nullptr }
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
	DestroyGL(0);
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
				VBlank();
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
			HBlank();
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
	case 0: return InitMainGL();
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
	case 0:
		DestroyMainGL();
		break;
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
	case 0:
		DrawMain();
		break;
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

void Gpu::VBlank()
{
	// Upload registers to gpu
	try {
		if (_mainRegistersBO.GetRefCount() > 0) {
			_mainRegistersBO->BufferSubData(0, sizeof(_mainDrawingRegisters), _mainDrawingRegisters);
		}
		DrawFrame(0);
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while updating graphics memory in vblank: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
	}
}

void Gpu::HBlank()
{
	if (_vcount < SCREEN_HEIGHT)
	{
		uint8_t *registers = _system.GetMemory().GetRegisters();
		// Write registers to buffer
		_mainDrawingRegisters[_vcount].dispcnt = IOREG16(registers, DISPCNT);
		_mainDrawingRegisters[_vcount].bgCnt[0] = IOREG16(registers, BG0CNT);
		_mainDrawingRegisters[_vcount].bgCnt[1] = IOREG16(registers, BG1CNT);
		_mainDrawingRegisters[_vcount].bgCnt[2] = IOREG16(registers, BG2CNT);
		_mainDrawingRegisters[_vcount].bgCnt[3] = IOREG16(registers, BG3CNT);
		_mainDrawingRegisters[_vcount].bgOfs[0].hOfs = IOREG16(registers, BG0HOFS) & 0x1FF;
		_mainDrawingRegisters[_vcount].bgOfs[0].vOfs = IOREG16(registers, BG0VOFS) & 0x1FF;
		_mainDrawingRegisters[_vcount].bgOfs[1].hOfs = IOREG16(registers, BG1HOFS) & 0x1FF;
		_mainDrawingRegisters[_vcount].bgOfs[1].vOfs = IOREG16(registers, BG1VOFS) & 0x1FF;
		_mainDrawingRegisters[_vcount].bgOfs[2].hOfs = IOREG16(registers, BG2HOFS) & 0x1FF;
		_mainDrawingRegisters[_vcount].bgOfs[2].vOfs = IOREG16(registers, BG2VOFS) & 0x1FF;
		_mainDrawingRegisters[_vcount].bgOfs[3].hOfs = IOREG16(registers, BG3HOFS) & 0x1FF;
		_mainDrawingRegisters[_vcount].bgOfs[3].vOfs = IOREG16(registers, BG3VOFS) & 0x1FF;
		_mainDrawingRegisters[_vcount].bgRefPoint[0].x = GetFixedPoint28(IOREG32(registers, BG2X_L));
		_mainDrawingRegisters[_vcount].bgRefPoint[0].y = GetFixedPoint28(IOREG32(registers, BG2Y_L));
		_mainDrawingRegisters[_vcount].bgRefPoint[1].x = GetFixedPoint28(IOREG32(registers, BG3X_L));
		_mainDrawingRegisters[_vcount].bgRefPoint[1].y = GetFixedPoint28(IOREG32(registers, BG3Y_L));
		_mainDrawingRegisters[_vcount].bgAffineMatrix[0].a = GetFixedPoint16(IOREG16(registers, BG2PA));
		_mainDrawingRegisters[_vcount].bgAffineMatrix[0].b = GetFixedPoint16(IOREG16(registers, BG2PB));
		_mainDrawingRegisters[_vcount].bgAffineMatrix[0].c = GetFixedPoint16(IOREG16(registers, BG2PC));
		_mainDrawingRegisters[_vcount].bgAffineMatrix[0].d = GetFixedPoint16(IOREG16(registers, BG2PD));
		_mainDrawingRegisters[_vcount].bgAffineMatrix[1].a = GetFixedPoint16(IOREG16(registers, BG3PA));
		_mainDrawingRegisters[_vcount].bgAffineMatrix[1].b = GetFixedPoint16(IOREG16(registers, BG3PB));
		_mainDrawingRegisters[_vcount].bgAffineMatrix[1].c = GetFixedPoint16(IOREG16(registers, BG3PC));
		_mainDrawingRegisters[_vcount].bgAffineMatrix[1].d = GetFixedPoint16(IOREG16(registers, BG3PD));
		_mainDrawingRegisters[_vcount].windim[0].x1 = (IOREG16(registers, WIN0H) >> 8) & 0xFF;
		_mainDrawingRegisters[_vcount].windim[0].x2 = (IOREG16(registers, WIN0H) >> 0) & 0xFF;
		_mainDrawingRegisters[_vcount].windim[0].y1 = (IOREG16(registers, WIN0V) >> 8) & 0xFF;
		_mainDrawingRegisters[_vcount].windim[0].y2 = (IOREG16(registers, WIN0V) >> 0) & 0xFF;
		_mainDrawingRegisters[_vcount].windim[1].x1 = (IOREG16(registers, WIN1H) >> 8) & 0xFF;
		_mainDrawingRegisters[_vcount].windim[1].x2 = (IOREG16(registers, WIN1H) >> 0) & 0xFF;
		_mainDrawingRegisters[_vcount].windim[1].y1 = (IOREG16(registers, WIN1V) >> 8) & 0xFF;
		_mainDrawingRegisters[_vcount].windim[1].y2 = (IOREG16(registers, WIN1V) >> 0) & 0xFF;
		_mainDrawingRegisters[_vcount].winCnt.winin = IOREG16(registers, WININ);
		_mainDrawingRegisters[_vcount].winCnt.winout = IOREG16(registers, WINOUT);
		_mainDrawingRegisters[_vcount].effects.bldcnt = IOREG16(registers, BLDCNT);
		_mainDrawingRegisters[_vcount].effects.bldalpha = IOREG16(registers, BLDALPHA);
		_mainDrawingRegisters[_vcount].effects.bldy = IOREG16(registers, BLDY);
		// Upload VRAM memory to gpu
		try {
			if (_mainVramBT.GetRefCount() > 0) {
				uint8_t *vram = _system.GetMemory().GetVRAM();
				_mainVramBT->GetBufferObject().BufferSubData(_vcount * VRAMSIZE_USED, VRAMSIZE_USED, vram);
			}
			if (_mainPaletteData.GetRefCount() > 0) {
				uint8_t *palettes = _system.GetMemory().GetPalettes();
				_mainPaletteData->UpdateData(0, 0, _vcount, 16, 32, 1, (char *)palettes, Texture3D::USHORT_1_5_5_5);
			}
			if (_mainOamBT.GetRefCount() > 0) {
				uint8_t *oam = _system.GetMemory().GetOAM();
				_mainOamBT->GetBufferObject().BufferSubData(_vcount * ORAMSIZE, ORAMSIZE, oam);
			}
		}
		catch (GraphicsException &e) {
			Log(Error, "Error while updating graphics memory in hblank: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		}
	}
}

int Gpu::GetModeBuffer(int lineNr)
{
	if (lineNr >= SCREEN_HEIGHT) lineNr = 0;
	return _mainDrawingRegisters[lineNr].dispcnt & 7;
}

void Gpu::GetBGSizeBuffer(int lineNr, int background, int &width, int &height)
{
	int mode = GetModeBuffer(lineNr);
	if (mode == 1) {
		mode = background == 2 ? 2 : 0;
	}
	switch (mode) {
	case 0:
		switch ((_mainDrawingRegisters[lineNr].bgCnt[background] >> 14) & 0x3) {
		case 0:
			width = 256;
			height = 256;
			break;
		case 1: 
			width = 512;
			height = 256;
			break;
		case 2:
			width = 256;
			height = 512;
			break;
		case 3: 
			width = 512;
			height = 512;
			break;
		}
		break;
	case 2:
		switch ((_mainDrawingRegisters[lineNr].bgCnt[background] >> 14) & 0x3) {
		case 0:
			width = 128;
			height = 128;
			break;
		case 1:
			width = 256;
			height = 256;
			break;
		case 2:
			width = 512;
			height = 512;
			break;
		case 3:
			width = 1024;
			height = 1024;
			break;
		}
		break;
	case 3:
		width = 240;
		height = 160;
		break;
	case 4:
		width = 240;
		height = 160;
		break;
	case 5:
		width = 160;
		height = 128;
		break;
	}
}

void Gpu::GetBGOffsetBuffer(int lineNr, int background, float &x, float &y)
{
	int mode = GetModeBuffer(lineNr);
	if (mode == 1) {
		mode = background == 2 ? 2 : 0;
	}
	switch (mode) {
	case 0:
		x = float(_mainDrawingRegisters[lineNr].bgOfs[background].hOfs);
		y = float(_mainDrawingRegisters[lineNr].bgOfs[background].vOfs);
		return;
	case 2:
		if (background == 2 || background == 3) {
			x = _mainDrawingRegisters[lineNr].bgRefPoint[background - 2].x;
			y = _mainDrawingRegisters[lineNr].bgRefPoint[background - 2].y;
			return;
		}
		break;
	}
	x = 0;
	y = 0;
}

void Gpu::SetupShaderInterface(ShaderProgram &program)
{
	program.SetUniform("screenSize", float(SCREEN_WIDTH), float(SCREEN_HEIGHT));
	program.SetUniform("DrawingRegisters", 0, *_mainRegistersBO);
	program.SetUniform("vramData", 0, *_mainVramBT);
	program.SetUniform("paletteData", 1, *_mainPaletteData);
	program.SetUniform("oamData", 2, *_mainOamBT);
}

bool Gpu::InitMainGL()
{
	try {
		if (_mainInitialized) {
			DestroyMainGL();
		}
		InitMainMixGL();
		InitMainBGGL();
		InitMainObjGL();
		
		InitVRAMDataGL();
		InitOAMDataGL();
		InitRegistersDataGL();
		InitPaletteDataGL();

		// Init frame buffers, 4 bg + 1 obj
		for (int i = 0; i < 5; i++) {
			_mainFrameBuffer[i].IncRef(SCREEN_WIDTH, SCREEN_HEIGHT);
			if (_mainFrameBuffer[i].GetRefCount() == 1) {
				_mainFrameBuffer[i]->AttachColorBuffer(0);
				_mainFrameBuffer[i]->AttachDepthTexture();
			}
		}
		_mainFrameBuffer[5].IncRef(SCREEN_WIDTH, SCREEN_HEIGHT);
		if (_mainFrameBuffer[5].GetRefCount() == 1) {
			_mainFrameBuffer[5]->AttachColorBuffer(0);
		}

		GL_CHECKED(glClearDepth(0));
	}
	catch (ShaderCompileException &e) {
		Log(Error, "Error while compiling shaders:\n %s", e.GetMsg());
		return false;
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while creating graphics main objects: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		return false;
	}
	_mainInitialized = true;
	return true;
}

void Gpu::InitMainBGGL()
{
	int vertexData[SCREEN_HEIGHT+1];
	for (int i = 0; i < SCREEN_HEIGHT+1; i++) {
		vertexData[i] = i;
	}
	ShaderSource vertexSource(resource_mainInterface_glsl, resource_mainInterface_glsl_len);
	ShaderSource geometrySource(vertexSource);
	ShaderSource fragmentSource(vertexSource);
	vertexSource.AddSource(resource_mainBG_vert_glsl, resource_mainBG_vert_glsl_len);
	geometrySource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
	geometrySource.AddSource(resource_mainBG_geo_glsl, resource_mainBG_geo_glsl_len);
	fragmentSource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
	fragmentSource.AddSource(resource_mainBG_frag_glsl, resource_mainBG_frag_glsl_len);

	_mainBGShader.IncRef("Main background");
	if (_mainBGShader.GetRefCount() == 1) {
		_mainBGShader->AddShader(ShaderProgram::Vertex, vertexSource);
		_mainBGShader->AddShader(ShaderProgram::Geometry, geometrySource);
		_mainBGShader->AddShader(ShaderProgram::Fragment, fragmentSource);
		_mainBGShader->Link();
	}

	_mainBGVertexData.IncRef(BufferObject::Type::Array);
	if (_mainBGVertexData.GetRefCount() == 1) {
		_mainBGVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
	}

	_mainBGVao = new VertexArrayObject();
	_mainBGVao->Begin();
	_mainBGVao->BindBuffer(0, *_mainBGVertexData, 2, VertexArrayObject::Int, false, 1 * sizeof(int), 0);
	_mainBGVao->End();

	GL_CHECKED(glClearDepth(0));
}

void Gpu::InitMainObjGL()
{
	int vertexData[SCREEN_HEIGHT + 1];
	for (int i = 0; i < SCREEN_HEIGHT + 1; i++) {
		vertexData[i] = i;
	}

	ShaderSource vertexSource(resource_mainInterface_glsl, resource_mainInterface_glsl_len);
	ShaderSource geometrySource(vertexSource);
	ShaderSource fragmentSource(vertexSource);
	vertexSource.AddSource(resource_mainObj_vert_glsl, resource_mainObj_vert_glsl_len);
	geometrySource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
	geometrySource.AddSource(resource_mainObj_geo_glsl, resource_mainObj_geo_glsl_len);
	fragmentSource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
	fragmentSource.AddSource(resource_mainObj_frag_glsl, resource_mainObj_frag_glsl_len);

	_mainObjShader.IncRef("Main Object");
	if (_mainObjShader.GetRefCount() == 1) {
		_mainObjShader->AddShader(ShaderProgram::Vertex, vertexSource);
		_mainObjShader->AddShader(ShaderProgram::Geometry, geometrySource);
		_mainObjShader->AddShader(ShaderProgram::Fragment, fragmentSource);
		_mainObjShader->Link();
	}

	_mainObjVertexData.IncRef(BufferObject::Type::Array);
	if (_mainObjVertexData.GetRefCount() == 1) {
		_mainObjVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
	}

	_mainObjVao = new VertexArrayObject();
	_mainObjVao->Begin();
	_mainObjVao->BindBuffer(0, *_mainBGVertexData, 2, VertexArrayObject::Int, false, 1 * sizeof(int), 0);
	_mainObjVao->End();

	GL_CHECKED(glClearDepth(0));
}

void Gpu::InitMainMixGL()
{
	const float vertexData[] = {
		// x,   y
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0
	};

	ShaderSource vertexSource(resource_mainInterface_glsl, resource_mainInterface_glsl_len);
	ShaderSource fragmentSource(vertexSource);
	vertexSource.AddSource(resource_mainMix_vert_glsl, resource_mainMix_vert_glsl_len);
	fragmentSource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
	fragmentSource.AddSource(resource_mainMix_frag_glsl, resource_mainMix_frag_glsl_len);
	
	_mainMixShader.IncRef("Main Mix");
	if (_mainMixShader.GetRefCount() == 1) {
		_mainMixShader->AddShader(ShaderProgram::Vertex, vertexSource);
		_mainMixShader->AddShader(ShaderProgram::Fragment, fragmentSource);
		_mainMixShader->Link();
	}

	_mainMixVertexData.IncRef(BufferObject::Type::Array);
	if (_mainMixVertexData.GetRefCount() == 1) {
		_mainMixVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
	}

	_mainMixVao = new VertexArrayObject();
	_mainMixVao->Begin();
	_mainMixVao->BindBuffer(0, *_mainMixVertexData, 2, VertexArrayObject::Float, false, 2 * sizeof(float), 0);
	_mainMixVao->End();

	GL_CHECKED(glClearDepth(0));
}

void Gpu::DestroyMainGL()
{
	if (_mainInitialized == false) return;
	_mainInitialized = false;
	DestroyMainMixGL();
	DestroyMainBGGL();
	DestroyMainObjGL();
	_mainVramBT.DecRef();
	_mainRegistersBO.DecRef();
	_mainPaletteData.DecRef();
	_mainOamBT.DecRef();
	for (int i = 0; i < 5; i++) {
		_mainFrameBuffer[i].DecRef();
	}
}

void Gpu::DestroyMainBGGL()
{
	_mainBGShader.DecRef();
	_mainBGVertexData.DecRef();
	if (_mainBGVao != nullptr) {
		delete _mainBGVao;
		_mainBGVao = nullptr;
	}
}

void Gpu::DestroyMainObjGL()
{
	_mainObjShader.DecRef();
	_mainObjVertexData.DecRef();
	if (_mainObjVao != nullptr) {
		delete _mainObjVao;
		_mainObjVao = nullptr;
	}
}

void Gpu::DestroyMainMixGL()
{
	_mainMixShader.DecRef();
	_mainMixVertexData.DecRef();
	if (_mainMixVao != nullptr) {
		delete _mainMixVao;
		_mainMixVao = nullptr;
	}
}

void Gpu::DrawMain()
{
	try {
		if (!_mainInitialized) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw main without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GL_CHECKED(glEnable(GL_DEPTH_TEST));
		GL_CHECKED(glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
		GL_CHECKED(glDepthFunc(GL_GEQUAL));

		// Get drawing framebuffer
		int mainFbo;
		GL_CHECKED(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mainFbo));

		for (int i = 0; i < 4; i++) {
			// Draw backgrounds
			_mainFrameBuffer[i]->Begin();
			DrawMainBGGL(i);
		}

		// Draw objects layer
		DrawMainObjGL();

		// restore framebuffer
		GL_CHECKED(glBindFramebuffer(GL_FRAMEBUFFER, mainFbo));
		DrawMainMix();
	}
	catch (GraphicsException &e) {
		Log(Error, "Graphics exception in drawing main: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
	}
}

void Gpu::DrawMainBGGL(int background)
{
	GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GL_CHECKED(glEnable(GL_DEPTH_TEST));
	GL_CHECKED(glDepthFunc(GL_GEQUAL));
	GL_CHECKED(glClearColor(0.0, 0.0, 0.0, 0.0));
	_mainBGShader->Begin();
	_mainBGVao->Begin();

	SetupShaderInterface(*_mainBGShader);
	_mainBGShader->SetUniform("background", background);

	GL_CHECKED(glDrawArrays(GL_POINTS, 0, SCREEN_HEIGHT+1));

	_mainBGVao->End();
	_mainBGShader->End();
}

void Gpu::DrawMainObjGL()
{
	GL_CHECKED(glEnable(GL_DEPTH_TEST));
	GL_CHECKED(glDepthFunc(GL_GEQUAL));
	GL_CHECKED(glClearColor(0.0, 0.0, 0.0, 0.0));

	_mainObjShader->Begin();
	_mainObjVao->Begin();

	SetupShaderInterface(*_mainObjShader);

	// Draw objects
	_mainFrameBuffer[4]->Begin();
	GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	_mainObjShader->SetUniform("maskDrawing", 0);
	GL_CHECKED(glDrawArrays(GL_POINTS, 0, SCREEN_HEIGHT + 1));

	// Draw object window mask
	_mainFrameBuffer[5]->Begin();
	GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
	_mainObjShader->SetUniform("maskDrawing", 1);
	GL_CHECKED(glDrawArrays(GL_POINTS, 0, SCREEN_HEIGHT + 1));
	
	_mainObjVao->End();
	_mainObjShader->End();
}

void Gpu::DrawMainMix()
{
	GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GL_CHECKED(glEnable(GL_DEPTH_TEST));
	GL_CHECKED(glDepthFunc(GL_GEQUAL));
	_mainMixShader->Begin();
	_mainMixVao->Begin();

	SetupShaderInterface(*_mainMixShader);
	_mainMixShader->SetUniform("background[0]", 3, _mainFrameBuffer[0]->GetColorBuffer(0));
	_mainMixShader->SetUniform("background[1]", 4, _mainFrameBuffer[1]->GetColorBuffer(0));
	_mainMixShader->SetUniform("background[2]", 5, _mainFrameBuffer[2]->GetColorBuffer(0));
	_mainMixShader->SetUniform("background[3]", 6, _mainFrameBuffer[3]->GetColorBuffer(0));
	_mainMixShader->SetUniform("backgroundDepth[0]", 7, _mainFrameBuffer[0]->GetDepthTexture());
	_mainMixShader->SetUniform("backgroundDepth[1]", 8, _mainFrameBuffer[1]->GetDepthTexture());
	_mainMixShader->SetUniform("backgroundDepth[2]", 9, _mainFrameBuffer[2]->GetDepthTexture());
	_mainMixShader->SetUniform("backgroundDepth[3]", 10, _mainFrameBuffer[3]->GetDepthTexture());
	_mainMixShader->SetUniform("objectLayer", 11, _mainFrameBuffer[4]->GetColorBuffer(0));
	_mainMixShader->SetUniform("objectLayerDepth", 12, _mainFrameBuffer[4]->GetDepthTexture());
	_mainMixShader->SetUniform("objectLayerMask", 13, _mainFrameBuffer[5]->GetColorBuffer(0));

	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

	_mainMixVao->End();
	_mainMixShader->End();
}

bool Gpu::InitPalettesGL(bool background)
{
	const float vertexData[] = {
		0, 0,
		1, 0,
		0, 1,
		1, 1,
	};
	int index = background ? 0 : 1;
	try {
		// Clear
		GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

		if (_paletteInitialized[index]) {
			DestroyPalettesGL(background);
		}
		_paletteVertexData.IncRef(BufferObject::Type::Array); 
		if (_paletteVertexData.GetRefCount() == 1) {
			_paletteVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
		}

		if (_paletteVao[index] == nullptr) {
			_paletteVao[index] = new VertexArrayObject();
			_paletteVao[index]->Begin();
			_paletteVao[index]->BindBuffer(0, *_paletteVertexData, 2, VertexArrayObject::Float, false, 2 * sizeof(float), 0 * sizeof(float));
			_paletteVao[index]->End();
		}

		ShaderSource vertexSource(resource_screenrect_vert_glsl, resource_screenrect_vert_glsl_len);
		ShaderSource fragmentSource(resource_mainInterface_glsl, resource_mainInterface_glsl_len);
		fragmentSource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
		fragmentSource.AddSource(resource_palette_frag_glsl, resource_palette_frag_glsl_len);

		_paletteShader.IncRef("Palette shader");
		if (_paletteShader.GetRefCount() == 1) {
			_paletteShader->AddShader(ShaderProgram::Vertex, vertexSource);
			_paletteShader->AddShader(ShaderProgram::Fragment, fragmentSource);
			_paletteShader->Link();
		}
	}
	catch (ShaderCompileException &e) {
		Log(Error, "Error while compiling shaders:\n %s", e.GetMsg());
		return false;
	}
	catch (GraphicsException &e) {
		Log(Error, "Error while creating palette graphics objects: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
		return false;
	}
	_paletteInitialized[index] = true;
	return true;
}

void Gpu::DestroyPalettesGL(bool background)
{
	int index = background ? 0 : 1;
	if (_paletteInitialized[index] == false) return;
	_paletteVertexData.DecRef();
	_paletteShader.DecRef();
	if (_paletteVao[index] != nullptr) {
		delete _paletteVao[index];
		_paletteVao[index] = nullptr;
	}
	_paletteInitialized[index] = false;
}

void Gpu::DrawPalettes(bool background)
{
	int index = background ? 0 : 1;
	try {
		if (!_paletteInitialized[index]) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw palette without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 512, 256));

		_paletteShader->Begin();
		_paletteVao[index]->Begin();

		SetupShaderInterface(*_paletteShader);
		_paletteShader->SetUniform("objectPalette", index);
		_paletteShader->SetUniform("screenSize", 512.0f, 256.0f);
		_paletteShader->SetUniform("lineNr", _vcount >= SCREEN_HEIGHT ? 0 : (int)_vcount);

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
		0, 0,
		1, 0,
		0, 1,
		1, 1,
	};
	try {
		if (_tilesInitialized[idx]) {
			DestroyTilesGL(idx);
		}

		_tilesVertexData.IncRef(BufferObject::Type::Array);
		if (_tilesVertexData.GetRefCount() == 1) {
			_tilesVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
		}

		if (_tilesVao[idx] == nullptr) {
			_tilesVao[idx] = new VertexArrayObject();
			_tilesVao[idx]->Begin();
			_tilesVao[idx]->BindBuffer(0, *_tilesVertexData, 2, VertexArrayObject::Float, false, 2 * sizeof(float), 0 * sizeof(float));
			_tilesVao[idx]->End();
		}

		ShaderSource vertexSource(resource_screenrect_vert_glsl, resource_screenrect_vert_glsl_len);
		ShaderSource fragmentSource(resource_mainInterface_glsl, resource_mainInterface_glsl_len);
		fragmentSource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
		fragmentSource.AddSource(resource_tiles_frag_glsl, resource_tiles_frag_glsl_len);

		_tilesShader.IncRef("Tiles shader");
		if (_tilesShader.GetRefCount() == 1) {
			_tilesShader->AddShader(ShaderProgram::Vertex, vertexSource);
			_tilesShader->AddShader(ShaderProgram::Fragment, fragmentSource);
			_tilesShader->Link();
		}
	}
	catch (ShaderCompileException &e) {
		Log(Error, "Error while compiling shaders:\n %s", e.GetMsg());
		return false;
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
	_tilesVertexData.DecRef();
	_tilesShader.DecRef();
	if (_tilesVao[idx] != nullptr) {
		delete _tilesVao[idx];
		_tilesVao[idx] = nullptr;
	}
	_tilesInitialized[idx] = false;
}

void Gpu::DrawTiles(int idx)
{
	const uint32_t tilesAddresses[3] = { 0x00000, 0x08000, 0x10000 };
	uint32_t address = tilesAddresses[idx];
	try {
		if (!_tilesInitialized[idx]) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw tiles without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 512, 512));
		
		_tilesShader->Begin();
		_tilesVao[idx]->Begin();
		
		int lineNr = _vcount >= SCREEN_HEIGHT ? 0 : _vcount;

		SetupShaderInterface(*_tilesShader);
		_tilesShader->SetUniform("largePalette", _debugTiles8BitDepth[idx] ? 1 : 0);
		_tilesShader->SetUniform("baseAddress", 0x8000 * idx);
		_tilesShader->SetUniform("lineNr", lineNr);
		_tilesShader->SetUniform("objectTiles", idx == 2 ? 1 : 0);
		_tilesShader->SetUniform("screenSize", 512.0f, 512.0f);
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
		0, 0,
		1, 0,
		0, 1,
		1, 1,
	};
	try {
		if (_oamInitialized) {
			DestroyOAMGL();
		}

		_oamVertexData.IncRef(BufferObject::Type::Array);
		if (_oamVertexData.GetRefCount() == 1) {
			_oamVertexData->BufferData(BufferObject::Usage::StaticDraw, sizeof(vertexData), vertexData);
		}

		if (_oamVao == nullptr) {
			_oamVao = new VertexArrayObject();
			_oamVao->Begin();
			_oamVao->BindBuffer(0, *_oamVertexData, 2, VertexArrayObject::Float, false, 2 * sizeof(float), 0 * sizeof(float));
			_oamVao->End();
		}

		ShaderSource vertexSource(resource_screenrect_vert_glsl, resource_screenrect_vert_glsl_len);
		ShaderSource fragmentSource(resource_mainInterface_glsl, resource_mainInterface_glsl_len);
		fragmentSource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
		fragmentSource.AddSource(resource_oam_frag_glsl, resource_oam_frag_glsl_len);

		_oamShader.IncRef("Oam shader");
		if (_oamShader.GetRefCount() == 1) {
			_oamShader->AddShader(ShaderProgram::Vertex, vertexSource);
			_oamShader->AddShader(ShaderProgram::Fragment, fragmentSource);
			_oamShader->Link();
		}
	}
	catch (ShaderCompileException &e) {
		Log(Error, "Error while compiling shaders:\n %s", e.GetMsg());
		return false;
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
	_oamVertexData.DecRef();
	_oamShader.DecRef();
	if (_oamVao != nullptr) {
		delete _oamVao;
		_oamVao = nullptr;
	}
	_oamInitialized = false;
}

void Gpu::DrawOAM()
{
	try {
		if (!_oamInitialized) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw oam without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		GL_CHECKED(glViewport(0, 0, 768, 384));
		
		_oamShader->Begin();
		_oamVao->Begin();

		int lineNr = _vcount >= SCREEN_HEIGHT ? 0 : _vcount;

		SetupShaderInterface(*_oamShader);
		_oamShader->SetUniform("lineNr", 80);
		_oamShader->SetUniform("screenSize", 768.0f, 384.0f);

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
	try {
		if (_bgInitialized[idx]) {
			DestroyBGGL(idx);
		}
		ShaderSource vertexSource(resource_mainInterface_glsl, resource_mainInterface_glsl_len);
		ShaderSource geometrySource(vertexSource);
		ShaderSource fragmentSource(vertexSource);
		vertexSource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
		vertexSource.AddSource(resource_bg_vert_glsl, resource_bg_vert_glsl_len);
		geometrySource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
		geometrySource.AddSource(resource_bg_geo_glsl, resource_bg_geo_glsl_len);
		fragmentSource.AddSource(resource_mainFuncs_glsl, resource_mainFuncs_glsl_len);
		fragmentSource.AddSource(resource_bg_frag_glsl, resource_bg_frag_glsl_len);

		// Create Shader program
		_bgShader.IncRef("Debug BG");
		if (_bgShader.GetRefCount() == 1) {
			_bgShader->AddShader(ShaderProgram::Vertex, vertexSource);
			_bgShader->AddShader(ShaderProgram::Geometry, geometrySource);
			_bgShader->AddShader(ShaderProgram::Fragment, fragmentSource);
			_bgShader->Link();
		}

		if (_bgVao[idx] == nullptr) {
			_bgVao[idx] = new VertexArrayObject();
		}

		InitGridShader();

		// Create tile buffer data
		InitVRAMDataGL();
		InitPaletteDataGL();
		InitRegistersDataGL();
	}
	catch (ShaderCompileException &e) {
		Log(Error, "Error while compiling shaders:\n %s", e.GetMsg());
		return false;
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
	_mainVramBT.DecRef();
	_mainRegistersBO.DecRef();
	_mainPaletteData.DecRef();
	if (_bgVao[idx] != nullptr) {
		delete _bgVao[idx];
		_bgVao[idx] = nullptr;
	}
}

void Gpu::DrawBG(int idx)
{
	try {
		if (!_bgInitialized[idx]) throw GraphicsException(GL_INVALID_OPERATION, "Tried to draw bg without initializing");
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GL_CHECKED(glEnable(GL_DEPTH_TEST));
		GL_CHECKED(glViewport(0, 0, 512, 512));
		
		_bgShader->Begin();
		_bgVao[idx]->Begin();
		
		SetupShaderInterface(*_bgShader);
		int lineNr = _vcount < SCREEN_HEIGHT ? (int)_vcount : 0;
		_bgShader->SetUniform("lineNr", lineNr);
		_bgShader->SetUniform("background", idx);
		_bgShader->SetUniform("screenSize", 512.0f, 512.0f);
		
		GL_CHECKED(glDrawArrays(GL_POINTS, 0, 128*128));

		_bgShader->End();
		
		int bgWidth, bgHeight;
		GetBGSizeBuffer(lineNr, idx, bgWidth, bgHeight);
		float bgX, bgY;
		GetBGOffsetBuffer(lineNr, idx, bgX, bgY);
		bgX /= bgWidth;
		bgY /= bgHeight;
		int mode = GetModeBuffer(lineNr);
		bool boxEnabled = !((mode == 1 && idx == 3) || (mode >= 2 && idx < 2) || (mode >= 3 && idx == 3));
		bool gridEnabled = boxEnabled;
		if (mode == 1) mode = idx == 2 ? 2 : 0;
		bool wrapAroundFlag = (_mainDrawingRegisters[lineNr].bgCnt[idx] & (1 << 13)) != 0;
		int bgMode = (_mainDrawingRegisters[lineNr].bgCnt[idx] >> 14) & 3;
		bool wraparound = mode == 0 || (mode == 2 && wrapAroundFlag && bgMode != 0);

		DrawGrid(gridEnabled, bgWidth / 8, bgHeight / 8, 0xFFAAAAAA, 0.01f,
			boxEnabled, wraparound, bgX, bgY, 240.0f / bgWidth, 160.0f / bgHeight, 0xFF0000FF, 0.0f);

		_bgVao[idx]->End();
		
	}
	catch (GraphicsException  &e) {
		Log(Error, "Graphics exception in drawing bg: %s\nStacktrace: %s", e.GetMsg(), e.GetStacktrace());
	}
}

void Gpu::InitGridShader()
{
	_gridShader.IncRef("Grid");
	if (_gridShader.GetRefCount() == 1) {
		ShaderSource vertexSource(resource_grid_vert_glsl, resource_grid_vert_glsl_len);
		ShaderSource geometrySource(resource_grid_geo_glsl, resource_grid_geo_glsl_len);
		ShaderSource fragmentSource(resource_lines_frag_glsl, resource_lines_frag_glsl_len);

		_gridShader->AddShader(ShaderProgram::Vertex, vertexSource);
		_gridShader->AddShader(ShaderProgram::Geometry, geometrySource);
		_gridShader->AddShader(ShaderProgram::Fragment, fragmentSource);
		_gridShader->Link();
	}

}

void Gpu::DestroyGridShader()
{
	_gridShader.DecRef();
}

void Gpu::DrawGrid(bool gridEnabled, int gridWidth, int gridHeight, int gridColor, float gridDepth,
	bool boxEnabled, bool boxWraparound, float boxX, float boxY, float boxWidth, float boxHeight, int boxColor, float boxDepth)
{
	_gridShader->Begin();

	_gridShader->SetUniform("gridSize", gridWidth, gridHeight);
	_gridShader->SetUniform("gridColor", float((gridColor >> 0) & 0xFF) / 255.0f,
		float((gridColor >> 8) & 0xFF) / 255.0f, float((gridColor >> 16) & 0xFF) / 255.0f,
		float((gridColor >> 24) & 0xFF) / 255.0f);
	_gridShader->SetUniform("gridEnabled", gridEnabled ? 1 : 0);
	_gridShader->SetUniform("boxSize", boxWidth * 2.0f, boxHeight * 2.0f);
	_gridShader->SetUniform("boxPos", boxX * 2.0f - 1.0f, 1.0f - boxY * 2.0f);
	_gridShader->SetUniform("boxEnabled", boxEnabled ? 1 : 0);
	_gridShader->SetUniform("boxWraparound", boxWraparound ? 1 : 0);
	_gridShader->SetUniform("boxColor", float((boxColor >> 0) & 0xFF) / 255.0f,
		float((boxColor >> 8) & 0xFF) / 255.0f, float((boxColor >> 16) & 0xFF) / 255.0f,
		float((boxColor >> 24) & 0xFF) / 255.0f);
	_gridShader->SetUniform("gridDepth", gridDepth);
	_gridShader->SetUniform("boxDepth", boxDepth);

	GL_CHECKED(glDrawArrays(GL_POINTS, 0, 1 + (gridWidth+63)/64 + (gridHeight+63) / 64));

	_gridShader->End();
}

void Gpu::InitPaletteDataGL()
{
	_mainPaletteData.IncRef(16, 32, SCREEN_HEIGHT, nullptr, Texture3D::RGB);
	if (_mainPaletteData.GetRefCount() == 1) {
		_mainPaletteData->SetFilter(Texture3D::Linear, Texture3D::Linear);
		_mainPaletteData->SetWrap(Texture3D::Repeat, Texture3D::Repeat);
	}
}

void Gpu::InitVRAMDataGL()
{
	_mainVramBT.IncRef(SCREEN_HEIGHT * VRAMSIZE_USED, nullptr, BufferTexture::Format::R8UI, BufferObject::Usage::StreamDraw);
}

void Gpu::InitOAMDataGL()
{
	_mainOamBT.IncRef(SCREEN_HEIGHT * ORAMSIZE, nullptr, BufferTexture::Format::R16UI, BufferObject::Usage::StreamDraw);
}

void Gpu::InitRegistersDataGL()
{
	_mainRegistersBO.IncRef(BufferObject::Type::Uniform);
	if (_mainRegistersBO.GetRefCount() == 1) {
		_mainRegistersBO->BufferData(BufferObject::Usage::StreamDraw, SCREEN_HEIGHT * sizeof(DrawingInfoData_t), _mainDrawingRegisters);
	}
}

