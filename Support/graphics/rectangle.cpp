#include <emuall/graphics/rectangle.h>

GuiRectangle::GuiRectangle(int x, int y, int w, int h, unsigned int color) :
	_x((float)x), _y((float)y), _w((float)w), _h((float)h), _bo(BufferObject::Type::Array)
{
	_r = (color & 0xFF) / 255.0f;
	_g = ((color >> 8) & 0xFF) / 255.0f;
	_b = ((color >> 16) & 0xFF) / 255.0f;
	_a = ((color >> 24) & 0xFF) / 255.0f;
	float data[] = {
		// x, y, r, g, b, a
		0, 1, _r, _g, _b, _a,
		1, 1, _r, _g, _b, _a,
		0, 0, _r, _g, _b, _a,
		1, 0, _r, _g, _b, _a,
	};
	_bo.BufferData(BufferObject::Usage::StaticDraw, sizeof(data), data);
}

GuiRectangle::~GuiRectangle()
{

}

void GuiRectangle::SetSize(float w, float h)
{
	_w = w;
	_h = h;
}

void GuiRectangle::SetPos(float x, float y)
{
	_x = x;
	_y = y;
}

void GuiRectangle::SetColor(float r, float g, float b, float a)
{
	_r = r;
	_g = g;
	_b = b;
	_a = a;
	float data[] = {
		r, g, b, a
	};
	_bo.BufferSubData(2 * sizeof(float), sizeof(data), data);
	_bo.BufferSubData(8 * sizeof(float), sizeof(data), data);
	_bo.BufferSubData(14 * sizeof(float), sizeof(data), data);
	_bo.BufferSubData(20 * sizeof(float), sizeof(data), data);
}

void GuiRectangle::SetColor(unsigned int color)
{
	float r = (color & 0xFF) / 255.0f;
	float g = ((color >> 8) & 0xFF) / 255.0f;
	float b = ((color >> 16) & 0xFF) / 255.0f;
	float a = ((color >> 24) & 0xFF) / 255.0f;
	SetColor(r, g, b, a);
}

void GuiRectangle::Draw(GuiRenderer &renderer)
{
	renderer.SolidBegin();
	renderer.SetSolidColor(_bo, 6 * sizeof(float), 2 * sizeof(float));
	renderer.SetSolidVertices(_bo, 6 * sizeof(float), 0);
	renderer.SetSolidPosition(_x, _y);
	renderer.SetSolidSize(_w, _h);
	renderer.DrawSolid(4);
	renderer.SolidEnd();
}
