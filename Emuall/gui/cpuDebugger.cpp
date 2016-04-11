

#include "cpuDebugger.h"
#include "../util/memDbg.h"
#include "IdList.h"

IMPLEMENT_CLASS(CPUDebugger, wxFrame)
BEGIN_EVENT_TABLE(CPUDebugger, wxFrame)
EVT_CLOSE(CPUDebugger::OnClose)
EVT_BUTTON(ID_Debug_Cpu_run, CPUDebugger::OnRun)
EVT_BUTTON(ID_Debug_Cpu_step, CPUDebugger::OnStep)
END_EVENT_TABLE()


CPUDebugger::CPUDebugger(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) :
	wxFrame(parent, id, title, pos, size, style)
{
	_emu.emu = NULL;
	_emu.handle = NULL;

	// Initialize the window
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	// Code section
	_codeView = new DisView(&_emu, this);
	bSizer1->Add(_codeView, 7, wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	// Register view
	_registerView = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_NO_SORT_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL | wxHSCROLL);
	_registerView->AppendColumn("Register");
	_registerView->AppendColumn("Value", wxLIST_FORMAT_LEFT);
	bSizer2->Add(_registerView, 2, wxEXPAND, 5);

	// Status view
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Status")), wxVERTICAL);
	
	_statusSizer = new wxGridSizer(0, 2, 0, 0);
	sbSizer1->Add(_statusSizer, 1, wxEXPAND, 5);
	this->SetAutoLayout(true);
	bSizer2->Add(sbSizer1, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
	
	bSizer2->Add(0, 0, 1, wxEXPAND, 5);
	// Add buttons
	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer(0, 2, 0, 0);
	_runButton = new wxButton(this, ID_Debug_Cpu_run, _("Run"));
	gSizer4->Add(_runButton, 1, wxALIGN_CENTER, 5);
	_stepButton = new wxButton(this, ID_Debug_Cpu_step, _("Step"));
	gSizer4->Add(_stepButton, 1, wxALIGN_CENTER, 5);
	_stepOverButton = new wxButton(this, ID_Debug_Cpu_stepOver, _("Step over"));
	gSizer4->Add(_stepOverButton, 1, wxALIGN_CENTER, 5);
	_stepOutButton = new wxButton(this, ID_Debug_Cpu_stepOut, _("Step out"));
	gSizer4->Add(_stepOutButton, 1, wxALIGN_CENTER, 5);
	bSizer2->Add(gSizer4, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 5);


	bSizer1->Add(bSizer2, 0, wxEXPAND, 5);

	this->SetSizerAndFit(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	SetEmulator(_emu);
}

CPUDebugger::~CPUDebugger()
{

}

void CPUDebugger::Update()
{
	if (_emu.emu == NULL)
		return;
	if (_emu.emu->IsRunning(_emu.handle) == 1)
	{
		// Emulator running
		_runButton->SetLabel(_("Stop"));
		_stepButton->Disable();
		_registerView->Disable();
		std::vector<wxCheckBox *>::iterator it;
		for (it = _statusWidget.begin(); it != _statusWidget.end(); ++it)
		{
			(*it)->Disable();
		}

	}
	else
	{
		// Emulator not running
		// Update button
		_runButton->SetLabel(_("Run"));
		_stepButton->Enable();
		// Update registers
		
		std::map<int, EmulatorRegister_t>::iterator it;
		for (it = _registers.begin(); it != _registers.end(); ++it)
		{
			_registerView->SetItem(it->first, 1, wxString::Format(wxString::Format("0x%%0%dX", (it->second.size +3)/ 4),
				 _emu.emu->GetValU(_emu.handle, it->second.id)));
		}
		_registerView->Enable();

		std::map<wxCheckBox *, EmulatorRegister_t>::iterator it2;
		for (it2 = _flags.begin(); it2 != _flags.end(); ++it2)
		{
			it2->first->SetValue(_emu.emu->GetValU(_emu.handle, it2->second.id) != 0);
			it2->first->Enable();
		}

	}
	_codeView->FollowCurrentLine();
	_codeView->Refresh();
}

void CPUDebugger::SetEmulator(const Emulator &emu)
{
	_emu = emu;
	if (_emu.emu == NULL)
	{
		// Clear everything
		_registerView->DeleteAllItems();
		_registerView->Disable();
		std::vector<wxCheckBox *>::iterator it;
		for (it = _statusWidget.begin(); it != _statusWidget.end(); ++it)
		{
			(*it)->Destroy();
		}
		_statusWidget.clear();
		_registers.clear();
		_flags.clear();
		_codeView->Update(0, 0);
		_runButton->Enable(false);
		_stepButton->Enable(false);
		_stepOverButton->Enable(false);
		_stepOutButton->Enable(false);

	}
	else {
		// Load registers
		_registerView->DeleteAllItems();
		std::vector<wxCheckBox *>::iterator it;
		for (it = _statusWidget.begin(); it != _statusWidget.end(); ++it)
		{
			(*it)->Destroy();
		}
		_statusWidget.clear();
		_flags.clear();
		_registers.clear();

		// Get cpu debugging information
		CpuDebuggerInfo_t config = _emu.emu->GetCpuDebuggerInfo();

		_codeView->Update(config.disassemblerSize, config.curLineId);

		std::vector<EmulatorRegister_t>::iterator it2;
		for (it2 = config.registers.begin(); it2 != config.registers.end(); ++it2)
		{
			// add register
			long pos = _registerView->InsertItem(_registerView->GetItemCount(), it2->name);
			if (it2->size == 0)
				it2->size = 32;
			_registerView->SetItem(pos, 1, wxString::Format(wxString::Format("0x%%0%dX", (it2->size + 1) / 4), 0));
			_registers[pos] = *it2;
		}
		for (it2 = config.flags.begin(); it2 != config.flags.end(); ++it2)
		{
			// add flag
			wxCheckBox *flag = new wxCheckBox(this, wxID_ANY, it2->name, wxDefaultPosition, wxDefaultSize, 0);
			flag->SetValue(0);
			_statusSizer->Add(flag, 0, wxALL, 5);
			_statusWidget.push_back(flag);
			_flags[flag] = *it2;
		}
		_registerView->Enable();

		_runButton->Enable(true);
		_stepButton->Enable(config.step);
		_stepOverButton->Enable(config.stepOver);
		_stepOutButton->Enable(config.stepOut);
	}
	Fit();
	Layout();
	Update();
}

void CPUDebugger::OnClose(wxCloseEvent &evt)
{
	if (evt.CanVeto())
	{
		evt.Veto(true);
		Show(false);
		return;
	}
	Destroy();
}

void CPUDebugger::OnRun(wxCommandEvent &evt)
{
	if (_emu.emu == NULL)
		return;
	if (_emu.emu->IsRunning(_emu.handle) == 1)
	{
		// Stop
		_emu.emu->Run(_emu.handle, 0);
	}
	else if (_emu.emu->IsRunning(_emu.handle) == 0)
	{
		// Run
		_emu.emu->Run(_emu.handle, 1);
	}

	m_parent->Update();
}

void CPUDebugger::OnStep(wxCommandEvent &evt)
{
	if (_emu.emu == NULL)
		return;
	_emu.emu->Step(_emu.handle);
	m_parent->Update();
}