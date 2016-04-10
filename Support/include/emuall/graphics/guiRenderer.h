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

	ShaderProgram                 program;
	VertexArrayObject             vao;

private:
	BufferObject         _vertices;
};

#endif
