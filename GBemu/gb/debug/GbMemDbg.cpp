#include "GbMemDbg.h"
#include "../mem/GbMem.h"
#include "../../../gui/IdList.h"
#include <wx/statline.h>
#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

IMPLEMENT_CLASS(GbMemDbg, wxFrame)
BEGIN_EVENT_TABLE(GbMemDbg, wxFrame)
	EVT_CLOSE(GbMemDbg::onClose)
	EVT_COMBOBOX(ID_Debug_Mem_mode, GbMemDbg::onModeChange)
	EVT_TEXT_ENTER(ID_Debug_Mem_gotoText, GbMemDbg::onGoto)
	EVT_BUTTON(ID_Debug_Mem_goto, GbMemDbg::onGoto)
END_EVENT_TABLE()

const wxString ModeSelection[] = 
{
	_("Active Memory"),
	_("ROM"),
	_("VRAM 0"),
	_("VRAM 1"),
	_("ERAM"),
	_("WRAM"),
	_("Registers")
};


GbMemDbg::GbMemDbg(wxFrame *parent, GbInfo_t *info) :
	wxFrame(parent, ID_Debug_Mem_window, _("Gameboy Memory Debugger"), 
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN)
{
	m_info = info;
	m_mode = 0;
	m_panel = new wxPanel(this);
	m_sizerH = new wxBoxSizer(wxHORIZONTAL);
	m_sizerH2 = new wxBoxSizer(wxHORIZONTAL);
	m_sizerV = new wxBoxSizer(wxVERTICAL);

	m_hexView = new HexView(this, 0x10000);
	m_grid = new wxGrid(m_panel, ID_Debug_Mem_hex, wxDefaultPosition, wxSize(550, 400));
	m_grid->SetDefaultCellFont(wxFont(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false));
	m_grid->DisableCellEditControl();
	m_grid->SetTable(m_hexView,true);
	
	for(int i = 0; i<0x10; i++)
	{
		m_grid->SetColSize(i, 20);
	}
	m_grid->SetColSize(0x10, 130);
	m_grid->DisableDragGridSize();

	m_modeBox = new wxComboBox(m_panel, ID_Debug_Mem_mode, _("Active Memory"),wxDefaultPosition, wxDefaultSize, 7, ModeSelection, wxCB_READONLY);

	m_sizerV->Add(new wxStaticText(m_panel, wxID_ANY, _("Memory:")));
	m_sizerV->Add(m_modeBox);
	m_sizerV->AddSpacer(5);
	m_sizerV->Add(new wxStaticLine(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),0, wxEXPAND,0);
	m_sizerV->AddSpacer(10);
	
	wxArrayString hexList;
	for(int i = 0; i <0x10; i++)
	{
		hexList.Add(wxString::Format("%X", i));
		if(i>0x9)
		{
			hexList.Add(wxString::Format("%x", i));
		}
	}
	m_hexInput = new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);
	m_hexInput->SetIncludes(hexList);
	m_gotoText = new wxTextCtrl(m_panel, ID_Debug_Mem_gotoText, wxEmptyString, wxDefaultPosition, wxSize(80, -1), wxTE_PROCESS_ENTER,*m_hexInput);
	m_gotoText->SetMaxLength(8);
	m_goto = new wxButton(m_panel, ID_Debug_Mem_goto, _("Goto"), wxDefaultPosition, wxSize(80, -1));
	
	m_sizerH2->Add(new wxStaticText(m_panel, wxID_ANY, _("Goto: ")));
	m_sizerH2->Add(m_gotoText, 0, wxALIGN_RIGHT, 0);
	m_sizerV->Add(m_sizerH2);
	m_sizerV->Add(m_goto, 0, wxALIGN_RIGHT,0);
	m_sizerV->AddSpacer(5);
	m_sizerV->Add(new wxStaticLine(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),0, wxEXPAND,0);
	m_sizerV->AddSpacer(10);

	m_sizeROM = new wxStaticText(m_panel, wxID_ANY,    _("ROM size: -"));
	m_activeROM = new wxStaticText(m_panel, wxID_ANY,  _("Active ROM: -"));
	m_activeVRAM = new wxStaticText(m_panel, wxID_ANY, _("Active VRAM: -"));
	m_activeWRAM = new wxStaticText(m_panel, wxID_ANY, _("Active WRAM: -"));
	m_activeERAM = new wxStaticText(m_panel, wxID_ANY, _("Active ERAM: -"));
	m_sizeERAM = new wxStaticText(m_panel, wxID_ANY,   _("ERAM size: -"));
	m_sizeROM->SetDoubleBuffered(true);
	m_activeROM->SetDoubleBuffered(true);
	m_sizerV->Add(m_sizeROM);
	m_sizerV->Add(m_activeROM);
	m_sizerV->Add(m_activeVRAM);
	m_sizerV->Add(m_activeWRAM);
	m_sizerV->Add(m_activeERAM);
	m_sizerV->Add(m_sizeERAM);

	m_sizerH->Add(m_grid, 0, wxEXPAND, 0);
	m_sizerH->Add(m_sizerV);
	m_panel->SetSizerAndFit(m_sizerH);
	Fit();
}

