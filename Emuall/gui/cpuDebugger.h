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

	Emulator mEmu;
	DisView *mCodeView;
	wxListCtrl *mRegisterView;
	std::vector<wxCheckBox *> mStatusWidgets;
	std::map<int, EmulatorRegister_t> mRegisters;
	std::map<wxCheckBox *, EmulatorRegister_t> mFlags;
	wxGridSizer* mStatusSizer;
	wxButton *mRunButton;
	wxButton *mStepButton;
	wxButton *mStepOverButton;
	wxButton *mStepOutButton;
};

#endif