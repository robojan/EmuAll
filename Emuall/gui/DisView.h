#ifndef _DISVIEW_H_
#define _DISVIEW_H_

#include <wx/wx.h>
#include "../Emulator/Emulator.h"

class DisView :
	public wxWindow
{
	DECLARE_CLASS(DisView);
	DECLARE_EVENT_TABLE();
public:
	DisView(Emulator *emu, wxWindow *parent, int windowId = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxFULL_REPAINT_ON_RESIZE | wxVSCROLL, const wxString &name = wxPanelNameStr);
	~DisView();

	void Update(unsigned int size, int emuCurLineId);
	void FollowCurrentLine();
protected:
	void OnPaint(wxPaintEvent &evt);
	void OnScroll(wxScrollWinEvent &evt);
	void OnDClick(wxMouseEvent &evt);
	wxSize DoGetBestClientSize() const;
private:
	int _emuCurLineId;
	unsigned int _numberOfLines;
	unsigned int _startingLine;
	unsigned int _endingLine;
	unsigned int _currentLine;
	int _startingY;
	int _lineHeight;
	Emulator *_emu;
};

#endif

