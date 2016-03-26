

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
wxFrame(parent, id, title, pos, size, style), _hexView(nullptr), _readOnly(true)
{
	_emu.emu = NULL;
	_emu.handle = NULL;

	// Initialize the window
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

	// Setup layout
	wxBoxSizer *bsizer1 = new wxBoxSizer(wxHORIZONTAL);

	// Create hexView
	_hexView = new HexView(this, 0);
	wxGrid *grid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 350), wxALWAYS_SHOW_SB|wxVSCROLL);
	grid->SetTable(_hexView, false);
	_hexView->SetSize(0);	
	bsizer1->Add(grid,0, wxEXPAND, 5);

	wxBoxSizer *bsizer2 = new wxBoxSizer(wxVERTICAL);

	// Create options
	wxFlexGridSizer *fgsizer1 = new wxFlexGridSizer(0,2,0,0);
	fgsizer1->SetFlexibleDirection(wxBOTH);

	// Create memory picker
	wxStaticText *label1 = new wxStaticText(this, wxID_ANY, wxT("Memory: "));
	_picker = new wxChoice(this, ID_Debug_Mem_picker);
	fgsizer1->Add(label1, 1, wxALL, 5);
	fgsizer1->Add(_picker, 1, wxALL, 5);

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
	_memInfo = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	_memInfo->InsertColumn(0, wxT("Info"));
	_memInfo->InsertColumn(1, wxT("Value"));

	// Add info
	bsizer2->Add(_memInfo,1, wxEXPAND, 5);
	
	bsizer1->Add(bsizer2, 0, wxEXPAND, 5);
	SetSizerAndFit(bsizer1);
	Layout();
	Centre(wxBOTH);

	SetEmulator(_emu);
}

MemDebugger::~MemDebugger()
{
	if (_hexView)
	{
		delete _hexView;
		_hexView = NULL;
	}
}

void MemDebugger::SetEmulator(const Emulator &emu)
{
	_emu = emu;
	if (_emu.emu != NULL)
	{
		MemDebuggerInfo_t config = _emu.emu->GetMemDebuggerInfo(_emu.handle);

		for (std::vector<EmulatorMemory_t>::iterator it = config.memories.begin(); it != config.memories.end(); ++it)
		{
			_memories[_picker->Append(it->name)] = *it;
		}
		_picker->Select(0);
		_hexView->SetSize(_memories[_picker->GetSelection()].size);


		for (std::vector<EmulatorValue_t>::iterator it = config.infos.begin(); it != config.infos.end(); ++it)
		{
			long pos = _memInfo->InsertItem(_memInfo->GetItemCount(), it->name);
			_infos[pos] = *it;
			_memInfo->SetItem(pos, 1, wxString::Format("%d", _emu.emu->GetValI(_emu.handle, it->id)));
		}

		_readOnly = config.readOnly;
	}
	else {
		_picker->Clear();
		_hexView->SetSize(0);
		_memInfo->DeleteAllItems();
		_memories.clear();
		_infos.clear();
		_readOnly = true;
	}
	Update();
}

void MemDebugger::Update()
{
	if (_emu.emu != NULL)
	{
		for (int i = 0; i < _memInfo->GetItemCount(); i++)
		{
			_memInfo->SetItem(i, 1, wxString::Format("%d", _emu.emu->GetValI(_emu.handle, _infos[i].id)));
		}
	}
	Refresh();
}

void MemDebugger::OnGoto(wxCommandEvent &evt)
{
	
	wxTextCtrl *gotoBox = (wxTextCtrl *)evt.GetEventObject();
	unsigned long address;
	evt.GetString().ToULong(&address, 16);
	if (address >= _hexView->GetSize())
		return;
	_hexView->GetView()->MakeCellVisible(address>>4, address&0xf);
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
	_hexView->SetSize(_memories[evt.GetSelection()].size);
	Update();
}

wxByte MemDebugger::OnGetValue(int address)
{
	if (_emu.emu == NULL)
		return 0xCD;
	return _emu.emu->GetMemoryData(_emu.handle, _picker->GetSelection(), address);
}

void MemDebugger::OnSetValue(int address, wxByte val)
{
	return;
}