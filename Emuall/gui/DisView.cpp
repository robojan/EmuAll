
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
	mEmu = emu;
	mNumberOfLines = 0;
	mStartingLine = 0;
	mCurrentLine = 0;
	mEndingLine = 0;
	mStartingY = 0;
	mLineHeight = 0;
	this->SetScrollbar(wxVERTICAL, 50, 1, 100);
}


DisView::~DisView()
{
}

void DisView::Update(unsigned int size, int emuCurLineId)
{
	if (mEmu->emu != NULL)
	{
		mNumberOfLines = size;
		mEmuCurLineId = emuCurLineId;
		mCurrentLine = mEmu->emu->GetValU(mEmu->handle, mEmuCurLineId);
	}
	else {
		mEmuCurLineId = 0;
		mNumberOfLines = 0;
		mStartingLine = 0;
		mCurrentLine = 0;
		mEndingLine = 0;
	}
}

void DisView::FollowCurrentLine()
{
	if (mEmu->emu == NULL || mEmuCurLineId <= 0)
		return;
	mCurrentLine = mEmu->emu->GetValU(mEmu->handle, mEmuCurLineId);
	if (mCurrentLine < mStartingLine || mCurrentLine >= mEndingLine)
	{
		// Current line is outside view
		mStartingLine = mCurrentLine;
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
	mStartingY = infoCorner.y;
	mLineHeight = dc.GetCharHeight();
	addressRect.SetHeight(mLineHeight);
	rawRect.SetHeight(mLineHeight);
	instrRect.SetHeight(mLineHeight);

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
	if (mEmu->emu != NULL)
	{

		unsigned int address = mStartingLine;
		while (addressRect.GetBottom() < codeRect.GetBottom())
		{
			// Draw a line
			const char *raw;
			const char *instr;
			char size = mEmu->emu->Disassemble(mEmu->handle, address, &raw, &instr);

			if (address == mCurrentLine) // Draw currentLine icon
			{
				dc.SetBrush(arrowBrush);
				dc.SetPen(transparentPen);
				wxPoint arrowPoint(0, addressRect.GetTop()+2);
				dc.DrawRoundedRectangle(arrowPoint, wxSize(infoCorner.x, mLineHeight-4), 3);
				dc.DrawRectangle(addressRect);
				dc.DrawRectangle(rawRect);
				dc.DrawRectangle(instrRect);
				dc.SetPen(codeTextPen);
			}
			if (mEmu->emu->IsBreakpoint(mEmu->handle, address))
			{
				// Breakpoint
				wxPoint breakPoint(infoCorner.x / 2, addressRect.GetTop() + mLineHeight / 2);
				dc.SetBrush(breakBrush);
				dc.SetPen(transparentPen);
				dc.DrawCircle(breakPoint, mLineHeight / 2);
				dc.SetPen(codeTextPen);
			}

			dc.DrawText(wxString::Format("0x%08X", address), addressRect.GetLeftTop());
			dc.DrawText(raw, rawRect.GetLeftTop());
			dc.DrawText(instr, instrRect.GetLeftTop());

			address += size;
			address %= mNumberOfLines;
			addressRect.Offset(0, addressRect.GetHeight());
			rawRect.Offset(0, rawRect.GetHeight());
			instrRect.Offset(0, instrRect.GetHeight());
		}
		mEndingLine = address;
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
	if(evt.GetOrientation() != wxSB_VERTICAL || mEmu->emu == NULL)
		return;
	static long lockedLine = 0;
	static bool scrolling = false;
	wxEventType type = evt.GetEventType();
	if (type == wxEVT_SCROLLWIN_THUMBTRACK)
	{
		if (!scrolling)
		{
			lockedLine = mStartingLine;
			scrolling = true;
		}
		int pos = evt.GetPosition()-50;
		int scrollAmount = pos*(mNumberOfLines/50);
		// if the result would wrap around
		if (lockedLine < -scrollAmount)
			mStartingLine = lockedLine + scrollAmount + mNumberOfLines;
		else
			mStartingLine = (lockedLine + scrollAmount);		
		mStartingLine %= mNumberOfLines;
	} else if (type == wxEVT_SCROLLWIN_THUMBRELEASE) {
		scrolling = false;
		lockedLine = 0;
	} else if (type == wxEVT_SCROLLWIN_LINEUP)
	{
		if (mStartingLine == 0)
			mStartingLine = mNumberOfLines;
		mStartingLine -= 1;
	} else if (type == wxEVT_SCROLLWIN_LINEDOWN)
	{
		mStartingLine += mEmu->emu->Disassemble(mEmu->handle, mStartingLine, NULL, NULL);
		mStartingLine %= mNumberOfLines;
	} else if (type == wxEVT_SCROLLWIN_PAGEUP)
	{
		if (mStartingLine < 50)
			mStartingLine += mNumberOfLines;
		mStartingLine -= 50;
	} else if (type == wxEVT_SCROLLWIN_PAGEDOWN)
	{
		mStartingLine += 50;
		mStartingLine %= mNumberOfLines;
	}
	Refresh();
}

void DisView::OnDClick(wxMouseEvent &evt)
{
	if (mEmu->emu == NULL || mLineHeight == 0)
		return;
	long x, y;
	int lineNr;
	unsigned int line = mStartingLine;
	// Get line
	evt.GetPosition(&x, &y);
	for (lineNr = (y - mStartingY) / mLineHeight; lineNr; --lineNr)
	{
		line += mEmu->emu->Disassemble(mEmu->handle, line, NULL, NULL);
	}
	// Check if breakpoint already exists
	if (mEmu->emu->IsBreakpoint(mEmu->handle, line))
	{
		// remove breakpoint
		mEmu->emu->RemoveBreakpoint(mEmu->handle, line);
	}
	else {
		// Add breakpoint
		mEmu->emu->AddBreakpoint(mEmu->handle, line);
	}
	Refresh();
}

wxSize DisView::DoGetBestClientSize() const
{
	return wxSize (450, 100);
}