GbMemDbg::~GbMemDbg()
{
	if(m_hexInput != NULL)
	{
		delete m_hexInput;
		m_hexInput = NULL;
	}
}

void GbMemDbg::updateContent(void)
{
	//Freeze();
	if(!this->IsShown())
	{
		return;
	}
	if(m_info == NULL || m_info->mem == NULL)
	{
		m_sizeROM->SetLabel(_("ROM size: -"));
		m_activeROM->SetLabel(_("Active ROM: -"));
		m_activeVRAM->SetLabel(_("Active VRAM: -"));
		m_activeWRAM->SetLabel(_("Active WRAM: -"));
		m_activeERAM->SetLabel(_("Active ERAM: -"));
		m_sizeERAM->SetLabel(_("ERAM size: -"));
		m_modeBox->SetSelection(0);
		m_modeBox->Enable(false);
	} else {
		m_sizeROM->SetLabel(wxString::Format(_("ROM size: %dKB"),m_info->mem->rom->length/1024));
		m_activeROM->SetLabel(wxString::Format(_("Active ROM: %d"), m_info->mem->romBank));
		m_activeVRAM->SetLabel(wxString::Format(_("Active VRAM: %d"), m_info->mem->vramBank));
		m_activeWRAM->SetLabel(wxString::Format(_("Active WRAM: %d"), m_info->mem->wramBank));
		m_activeERAM->SetLabel(wxString::Format(_("Active ERAM: %d"), m_info->mem->eramBank));
		m_sizeERAM->SetLabel(wxString::Format(_("ERAM size: %dKB"), m_info->mem->eramSize*ERAM_BANK_SIZE/1024));
		m_modeBox->Enable(true);
	}
	//Thaw();
}

void GbMemDbg::setValue(int address, wxByte val)
{
	if(m_info == NULL || m_info->mem == NULL)
	{
		return;
	}
	switch(m_mode)
	{
	case 0: // Active memory
		m_info->mem->write(address, val);
		break;
	case 1: // ROM
		break;
	case 2: // VRAM 0	
		m_info->mem->vram[0][address-0x8000] = val;
		break;
	case 3: // VRAM 1
		m_info->mem->vram[1][address-0x8000] = val;
		break;
	case 4: // ERAM
		m_info->mem->eram[address/(ERAM_BANK_SIZE)][address%(ERAM_BANK_SIZE)] = val;
		break;
	case 5: // WRAM
		m_info->mem->wram[address/(WRAM_BANK_SIZE)][address%(WRAM_BANK_SIZE)] = val;
		break;
	case 6: // Registers
		m_info->mem->write(address, val);
		break;
	}
	
	return;
}

