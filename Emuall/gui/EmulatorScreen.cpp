
#include <GL/glew.h>
#include "EmulatorScreen.h"
#include "../resources/resources.h"
#include "../util/log.h"
#include "../util/Options.h"
#include <emuall/graphics/graphicsException.h>
#include <emuall/graphics/texture.h>
#include <emuall/graphics/font.h>

static GLfloat fboData[] = {
	// X    Y     Z      U    V
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

EmulatorScreen::EmulatorScreen(wxWindow *parent, GLPaneI *callback, int user, wxWindowID id, const wxPoint &pos,
	const wxSize &size, long style, const wxGLAttributes &attr, const wxGLContextAttrs &ctxAttr) :
	GLPane(parent, callback, user, id, pos, size, style, attr, ctxAttr), 
	_fbo(nullptr), _vbo(nullptr), _vao(nullptr), _initialized(false), _guiRenderer(nullptr), _messageFont(nullptr)
{
}

EmulatorScreen::~EmulatorScreen()
{
	if (_vao != nullptr) {
		delete _vao;
	}
	if (_vbo != nullptr) {
		delete _vbo;
	}
	if (_fbo != nullptr) {
		delete _fbo;
	}
	if (_guiRenderer != nullptr) {
		delete _guiRenderer;
	}
	if (_messageFont != nullptr) {
		delete _messageFont;
	}
}

void EmulatorScreen::InitGL()
{
	if (!_initialized) {
		Options &options = Options::GetSingleton();
		// Create vao
		if (_vao == nullptr) {
			_vao = new VertexArrayObject();
		}
		if (_vbo == nullptr) {
			_vbo = new BufferObject(BufferObject::Array);
		}
		if (_guiRenderer == nullptr) {
			_guiRenderer = new GuiRenderer();
		}
		if (_messageFont == nullptr) {
			_messageFont = new Font(options.videoOptions.messageFont,
				options.videoOptions.messageFontIdx, options.videoOptions.messageFontSize);
		}
		_vao->Begin();
		_vbo->BufferData(BufferObject::StaticDraw, sizeof(fboData), fboData);
		_vao->BindBuffer(0, *_vbo, 3, VertexArrayObject::Float, false, 5 * sizeof(float), 0);
		_vao->BindBuffer(1, *_vbo, 2, VertexArrayObject::Float, false, 5 * sizeof(float), 3 * sizeof(float));
		_vao->End();

		SetPostProcessingFilter((EmulatorScreen::Filter)options.videoOptions.filter);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		filterSrcLen = resource_nearest_frag_glsl_len;
		break;
	case BiLinear:
		filterSrc = (char *)resource_bilinear_frag_glsl;
		filterSrcLen = resource_bilinear_frag_glsl_len;
		break;
	case BicubicTriangular:
		filterSrc = (char *)resource_bicubic_triangular_frag_glsl;
		filterSrcLen = resource_bicubic_triangular_frag_glsl_len;
		break;
	case BicubicBell:
		filterSrc = (char *)resource_bicubic_bell_frag_glsl;
		filterSrcLen = resource_bicubic_bell_frag_glsl_len;
		break;
	case BicubicBSpline:
		filterSrc = (char *)resource_bicubic_bspline_frag_glsl;
		filterSrcLen = resource_bicubic_bspline_frag_glsl_len;
		break;
	case BicubicCatMullRom:
		filterSrc = (char *)resource_bicubic_catmull_frag_glsl;
		filterSrcLen = resource_bicubic_catmull_frag_glsl_len;
		break;
	}
	Options::GetSingleton().videoOptions.filter = (int)filter;
	if (_shader.IsLinked()) {
		_shader.Clean();
	}
	// load shader
	if (!_shader.AddShader(ShaderProgram::Vertex, (char *)resource_post_vert_glsl, resource_post_vert_glsl_len))
	{
		Log(Error, "%s", _shader.GetLog());
		return;
	}
	if (!_shader.AddShader(ShaderProgram::Fragment, (char *)resource_post_frag_glsl, resource_post_frag_glsl_len)) {
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
		if (Options::GetSingleton().videoOptions.keepAspect)
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
		_vao->Begin();
		Texture &frame = _fbo->GetColorBuffer(0);
		_shader.SetUniform("textureSampler", 0, frame);

		// Draw the screen
		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

		frame.End();
		_vao->End();
		_shader.End();

		GL_CHECKED(glViewport(0, 0, size.GetWidth(), size.GetHeight()));
		_guiRenderer->SetWindowSize(size.GetWidth(), size.GetHeight());
		_messageFont->DrawText(*_guiRenderer, 10, size.GetHeight() - 10, "The Quick brown fox jumped over the lazy dog\n1234567890!@#$%^&*()-=[]{}_+;:'\"\\|,./<>?",
			0xFF0000FF, 1.0f, 200, Alignment::Left, 200, Alignment::Bottom);
	}
	catch (GraphicsException & e) {
		Log(Error, "GraphicsException caught: %s\nStacktrace:\n%s", e.GetMsg(), e.GetStacktrace());
	}
	catch (FontException & e) {
		Log(Error, "FontException caught: %s\nStacktrace:\n%s", e.GetMsg(), e.GetStacktrace());
	}

	SwapBuffers();
}
