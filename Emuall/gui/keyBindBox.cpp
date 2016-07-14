
#include "keyBindBox.h"
#include "../util/memDbg.h"

KeyBindBox::KeyBindBox(const std::string &emuName, int key, int idx, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name) :
	wxTextCtrl(parent, id, wxEmptyString, pos, size, style | wxTE_READONLY | wxWANTS_CHARS, wxDefaultValidator, name), _key(key), _emuname(emuName), _idx(idx)
{
}

KeyBindBox::~KeyBindBox()
{
	
}

int KeyBindBox::GetKey()
{
	return _key;
}


int KeyBindBox::GetIdx()
{
	return _idx;
}

std::string KeyBindBox::GetName()
{
	return _emuname;
}

