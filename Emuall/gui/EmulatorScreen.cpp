
#include <GL/glew.h>
#include "EmulatorScreen.h"
#include "../resources/resources.h"
#include "../util/log.h"
#include "../util/Options.h"
#include <emuall/graphics/graphicsException.h>

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
		GL_CHECKED(glGenVertexArrays(1, &_vao));
		GL_CHECKED(glBindVertexArray(_vao));

		// Generate vertex buffer
		GL_CHECKED(glGenBuffers(1, &_vbo));
		GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
		GL_CHECKED(glBufferData(GL_ARRAY_BUFFER, sizeof(fboData), fboData, GL_STATIC_DRAW));

		SetPostProcessingFilter((EmulatorScreen::Filter)Options::GetInstance().videoOptions.filter);
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
	_fbo->GetColorBuffer(0).SetWrap(Texture::ClampToEdge, Texture::ClampToEdge);

}

void EmulatorScreen::SetPostProcessingFilter(Filter filter)
{
	char *filterSrc;
	int filterSrcLen;
	switch (filter) {
	default:
		filter = Nearest;
	case Nearest:
		filterSrc = (char *)resource_nearest_frag_glsl;
		filterSrcLen = sizeof(resource_nearest_frag_glsl);
		break;
	case BiLinear:
		filterSrc = (char *)resource_bilinear_frag_glsl;
		filterSrcLen = sizeof(resource_bilinear_frag_glsl);
		break;
	case BicubicTriangular:
		filterSrc = (char *)resource_bicubic_triangular_frag_glsl;
		filterSrcLen = sizeof(resource_bicubic_triangular_frag_glsl);
		break;
	case BicubicBell:
		filterSrc = (char *)resource_bicubic_bell_frag_glsl;
		filterSrcLen = sizeof(resource_bicubic_bell_frag_glsl);
		break;
	case BicubicBSpline:
		filterSrc = (char *)resource_bicubic_bspline_frag_glsl;
		filterSrcLen = sizeof(resource_bicubic_bspline_frag_glsl);
		break;
	case BicubicCatMullRom:
		filterSrc = (char *)resource_bicubic_catmull_frag_glsl;
		filterSrcLen = sizeof(resource_bicubic_catmull_frag_glsl);
		break;
	}
	Options::GetInstance().videoOptions.filter = (int)filter;
	if (_shader.IsLinked()) {
		_shader.Clean();
	}
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
	if (!_shader.AddShader(ShaderProgram::Fragment, filterSrc, filterSrcLen)) {
		Log(Error, "%s", _shader.GetLog());
		return;
	}
	if (!_shader.Link()) {
		Log(Error, "%s", _shader.GetLog());
		return;
	}

}

void EmulatorScreen::Render(wxPaintEvent &evt)
{
	if (!IsShownOnScreen())
		return;
	SetCurrentContext();
	wxPaintDC(this);


	try {
		GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));

		if (_fbo == nullptr) {
			wxSize size = GetSize();
			SetFrameBufferSize(size.GetWidth(), size.GetHeight());
			SwapBuffers();
			return;
		}

		_fbo->Begin();
		if (_clearColourChanged)
		{
			GL_CHECKED(glClearColor(_clearR, _clearG, _clearB, _clearA));
			_clearColourChanged = false;
		}
		InitGL();
		if (GLPane::_initialized) {
			_callback->DrawGL(_user);
		}
		else {
			GL_CHECKED(glClear(GL_COLOR_BUFFER_BIT));
		}
		_fbo->End();

		// Setup viewport
		wxSize size = GetClientSize();
		if (Options::GetInstance().videoOptions.keepAspect)
		{
			if (((float)size.GetWidth()) / size.GetHeight() > ((float)_width) / _height)
			{
				int newWidth = static_cast<int>((((float)_width) / _height)*size.GetHeight());
				GL_CHECKED(glViewport((size.GetWidth() - newWidth) / 2, 0, newWidth, size.GetHeight()));
			}
			else {
				int newHeight = static_cast<int>((((float)_height) / _width)*size.GetWidth());
				GL_CHECKED(glViewport(0, (size.GetHeight() - newHeight) / 2, size.GetWidth(), newHeight));
			}
		}
		else {
			GL_CHECKED(glViewport(0, 0, size.GetWidth(), size.GetHeight()));
		}

		// Render the framebuffer
		// Enable shader and load buffers
		_shader.Begin();
		GL_CHECKED(glEnableVertexAttribArray(0));
		GL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
		GL_CHECKED(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0));
		GL_CHECKED(glEnableVertexAttribArray(1));
		GL_CHECKED(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat))));
		Texture &frame = _fbo->GetColorBuffer(0);
		_shader.SetUniform("textureSampler", 0, frame);

		// Draw the screen
		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

		// Unload the buffers
		GL_CHECKED(glDisableVertexAttribArray(0));
		GL_CHECKED(glDisableVertexAttribArray(1));
		frame.End();
		_shader.End();
	}
	catch (GraphicsException & e) {
		Log(Error, "GraphicsException caught: %s\nStacktrace:\n%s", e.GetMsg(), e.GetStacktrace());
	}

	SwapBuffers();
}
