#include "GbGpu.h"

#include <GL/glew.h>
#include <gl/wglew.h>

#include <emuall/graphics/graphicsException.h>

#include "../../resources/resources.h"
#include "../../util/log.h"

bool GbGpu::InitGL(int user)
{
	// Data for quad which will be displayed on the screen
	static const GLfloat screenData[] = {
		// x     y      z    u                      v
		-1.0f, 1.0f,  0.0f, 0.0f,                  0.0f,
		1.0f,  1.0f,  0.0f, SCREEN_WIDTH / 256.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,                  SCREEN_HEIGHT / 256.0f,
		1.0f,  -1.0f, 0.0f, SCREEN_WIDTH / 256.0f, SCREEN_HEIGHT / 256.0f,
	};
	static const GLfloat tilesData[] = {
		// x    y      z     u             v
		-1.0f, 1.0f,  0.0f, 0.0f,         0.0f,
		1.0f,  1.0f,  0.0f, 289 / 512.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,         217 / 512.0f,
		1.0f,  -1.0f, 0.0f, 289 / 512.0f, 217 / 512.0f,
	};
	static const GLfloat oamData [] = {
		// x    y       z    u             v
		-1.0f, 1.0f,  0.0f, 0.0f,        0.0f,
		1.0f,  1.0f,  0.0f, 73 / 128.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,        86 / 128.0f,
		1.0f,  -1.0f, 0.0f, 73 / 128.0f, 86 / 128.0f,
	};
	static const GLfloat bgData [] = {
		// x    y       z    u      v
		-1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
		1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,
	};

	Log(Message, "OpenGL information: \n\tVersion: %s\n\tVendor: %s\n\tRenderer: %s",
		glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));

	try {
		switch (user)
		{
		case 0: {// main screen
				 // Generate texture for drawing
			_texture.LoadTexture(256, 256, NULL, Texture::RGB);
			_texture.Bind();
			_texture.SetFilter(Texture::Nearest, Texture::Nearest);
			
			if (_surfaceVAO == nullptr) {
				delete _surfaceVAO;
			}
			if (_surfaceBO== nullptr) {
				delete _surfaceBO;
			}
			_surfaceVAO = new VertexArrayObject();
			_surfaceBO = new BufferObject(BufferObject::Type::Array);
			_surfaceBO->BufferData(BufferObject::Usage::StaticDraw, sizeof(screenData), screenData);
			_surfaceVAO->Begin();
			_surfaceVAO->BindBuffer(0, *_surfaceBO, 3, VertexArrayObject::Float, false, 5 * sizeof(GLfloat), 0);
			_surfaceVAO->BindBuffer(1, *_surfaceBO, 2, VertexArrayObject::Float, false, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
			_surfaceVAO->End();

			// Initialize viewport
			GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

			// Load shaders
			try {
				_shader.AddShader(ShaderProgram::Vertex, ShaderSource(resource_gb_vert_glsl, resource_gb_vert_glsl_len));
				_shader.AddShader(ShaderProgram::Fragment, ShaderSource(resource_gb_frag_glsl, resource_gb_frag_glsl_len));
				_shader.Link();
			}
			catch (ShaderCompileException &e) {
				Log(Error, "Shader Compile Exception:\n%s", e.GetMsg());
				return false;
			}
			catch (GraphicsException &e) {
				Log(Error, "Graphics Exception:\n%s", e.GetMsg());
				return false;
			}
			break;
		}
		case 1: {// Background screen
			_BGTexture.LoadTexture(256, 256, NULL, Texture::RGB);
			_BGTexture.Bind();
			_BGTexture.SetFilter(Texture::Nearest, Texture::Nearest);
			if (_BGVAO == nullptr) {
				delete _BGVAO;
			}
			if (_BGBO == nullptr) {
				delete _BGBO;
			}
			_BGVAO = new VertexArrayObject();
			_BGBO = new BufferObject(BufferObject::Type::Array);
			_BGBO->BufferData(BufferObject::Usage::StaticDraw, sizeof(bgData), bgData);
			_BGVAO->Begin();
			_BGVAO->BindBuffer(0, *_BGBO, 3, VertexArrayObject::Float, false, 5 * sizeof(GLfloat), 0);
			_BGVAO->BindBuffer(1, *_BGBO, 2, VertexArrayObject::Float, false, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
			_BGVAO->End();
			// Load shaders
			try {
				_BGShader.AddShader(ShaderProgram::Vertex, ShaderSource(resource_gb_vert_glsl, resource_gb_vert_glsl_len));
				_BGShader.AddShader(ShaderProgram::Fragment, ShaderSource(resource_gb_frag_glsl, resource_gb_frag_glsl_len));
				_BGShader.Link();
			}
			catch (ShaderCompileException &e) {
				Log(Error, "Shader Compile Exception:\n%s", e.GetMsg());
				return false;
			}
			catch (GraphicsException &e) {
				Log(Error, "Graphics Exception:\n%s", e.GetMsg());
				return false;
			}
			break;
		}
		case 2: {// Tiles screen
			_TiTexture.LoadTexture(512, 512, NULL, Texture::RGB);
			_TiTexture.Bind();
			_TiTexture.SetFilter(Texture::Nearest, Texture::Nearest);
			if (_TiVAO == nullptr) {
				delete _TiVAO;
			}
			if (_TiBO == nullptr) {
				delete _TiBO;
			}
			_TiVAO = new VertexArrayObject();
			_TiBO = new BufferObject(BufferObject::Type::Array);
			_TiBO->BufferData(BufferObject::Usage::StaticDraw, sizeof(tilesData), tilesData);
			_TiVAO->Begin();
			_TiVAO->BindBuffer(0, *_TiBO, 3, VertexArrayObject::Float, false, 5 * sizeof(GLfloat), 0);
			_TiVAO->BindBuffer(1, *_TiBO, 2, VertexArrayObject::Float, false, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
			_TiVAO->End();
			GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			// Load shaders
			try {
				_TiShader.AddShader(ShaderProgram::Vertex, ShaderSource(resource_gb_vert_glsl, resource_gb_vert_glsl_len));
				_TiShader.AddShader(ShaderProgram::Fragment, ShaderSource(resource_gb_frag_glsl, resource_gb_frag_glsl_len));
				_TiShader.Link();
			}
			catch (ShaderCompileException &e) {
				Log(Error, "Shader Compile Exception:\n%s", e.GetMsg());
				return false;
			}
			catch (GraphicsException &e) {
				Log(Error, "Graphics Exception:\n%s", e.GetMsg());
				return false;
			}
			break;
		}
		case 3: {// OAM screen
			_OAMTexture.LoadTexture(128, 128, NULL, Texture::RGB);
			_OAMTexture.Bind();
			_OAMTexture.SetFilter(Texture::Nearest, Texture::Nearest);
			if (_OAMVAO == nullptr) {
				delete _OAMVAO;
			}
			if (_OAMBO == nullptr) {
				delete _OAMBO;
			}
			_OAMVAO = new VertexArrayObject();
			_OAMBO = new BufferObject(BufferObject::Type::Array);
			_OAMBO->BufferData(BufferObject::Usage::StaticDraw, sizeof(oamData), oamData);
			_OAMVAO->Begin();
			_OAMVAO->BindBuffer(0, *_OAMBO, 3, VertexArrayObject::Float, false, 5 * sizeof(GLfloat), 0);
			_OAMVAO->BindBuffer(1, *_OAMBO, 2, VertexArrayObject::Float, false, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
			_OAMVAO->End();
			GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			// Load shaders
			try {
				_OAMShader.AddShader(ShaderProgram::Vertex, ShaderSource(resource_gb_vert_glsl, resource_gb_vert_glsl_len));
				_OAMShader.AddShader(ShaderProgram::Fragment, ShaderSource(resource_gb_frag_glsl, resource_gb_frag_glsl_len));
				_OAMShader.Link();
			}
			catch (ShaderCompileException &e) {
				Log(Error, "Shader Compile Exception:\n%s", e.GetMsg());
				return false;
			}
			catch (GraphicsException &e) {
				Log(Error, "Graphics Exception:\n%s", e.GetMsg());
				return false;
			}
			break;
		}
		default: {
			Log(Warn, "Initializing unknown screen");
			return false;
		}
		}
	}
	catch (GraphicsException &e) {
		Log(Error, "GLerror during emulator initGL(%d): %s\n%s", user, e.GetMsg(), e.GetStacktrace());
	}
	return true;
}

void GbGpu::DestroyGL(int user)
{
	try {
		switch (user)
		{
		case 0: {// main screen
			_texture.Clean();
			if (_surfaceVAO == nullptr) {
				delete _surfaceVAO;
			}
			if (_surfaceBO == nullptr) {
				delete _surfaceBO;
			}
			_shader.Clean();
			break;
		}
		case 1: {// BG screen
			_BGTexture.Clean();
			if (_BGVAO == nullptr) {
				delete _BGVAO;
			}
			if (_BGBO == nullptr) {
				delete _BGBO;
			}
			_BGShader.Clean();
			break;
		}
		case 2: {// Tiles screen
			_TiTexture.Clean();
			if (_TiVAO == nullptr) {
				delete _TiVAO;
			}
			if (_TiBO == nullptr) {
				delete _TiBO;
			}
			_TiShader.Clean();
			break;
		}
		case 3: {// OAM screen
			_OAMTexture.Clean();
			if (_OAMVAO == nullptr) {
				delete _OAMVAO;
			}
			if (_OAMBO == nullptr) {
				delete _OAMBO;
			}
			_OAMShader.Clean();
			break;
		}
		}
	}
	catch (GraphicsException &e) {
		Log(Error, "GLerror during emulator destroyGL(%d): %s\n%s", user, e.GetMsg(), e.GetStacktrace());
	}
}

void GbGpu::drawGL(int user)
{
	try {
		// Clear screen
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));

		switch (user)
		{
		case 0: {// main screen		
				 // Setup viewport
			GL_CHECKED(glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

			const char *data = (const char *)((_screen == _screen_buffer1) ? _screen_buffer2 : _screen_buffer1);
			_texture.UpdateData(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, data, Texture::RGBA);

			// Enable shader and load buffers
			_shader.Begin();
			_surfaceVAO->Begin();
			_texture.Bind();

			// Draw the screen
			GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
			_texture.UnBind();

			_surfaceVAO->End();
			_shader.End();
			break;
		}
		case 1: // BG screen
			DrawDebugBackground();
			break;
		case 2: // Tiles screen
			DrawDebugTiles();
			break;
		case 3: // Oam screen
			DrawDebugOAM();
			break;
		default:
			Log(Warn, "Drawing unknown screen %d", user);
		}
	}
	catch (GraphicsException &e) {
		Log(Error, "GLerror during emulator drawGL(%d): %s\n%s", user, e.GetMsg(), e.GetStacktrace());
	}
}

void GbGpu::DrawDebugBackground()
{
	bool bgd = _BGDebug.dataMode == 2 ? (_gb->_mem->read(LCDC) & LCDC_BS) != 0 : _BGDebug.dataMode == 1;
	bool bgm = _BGDebug.mapMode == 2 ? (_gb->_mem->read(LCDC) & LCDC_BTS) != 0 : _BGDebug.mapMode == 1;
	bool gbc = _gb->IsGBC();
	bool grid = _BGDebug.grid;
	for (int ty = 0; ty < 32; ty++)
	{
		for (int tx = 0; tx < 32; tx++)
		{
			gbBgMap tile = getTileData(bgd, bgm, tx + (ty * 32));
			for (int y = 0; y < 8; y++)
			{
				for (int x = 0; x < 8; x++)
				{
					gbByte yindex = (tile.attributes & BGMAP_ATTR_VFLIP) ? 7 - y : y;
					gbByte xIndex = (tile.attributes & BGMAP_ATTR_HFLIP) ? 7 - x : x;
					gbByte pixel = getTilePixel(tile.tile, xIndex, yindex);
					//gbByte lineData = ((tile.tile[yindex*2] & (1<<xIndex)) ? 0x1 : 0x0)|((tile.tile[yindex*2+1] & (1<<xIndex)) ? 0x2 : 0x0);
					gbColor col;
					if (gbc)
					{
						col = convertPaletteData(pixel, tile.attributes & BGMAP_PALETTE, _bgPaletteData);
					}
					else {
						col = convertPaletteData(pixel, _gb->_mem->read(BGP));
					}
					if (grid && (x == 0 || y == 0))
					{
						col.color = ~col.color;
					}
					col.p = 255;
					_debugDrawBuffer[(tx * 8 + x + (ty * 8 + y) * 256)] = col;
				}
			}
		}
	}
	gbByte scx = _gb->_mem->read(SCX);
	gbByte scy = _gb->_mem->read(SCY);
	for (int x = scx; x < scx + SCREEN_WIDTH; x++)
	{
		_debugDrawBuffer[((x % 256) + scy * 256)].g ^= 0xFF;
		_debugDrawBuffer[((x % 256) + ((scy + SCREEN_HEIGHT) % 256) * 256)].g ^= 0xFF;
	}
	for (int y = scy; y < scy + SCREEN_HEIGHT; y++)
	{
		_debugDrawBuffer[(scx + (y % 256) * 256)].g ^= 0xFF;
		_debugDrawBuffer[(((scx + SCREEN_WIDTH) % 256) + (y % 256) * 256)].g ^= 0xFF;
	}


	// Setup viewport
	GL_CHECKED(glViewport(0, 0, 256, 256));

	GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));

	_BGTexture.UpdateData(0, 0, 256, 256, (const char *)_debugDrawBuffer, Texture::RGBA);

	// Enable shader and load buffers
	_BGShader.Begin();
	_BGVAO->Begin();
	_BGTexture.Bind();


	// Draw the screen
	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

	// Unload the buffers
	_BGTexture.UnBind();
	_BGVAO->End();
	_BGShader.End();
}

