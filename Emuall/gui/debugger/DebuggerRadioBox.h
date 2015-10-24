#ifndef _DEBUGGERRADIOBOX_H
#define _DEBUGGERRADIOBOX_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"

namespace Debugger {

	class DebuggerRadioBox : public wxEvtHandler, public DebuggerElement
	{
	public:
		DebuggerRadioBox(Emulator *emu, const pugi::xml_node &node);
		virtual ~DebuggerRadioBox();

		void UpdateInfo();
		wxRadioBox *GetWidget(wxWindow *parent, wxWindowID id);

	private:
		void OnClicked(wxCommandEvent &evt);

		struct Item {
			int id;
			std::string name;
			bool readOnly;
			std::map<std::string, int> options;
		};

		Emulator *mEmu;
		wxRadioBox *mWidget;
		Item mItem;
	};
}



#endif