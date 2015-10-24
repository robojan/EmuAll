
#include "keyBindBox.h"
#include "../util/memDbg.h"

KeyBindBox::KeyBindBox(const std::string &emuName, int key, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name) :
	wxTextCtrl(parent, id, wxEmptyString, pos, size, style | wxTE_READONLY | wxWANTS_CHARS, wxDefaultValidator, name)
{
	mKey = key;
	mEmuname = emuName;
}

KeyBindBox::~KeyBindBox()
{
	
}

int KeyBindBox::GetKey()
{
	return mKey;
}

std::string KeyBindBox::GetName()
{
	return mEmuname;
}

