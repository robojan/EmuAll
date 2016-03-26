#ifndef _CPUDEBUGGER_H_
#define _CPUDEBUGGER_H_

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <list>
#include <map>
#include "../Emulator/Emulator.h"
#include "DisView.h"

class CPUDebugger : public wxFrame
{
	DECLARE_CLASS(CPUDebugger);
	DECLARE_EVENT_TABLE();
public:
	CPUDebugger(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
	~CPUDebugger();

	void SetEmulator(const Emulator &emu);
	void Update();
private: 
	void OnClose(wxCloseEvent &evt);
	void OnRun(wxCommandEvent &evt);
	void OnStep(wxCommandEvent &evt);

	Emulator _emu;
	DisView *_codeView;
	wxListCtrl *_registerView;
	std::vector<wxCheckBox *> _statusWidget;
	std::map<int, EmulatorRegister_t> _registers;
	std::map<wxCheckBox *, EmulatorRegister_t> _flags;
	wxGridSizer* _statusSizer;
	wxButton *_runButton;
	wxButton *_stepButton;
	wxButton *_stepOverButton;
	wxButton *_stepOutButton;
};

#endif