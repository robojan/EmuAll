#ifndef _DEBUGGERCHECKBOX_H
#define _DEBUGGERCHECKBOX_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"


namespace Debugger
{

	class DebuggerCheckbox : public wxEvtHandler, public DebuggerElement
	{
	public:
		DebuggerCheckbox(Emulator *emu, const pugi::xml_node &node, bool read = true, bool write = true);
		virtual ~DebuggerCheckbox();

		void UpdateInfo();
		wxCheckBox *GetWidget(wxWindow *parent, wxWindowID id);

	private:
		void OnClicked(wxCommandEvent &evt);

		struct Item {
			int id;
			std::string name;
			bool read;
			bool write;
		};

		Emulator *mEmu;
		wxCheckBox *mWidget;
		Item mCheckbox;
	};

}

#endif