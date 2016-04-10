#ifndef EMULATORSCREEN_H_
#define EMULATORSCREEN_H_

#include "GLPane.h"
#include <emuall/graphics/ShaderProgram.h>
#include <emuall/graphics/VertexArrayObject.h>
#include <emuall/graphics/frameBuffer.h>
#include <emuall/graphics/guiRenderer.h>
#include <emuall/graphics/font.h>

class EmulatorScreen : public GLPane {
public:
	enum Filter {
		Nearest,
		BiLinear,
		BicubicTriangular,
		BicubicBell,
		BicubicBSpline,
		BicubicCatMullRom,
	};
	EmulatorScreen(wxWindow *parent, GLPaneI *callback, int user, wxWindowID id, const wxPoint &pos,
		const wxSize &size, long style, const wxGLAttributes &attr, const wxGLContextAttrs &ctxAttr);
	virtual ~EmulatorScreen();

	virtual void InitGL();
	virtual void DestroyGL();
	void SetFrameBufferSize(int width, int height);
	void SetPostProcessingFilter(Filter filter);

protected:
	virtual void Render(wxPaintEvent &evt);

private:
	void Create();

	bool _initialized;
	int _width;
	int _height;
	VertexArrayObject *_vao;
	BufferObject *_vbo;
	ShaderProgram _shader;
	FrameBuffer *_fbo;
	GuiRenderer *_guiRenderer;
	Font *_messageFont;
};

#endif