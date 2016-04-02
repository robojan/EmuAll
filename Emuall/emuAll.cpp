
#include <wx/wx.h>
#include <wx/evtloop.h>
#include <wx/scopedptr.h>
#include "gui/MainFrame.h"
#include "emuAll.h"
#include "util/memDbg.h"
 
#include <emuall/exception.h>

IMPLEMENT_APP(EmuAll);



bool EmuAll::OnInit()
{
	// Start the program
	_mainFrame = new MainFrame(_("EmuAll"), wxDefaultPosition, wxSize(400,300));
	_mainFrame->Show(true);
	SetTopWindow(_mainFrame);
	SetAppDisplayName(_("EmuAll"));

	// Set timer resolution
#ifdef _WIN32
	timeBeginPeriod(1000);
#endif
	
	return true;
}

int EmuAll::OnExit()
{
	// Application wide destructor code
#ifdef _WIN32
	timeEndPeriod(1000);
#endif
	return 0;
}

