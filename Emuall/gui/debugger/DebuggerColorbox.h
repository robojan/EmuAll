#ifndef _DEBUGGERCOLORBOX_H
#define _DEBUGGERCOLORBOX_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"


namespace Debugger
{

	class DebuggerColorbox : public DebuggerElement
	{
	public:
		DebuggerColorbox(Emulator *emu, const pugi::xml_node &node);
		virtual ~DebuggerColorbox();

		void UpdateInfo();
		wxPanel *GetWidget(wxWindow *parent, wxWindowID id);

	private:

		Emulator *_emu;
		wxPanel *_widgets;
		int _id;
	};

}

#endif