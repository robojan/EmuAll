#pragma once

#include <GBAemu/memory/memory.h>
#include <emuall/graphics/ShaderProgram.h>
#include <emuall/graphics/BufferObject.h>
#include <emuall/graphics/texture.h>
#include <emuall/graphics/bufferTexture.h>
#include <emuall/graphics/VertexArrayObject.h>

class Gba;

class Gpu : public MemoryEventHandler{
public:
	Gpu(Gba &system);
	~Gpu();

	void Tick();

	virtual void HandleEvent(uint32_t address, int size) override;

	// Drawing functions
	bool InitGL(int id);
	void DestroyGL(int id);
	void Reshape(int id, int width, int height, bool keepAspect);
	void Draw(int id);


	bool _debugTiles8BitDepth[3];
	bool _debugTilesGridEnabled[3];
private:
	void RegisterEvents();

	// Palette drawing
	bool InitPalettesGL(bool background);
	void DestroyPalettesGL(bool background);
	void DrawPalettes(bool background);

	// Tile drawing
	bool InitTilesGL(int idx);
	void DestroyTilesGL(int idx);
	void DrawTiles(int idx);
	


	Gba &_system;
	uint_fast8_t _vcount;
	uint_fast16_t _hcount;

	// Drawing objects
	// Palette drawing
	ShaderProgram *_paletteShader[2];
	Texture *_paletteData[2];
	BufferObject *_paletteVertexData[2];
	VertexArrayObject *_paletteVao[2];
	// Tile drawing
	ShaderProgram *_tilesShader[3];
	Texture *_tilesPaletteData[3];
	BufferTexture *_tilesData[3];
	BufferObject *_tilesVertexData[3];
	VertexArrayObject *_tilesVao[3];
};