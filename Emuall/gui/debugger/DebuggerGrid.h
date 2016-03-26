#ifndef _DEBUGGERGRID_H
#define _DEBUGGERGRID_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"


namespace Debugger
{

	class DebuggerGrid : public DebuggerElement
	{
	public:
		DebuggerGrid(Emulator *emu, const pugi::xml_node &node);
		virtual ~DebuggerGrid();

		void UpdateInfo();
		wxWindow *GetWidget(wxWindow *parent, wxWindowID id);

	private:
		struct Item
		{
			DebuggerElement *element;
			int proportion;
		};
		Emulator *_emu;
		wxWindow *_widget;
		wxString _name;
		int _rows;
		int _colums;
		std::vector<Item> _items;
	};
}

#endif