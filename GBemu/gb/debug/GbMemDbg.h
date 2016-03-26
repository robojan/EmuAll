#ifndef _GBMEMDBG_H
#define _GBMEMDBG_H

#include <wx/wx.h>
#include "../gbInfo.h"
#include "../../../gui/HexView.h"

class GbMemDbg : public wxFrame, HexViewI
{
	DECLARE_CLASS(GbMemDbg)
	DECLARE_EVENT_TABLE()
public:
	GbMemDbg(wxFrame *parent, GbInfo_t *info);
	~GbMemDbg();
	wxByte getValue(int address);
	void setValue(int address, wxByte val);

	void updateContent(void);

	void onClose(wxCloseEvent &evt);
	void onModeChange(wxCommandEvent &evt);
	void onGoto(wxCommandEvent &evt);
private:
	wxPanel			*m_panel;
	wxBoxSizer		*m_sizerH;
	wxBoxSizer		*m_sizerH2;
	wxBoxSizer		*m_sizerH3;
	wxBoxSizer		*m_sizerV;
	wxGrid			*m_grid;
	HexView			*m_hexView;
	wxComboBox		*m_modeBox;
	wxTextCtrl		*m_gotoText;
	wxTextValidator	*m_hexInput;
	wxButton		*m_goto;
	wxStaticText	*m_sizeROM;
	wxStaticText	*m_activeROM;
	wxStaticText	*m_activeVRAM;
	wxStaticText	*m_activeWRAM;
	wxStaticText	*m_activeERAM;
	wxStaticText	*m_sizeERAM;

	GbInfo_t		*m_info;
	int				m_mode;
};

#endif