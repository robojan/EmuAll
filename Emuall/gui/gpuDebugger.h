#ifndef _GPUDEBUGGER_H
#define _GPUDEBUGGER_H

#include <wx/wx.h>
#include "../Emulator/Emulator.h"
#include "debugger\DebuggerRoot.h"

class GPUDebugger : public wxFrame
{
	DECLARE_CLASS(GPUDebugger);
	DECLARE_EVENT_TABLE();
public:
	GPUDebugger(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
	~GPUDebugger();

	void SetEmulator(const Emulator &emu);
	void Update();
private:
	void OnClose(wxCloseEvent &evt);
	void OnTimer(wxTimerEvent &evt);

	Emulator mEmu;
	Debugger::DebuggerRoot *mRoot;
	wxTimer *mTimer;
};

#endif