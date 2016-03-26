
#include <bitset>

#include "GbCpuDbg.h"
#include "../GbSystem.h"
#include "../../../gui/IdList.h"
#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

IMPLEMENT_CLASS(GbCpuDbg, wxFrame)
BEGIN_EVENT_TABLE(GbCpuDbg, wxFrame)
	EVT_CLOSE(GbCpuDbg::onClose)
	EVT_BUTTON(ID_Debug_Cpu_step, GbCpuDbg::onStep)
	EVT_BUTTON(ID_Debug_Cpu_run, GbCpuDbg::onRun)
	EVT_TEXT(ID_Debug_Cpu_break, GbCpuDbg::onBreak)
END_EVENT_TABLE()

GbCpuDbg::GbCpuDbg(wxFrame *parent, GbInfo_t *info) :
	wxFrame(parent, ID_Debug_Cpu_window, _("Gameboy CPU debugger"), 
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN)
{
	m_info = info;

	m_panel = new wxPanel(this);
	m_sizerV = new wxBoxSizer(wxVERTICAL);
	m_sizerV2 = new wxBoxSizer(wxVERTICAL);
	m_sizerV3 = new wxBoxSizer(wxVERTICAL);
	m_sizerH1 = new wxBoxSizer(wxHORIZONTAL);
	m_sizerH2 = new wxBoxSizer(wxHORIZONTAL);
	m_sizerH3 = new wxBoxSizer(wxHORIZONTAL);

	
	m_infoList = new wxListCtrl(m_panel, -1, wxDefaultPosition, wxSize(245,-1), wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
	
	m_infoListCol1 = new wxListItem();
	m_infoListCol1->SetId(0);
	m_infoListCol1->SetWidth(70);
	m_infoListCol1->SetText(wxT("Register"));
	m_infoListCol2 = new wxListItem();
	m_infoListCol2->SetId(1);
	m_infoListCol2->SetWidth(55);
	m_infoListCol2->SetText(wxT("Hex"));
	m_infoListCol3 = new wxListItem();
	m_infoListCol3->SetId(2);
	m_infoListCol3->SetWidth(110);
	m_infoListCol3->SetText(wxT("Binary"));
	m_infoList->InsertColumn(0l, *m_infoListCol1);
	m_infoList->InsertColumn(1l, *m_infoListCol2);
	m_infoList->InsertColumn(2l, *m_infoListCol3);

	m_infoList->InsertItem(0, _("AF"));
	m_infoList->InsertItem(1, _("BC"));
	m_infoList->InsertItem(2, _("DE"));
	m_infoList->InsertItem(3, _("HL"));
	m_infoList->InsertItem(4, _("SP"));
	m_infoList->InsertItem(5, _("PC"));

	m_flags_lbl = new wxStaticText(m_panel, wxID_ANY, _("Flags: "));
	m_flags_lbl->SetBackgroundColour(m_panel->GetBackgroundColour());

	m_stopped = new wxCheckBox(m_panel, wxID_ANY, _("Stopped"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_stopped->SetBackgroundColour(m_panel->GetBackgroundColour());
	m_halted = new wxCheckBox(m_panel, wxID_ANY, _("Halted"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_halted->SetBackgroundColour(m_panel->GetBackgroundColour());
	m_doubleSpeed = new wxCheckBox(m_panel, wxID_ANY, _("Double speed"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_doubleSpeed->SetBackgroundColour(m_panel->GetBackgroundColour());
	m_ime = new wxCheckBox(m_panel, wxID_ANY, _("IME"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_ime->SetBackgroundColour(m_panel->GetBackgroundColour());

	m_flags_C = new wxCheckBox(m_panel, wxID_ANY, _("C"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_flags_C->SetBackgroundColour(m_panel->GetBackgroundColour());
	m_flags_H = new wxCheckBox(m_panel, wxID_ANY, _("H"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_flags_H->SetBackgroundColour(m_panel->GetBackgroundColour());
	m_flags_N = new wxCheckBox(m_panel, wxID_ANY, _("N"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_flags_N->SetBackgroundColour(m_panel->GetBackgroundColour());
	m_flags_Z = new wxCheckBox(m_panel, wxID_ANY, _("Z"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	m_flags_Z->SetBackgroundColour(m_panel->GetBackgroundColour());

	wxArrayString hexList;
	for(int i = 0; i <0x10; i++)
	{
		hexList.Add(wxString::Format("%X", i));
		if(i>0x9)
		{
			hexList.Add(wxString::Format("%x", i));
		}
	}
	wxTextValidator hexInput(wxFILTER_INCLUDE_CHAR_LIST);
	hexInput.SetIncludes(hexList);

	m_breakpoint = new wxTextCtrl(m_panel, ID_Debug_Cpu_break, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, hexInput);

	m_but_step = new wxButton(m_panel, ID_Debug_Cpu_step, _("Step"));
	m_but_run = new wxButton(m_panel, ID_Debug_Cpu_run, _("Run"));

	m_sizerV->Add(m_infoList, 0, wxEXPAND, 0);
	m_sizerH1->Add(m_flags_lbl);
	m_sizerH1->Add(m_flags_C);
	m_sizerH1->Add(m_flags_H);
	m_sizerH1->Add(m_flags_N);
	m_sizerH1->Add(m_flags_Z);
	m_sizerV->Add(m_sizerH1, 0, wxEXPAND, 0);
	m_sizerV2->Add(m_stopped);
	m_sizerV3->Add(m_halted);
	m_sizerV2->Add(m_doubleSpeed);
	m_sizerV3->Add(m_ime);
	m_sizerV2->Add(new wxStaticText(m_panel, wxID_ANY, _("Breakpoint:")));
	m_sizerV3->Add(m_breakpoint);
	m_sizerH3->Add(m_sizerV2);
	m_sizerH3->Add(m_sizerV3);
	m_sizerV->Add(m_sizerH3);
	m_sizerH2->Add(m_but_step);
	m_sizerH2->Add(m_but_run);
	m_sizerV->Add(m_sizerH2, 0, wxALIGN_CENTER_HORIZONTAL, 0);
	m_panel->SetSizerAndFit(m_sizerV);
	m_panel->Layout();
	Fit();

	updateContent();
}

GbCpuDbg::~GbCpuDbg()
{
	if(m_infoListCol1 != NULL)
	{
		delete m_infoListCol1;
		m_infoListCol1 = NULL;
	}
	if(m_infoListCol2 != NULL)
	{
		delete m_infoListCol2;
		m_infoListCol1 = NULL;
	}
	if(m_infoListCol2 != NULL)
	{
		delete m_infoListCol3;
		m_infoListCol3 = NULL;
	}
}

void GbCpuDbg::onClose(wxCloseEvent &evt)
{
	if(evt.CanVeto())
	{
		evt.Veto(true);
		Show(false);
		return;
	}
	Destroy();
}

void GbCpuDbg::onStep(wxCommandEvent &evt)
{
	if(m_info == NULL || m_info->system == NULL)
		return;
	m_info->system->step();
}

void GbCpuDbg::onRun(wxCommandEvent &evt)
{
	if(m_info == NULL || m_info->system == NULL)
		return;
	if(m_info->system->isRunning())
	{
		m_info->system->run(false);
	} else {
		m_info->system->run(true);
	}
}

void GbCpuDbg::updateContent()
{
	if(!this->IsShown())
	{
		return;
	}
	if( m_info == NULL)
	{
		wxLogError("GbCpuDbg: Null pointer to information");
		return;
	}
	if( m_info->cpu == NULL || m_info->system == NULL)
	{
		m_infoList->Freeze();
		for(int i = 0; i< m_infoList->GetItemCount(); i++)
		{
			m_infoList->SetItem(i, 1, _("-"));
			m_infoList->SetItem(i, 2, _("-"));
		}
		m_infoList->Thaw();
		m_flags_C->SetValue(false);
		m_flags_H->SetValue(false);
		m_flags_N->SetValue(false);
		m_flags_Z->SetValue(false);
		m_but_run->Enable(false);
		m_but_run->SetLabel(_("Run"));
		m_but_step->Enable(false);
		m_flags_Z->Enable(false);
		m_flags_N->Enable(false);
		m_flags_H->Enable(false);
		m_flags_C->Enable(false);
		m_infoList->Enable(false);
		m_stopped->Enable(false);
		m_halted->Enable(false);
		m_doubleSpeed->Enable(false);
		m_ime->Enable(false);
	} else {
		m_infoList->Freeze();
		m_infoList->SetItem(0, 1, wxString::Format<uint16_t>("%04X", m_info->cpu->regAF));
		m_infoList->SetItem(0, 2, std::bitset<16>(m_info->cpu->regAF).to_string());
		m_infoList->SetItem(1, 1, wxString::Format<uint16_t>("%04X", m_info->cpu->regBC));
		m_infoList->SetItem(1, 2, std::bitset<16>(m_info->cpu->regBC).to_string());
		m_infoList->SetItem(2, 1, wxString::Format<uint16_t>("%04X", m_info->cpu->regDE));
		m_infoList->SetItem(2, 2, std::bitset<16>(m_info->cpu->regDE).to_string());
		m_infoList->SetItem(3, 1, wxString::Format<uint16_t>("%04X", m_info->cpu->regHL));
		m_infoList->SetItem(3, 2, std::bitset<16>(m_info->cpu->regHL).to_string());
		m_infoList->SetItem(4, 1, wxString::Format<uint16_t>("%04X", m_info->cpu->regSP));
		m_infoList->SetItem(4, 2, std::bitset<16>(m_info->cpu->regSP).to_string());
		m_infoList->SetItem(5, 1, wxString::Format<uint16_t>("%04X", m_info->cpu->regPC));
		m_infoList->SetItem(5, 2, std::bitset<16>(m_info->cpu->regPC).to_string());
		m_infoList->Thaw();
		m_flags_Z->SetValue((m_info->cpu->regF & 0x80) != 0);
		m_flags_N->SetValue((m_info->cpu->regF & 0x40) != 0);
		m_flags_H->SetValue((m_info->cpu->regF & 0x20) != 0);
		m_flags_C->SetValue((m_info->cpu->regF & 0x10) != 0);
		m_stopped->SetValue(m_info->cpu->stopped);
		m_halted->SetValue(m_info->cpu->halted);
		m_doubleSpeed->SetValue(m_info->cpu->doubleSpeed);
		m_ime->SetValue(m_info->cpu->ime);
		m_but_run->Enable(true);
		if(!m_info->system->isRunning())
		{
			m_but_run->SetLabel(_("Run"));
			m_but_step->Enable(true);
			m_flags_Z->Enable(true);
			m_flags_N->Enable(true);
			m_flags_H->Enable(true);
			m_flags_C->Enable(true);
			m_infoList->Enable(true);
			m_stopped->Enable(true);
			m_halted->Enable(true);
			m_doubleSpeed->Enable(true);
			m_ime->Enable(true);
		} else {
			m_but_run->SetLabel(_("Stop"));
			m_but_step->Enable(false);
			m_flags_Z->Enable(false);
			m_flags_N->Enable(false);
			m_flags_H->Enable(false);
			m_flags_C->Enable(false);
			m_infoList->Enable(false);
			m_stopped->Enable(false);
			m_halted->Enable(false);
			m_doubleSpeed->Enable(false);
			m_ime->Enable(false);
		}
	}
}

void GbCpuDbg::onBreak(wxCommandEvent &evt)
{
	if(!evt.GetString().IsEmpty())
	{
		unsigned long point;
		evt.GetString().ToULong(&point,16);
		m_info->system->setBreakpoint(point);
	} else {
		m_info->system->setBreakpoint(-1);
	}
}