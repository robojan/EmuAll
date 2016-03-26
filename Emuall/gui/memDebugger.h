#ifndef _MEMDEBUGGER_H_
#define _MEMDEBUGGER_H_

#include <wx/wx.h>
#include <map>
#include <wx/listctrl.h>
#include "../Emulator/Emulator.h"
#include "HexView.h"

class MemDebugger : public wxFrame, public HexViewI
{
	DECLARE_CLASS(MEMDebugger);
	DECLARE_EVENT_TABLE();
public:
	MemDebugger(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
	~MemDebugger();

	void SetEmulator(const Emulator &emu);
	void Update();
private:
	void OnGoto(wxCommandEvent &evt);
	void OnClose(wxCloseEvent &evt);
	void OnMemoryChange(wxCommandEvent &evt);
	wxByte OnGetValue(int address);
	void OnSetValue(int address, wxByte val);


	Emulator _emu;
	HexView *_hexView;
	wxChoice *_picker;
	std::map<int, EmulatorMemory_t> _memories;
	std::map<int, EmulatorValue_t> _infos;
	bool _readOnly;
	wxListCtrl *_memInfo;

};

#endif