
#include "DisView.h"
#include "../util/memDbg.h"
#include "../util/log.h"

IMPLEMENT_CLASS(DisView,wxWindow)
BEGIN_EVENT_TABLE(DisView,wxWindow)
	EVT_PAINT(DisView::OnPaint)
	EVT_SCROLLWIN(DisView::OnScroll)
	EVT_LEFT_DCLICK(DisView::OnDClick)
END_EVENT_TABLE()

DisView::DisView(Emulator *emu, wxWindow *parent, int windowId, const wxPoint &pos, const wxSize &size, long style, const wxString &name) :
	wxWindow(parent, windowId, pos, size, style)
{
	_emu = emu;
	_numberOfLines = 0;
	_startingLine = 0;
	_currentLine = 0;
	_endingLine = 0;
	_startingY = 0;
	_lineHeight = 0;
	this->SetScrollbar(wxVERTICAL, 50, 1, 100);
}


DisView::~DisView()
{
}

void DisView::Update(unsigned int size, int emuCurLineId)
{
	if (_emu->emu != NULL)
	{
		_numberOfLines = size;
		_emuCurLineId = emuCurLineId;
		_currentLine = _emu->emu->GetValU(_emu->handle, _emuCurLineId);
	}
	else {
		_emuCurLineId = 0;
		_numberOfLines = 0;
		_startingLine = 0;
		_currentLine = 0;
		_endingLine = 0;
	}
}

void DisView::FollowCurrentLine()
{
	if (_emu->emu == NULL || _emuCurLineId <= 0)
		return;
	_currentLine = _emu->emu->GetValU(_emu->handle, _emuCurLineId);
	if (_currentLine < _startingLine || _currentLine >= _endingLine)
	{
		// Current line is outside view
		_startingLine = _currentLine;
	}
}


void DisView::OnPaint(wxPaintEvent &evt)
{
	wxPaintDC dc(this);
	// Initialize tools
	wxBrush infoBrush(wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_INACTIVECAPTION), wxBRUSHSTYLE_SOLID);
	wxBrush arrowBrush(wxColour(0xF7, 0xEA, 0x59));
	wxBrush breakBrush(wxColour(0xFC, 0x0D, 0x00));
	wxFont infoFont(wxSystemSettings::GetFont(wxSystemFont::wxSYS_SYSTEM_FONT));
	wxFont codeFont(wxSystemSettings::GetFont(wxSystemFont::wxSYS_ANSI_FIXED_FONT));
	wxPen transparentPen(wxColour(), 0, wxPenStyle::wxPENSTYLE_TRANSPARENT);
	wxPen infoTextPen(wxSystemSettings::GetColour(wxSystemColour::wxSYS_COLOUR_INFOTEXT));
	wxPen codeTextPen = infoTextPen;
	dc.SetFont(infoFont);
	dc.SetPen(infoTextPen);

	// Initialize sizes
	wxRect clientRect(dc.GetSize());
	wxRect codeRect = clientRect;
	wxPoint infoCorner(20, dc.GetCharHeight() + 10);
	wxRect addressLabelRect(infoCorner.x, 0, 0, infoCorner.y);
	wxRect rawLabelRect = addressLabelRect;
	wxRect instrLabelRect = rawLabelRect;

	codeRect.SetTopLeft(infoCorner);
	addressLabelRect.SetWidth(dc.GetFontMetrics().averageWidth*16);
	rawLabelRect.Offset(addressLabelRect.GetWidth(), 0);
	rawLabelRect.SetWidth(dc.GetFontMetrics().averageWidth * 18);
	instrLabelRect.SetLeft(rawLabelRect.GetRight());
	instrLabelRect.SetRight(codeRect.GetRight());
	wxRect addressRect = addressLabelRect;
	wxRect rawRect = rawLabelRect;
	wxRect instrRect = instrLabelRect;
	addressRect.Offset(0, addressLabelRect.GetHeight());
	rawRect.Offset(0, rawRect.GetHeight());
	instrRect.Offset(0, instrRect.GetHeight());
	dc.SetFont(codeFont);
	_startingY = infoCorner.y;
	_lineHeight = dc.GetCharHeight();
	addressRect.SetHeight(_lineHeight);
	rawRect.SetHeight(_lineHeight);
	instrRect.SetHeight(_lineHeight);

	// Clear background
	//ClearBackground();

	// Paint info bars
	dc.SetBrush(infoBrush);
	dc.SetFont(infoFont);
	dc.SetPen(transparentPen);
	dc.DrawRectangle(0,0, dc.GetSize().GetWidth(), infoCorner.y);
	dc.DrawRectangle(0, infoCorner.y, infoCorner.x, clientRect.GetHeight()-infoCorner.y);
	dc.SetPen(infoTextPen);
	wxPoint labelVertCenterOffset(0,(addressLabelRect.GetHeight() - dc.GetCharHeight())/2);
	dc.DrawText(_("Address:"), addressLabelRect.GetLeftTop() + labelVertCenterOffset);
	dc.DrawText(_("Raw:"), rawLabelRect.GetLeftTop() + labelVertCenterOffset);
	dc.DrawText(_("Instruction:"), instrLabelRect.GetLeftTop() + labelVertCenterOffset);

	// Paint code section
	dc.SetFont(codeFont);
	dc.SetPen(codeTextPen);
	if (_emu->emu != NULL && _numberOfLines > 0)
	{

		unsigned int address = _startingLine;
		while (addressRect.GetBottom() < codeRect.GetBottom())
		{
			// Draw a line
			const char *raw;
			const char *instr;
			char size = _emu->emu->Disassemble(_emu->handle, address, &raw, &instr);

			if (address == _currentLine) // Draw currentLine icon
			{
				dc.SetBrush(arrowBrush);
				dc.SetPen(transparentPen);
				wxPoint arrowPoint(0, addressRect.GetTop()+2);
				dc.DrawRoundedRectangle(arrowPoint, wxSize(infoCorner.x, _lineHeight-4), 3);
				dc.DrawRectangle(addressRect);
				dc.DrawRectangle(rawRect);
				dc.DrawRectangle(instrRect);
				dc.SetPen(codeTextPen);
			}
			if (_emu->emu->IsBreakpoint(_emu->handle, address))
			{
				// Breakpoint
				wxPoint breakPoint(infoCorner.x / 2, addressRect.GetTop() + _lineHeight / 2);
				dc.SetBrush(breakBrush);
				dc.SetPen(transparentPen);
				dc.DrawCircle(breakPoint, _lineHeight / 2);
				dc.SetPen(codeTextPen);
			}

			dc.DrawText(wxString::Format("0x%08X", address), addressRect.GetLeftTop());
			dc.DrawText(raw, rawRect.GetLeftTop());
			dc.DrawText(instr, instrRect.GetLeftTop());

			address += size;
			address %= _numberOfLines;
			addressRect.Offset(0, addressRect.GetHeight());
			rawRect.Offset(0, rawRect.GetHeight());
			instrRect.Offset(0, instrRect.GetHeight());
		}
		_endingLine = address;
	}
	else {
		// Draw no emulator loaded text
		wxPoint centerCode(codeRect.GetBottomLeft());
		centerCode.y -= codeRect.GetHeight()/2;
		dc.DrawText(_("There is no emulator loaded"),centerCode);
	}
}

