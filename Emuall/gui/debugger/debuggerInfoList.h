#ifndef _DEBUGGERINFOLIST_H
#define _DEBUGGERINOFLIST_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <map>
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"

namespace Debugger {
	class DebuggerInfoList : public DebuggerElement
	{
	public:
		DebuggerInfoList(Emulator *emu, const pugi::xml_node &node);
		virtual ~DebuggerInfoList();

		void UpdateInfo();
		wxListCtrl *GetWidget(wxWindow *parent, wxWindowID id);

	private:
		struct Item {
			enum Mode {
				dec,
				hex,
				oct,
				flag,
				string, 
				fp,
			};
			wxString name;
			wxString format;
			int id;
			int size;
			bool readOnly;
			Mode mode;
		};

		Emulator *_emu;
		wxListCtrl *_widget;
		std::map<int, Item> _items;
		int _width;

	};
}


#endif