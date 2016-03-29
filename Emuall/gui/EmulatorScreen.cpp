
#include <GL/glew.h>
#include "EmulatorScreen.h"
#include "../resources/resources.h"
#include "../util/log.h"
#include "../util/Options.h"

static GLfloat fboData[] = {
	// X    Y     Z      U    V
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

EmulatorScreen::EmulatorScreen(wxWindow *parent, GLPaneI *callback, int user, wxWindowID id, const wxPoint &pos,
	const wxSize &size, long style, const wxGLAttributes &attr, const wxGLContextAttrs &ctxAttr) :
	GLPane(parent, callback, user, id, pos, size, style, attr, ctxAttr), _fbo(nullptr), _initialized(false)
{
}

EmulatorScreen::~EmulatorScreen()
{
	if (_fbo != nullptr) {
		delete _fbo;
	}
}

void EmulatorScreen::InitGL()
{
	if (!_initialized) {

		// Create vao
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// Generate vertex buffer
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fboData), fboData, GL_STATIC_DRAW);

		// load shader
		if (!_shader.AddShader(ShaderProgram::Vertex, (char *)resource_post_vert_glsl, sizeof(resource_post_vert_glsl)))
		{
			Log(Error, "%s", _shader.GetLog());
			return;
		}
		if (!_shader.AddShader(ShaderProgram::Fragment, (char *)resource_post_frag_glsl, sizeof(resource_post_frag_glsl))) {
			Log(Error, "%s", _shader.GetLog());
			return;
		}
		if (!_shader.Link()) {
			Log(Error, "%s", _shader.GetLog());
			return;
		}
		_initialized = true;
	}
	if (!GLPane::_initialized) {
		GLPane::InitGL();
	}
}

void EmulatorScreen::DestroyGL()
{
	GLPane::DestroyGL();
}

void EmulatorScreen::SetFrameBufferSize(int width, int height)
{
	_width = width;
	_height = height;
	if (_fbo != nullptr) {
		delete _fbo;
		_fbo = nullptr;
	}
	_fbo = new FrameBuffer(width, height);
	_fbo->AttachColorBuffer(0);
	_fbo->AttachDepthBuffer();
}

void EmulatorScreen::Render(wxPaintEvent &evt)
{
	if (!IsShownOnScreen())
		return;
	SetCurrentContext();
	wxPaintDC(this);

	glClear(GL_COLOR_BUFFER_BIT);

	if (_fbo == nullptr) {
		wxSize size = GetSize();
		SetFrameBufferSize(size.GetWidth(), size.GetHeight());
		SwapBuffers();
		return;
	}

	_fbo->Begin();
	if (_clearColourChanged)
	{
		glClearColor(_clearR, _clearG, _clearB, _clearA);
		_clearColourChanged = false;
	}

	InitGL();
	if (GLPane::_initialized) {
		_callback->DrawGL(_user);
	}
	else {
		glClear(GL_COLOR_BUFFER_BIT);
	}
	_fbo->End();

	// Setup viewport
	wxSize size = GetClientSize();
	if (Options::GetInstance().videoOptions.keepAspect)
	{
		if (((float)size.GetWidth()) / size.GetHeight() > ((float)_width) / _height)
		{
			int newWidth = static_cast<int>((((float)_width) / _height)*size.GetHeight());
			glViewport((size.GetWidth() - newWidth) / 2, 0, newWidth, size.GetHeight());
		}
		else {
			int newHeight = static_cast<int>((((float)_height) / _width)*size.GetWidth());
			glViewport(0, (size.GetHeight() - newHeight) / 2, size.GetWidth(), newHeight);
		}
	}
	else {
		glViewport(0, 0, size.GetWidth(), size.GetHeight());
	}

	// Render the framebuffer
	// Enable shader and load buffers
	_shader.Begin();
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 *  sizeof(GLfloat)));
	_fbo->GetColorBuffer(0).Begin();

	// Draw the screen
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	_fbo->GetColorBuffer(0).End();

	// Unload the buffers
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	_shader.End();

	SwapBuffers();
}
