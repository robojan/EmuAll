
#include "DebuggerScreen.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerScreen::DebuggerScreen(Emulator *emu, const pugi::xml_node &node) :
	mEmu(emu), mWidget(NULL)
{
	wxASSERT(strcmp(node.name(), "screen") == 0);
	wxASSERT(emu != NULL);
	// Add screen
	mScreen.id = node.attribute("id").as_int(-1);
	mScreen.width = node.child("width").text().as_int(-1);
	mScreen.height = node.child("height").text().as_int(-1);
	mScreen.mouseX = node.child("mousex").text().as_int(-1);
	mScreen.mouseY = node.child("mousey").text().as_int(-1);
}

DebuggerScreen::~DebuggerScreen()
{

}

void DebuggerScreen::DrawGL(int user)
{
	if (mEmu->emu != NULL)
	{
		mEmu->emu->Draw(mEmu->handle, user);
	}
	else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

bool DebuggerScreen::InitGL(int user)
{
	if (mEmu->emu != NULL)
	{
		return mEmu->emu->InitGL(mEmu->handle, user);
	}
	return false;
}

void DebuggerScreen::DestroyGL(int user)
{
	if (mEmu->emu != NULL)
	{
		return mEmu->emu->DestroyGL(mEmu->handle, user);
	}
}

GLPane *DebuggerScreen::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	// Create screen
	int args [] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 32, 0 };

	mWidget = new GLPane(parent, this, mScreen.id, id, wxDefaultPosition,
		wxSize(mScreen.width, mScreen.height), wxFULL_REPAINT_ON_RESIZE, args);
	mWidget->Bind(wxEVT_MOTION, &DebuggerScreen::OnMotion, this);
	mWidget->SetMaxSize(wxSize(mScreen.width, mScreen.height));
	wxColour bgColour = parent->GetBackgroundColour();
	mWidget->SetClearColour(bgColour);

	UpdateInfo();
	return mWidget;
}

void DebuggerScreen::OnMotion(wxMouseEvent &evt)
{
	if (mEmu->emu == NULL)
		return;
	if (mScreen.mouseX != -1)
	{
		mEmu->emu->SetValI(mEmu->handle, mScreen.mouseX, evt.GetX());
	}
	if (mScreen.mouseY != -1)
	{
		mEmu->emu->SetValI(mEmu->handle, mScreen.mouseY, evt.GetY());
	}
		
}

void DebuggerScreen::UpdateInfo()
{
	if (mWidget == NULL || mEmu->emu == NULL)
		return;

	if (mWidget->IsShownOnScreen())
	{
		mWidget->Refresh();
	}
}