void DisView::OnScroll(wxScrollWinEvent &evt)
{
	if(evt.GetOrientation() != wxSB_VERTICAL || _emu->emu == NULL)
		return;
	static long lockedLine = 0;
	static bool scrolling = false;
	wxEventType type = evt.GetEventType();
	if (type == wxEVT_SCROLLWIN_THUMBTRACK)
	{
		if (!scrolling)
		{
			lockedLine = _startingLine;
			scrolling = true;
		}
		int pos = evt.GetPosition()-50;
		int scrollAmount = pos*(_numberOfLines/50);
		// if the result would wrap around
		if (lockedLine < -scrollAmount)
			_startingLine = lockedLine + scrollAmount + _numberOfLines;
		else
			_startingLine = (lockedLine + scrollAmount);		
		_startingLine %= _numberOfLines;
	} else if (type == wxEVT_SCROLLWIN_THUMBRELEASE) {
		scrolling = false;
		lockedLine = 0;
	} else if (type == wxEVT_SCROLLWIN_LINEUP)
	{
		if (_startingLine == 0)
			_startingLine = _numberOfLines;
		_startingLine -= 1;
	} else if (type == wxEVT_SCROLLWIN_LINEDOWN)
	{
		_startingLine += _emu->emu->Disassemble(_emu->handle, _startingLine, NULL, NULL);
		_startingLine %= _numberOfLines;
	} else if (type == wxEVT_SCROLLWIN_PAGEUP)
	{
		if (_startingLine < 50)
			_startingLine += _numberOfLines;
		_startingLine -= 50;
	} else if (type == wxEVT_SCROLLWIN_PAGEDOWN)
	{
		_startingLine += 50;
		_startingLine %= _numberOfLines;
	}
	Refresh();
}

void DisView::OnDClick(wxMouseEvent &evt)
{
	if (_emu->emu == NULL || _lineHeight == 0)
		return;
	long x, y;
	int lineNr;
	unsigned int line = _startingLine;
	// Get line
	evt.GetPosition(&x, &y);
	for (lineNr = (y - _startingY) / _lineHeight; lineNr; --lineNr)
	{
		line += _emu->emu->Disassemble(_emu->handle, line, NULL, NULL);
	}
	// Check if breakpoint already exists
	if (_emu->emu->IsBreakpoint(_emu->handle, line))
	{
		// remove breakpoint
		_emu->emu->RemoveBreakpoint(_emu->handle, line);
	}
	else {
		// Add breakpoint
		_emu->emu->AddBreakpoint(_emu->handle, line);
	}
	Refresh();
}

wxSize DisView::DoGetBestClientSize() const
{
	return wxSize (450, 100);
}