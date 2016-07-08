#include <GL/glew.h>

#include <emuall/graphics/guiRenderer.h>
#include <emuall/graphics/graphicsException.h>
#include "../resources/resources.h"

void GuiRenderer::SetWindowSize(int width, int height)
{
	_TQprogram.Begin();
	_TQprogram.SetUniform("screenSize", width, height);
	_solidProgram.Begin();
	_solidProgram.SetUniform("screenSize", width, height);
	_solidProgram.End();
}

void GuiRenderer::SetTQSize(float width, float height)
{
	_TQprogram.SetUniform("size", width, height);
}

void GuiRenderer::SetTQColor(unsigned int color)
{
	SetTQColor((color & 0xFF) / 255.0f, ((color >> 8) & 0xFF) / 255.0f, 
		((color >> 16) & 0xFF) / 255.0f, ((color >> 24) & 0xFF) / 255.0f);
}

void GuiRenderer::SetTQColor(float r, float g, float b, float a)
{
	_TQprogram.SetUniform("color", r, g, b, a);
}

void GuiRenderer::SetTQUVs(BufferObject &object, int stride, int offset)
{
	_TQVao.BindBuffer(1, object, 2, VertexArrayObject::Float, false, stride, offset);
}

void GuiRenderer::SetTQTexture(Texture &texture)
{
	_TQprogram.SetUniform("tex0", 0, texture);
}

void GuiRenderer::SetTQPosition(float x, float y)
{
	_TQprogram.SetUniform("position", x, y);
}

void GuiRenderer::SetTQScale(float x, float y)
{
	_TQprogram.SetUniform("scale", x, y);
}

void GuiRenderer::TQBegin()
{
	_TQprogram.Begin();
	_TQVao.Begin();
}

void GuiRenderer::TQEnd()
{
	_TQVao.End();
	_TQprogram.End();
}

void GuiRenderer::DrawTQ()
{
	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
}

void GuiRenderer::SetSolidSize(float width, float height)
{
	_solidProgram.SetUniform("size", width, height);
}

void GuiRenderer::SetSolidColor(BufferObject &object, int stride, int offset)
{
	_solidVao.BindBuffer(1, object, 4, VertexArrayObject::Float, false, stride, offset);
}

void GuiRenderer::SetSolidVertices(BufferObject &object, int stride, int offset)
{
	_solidVao.BindBuffer(0, object, 2, VertexArrayObject::Float, false, stride, offset);
}

void GuiRenderer::SetSolidPosition(float x, float y)
{
	_solidProgram.SetUniform("position", x, y);
}

void GuiRenderer::SolidBegin()
{
	_solidProgram.Begin();
	_solidVao.Begin();
}

void GuiRenderer::SolidEnd()
{
	_solidVao.End();
	_solidProgram.End();
}

void GuiRenderer::DrawSolid(int numVertices)
{
	GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices));
}

GuiRenderer::GuiRenderer() :
	_TQVertices(BufferObject::Type::Array)
{
	static float verticesData[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	_TQVertices.BufferData(BufferObject::Usage::StaticDraw, sizeof(verticesData), verticesData);

	_TQVao.Begin();
	_TQVao.BindBuffer(0, _TQVertices, 2, VertexArrayObject::Float);
	_TQVao.End();

	_TQprogram.AddShader(ShaderProgram::Vertex, ShaderSource(resource_tq_vert_glsl, resource_tq_vert_glsl_len));
	_TQprogram.AddShader(ShaderProgram::Fragment, ShaderSource(resource_tq_frag_glsl, resource_tq_frag_glsl_len));
	_TQprogram.Link();

	_solidProgram.AddShader(ShaderProgram::Vertex, ShaderSource(resource_solid_vert_glsl, resource_solid_vert_glsl_len));
	_solidProgram.AddShader(ShaderProgram::Fragment, ShaderSource(resource_solid_frag_glsl, resource_solid_frag_glsl_len));
	_solidProgram.Link();
}

GuiRenderer::~GuiRenderer()
{

}
