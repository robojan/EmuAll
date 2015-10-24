
#include "GLPane.h"
#include "../util/memDbg.h"
#include <GL/GLU.h>
#include <limits.h>

IMPLEMENT_CLASS(GLPane, wxGLCanvas);
BEGIN_EVENT_TABLE(GLPane, wxGLCanvas)
	EVT_PAINT(GLPane::Render)
END_EVENT_TABLE()

GLPane::GLPane(wxWindow *parent, GLPaneI *callback, int user, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, int *args) :
wxGLCanvas(parent, id, args, pos, size, style), mUser(user), mClearR(0.0), mClearG(0.0), mClearB(0.0), mClearA(1.0), mInitialized(false)
{
	wxASSERT(callback != NULL);
	mContext = new wxGLContext(this);
	mCallback = callback;
}

GLPane::~GLPane()
{
	DestroyGL();
	delete mContext;
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
	mClearR = r; mClearG = g; mClearB = b; mClearA = a; // set colour
	mClearColourChanged=true;
}

void GLPane::DestroyGL()
{
	if (IsShownOnScreen())
	{
		wxGLCanvas::SetCurrent(*mContext);
	}
	mCallback->DestroyGL(mUser);
	mInitialized = false;
}

void GLPane::Render( wxPaintEvent &evt)
{
	if (!IsShownOnScreen())
		return;
	wxGLCanvas::SetCurrent(*mContext);
	wxPaintDC(this);


	if (mClearColourChanged)
	{
		glClearColor(mClearR, mClearG, mClearB, mClearA);
		mClearColourChanged = false;
	}

	if (!mInitialized )
	{
		glClear(GL_COLOR_BUFFER_BIT);
		mInitialized = mCallback->InitGL(mUser);;
	}
	else
	{
		mCallback->DrawGL(mUser);
	}

	SwapBuffers();
}
