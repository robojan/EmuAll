#ifndef _DEBUGGERBOXSIZER_H
#define _DEBUGGERBOXSIZER_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"


namespace Debugger
{

	class DebuggerBoxSizer : public DebuggerElement
	{
	public:
		DebuggerBoxSizer(Emulator *emu, const pugi::xml_node &node, int orient);
		virtual ~DebuggerBoxSizer();

		void UpdateInfo();
		wxPanel *GetWidget(wxWindow *parent, wxWindowID id);

	private:
		struct Item
		{
			DebuggerElement *element;
			int proportion;
		};
		Emulator *_emu;
		wxPanel *_widget;
		int _orient;
		std::vector<Item> _items;
	};
}

#endif