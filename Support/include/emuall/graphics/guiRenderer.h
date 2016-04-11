#ifndef GUIRENDERER_H_
#define GUIRENDERER_H_

#include <emuall/graphics/ShaderProgram.h>
#include <emuall/graphics/VertexArrayObject.h>
#include <emuall/graphics/BufferObject.h>

class DLLEXPORT GuiRenderer {
public:
	GuiRenderer();
	~GuiRenderer();

	void SetWindowSize(int width, int height);

	// Textured quad
	void SetTQSize(float width, float height);
	void SetTQColor(unsigned int color);
	void SetTQColor(float r, float g, float b, float a);
	void SetTQUVs(BufferObject &object, int stride = 0, int offset = 0);
	void SetTQTexture(Texture &texture);
	void SetTQPosition(float x, float y);
	void SetTQScale(float x, float y);
	void TQBegin();
	void TQEnd();
	void DrawTQ();

	// Solid colored object
	void SetSolidSize(float width, float height);
	void SetSolidColor(BufferObject &object, int stride = 0, int offset = 0);
	void SetSolidVertices(BufferObject &object, int stride = 0, int offset = 0);
	void SetSolidPosition(float x, float y);
	void SolidBegin();
	void SolidEnd();
	void DrawSolid(int numVertices);


private:
	ShaderProgram        _TQprogram;
	VertexArrayObject    _TQVao;
	BufferObject         _TQVertices;
	VertexArrayObject    _solidVao;
	ShaderProgram        _solidProgram;
};

#endif
