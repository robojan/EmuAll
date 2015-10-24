
#include "gpuDebugger.h"
#include "../util/log.h"
#include "IdList.h"

IMPLEMENT_CLASS(GPUDebugger, wxFrame)
BEGIN_EVENT_TABLE(GPUDebugger, wxFrame)
	EVT_CLOSE(GPUDebugger::OnClose)
	EVT_TIMER(ID_Debug_Gpu_timer, GPUDebugger::OnTimer)
END_EVENT_TABLE()

GPUDebugger::GPUDebugger(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) :
	wxFrame(parent, id, title, pos, size, style), mRoot(NULL)
{
	mEmu.emu = NULL;
	mEmu.handle = NULL;
	SetEmulator(mEmu);
	mTimer = new wxTimer(this, ID_Debug_Gpu_timer);
	mTimer->Start(200); // 5 Hz update rate
}

GPUDebugger::~GPUDebugger()
{
	mEmu.emu = NULL;
	mEmu.handle = NULL;
	mTimer->Stop();
	if (mRoot != NULL)
	{
		delete mRoot;
		mRoot = NULL;
	}
}

void GPUDebugger::SetEmulator(const Emulator &emu)
{
	wxBoxSizer *vbSizer1 = new wxBoxSizer(wxVERTICAL);
	if (emu.emu == NULL)
	{
		DestroyChildren();
		if (mRoot != NULL)
		{
			delete mRoot;
			mRoot = NULL;
		}
		mEmu = emu;
		// Create empty page
		wxPanel *emptyPage = new wxPanel(this);
		wxBoxSizer *hbSizer1 = new wxBoxSizer(wxHORIZONTAL);
		wxStaticText *label1 = new wxStaticText(emptyPage, wxID_ANY, _("No emulator loaded"));
		hbSizer1->AddStretchSpacer();
		hbSizer1->Add(label1, 1, wxALIGN_CENTER, 5);
		hbSizer1->AddStretchSpacer();
		emptyPage->SetSizerAndFit(hbSizer1);
		vbSizer1->Add(emptyPage, 1, wxEXPAND | wxALL, 0);
	}
	else {
		DestroyChildren();
		if (mRoot != NULL)
		{
			delete mRoot;
			mRoot = NULL;
		}
		mEmu = emu;
		mRoot = mEmu.emu->GetGpuDebuggerInfo(&mEmu);

		wxPanel *panel = mRoot->GetWidget(this, wxID_ANY);
		vbSizer1->Add(panel, 1, wxEXPAND | wxALL, 0);
	}

	SetSizerAndFit(vbSizer1);
	Layout();
	SetMinSize(GetBestSize());
	SetSize(GetBestSize());
	Update();
}

void GPUDebugger::Update()
{
	if (mRoot != NULL)
	{
		mRoot->UpdateInfo();
	}
}

void GPUDebugger::OnTimer(wxTimerEvent &evt)
{
	Update();
}

void GPUDebugger::OnClose(wxCloseEvent &evt)
{
	if (evt.CanVeto())
	{
		evt.Veto(true);
		Show(false);
		return;
	}
	Destroy();
}