#ifndef _HEXVIEW_H
#define _HEXVIEW_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <stdint.h>

class HexViewI
{
	friend class HexView;
private:
	virtual wxByte OnGetValue(int address) = 0;
	virtual void OnSetValue(int address, wxByte val) = 0;
};

class HexView : public wxGridTableBase
{
public:
	HexView(HexViewI *handler, wxUint32 size);
	~HexView();
	int GetNumberRows();
	int GetNumberCols();
	wxString GetColLabelValue(int col);
	wxString GetRowLabelValue(int row);
	wxString GetValue( int row, int col);
	void SetValue(int , int , const wxString& );
	bool IsEmptyCell(int, int);
	void SetSize(wxUint32 size);
	wxUint32 GetSize(void) { return mSize; }
	void SetOffset(wxUint32 offset);
	wxUint32 GetOffset(void) { return mOffset; }
private:
	HexViewI		*mHandler;
	wxUint32		mSize;
	wxUint32		mOffset;
};

#endif