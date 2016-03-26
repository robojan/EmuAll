#ifndef _GBCPUDBG_H
#define _GBCPUDBG_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../cpu/GbCpu.h"
#include "../gbInfo.h"


class GbCpuDbg : public wxFrame
{
	DECLARE_CLASS(GbCpuDbg);
	DECLARE_EVENT_TABLE()
public:
	GbCpuDbg(wxFrame *parent, GbInfo_t *info);
	~GbCpuDbg(void);

	void updateContent(void);
private:
	void onClose(wxCloseEvent &evt);
	void onStep(wxCommandEvent &evt);
	void onRun(wxCommandEvent &evt);
	void onBreak(wxCommandEvent &evt);

	GbInfo_t	*m_info;

	wxPanel		*m_panel;
	wxBoxSizer	*m_sizerV;
	wxBoxSizer	*m_sizerV2;
	wxBoxSizer	*m_sizerV3;
	wxBoxSizer	*m_sizerH1;
	wxBoxSizer	*m_sizerH2;
	wxBoxSizer	*m_sizerH3;

	wxListCtrl	*m_infoList;
	wxListItem	*m_infoListCol1;
	wxListItem	*m_infoListCol2;
	wxListItem	*m_infoListCol3;

	wxStaticText *m_flags_lbl;
	wxCheckBox	*m_flags_C;
	wxCheckBox	*m_flags_H;
	wxCheckBox	*m_flags_N;
	wxCheckBox	*m_flags_Z;

	wxCheckBox	*m_stopped;
	wxCheckBox	*m_halted;
	wxCheckBox	*m_doubleSpeed;
	wxCheckBox	*m_ime;

	wxTextCtrl	*m_breakpoint;

	wxButton	*m_but_step;
	wxButton	*m_but_run;
};

#endif