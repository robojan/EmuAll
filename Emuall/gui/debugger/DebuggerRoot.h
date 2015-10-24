#ifndef _DEBUGGERROOT_H
#define _DEBUGGERROOT_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include <vector>
#include "../../util/xml/pugixml.hpp"

class Emulator;

namespace Debugger
{

	class DebuggerRoot : public wxEvtHandler, public DebuggerElement
	{
	public:
		DebuggerRoot(Emulator *emu, const pugi::xml_node &node);
		virtual ~DebuggerRoot();

		void UpdateInfo();
		wxPanel *GetWidget(wxWindow *parent, wxWindowID id);

	private:
		struct Item
		{
			DebuggerElement *element;
			int proportion;
		};
		Emulator *mEmu;
		wxPanel *mWidget;
		std::vector<Item> mItems;
	};
}

#endif