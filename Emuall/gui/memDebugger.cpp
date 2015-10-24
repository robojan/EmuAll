

#include "memDebugger.h"
#include "../util/memDbg.h"
#include "IdList.h"


IMPLEMENT_CLASS(MemDebugger, wxFrame)
BEGIN_EVENT_TABLE(MemDebugger, wxFrame)
EVT_CLOSE(MemDebugger::OnClose)
EVT_CHOICE(ID_Debug_Mem_picker, MemDebugger::OnMemoryChange)
EVT_TEXT_ENTER(ID_Debug_Mem_goto, MemDebugger::OnGoto)
END_EVENT_TABLE()


MemDebugger::MemDebugger(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) :
wxFrame(parent, id, title, pos, size, style), mHexView(nullptr), mReadOnly(true)
{
	mEmu.emu = NULL;
	mEmu.handle = NULL;

	// Initialize the window
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

	// Setup layout
	wxBoxSizer *bsizer1 = new wxBoxSizer(wxHORIZONTAL);

	// Create hexView
	mHexView = new HexView(this, 0);
	wxGrid *grid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 350), wxALWAYS_SHOW_SB|wxVSCROLL);
	grid->SetTable(mHexView, false);
	mHexView->SetSize(0);	
	bsizer1->Add(grid,0, wxEXPAND, 5);

	wxBoxSizer *bsizer2 = new wxBoxSizer(wxVERTICAL);

	// Create options
	wxFlexGridSizer *fgsizer1 = new wxFlexGridSizer(0,2,0,0);
	fgsizer1->SetFlexibleDirection(wxBOTH);

	// Create memory picker
	wxStaticText *label1 = new wxStaticText(this, wxID_ANY, wxT("Memory: "));
	mPicker = new wxChoice(this, ID_Debug_Mem_picker);
	fgsizer1->Add(label1, 1, wxALL, 5);
	fgsizer1->Add(mPicker, 1, wxALL, 5);

	// Create goto box
	fgsizer1->Add(new wxStaticText(this, wxID_ANY, wxT("Goto: ")), 1, wxALL, 5);
	wxTextValidator hexValidator(wxFILTER_INCLUDE_CHAR_LIST);
	wxArrayString hexList;
	for (int i = 0; i <0x10; i++)
	{
		hexList.Add(wxString::Format("%X", i));
		if (i>0x9)
		{
			hexList.Add(wxString::Format("%x", i));
		}
	}
	hexValidator.SetIncludes(hexList);
	fgsizer1->Add(new wxTextCtrl(this, ID_Debug_Mem_goto, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER,hexValidator), 1, wxALL, 5);

	// Add options
	bsizer2->Add(fgsizer1, 1, wxEXPAND, 5);

	// Create info
	mMemInfo = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	mMemInfo->InsertColumn(0, wxT("Info"));
	mMemInfo->InsertColumn(1, wxT("Value"));

	// Add info
	bsizer2->Add(mMemInfo,1, wxEXPAND, 5);
	
	bsizer1->Add(bsizer2, 0, wxEXPAND, 5);
	SetSizerAndFit(bsizer1);
	Layout();
	Centre(wxBOTH);

	SetEmulator(mEmu);
}

MemDebugger::~MemDebugger()
{
	if (mHexView)
	{
		delete mHexView;
		mHexView = NULL;
	}
}

void MemDebugger::SetEmulator(const Emulator &emu)
{
	mEmu = emu;
	if (mEmu.emu != NULL)
	{
		MemDebuggerInfo_t config = mEmu.emu->GetMemDebuggerInfo(mEmu.handle);

		for (std::vector<EmulatorMemory_t>::iterator it = config.memories.begin(); it != config.memories.end(); ++it)
		{
			mMemories[mPicker->Append(it->name)] = *it;
		}
		mPicker->Select(0);
		mHexView->SetSize(mMemories[mPicker->GetSelection()].size);


		for (std::vector<EmulatorValue_t>::iterator it = config.infos.begin(); it != config.infos.end(); ++it)
		{
			long pos = mMemInfo->InsertItem(mMemInfo->GetItemCount(), it->name);
			mInfos[pos] = *it;
			mMemInfo->SetItem(pos, 1, wxString::Format("%d", mEmu.emu->GetValI(mEmu.handle, it->id)));
		}

		mReadOnly = config.readOnly;
	}
	else {
		mPicker->Clear();
		mHexView->SetSize(0);
		mMemInfo->DeleteAllItems();
		mMemories.clear();
		mInfos.clear();
		mReadOnly = true;
	}
	Update();
}

void MemDebugger::Update()
{
	if (mEmu.emu != NULL)
	{
		for (int i = 0; i < mMemInfo->GetItemCount(); i++)
		{
			mMemInfo->SetItem(i, 1, wxString::Format("%d", mEmu.emu->GetValI(mEmu.handle, mInfos[i].id)));
		}
	}
	Refresh();
}

void MemDebugger::OnGoto(wxCommandEvent &evt)
{
	
	wxTextCtrl *gotoBox = (wxTextCtrl *)evt.GetEventObject();
	unsigned long address;
	evt.GetString().ToULong(&address, 16);
	if (address >= mHexView->GetSize())
		return;
	mHexView->GetView()->MakeCellVisible(address>>4, address&0xf);
	gotoBox->SetValue(wxEmptyString);
}

void MemDebugger::OnClose(wxCloseEvent &evt)
{
	if (evt.CanVeto())
	{
		evt.Veto(true);
		Show(false);
		return;
	}
	Destroy();
}

void MemDebugger::OnMemoryChange(wxCommandEvent &evt)
{
	mHexView->SetSize(mMemories[evt.GetSelection()].size);
	Update();
}

wxByte MemDebugger::OnGetValue(int address)
{
	if (mEmu.emu == NULL)
		return 0xCD;
	return mEmu.emu->GetMemoryData(mEmu.handle, mPicker->GetSelection(), address);
}

void MemDebugger::OnSetValue(int address, wxByte val)
{
	return;
}