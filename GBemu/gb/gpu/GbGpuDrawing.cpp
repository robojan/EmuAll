#include "GbGpu.h"

#include <GL/glew.h>
#include <gl/wglew.h>

#include <emuall/graphics/graphicsException.h>

#include "../../resources/resources.h"
#include "../../util/log.h"

bool GbGpu::InitGL(int user)
{
	// Data for quad which will be displayed on the screen
	static const GLfloat square[] = {
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
	};
	static const GLfloat screenUVData[] = {
		0.0f, 0.0f,
		SCREEN_WIDTH / 256.0f, 0.0f,
		0.0f, SCREEN_HEIGHT / 256.0f,
		SCREEN_WIDTH / 256.0f, SCREEN_HEIGHT / 256.0f,
	};
	static const GLfloat tilesUVData[] = {
		0.0f, 0.0f,
		289 / 512.0f, 0.0f,
		0.0f, 217 / 512.0f,
		289 / 512.0f, 217 / 512.0f,
	};
	static const GLfloat oamUVData [] = {
		0.0f, 0.0f,
		73 / 128.0f, 0.0f,
		0.0f, 86 / 128.0f,
		73 / 128.0f, 86 / 128.0f,
	};
	static const GLfloat totalUVData [] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};

	Log(Message, "OpenGL information: \n\tVersion: %s\n\tVendor: %s\n\tRenderer: %s",
		glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));

	try {
		switch (user)
		{
		case 0: {// main screen
				 // Generate texture for drawing
			_texture.LoadTexture(256, 256, NULL, Texture::RGB);
			_texture.Begin();
			_texture.SetFilter(Texture::Nearest, Texture::Nearest);

			GL_CHECKED(glGenVertexArrays(1, &_vao));
			GL_CHECKED(glBindVertexArray(_vao));

			// Generate vertex buffer
			GL_CHECKED(glGenBuffers(1, &_surfaceVBO));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _surfaceVBO));
			GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW));

			// Generate UV buffer
			GL_CHECKED(glGenBuffers(1, &_surfaceUVBO));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _surfaceUVBO));
			GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(screenUVData), screenUVData, GL_STATIC_DRAW));

			// Initialize viewport
			GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

			// Load shaders
			bool result = _shader.AddShader(ShaderProgram::Vertex, (char *)resource_gb_vert, sizeof(resource_gb_vert));
			if (!result) {
				Log(Error, "%s", _shader.GetLog());
				return false;
			}
			result = _shader.AddShader(ShaderProgram::Fragment, (char *)resource_gb_frag, sizeof(resource_gb_frag));
			if (!result) {
				Log(Error, "%s", _shader.GetLog());
				return false;
			}
			result = _shader.Link();
			if (!result) {
				Log(Error, "%s", _shader.GetLog());
				return false;
			}
			break;
		}
		case 1: {// Background screen
			_BGTexture.LoadTexture(256, 256, NULL, Texture::RGB);
			_BGTexture.Begin();
			_BGTexture.SetFilter(Texture::Nearest, Texture::Nearest);
			GL_CHECKED(glGenVertexArrays(1, &_BGVAO));
			GL_CHECKED(glBindVertexArray(_BGVAO));
			GL_CHECKED(glGenBuffers(1, &_BGVBO));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _BGVBO));
			GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW));
			GL_CHECKED(glGenBuffers(1, &_BGUVBO));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _BGUVBO));
			GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(totalUVData), totalUVData, GL_STATIC_DRAW));
			// Load shaders
			bool result = _BGShader.AddShader(ShaderProgram::Vertex, (char *)resource_gb_vert, sizeof(resource_gb_vert));
			if (!result) {
				Log(Error, "%s", _BGShader.GetLog());
				return false;
			}
			result = _BGShader.AddShader(ShaderProgram::Fragment, (char *)resource_gb_frag, sizeof(resource_gb_frag));
			if (!result) {
				Log(Error, "%s", _BGShader.GetLog());
				return false;
			}
			result = _BGShader.Link();
			if (!result) {
				Log(Error, "%s", _BGShader.GetLog());
				return false;
			}
			break;
		}
		case 2: {// Tiles screen
			_TiTexture.LoadTexture(512, 512, NULL, Texture::RGB);
			_TiTexture.Begin();
			_TiTexture.SetFilter(Texture::Nearest, Texture::Nearest);
			GL_CHECKED(glGenVertexArrays(1, &_TiVAO));
			GL_CHECKED(glBindVertexArray(_TiVAO));
			GL_CHECKED(glGenBuffers(1, &_TiVBO));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _TiVBO));
			GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW));
			GL_CHECKED(glGenBuffers(1, &_TiUVBO));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _TiUVBO));
			GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(tilesUVData), tilesUVData, GL_STATIC_DRAW));
			GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			// Load shaders
			bool result = _TiShader.AddShader(ShaderProgram::Vertex, (char *)resource_gb_vert, sizeof(resource_gb_vert));
			if (!result) {
				Log(Error, "%s", _TiShader.GetLog());
				return false;
			}
			result = _TiShader.AddShader(ShaderProgram::Fragment, (char *)resource_gb_frag, sizeof(resource_gb_frag));
			if (!result) {
				Log(Error, "%s", _TiShader.GetLog());
				return false;
			}
			result = _TiShader.Link();
			if (!result) {
				Log(Error, "%s", _TiShader.GetLog());
				return false;
			}
			break;
		}
		case 3: {// OAM screen
			_OAMTexture.LoadTexture(128, 128, NULL, Texture::RGB);
			_OAMTexture.Begin();
			_OAMTexture.SetFilter(Texture::Nearest, Texture::Nearest);
			GL_CHECKED(glGenVertexArrays(1, &_OAMVAO));
			GL_CHECKED(glBindVertexArray(_OAMVAO));
			GL_CHECKED(glGenBuffers(1, &_OAMVBO));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _OAMVBO));
			GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW));
			GL_CHECKED(glGenBuffers(1, &_OAMUVBO));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _OAMUVBO));
			GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(oamUVData), oamUVData, GL_STATIC_DRAW));
			GL_CHECKED(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			// Load shaders
			bool result = _OAMShader.AddShader(ShaderProgram::Vertex, (char *)resource_gb_vert, sizeof(resource_gb_vert));
			if (!result) {
				Log(Error, "%s", _OAMShader.GetLog());
				return false;
			}
			result = _OAMShader.AddShader(ShaderProgram::Fragment, (char *)resource_gb_frag, sizeof(resource_gb_frag));
			if (!result) {
				Log(Error, "%s", _OAMShader.GetLog());
				return false;
			}
			result = _OAMShader.Link();
			if (!result) {
				Log(Error, "%s", _OAMShader.GetLog());
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
			GL_CHECKED(glDeleteVertexArrays(1, &_vao));
			GL_CHECKED(glDeleteBuffers(1, &_surfaceVBO));
			GL_CHECKED(glDeleteBuffers(1, &_surfaceUVBO));
			_shader.Clean();
			break;
		}
		case 1: {// BG screen
			_BGTexture.Clean();
			GL_CHECKED(glDeleteVertexArrays(1, &_BGVAO));
			GL_CHECKED(glDeleteBuffers(1, &_BGVBO));
			GL_CHECKED(glDeleteBuffers(1, &_BGUVBO));
			_BGShader.Clean();
			break;
		}
		case 2: {// Tiles screen
			_TiTexture.Clean();
			GL_CHECKED(glDeleteVertexArrays(1, &_TiVAO));
			GL_CHECKED(glDeleteBuffers(1, &_TiVBO));
			GL_CHECKED(glDeleteBuffers(1, &_TiUVBO));
			_TiShader.Clean();
			break;
		}
		case 3: {// OAM screen
			_OAMTexture.Clean();
			GL_CHECKED(glDeleteVertexArrays(1, &_OAMVAO));
			GL_CHECKED(glDeleteBuffers(1, &_OAMVBO));
			GL_CHECKED(glDeleteBuffers(1, &_OAMUVBO));
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
			GL_CHECKED(glEnableVertexAttribArray(0));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _surfaceVBO));
			GL_CHECKED(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0));
			GL_CHECKED(glEnableVertexAttribArray(1));
			GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _surfaceUVBO));
			GL_CHECKED(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0));
			_texture.Begin();

			// Draw the screen
			GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
			_texture.End();

			// Unload the buffers
			GL_CHECKED(glDisableVertexAttribArray(0));
			GL_CHECKED(glDisableVertexAttribArray(1));
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
	GL_CHECKED(glEnableVertexAttribArray(0));
	GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _BGVBO));
	GL_CHECKED(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0));
	GL_CHECKED(glEnableVertexAttribArray(1));
	GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _BGUVBO));
	GL_CHECKED(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0));
	_BGTexture.Begin();


	// Draw the screen
	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

	// Unload the buffers
	GL_CHECKED(glDisableVertexAttribArray(0));
	GL_CHECKED(glDisableVertexAttribArray(1));
	GL_CHECKED(glBindTexture(GL_TEXTURE_2D, 0));
	_BGTexture.End();
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
	GL_CHECKED(glEnableVertexAttribArray(0));
	GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _TiVBO));
	GL_CHECKED(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0));
	GL_CHECKED(glEnableVertexAttribArray(1));
	GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _TiUVBO));
	GL_CHECKED(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0));
	_TiTexture.Begin();

	// Draw the screen
	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	_TiTexture.End();

	// Unload the buffers
	GL_CHECKED(glDisableVertexAttribArray(0));
	GL_CHECKED(glDisableVertexAttribArray(1));
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
	GL_CHECKED(glEnableVertexAttribArray(0));
	GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _OAMVBO));
	GL_CHECKED(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0));
	GL_CHECKED(glEnableVertexAttribArray(1));
	GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _OAMUVBO));
	GL_CHECKED(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0));

	_OAMTexture.Begin();

	// Draw the screen
	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

	// Unload the buffers
	GL_CHECKED(glDisableVertexAttribArray(0));
	GL_CHECKED(glDisableVertexAttribArray(1));
	_OAMTexture.End();
	_OAMShader.End();
}

void GbGpu::Reshape(int id, int width, int height, bool keepAspect)
{
	// Let the host do the sizing
	/*
	if (keepAspect)
	{
		if (((float) width) / height > ((float) SCREEN_WIDTH) / SCREEN_HEIGHT)
		{
			int newWidth = static_cast<int>((((float) SCREEN_WIDTH) / SCREEN_HEIGHT)*height);
			glViewport((width - newWidth) / 2, 0, newWidth, height);
		}
		else {
			int newHeight = static_cast<int>((((float) SCREEN_HEIGHT) / SCREEN_WIDTH)*width);
			glViewport(0, (height - newHeight) / 2, width, newHeight);
		}
	}
	else {
		glViewport(0, 0, width, height);
	}
	*/
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
