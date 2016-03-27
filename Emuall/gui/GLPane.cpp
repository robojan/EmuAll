
#include "GLPane.h"
#include "../util/memDbg.h"
#include <limits.h>
#include "../util/log.h"

IMPLEMENT_CLASS(GLPane, wxGLCanvas);
BEGIN_EVENT_TABLE(GLPane, wxGLCanvas)
	EVT_PAINT(GLPane::Render)
END_EVENT_TABLE()

GLPane::GLPane(wxWindow *parent, GLPaneI *callback, int user, wxWindowID id, const wxPoint &pos, 
	const wxSize &size, long style, const wxGLAttributes &attr, const wxGLContextAttrs &ctxAttr) :
wxGLCanvas(parent, attr, id, pos, size, style), _user(user), _clearR(0.0), _clearG(0.0), 
_clearB(0.0), _clearA(1.0), _initialized(false), _GLAttr(attr), _CtxAttr(ctxAttr)
{
	wxASSERT(callback != NULL);
	_context = new wxGLContext(this, NULL, &_CtxAttr);
	if (!_context->IsOK()) {
		Log(Error, "Could not create openGL context");
	}
	
	_callback = callback;

}

GLPane::~GLPane()
{
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
	SetCurrent(*_context);
	wxPaintDC(this);

	if (_clearColourChanged)
	{
		glClearColor(_clearR, _clearG, _clearB, _clearA);
		_clearColourChanged = false;
	}

	if (!_initialized)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		_initialized = _callback->InitGL(_user);
	}
	else {
		_callback->DrawGL(_user);
	}

	SwapBuffers();
}
