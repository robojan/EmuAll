#ifndef _GLPANE_H
#define _GLPANE_H

#include <wx/wx.h>
#include <wx/glcanvas.h>

class GLPaneI
{
public:
	virtual bool InitGL(int user) = 0;
	virtual void DrawGL(int user) = 0;
	virtual void DestroyGL(int user) = 0;
};

class GLPane : public wxGLCanvas
{
	DECLARE_CLASS(GLPane);
	DECLARE_EVENT_TABLE();
public:
	GLPane(wxWindow *parent, GLPaneI *callback, int user, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, int *args);
	~GLPane();

	void SetUserData(int user) { mUser = user; }
	int GetUserData() const { return mUser; }

	void DestroyGL();

	void SetClearColour(float r, float g, float b, float a);
	void SetClearColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void SetClearColour(wxColour colour);

private:
	void Render(wxPaintEvent &evt);
	wxGLContext	*mContext;
	GLPaneI		*mCallback;
	int			mUser;
	bool		mInitialized;
	float		mClearR, mClearG, mClearB, mClearA;
	bool		mClearColourChanged;
};

#endif