void GbGpu::DrawDebugTiles()
{
	for (int y = 0; y < 217; y++)
	{
		int yi = (y - 1) % 9;
		int ty = (y - 1) / 9;
		for (int x = 0; x < 289; x++)
		{
			int xi = (x - 1) % 9;
			int tx = (x - 1) / 9;
			if (xi == 8 || yi == 8 || y == 0 || x == 0)
			{
				_debugDrawBuffer[(y * 289 + x)].color = 0xFF000000;
				continue;
			}
					
			gbByte pixel = getTilePixel(_gb->_mem->_vram[tx / 16] + (ty * 16 * 8 * 2 + (tx % 16) * 8 * 2), xi, yi);
			switch (pixel)
			{
			case 0:
				_debugDrawBuffer[(y * 289 + x)].color = 0xFFFFFFFF;
				break;
			case 1:
				_debugDrawBuffer[(y * 289 + x)].color = 0xFFD3D3D3;
				break;
			case 2:
				_debugDrawBuffer[(y * 289 + x)].color = 0xFFA9A9A9;
				break;
			case 3:
				_debugDrawBuffer[(y * 289 + x)].color = 0xFF000000;
				break;
			}
		}
	}
	// Setup viewport
	GL_CHECKED(glViewport(0, 0, 346, 260));

	_TiTexture.UpdateData(0, 0, 289, 217, (const char *)_debugDrawBuffer, Texture::RGBA);

	// Enable shader and load buffers
	_TiShader.Begin();
	_TiVAO->Begin();
	_TiTexture.Bind();

	// Draw the screen
	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	_TiTexture.UnBind();
	_TiVAO->End();
	_TiShader.End();
}

