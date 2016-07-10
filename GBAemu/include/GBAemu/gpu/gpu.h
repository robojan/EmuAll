#pragma once
#include <memory>

#include <GBAemu/defines.h>
#include <GBAemu/memory/memory.h>
#include <emuall/graphics/ShaderProgram.h>
#include <emuall/graphics/BufferObject.h>
#include <emuall/graphics/texture.h>
#include <emuall/graphics/texture3D.h>
#include <emuall/graphics/bufferTexture.h>
#include <emuall/graphics/VertexArrayObject.h>
#include <emuall/graphics/frameBuffer.h>
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

	void VBlank();
	void HBlank();

	int GetModeBuffer(int lineNr);
	void GetBGSizeBuffer(int lineNr, int background, int &width, int &height);
	void GetBGOffsetBuffer(int lineNr, int background, float &x, float &y);

	// Main drawing
	void SetupShaderInterface(ShaderProgram &program);
	bool InitMainGL();
	void InitMainBGGL();
	void InitMainObjGL();
	void InitMainMixGL();
	void DestroyMainGL();
	void DestroyMainBGGL();
	void DestroyMainObjGL();
	void DestroyMainMixGL();
	void DrawMain();
	void DrawMainBGGL(int background);
	void DrawMainObjGL();
	void DrawMainMix();

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

	void InitGridShader();
	void DestroyGridShader();
	void DrawGrid(bool gridEnabled, int gridWidth, int gridHeight, int gridColor, float gridDepth,
		bool boxEnabled, bool boxWraparound, float boxX, float boxY, float boxWidth, float boxHeight, int boxColor, float boxDepth);
	
	void InitPaletteDataGL();
	void InitVRAMDataGL();
	void InitOAMDataGL();
	void InitRegistersDataGL();

	Gba &_system;
	uint_fast8_t _vcount;
	uint_fast16_t _hcount;

	// Drawing objects
	typedef struct {
		int dispcnt;
		int pad0[3];
		int bgCnt[4];
		struct {
			int hOfs;
			int vOfs;
			int pad0[2];
		} bgOfs[4];
		struct {
			float x;
			float y;
			int pad0[2];
		} bgRefPoint[2];
		struct {
			float a;
			float b;
			float c;
			float d; 
		} bgAffineMatrix[2];
		struct {
			int x1;
			int y1;
			int x2;
			int y2;
		} windim[2];
		struct {
			int winin;
			int winout;
			int pad0[2];
		} winCnt;
		struct {
			int bldcnt;
			int bldalpha;
			int bldy;
			int pad0;
		} effects;
	} DrawingInfoData_t;
	// Main drawing
	bool _mainInitialized;
	// Main Mix drawing
	CounterPtr<ShaderProgram> _mainMixShader;
	CounterPtr<BufferObject> _mainMixVertexData;
	VertexArrayObject *_mainMixVao;
	CounterPtr<ShaderProgram> _mainBGShader;
	CounterPtr<BufferObject> _mainBGVertexData;
	VertexArrayObject *_mainBGVao;
	CounterPtr<ShaderProgram> _mainObjShader;
	CounterPtr<BufferObject> _mainObjVertexData;
	VertexArrayObject *_mainObjVao;

	CounterPtr<BufferTexture> _mainVramBT;
	CounterPtr<BufferTexture> _mainOamBT;
	CounterPtr<BufferObject> _mainRegistersBO;
	CounterPtr<Texture3D> _mainPaletteData;
	CounterPtr<FrameBuffer> _mainFrameBuffer[6];
	DrawingInfoData_t _mainDrawingRegisters[SCREEN_HEIGHT];

	// Palette drawing
	bool _paletteInitialized[2];
	CounterPtr<ShaderProgram> _paletteShader;
	CounterPtr<BufferObject> _paletteVertexData;
	VertexArrayObject *_paletteVao[2];
	// Tile drawing
	bool _tilesInitialized[3];
	CounterPtr<ShaderProgram> _tilesShader;
	CounterPtr<BufferObject> _tilesVertexData;
	VertexArrayObject *_tilesVao[3];
	// OAM drawing
	bool _oamInitialized;
	CounterPtr<ShaderProgram> _oamShader;
	CounterPtr<BufferObject> _oamVertexData;
	VertexArrayObject *_oamVao;
	// BG drawing
	bool _bgInitialized[4];
	CounterPtr<ShaderProgram> _bgShader;
	VertexArrayObject *_bgVao[4];

	CounterPtr<ShaderProgram> _gridShader;

};