#include <GL/glew.h>

#include <emuall/graphics/guiRenderer.h>
#include <emuall/graphics/graphicsException.h>
#include "../resources/resources.h"

void GuiRenderer::SetWindowSize(int width, int height)
{
	program.Begin();
	program.SetUniform("screenSize", width, height);
	program.End();
}

GuiRenderer::GuiRenderer() :
	_vertices(BufferObject::Array)
{
	static float verticesData[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	_vertices.BufferData(BufferObject::StaticDraw, sizeof(verticesData), verticesData);

	vao.Begin();
	vao.BindBuffer(0, _vertices, 2, VertexArrayObject::Float);
	vao.End();

	if (!program.AddShader(ShaderProgram::Vertex, (const char *)resource_gui_vert_glsl, resource_gui_vert_glsl_len)) {
		throw GraphicsException(GL_INVALID_OPERATION, program.GetLog());
	}
	if (!program.AddShader(ShaderProgram::Fragment, (const char *)resource_gui_frag_glsl, resource_gui_frag_glsl_len)) {
		throw GraphicsException(GL_INVALID_OPERATION, program.GetLog());
	}
	if (!program.Link()) {
		throw GraphicsException(GL_INVALID_OPERATION, program.GetLog());
	}
}

GuiRenderer::~GuiRenderer()
{

}
