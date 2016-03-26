#ifndef _KEYBINDBOX_H_
#define _KEYBINDBOX_H_

#include <wx/wx.h>
#include "../util/Options.h"

class KeyBindBox : public wxTextCtrl
{
public:
	KeyBindBox(const std::string &emuName, int key, wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0L, const wxString &name = wxTextCtrlNameStr);
	~KeyBindBox();

	std::string GetName();
	int GetKey();

private:
	int _key;
	std::string _emuname;
};

#endif