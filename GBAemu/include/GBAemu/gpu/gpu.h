#pragma once
#include <memory>

#include <GBAemu/memory/memory.h>
#include <emuall/graphics/ShaderProgram.h>
#include <emuall/graphics/BufferObject.h>
#include <emuall/graphics/texture.h>
#include <emuall/graphics/bufferTexture.h>
#include <emuall/graphics/VertexArrayObject.h>
#include <emuall/util/counterPointer.h>

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

	// OAM drawing
	bool InitOAMGL();
	void DestroyOAMGL();
	void DrawOAM();

	// BG drawing
	bool InitBGGL(int idx);
	void DestroyBGGL(int idx);
	void DrawBG(int idx);
	
	void InitPaletteDataGL();
	void InitVRAMDataGL();
	void InitOAMAttributeDataGL();
	Gba &_system;
	uint_fast8_t _vcount;
	uint_fast16_t _hcount;

	// Drawing objects
	// Palette drawing
	bool _paletteInitialized[2];
	CounterPtr<ShaderProgram> _paletteShader;
	CounterPtr<Texture> _paletteData;
	CounterPtr<BufferObject> _paletteVertexData;
	VertexArrayObject *_paletteVao[2];
	// Tile drawing
	bool _tilesInitialized[3];
	CounterPtr<ShaderProgram> _tilesShader;
	CounterPtr<BufferTexture> _vramBT;
	CounterPtr<BufferObject> _tilesVertexData;
	VertexArrayObject *_tilesVao[3];
	// OAM drawing
	bool _oamInitialized;
	CounterPtr<ShaderProgram> _oamShader;
	CounterPtr<BufferTexture> _oamBT;
	CounterPtr<BufferObject> _oamVertexData;
	VertexArrayObject *_oamVao;
	// BG drawing
	bool _bgInitialized[4];
	CounterPtr<ShaderProgram> _bgShader;
	CounterPtr<BufferObject> _bgVertexData;
	VertexArrayObject *_bgVao[4];

};