void GbGpu::DrawDebugOAM()
{
	memset(_debugDrawBuffer, 0, 73 * 86 * sizeof(gbColor));
	bool large = (_gb->_mem->read(LCDC) & LCDC_OS) != 0;
	bool gbc = (_gb->_mem->read(CGB) & 0x80) != 0;
	for (int i = 0; i < 40; i++)
	{
		int ox = (i % 8) * 9 + 1;
		int oy = (i / 8) * 17 + 1;
		gbSprite sprite;
		sprite.y = _gb->_mem->_mem[0xF][OAM_OFFSET + i * 4 + 0];
		sprite.x = _gb->_mem->_mem[0xF][OAM_OFFSET + i * 4 + 1];
		sprite.loc = _gb->_mem->_mem[0xF][OAM_OFFSET + i * 4 + 2];
		sprite.attr = _gb->_mem->_mem[0xF][OAM_OFFSET + i * 4 + 3];
		for (int y = 0; y < 16; y++)
		{
			gbByte yindex;
			if (large)
			{
				sprite.loc &= 0xFE;
				if (sprite.attr & OAM_YFLIP)
				{
					if (y < 8) sprite.loc |= 0x01;
					yindex = 7 - (y % 8);
				}
				else {
					if (y > 7) sprite.loc |= 0x01;
					yindex = y % 8;
				}
			}
			else {
				yindex = (sprite.attr & OAM_YFLIP) ? 7 - y : y;
				if (y > 7)
				{
					gbColor col;
					col.color = 0xFFFFFFFF;
					for (int x = 0; x < 8; x++)
					{
						_debugDrawBuffer[(oy + y) * 73 + (ox + x)] = col;
					}
					continue;
				}
			}

			gbByte *ptr = _gb->_mem->_vram[(sprite.attr & OAM_VRAM) ? 1 : 0] + SPRITE_OFFSET + sprite.loc * 2 * 8;
			for (int x = 0; x < 8; x++)
			{
				gbByte xindex = (sprite.attr & OAM_XFLIP) ? 7 - x : x;
				gbByte pixel = _gb->_gpu->getTilePixel(ptr, xindex, yindex);
				gbColor col;
				if (gbc)
				{
					col = _gb->_gpu->convertPaletteData(pixel, sprite.attr & OAM_PALETTE, _oPaletteData);
				}
				else {
					col = _gb->_gpu->convertPaletteData(pixel, _gb->_mem->read((sprite.attr & OAM_PALETTENR) ? OBP1 : OBP0));
				}
				col.p = 255;
				_debugDrawBuffer[(oy + y)*73 + (ox + x)] = col;
			}
		}
	}
	// Setup viewport
	GL_CHECKED(glViewport(0, 0, 265, 326));

	_OAMTexture.UpdateData(0, 0, 73, 86, (const char *)_debugDrawBuffer, Texture::RGBA);

	// Enable shader and load buffers
	_OAMShader.Begin();
	_OAMVAO->Begin();
	_OAMTexture.Bind();

	// Draw the screen
	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

	_OAMTexture.UnBind();
	_OAMVAO->End();
	_OAMShader.End();
}

