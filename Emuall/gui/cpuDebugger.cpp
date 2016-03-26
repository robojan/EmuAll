

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
	mEmu.emu = NULL;
	mEmu.handle = NULL;

	// Initialize the window
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	// Code section
	mCodeView = new DisView(&mEmu, this);
	bSizer1->Add(mCodeView, 7, wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	// Register view
	mRegisterView = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_NO_SORT_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL | wxHSCROLL);
	mRegisterView->AppendColumn("Register");
	mRegisterView->AppendColumn("Value", wxLIST_FORMAT_LEFT);
	bSizer2->Add(mRegisterView, 2, wxEXPAND, 5);

	// Status view
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Status")), wxVERTICAL);
	
	mStatusSizer = new wxGridSizer(0, 2, 0, 0);
	sbSizer1->Add(mStatusSizer, 1, wxEXPAND, 5);
	this->SetAutoLayout(true);
	bSizer2->Add(sbSizer1, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
	
	bSizer2->Add(0, 0, 1, wxEXPAND, 5);
	// Add buttons
	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer(0, 2, 0, 0);
	mRunButton = new wxButton(this, ID_Debug_Cpu_run, _("Run"));
	gSizer4->Add(mRunButton, 1, wxALIGN_CENTER, 5);
	mStepButton = new wxButton(this, ID_Debug_Cpu_step, _("Step"));
	gSizer4->Add(mStepButton, 1, wxALIGN_CENTER, 5);
	mStepOverButton = new wxButton(this, ID_Debug_Cpu_stepOver, _("Step over"));
	gSizer4->Add(mStepOverButton, 1, wxALIGN_CENTER, 5);
	mStepOutButton = new wxButton(this, ID_Debug_Cpu_stepOut, _("Step out"));
	gSizer4->Add(mStepOutButton, 1, wxALIGN_CENTER, 5);
	bSizer2->Add(gSizer4, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 5);


	bSizer1->Add(bSizer2, 0, wxEXPAND, 5);

	this->SetSizerAndFit(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	SetEmulator(mEmu);
}

CPUDebugger::~CPUDebugger()
{

}

void CPUDebugger::Update()
{
	if (mEmu.emu == NULL)
		return;
	if (mEmu.emu->IsRunning(mEmu.handle) == 1)
	{
		// Emulator running
		mRunButton->SetLabel(_("Stop"));
		mStepButton->Disable();
		mRegisterView->Disable();
		std::vector<wxCheckBox *>::iterator it;
		for (it = mStatusWidgets.begin(); it != mStatusWidgets.end(); ++it)
		{
			(*it)->Disable();
		}

	}
	else if (mEmu.emu->IsRunning(mEmu.handle) == 0)
	{
		// Emulator not running
		// Update button
		mRunButton->SetLabel(_("Run"));
		mStepButton->Enable();
		// Update registers
		
		std::map<int, EmulatorRegister_t>::iterator it;
		for (it = mRegisters.begin(); it != mRegisters.end(); ++it)
		{
			mRegisterView->SetItem(it->first, 1, wxString::Format(wxString::Format("0x%%0%dX", (it->second.size +3)/ 4),
				 mEmu.emu->GetValU(mEmu.handle, it->second.id)));
		}
		mRegisterView->Enable();

		std::map<wxCheckBox *, EmulatorRegister_t>::iterator it2;
		for (it2 = mFlags.begin(); it2 != mFlags.end(); ++it2)
		{
			it2->first->SetValue(mEmu.emu->GetValU(mEmu.handle, it2->second.id) != 0);
			it2->first->Enable();
		}

	}
	mCodeView->FollowCurrentLine();
	mCodeView->Refresh();
}

void CPUDebugger::SetEmulator(const Emulator &emu)
{
	mEmu = emu;
	if (mEmu.emu == NULL)
	{
		// Clear everything
		mRegisterView->DeleteAllItems();
		mRegisterView->Disable();
		std::vector<wxCheckBox *>::iterator it;
		for (it = mStatusWidgets.begin(); it != mStatusWidgets.end(); ++it)
		{
			(*it)->Destroy();
		}
		mStatusWidgets.clear();
		mRegisters.clear();
		mFlags.clear();
		mCodeView->Update(0, 0);
		mRunButton->Enable(false);
		mStepButton->Enable(false);
		mStepOverButton->Enable(false);
		mStepOutButton->Enable(false);

	}
	else {
		// Load registers
		mRegisterView->DeleteAllItems();
		std::vector<wxCheckBox *>::iterator it;
		for (it = mStatusWidgets.begin(); it != mStatusWidgets.end(); ++it)
		{
			(*it)->Destroy();
		}
		mStatusWidgets.clear();
		mFlags.clear();
		mRegisters.clear();

		// Get cpu debugging information
		CpuDebuggerInfo_t config = mEmu.emu->GetCpuDebuggerInfo();

		mCodeView->Update(config.disassemblerSize, config.curLineId);

		std::vector<EmulatorRegister_t>::iterator it2;
		for (it2 = config.registers.begin(); it2 != config.registers.end(); ++it2)
		{
			// add register
			long pos = mRegisterView->InsertItem(mRegisterView->GetItemCount(), it2->name);
			if (it2->size == 0)
				it2->size = 32;
			mRegisterView->SetItem(pos, 1, wxString::Format(wxString::Format("0x%%0%dX", (it2->size + 1) / 4), 0));
			mRegisters[pos] = *it2;
		}
		for (it2 = config.flags.begin(); it2 != config.flags.end(); ++it2)
		{
			// add flag
			wxCheckBox *flag = new wxCheckBox(this, wxID_ANY, it2->name, wxDefaultPosition, wxDefaultSize, 0);
			flag->SetValue(0);
			mStatusSizer->Add(flag, 0, wxALL, 5);
			mStatusWidgets.push_back(flag);
			mFlags[flag] = *it2;
		}
		mRegisterView->Enable();

		mRunButton->Enable(true);
		mStepButton->Enable(config.step);
		mStepOverButton->Enable(config.stepOver);
		mStepOutButton->Enable(config.stepOut);
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
	if (mEmu.emu == NULL)
		return;
	if (mEmu.emu->IsRunning(mEmu.handle) == 1)
	{
		// Stop
		mEmu.emu->Run(mEmu.handle, 0);
	}
	else if (mEmu.emu->IsRunning(mEmu.handle) == 0)
	{
		// Run
		mEmu.emu->Run(mEmu.handle, 1);
	}

	m_parent->Update();
}

void CPUDebugger::OnStep(wxCommandEvent &evt)
{
	if (mEmu.emu == NULL)
		return;
	mEmu.emu->Step(mEmu.handle);
	m_parent->Update();
}