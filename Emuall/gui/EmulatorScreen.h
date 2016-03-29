#ifndef EMULATORSCREEN_H_
#define EMULATORSCREEN_H_

#include "GLPane.h"
#include <emuall/graphics/ShaderProgram.h>
#include <emuall/graphics/frameBuffer.h>

class EmulatorScreen : public GLPane {
public:
	EmulatorScreen(wxWindow *parent, GLPaneI *callback, int user, wxWindowID id, const wxPoint &pos,
		const wxSize &size, long style, const wxGLAttributes &attr, const wxGLContextAttrs &ctxAttr);
	virtual ~EmulatorScreen();

	virtual void InitGL();
	virtual void DestroyGL();
	void SetFrameBufferSize(int width, int height);

protected:
	virtual void Render(wxPaintEvent &evt);

private:
	void Create();

	bool _initialized;
	unsigned int _vao;
	unsigned int _vbo;
	int _width;
	int _height;
	ShaderProgram _shader;
	FrameBuffer *_fbo;
};

#endif