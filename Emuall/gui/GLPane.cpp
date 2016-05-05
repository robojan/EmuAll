
#include "GLPane.h"
#include "../util/memDbg.h"
#include <limits.h>
#include "../util/log.h"
#include <GL/glu.h>
#include "GLContextManager.h"

IMPLEMENT_CLASS(GLPane, wxGLCanvas);
BEGIN_EVENT_TABLE(GLPane, wxGLCanvas)
	EVT_PAINT(GLPane::Render)
END_EVENT_TABLE()

GLPane::GLPane(wxWindow *parent, GLPaneI *callback, int user, wxWindowID id, const wxPoint &pos, 
	const wxSize &size, long style, const wxGLAttributes &attr, const wxGLContextAttrs &ctxAttr, 
	int shareId) :
wxGLCanvas(parent, attr, id, pos, size, style), _user(user), _clearR(0.0), _clearG(0.0), 
_clearB(0.0), _clearA(1.0), _initialized(false), _GLAttr(attr), _CtxAttr(ctxAttr), _shareId(shareId)
{
	wxASSERT(callback != NULL);
	wxGLContext *shareContext = GLContextManager::GetSingleton().GetMasterContext(shareId);
	_context = new wxGLContext(this, shareContext, &_CtxAttr);
	if (!_context->IsOK()) {
		Log(Error, "Could not create openGL context");
	}
	if (shareId >= 0) {
		GLContextManager::GetSingleton().AddContext(shareId, _context);
	}
	
	_callback = callback;

}

GLPane::~GLPane()
{
	GLContextManager::GetSingleton().RemoveContext(_shareId, _context);
	DestroyGL();
	delete _context;
}

void GLPane::SetClearColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	SetClearColour(r / (float) UCHAR_MAX, g / (float) UCHAR_MAX, b / (float) UCHAR_MAX, a / (float) UCHAR_MAX);
}

void GLPane::SetClearColour(wxColour colour)
{
	SetClearColour(colour.Red(), colour.Green(), colour.Blue(), colour.Alpha());
}

void GLPane::SetClearColour(float r, float g, float b, float a)
{
	_clearR = r; _clearG = g; _clearB = b; _clearA = a; // set colour
	_clearColourChanged=true;
}

void GLPane::SetCurrentContext()
{
	SetCurrent(*_context);
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		Log(Warn, "Outstanding GL errors on context switch: %s", gluErrorString(error));
	}
}

void GLPane::InitGL()
{
	glClear(GL_COLOR_BUFFER_BIT);
	_initialized = _callback->InitGL(_user);
}

void GLPane::DestroyGL()
{
	if (IsShownOnScreen())
	{
		SetCurrent(*_context);
	}
	_callback->DestroyGL(_user);
	_initialized = false;
}

void GLPane::Render( wxPaintEvent &evt)
{
	if (!IsShownOnScreen())
		return;
	SetCurrentContext();
	wxPaintDC(this);

	if (_clearColourChanged)
	{
		glClearColor(_clearR, _clearG, _clearB, _clearA);
		_clearColourChanged = false;
	}

	if (!_initialized)
	{
		InitGL();
	}
	else {
		_callback->DrawGL(_user);
	}

	SwapBuffers();
}