void GbGpu::Reshape(int id, int width, int height, bool keepAspect)
{
}


void GbGpu::drawing(gbByte ly)
{
	_gb->_mem->setAccessable(false, false);
	gbByte lcds = _gb->_mem->read(LCDS);
	gbByte lcdc = _gb->_mem->read(LCDC);

	_gb->_mem->write(LCDS, (lcds & ~LCDS_M_M) | LCDS_M_T);

	// get modes
	bool be = (lcdc & LCDC_BD) != 0; //Background enabled
	bool we = (lcdc & LCDC_WE) != 0; // Window enabled
	bool oe = (lcdc & LCDC_OE) != 0; // Sprite(OBJ) enabled
	bool gbc = (_gb->_mem->read(CGB) & 0x80) != 0; // gameboy color mode
	bool bgd = (lcdc & LCDC_BS) != 0; // Background data location
	bool bgm = (lcdc & LCDC_BTS) != 0; // Background map location
	bool wtm = (lcdc & LCDC_TS) != 0; // Window tile map location
	bool large = (_gb->_mem->read(LCDC) & LCDC_OS) != 0; // 8 by 16 sprites
	gbByte scx = _gb->_mem->read(SCX); // Background scroll x
	gbByte scy = _gb->_mem->read(SCY); // Background scroll y
	gbByte wx = _gb->_mem->read(WX); // window start x
	gbByte wy = _gb->_mem->read(WY); // window start y

	// Draw the background
	gbByte bg_y = (ly + scy) % 256;
	gbByte t_y = bg_y / 8;
	gbByte ti_y = bg_y % 8;
	for (int x = scx; x < scx + SCREEN_WIDTH; x++)
	{
		gbBgMap tile = getTileData(bgd, bgm, t_y * 32 + (x % 256) / 8); // retrieve the correct tile information
		gbByte xindex = (tile.attributes & BGMAP_ATTR_HFLIP) ? 7 - (x % 8) : (x % 8); // flip on the x axis
		gbByte yindex = (tile.attributes & BGMAP_ATTR_VFLIP) ? 7 - (ti_y) : (ti_y); // Flip on the y axis
		gbByte pixel = getTilePixel(tile.tile, xindex, yindex); // Get the pixel data
		if (gbc)
		{
			gbColor col = convertPaletteData(pixel, tile.attributes & BGMAP_PALETTE, _bgPaletteData);
			_screen[ly * SCREEN_WIDTH + x - scx].r = col.r;
			_screen[ly * SCREEN_WIDTH + x - scx].g = col.g;
			_screen[ly * SCREEN_WIDTH + x - scx].b = col.b;
			_screen[ly * SCREEN_WIDTH + x - scx].p = pixel | (tile.attributes & PRIO_BG);
		}
		else {
			if (be)
			{
				gbColor col = convertPaletteData(pixel, _gb->_mem->read(BGP));
				_screen[ly * SCREEN_WIDTH + x - scx].r = col.r;
				_screen[ly * SCREEN_WIDTH + x - scx].g = col.g;
				_screen[ly * SCREEN_WIDTH + x - scx].b = col.b;
				_screen[ly * SCREEN_WIDTH + x - scx].p = pixel | (tile.attributes & PRIO_BG);
			}
			else {
				_screen[ly * SCREEN_WIDTH + x - scx].r = 0xFF;
				_screen[ly * SCREEN_WIDTH + x - scx].g = 0xFF;
				_screen[ly * SCREEN_WIDTH + x - scx].b = 0xFF;
				_screen[ly * SCREEN_WIDTH + x - scx].p = 0x00;
			}
		}
	}
	// Draw the Window
	if (we && !(wy > ly) && (be || gbc))
	{
		gbByte w_y = (ly - wy) / 8;
		gbByte wi_y = (ly - wy) % 8;
		for (int x = wx - 7; x < SCREEN_WIDTH; x++)
		{
			if (x < 0)
			{
				continue;
			}
			gbBgMap tile = getTileData(bgd, wtm, w_y * 32 + (x - (wx - 7)) / 8); // retrieve the correct tile information
			gbByte xindex = (tile.attributes & BGMAP_ATTR_HFLIP) ? 7 - ((x - (wx - 7)) % 8) : ((x - (wx - 7)) % 8); // flip on the x axis
			gbByte yindex = (tile.attributes & BGMAP_ATTR_VFLIP) ? 7 - (wi_y) : (wi_y); // Flip on the y axis
			gbByte pixel = getTilePixel(tile.tile, xindex, yindex); // Get the pixel data
			gbColor col;
			if (gbc)
			{
				col = convertPaletteData(pixel, tile.attributes & BGMAP_PALETTE, _bgPaletteData);
			}
			else {
				col = convertPaletteData(pixel, _gb->_mem->read(BGP));
			}
			_screen[ly * SCREEN_WIDTH + x].r = col.r;
			_screen[ly * SCREEN_WIDTH + x].g = col.g;
			_screen[ly * SCREEN_WIDTH + x].b = col.b;
			_screen[ly * SCREEN_WIDTH + x].p &= ~0x3;
			_screen[ly * SCREEN_WIDTH + x].p |= PRIO_WI | pixel | ((tile.attributes & BGMAP_ATTR_PRIO) ? PRIO_BG : 0x00);
		}
	}
	// Draw the prebuffered sprites
	if (oe)
	{
		gbByte *ptr = _gb->_mem->_mem[0xF] + OAM_OFFSET;

		for (int i = 39; i >= 0; i--)
		{
			if (_lineSprites[i] == -1)
				continue;
			gbSprite sprite;
			sprite.y = ptr[_lineSprites[i] * 4];
			sprite.x = ptr[_lineSprites[i] * 4 + 1];
			sprite.nr = ptr[_lineSprites[i] * 4 + 2];
			sprite.attr = ptr[_lineSprites[i] * 4 + 3];
			gbByte iny = ly - (sprite.y - 16);
			gbByte tileNr = sprite.nr;
			gbByte yindex;
			if (large)
			{
				tileNr &= 0xFE;
				if (sprite.attr & OAM_YFLIP)
				{
					if (iny < 8) tileNr |= 0x01;
					yindex = 7 - (iny % 8);
				}
				else {
					if (iny > 7) tileNr |= 0x01;
					yindex = iny % 8;
				}
			}
			else {
				yindex = (sprite.attr & OAM_YFLIP) ? 7 - iny : iny;
				if (iny > 7)
				{
					continue;
				}
			}
			gbByte *ptr = _gb->_mem->_vram[(sprite.attr & OAM_VRAM) ? 1 : 0] + SPRITE_OFFSET + tileNr * 2 * 8;
			for (int x = 0; x < 8; x++)
			{
				gbByte screenx = (sprite.x - 8 + x);
				if (screenx < 0 || screenx >= SCREEN_WIDTH ||
					((_screen[ly * SCREEN_WIDTH + screenx].p & PRIO_BG) != 0))
				{
					// when the sprite is offscreen, dont draw it
					// or the background has priority
					continue;
				}
				gbByte xindex = (sprite.attr & OAM_XFLIP) ? 7 - x : x;
				gbByte pixel = getTilePixel(ptr, xindex, yindex);
				if (pixel == 0)
				{
					// Color 0 is invisable
					continue;
				}
				gbColor col;
				if (gbc)
				{
					col = convertPaletteData(pixel, sprite.attr & OAM_PALETTE, _oPaletteData);
				}
				else {
					col = convertPaletteData(pixel, _gb->_mem->read((sprite.attr & OAM_PALETTENR) ? OBP1 : OBP0));
				}
				if (sprite.attr & OAM_PRIO)
				{
					// OBJ behind bg color 1-3
					if ((_screen[ly * SCREEN_WIDTH + screenx].p & 0x3) == 0)
					{
						_screen[ly * SCREEN_WIDTH + screenx].r = col.r;
						_screen[ly * SCREEN_WIDTH + screenx].g = col.g;
						_screen[ly * SCREEN_WIDTH + screenx].b = col.b;
					}
				}
				else {
					// OBJ above bg
					_screen[ly * SCREEN_WIDTH + screenx].r = col.r;
					_screen[ly * SCREEN_WIDTH + screenx].g = col.g;
					_screen[ly * SCREEN_WIDTH + screenx].b = col.b;
				}
			}
		}
	}

}
