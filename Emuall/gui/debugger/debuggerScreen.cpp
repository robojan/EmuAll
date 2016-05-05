
#include "DebuggerScreen.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerScreen::DebuggerScreen(Emulator *emu, const pugi::xml_node &node) :
	_emu(emu), _widget(NULL)
{
	wxASSERT(strcmp(node.name(), "screen") == 0);
	wxASSERT(emu != NULL);
	// Add screen
	_screen.id = node.attribute("id").as_int(-1);
	_screen.shareId = node.attribute("contextShare").as_int(-1);
	_screen.width = node.child("width").text().as_int(-1);
	_screen.height = node.child("height").text().as_int(-1);
	_screen.mouseX = node.child("mousex").text().as_int(-1);
	_screen.mouseY = node.child("mousey").text().as_int(-1);
}

DebuggerScreen::~DebuggerScreen()
{

}

void DebuggerScreen::DrawGL(int user)
{
	if (_emu->emu != NULL)
	{
		_emu->emu->Draw(_emu->handle, user);
	}
	else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

bool DebuggerScreen::InitGL(int user)
{
	if (_emu->emu != NULL)
	{
		return _emu->emu->InitGL(_emu->handle, user);
	}
	return false;
}

void DebuggerScreen::DestroyGL(int user)
{
	if (_emu->emu != NULL)
	{
		return _emu->emu->DestroyGL(_emu->handle, user);
	}
}

GLPane *DebuggerScreen::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widget != NULL)
	{
		return _widget; // Widget already created
	}

	// Create screen
	wxGLAttributes glAttr;
	glAttr.PlatformDefaults();
	glAttr.DoubleBuffer();
	glAttr.RGBA();
	glAttr.Depth(24);
	glAttr.Stencil(8);
	glAttr.EndList();
	wxGLContextAttrs ctxAttr;
	ctxAttr.CoreProfile();
	ctxAttr.OGLVersion(3, 2);
	ctxAttr.ForwardCompatible();
	ctxAttr.EndList();
	_widget = new GLPane(parent, this, _screen.id, id, wxDefaultPosition,
		wxSize(_screen.width, _screen.height), wxFULL_REPAINT_ON_RESIZE, glAttr, ctxAttr,
		_screen.shareId);
	_widget->Bind(wxEVT_MOTION, &DebuggerScreen::OnMotion, this);
	_widget->SetMaxSize(wxSize(_screen.width, _screen.height));
	wxColour bgColour = parent->GetBackgroundColour();
	_widget->SetClearColour(bgColour);

	UpdateInfo();
	return _widget;
}

void DebuggerScreen::OnMotion(wxMouseEvent &evt)
{
	if (_emu->emu == NULL)
		return;
	if (_screen.mouseX != -1)
	{
		_emu->emu->SetValI(_emu->handle, _screen.mouseX, evt.GetX());
	}
	if (_screen.mouseY != -1)
	{
		_emu->emu->SetValI(_emu->handle, _screen.mouseY, evt.GetY());
	}
		
}

void DebuggerScreen::UpdateInfo()
{
	if (_widget == NULL || _emu->emu == NULL)
		return;

	if (_widget->IsShownOnScreen())
	{
		_widget->Refresh();
	}
}