wxByte GbMemDbg::getValue(int address)
{
	if(m_info == NULL || m_info->mem == NULL)
	{
		return 0;
	}
	switch(m_mode)
	{
	case 0: // Active memory
		return m_info->mem->read(address);
	case 1: // ROM
		return m_info->mem->rom->data[address];
	case 2: // VRAM 0		
		return m_info->mem->vram[0][address];
	case 3: // VRAM 1
		return m_info->mem->vram[1][address];
	case 4: // ERAM
		return m_info->mem->eram[address/(ERAM_BANK_SIZE)][address%(ERAM_BANK_SIZE)];
	case 5: // WRAM
		return m_info->mem->wram[address/(WRAM_BANK_SIZE)][address%(WRAM_BANK_SIZE)];
	case 6: // Registers
		return m_info->mem->regs[address+0xE00];
	}
	return 0;
}

void GbMemDbg::onGoto(wxCommandEvent &evt)
{
	unsigned long address = 0;
	wxString text = m_gotoText->GetValue();
	if(text.IsEmpty())
	{
		evt.Skip(false);
		return;
	}
	if(!text.ToULong(&address, 16))
	{
		evt.Skip(false);
		return;
	}
	address -= m_hexView->GetOffset();
	if(address >= m_hexView->GetSize())
	{
		address = m_hexView->GetSize()-1;
	}
	m_grid->SelectBlock(address>>4, address & 0xf, address>>4, address & 0xf, false);
	m_grid->MakeCellVisible(address>>4, address & 0xf);
}

void GbMemDbg::onClose(wxCloseEvent &evt)
{
	if(evt.CanVeto())
	{
		evt.Veto(true);
		Show(false);
		return;
	}
	Destroy();
}

void GbMemDbg::onModeChange(wxCommandEvent &evt)
{
	wxString select = m_modeBox->GetValue();
	for(int i = 0; i< 7; i++)
	{
		if(select.Cmp(ModeSelection[i])== 0)
		{
			m_mode = i;
			if(!(m_info == NULL || m_info->mem == NULL))
			{
				switch(i)
				{
				case 0: // Active memory
					m_hexView = new HexView(this, 0x10000); // Make a new Hexview, because when it updates the old one is deleted :S
					m_hexView->SetOffset(0);
					m_grid->SetTable(m_hexView, true);
					break;
				case 1: // ROM
					m_hexView = new HexView(this, m_info->mem->rom->length); // Make a new Hexview, because when it updates the old one is deleted :S
					m_hexView->SetOffset(0);
					m_grid->SetTable(m_hexView, true);
					break;
				case 2: // VRAM 0
				case 3: // VRAM 1
					m_hexView = new HexView(this, VRAM_BANK_SIZE); // Make a new Hexview, because when it updates the old one is deleted :S
					m_hexView->SetOffset(0x800);
					m_grid->SetTable(m_hexView, true);
					break;
				case 4: // ERAM
					m_hexView = new HexView(this, m_info->mem->eramSize * ERAM_BANK_SIZE); // Make a new Hexview, because when it updates the old one is deleted :S
					m_hexView->SetOffset(0);
					m_grid->SetTable(m_hexView, true);
					break;
				case 5: // WRAM
					m_hexView = new HexView(this, 8*WRAM_BANK_SIZE); // Make a new Hexview, because when it updates the old one is deleted :S
					m_hexView->SetOffset(0);
					m_grid->SetTable(m_hexView, true);
					break;
				case 6: // Registers
					m_hexView = new HexView(this, 0x10000-0xFE00); // Make a new Hexview, because when it updates the old one is deleted :S
					m_hexView->SetOffset(0xFE0);
					m_grid->SetTable(m_hexView, true);
					break;
				}
			}
			for(int i = 0; i<0x10; i++)
			{
				m_grid->SetColSize(i, 20);
			}
			m_grid->SetColSize(0x10, 130);
			m_grid->DisableDragGridSize();
			m_grid->Refresh(true);
		} 
	}
}