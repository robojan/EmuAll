
#include "HexView.h"
#include "../util/memDbg.h"

wxChar ToPrintable(uint8_t c)
{
	if (c < 0x20 || c >= 0x7F && c <= 0x9F)
	{
		return '.';
	}
	return c;
}

HexView::HexView(HexViewI *handler, wxUint32 size) : wxGridTableBase()
{
	mHandler = handler;
	mSize = size;
	mOffset = 0;
}

HexView::~HexView()
{
	if (GetView())
	{
		GetView()->SetTable(NULL);
	}
}

int HexView::GetNumberRows()
{
	return (mSize+0xf)/0x10;
}

int HexView::GetNumberCols()
{
	// Address 0-F ASCII
	return 0x10+1;
}

wxString HexView::GetColLabelValue(int col)
{
	if(col == GetNumberCols()-1)
	{
		return _("ASCII");
	}
	return wxString::Format("%X", col);
}

wxString HexView::GetRowLabelValue(int row)
{
	return wxString::Format("0x%X0", row+mOffset);
}

wxString HexView::GetValue( int row, int col)
{
	if(mHandler == NULL)
	{
		return _("-");
	}
	if(col == GetNumberCols()-1)
	{
		// ascii representation
		wxString ascii;
		for (int i = 0; i < 0x10; ++i)
		{
			if (i == 8)
				ascii.Append(' ');
			if ((((unsigned int)row << 4) + i) >= mSize)
			{
				ascii.Append(' ');
				continue;
			}
			ascii.Append(ToPrintable(mHandler->OnGetValue((row << 4) + i)));

		}
		return ascii;
	}
	if ((((unsigned int) row << 4) + col) >= mSize)
		return wxEmptyString;
	return wxString::Format("%02X", mHandler->OnGetValue((row << 4) + col));
}

void HexView::SetValue(int row, int col, const wxString &data)
{
	if(mHandler == NULL || col == 0x10)
	{
		return;
	}
	long val;
	data.ToLong(&val, 16);
	mHandler->OnSetValue(mOffset*0x10 + row*0x10+col, (wxByte)(val&0xFF));
}

bool HexView::IsEmptyCell(int, int)
{
	return true;
}

void HexView::SetSize(wxUint32 size)
{
	mSize = size;
	if (GetView())
	{
		wxGrid *grid = GetView();
		grid->SetTable(this, false); // Force update table
		grid->EnableEditing(true);
		grid->EnableGridLines(true);
		grid->EnableDragGridSize(false);
		grid->SetMargins(0, 0);
		grid->EnableDragColMove(false);
		grid->EnableDragColSize(false);
		grid->SetColLabelSize(30);
		grid->SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
		grid->EnableDragRowSize(false);
		grid->SetRowLabelSize(70);
		grid->SetRowLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
		grid->SetDefaultColSize(25, true);
		grid->SetColumnWidth(0x10, 170);
		grid->SetDefaultCellFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	}
}

void HexView::SetOffset(wxUint32 offset)
{
	mOffset = offset;
}