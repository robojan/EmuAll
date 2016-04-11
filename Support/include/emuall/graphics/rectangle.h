#ifndef RECTANGLE_H_
#define RECTANGLE_H_

#include <emuall/graphics/guiRenderer.h>
#include <emuall/common.h>

class DLLEXPORT GuiRectangle {
public:
	GuiRectangle(int x, int y, int w, int h, unsigned int color);
	virtual ~GuiRectangle();

	void SetSize(float w, float h);
	void SetPos(float x, float y);
	void SetColor(float r, float g, float b, float a);
	void SetColor(unsigned int color);

	void Draw(GuiRenderer &renderer);
private:
	float _x;
	float _y;
	float _w;
	float _h;
	float _r;
	float _g;
	float _b;
	float _a;

	BufferObject _bo;
};

#endif
