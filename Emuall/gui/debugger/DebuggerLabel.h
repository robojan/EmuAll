#ifndef _DEBUGGERLABEL_H
#define _DEBUGGERLABEL_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"


namespace Debugger
{

	class DebuggerLabel : public wxEvtHandler, public DebuggerElement
	{
	public:
		DebuggerLabel(Emulator *emu, const pugi::xml_node &node);
		virtual ~DebuggerLabel();

		void UpdateInfo();
		wxStaticText *GetWidget(wxWindow *parent, wxWindowID id);

	private:

		Emulator *_emu;
		wxStaticText *_widget;
		wxString _label;
	};

}

#endif