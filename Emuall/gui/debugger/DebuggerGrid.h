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
		Emulator *mEmu;
		wxWindow *mWidget;
		wxString mName;
		int mRows;
		int mColumns;
		std::vector<Item> mItems;
	};
}

#endif