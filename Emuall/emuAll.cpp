
#include <wx/wx.h>
#include "gui/MainFrame.h"
#include "emuAll.h"
#include "util/memDbg.h"
 
IMPLEMENT_APP(EmuAll);

bool EmuAll::OnInit()
{
	// Start the program
	_mainFrame = new MainFrame(_("EmuAll"), wxDefaultPosition, wxSize(400,300));
	_mainFrame->Show(true);
	SetTopWindow(_mainFrame);
	SetAppDisplayName(_("EmuAll"));
	return true;
}

int EmuAll::OnExit()
{
	// Application wide destructor code
	return 0;
}