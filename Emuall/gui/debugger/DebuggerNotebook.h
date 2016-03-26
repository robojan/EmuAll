#ifndef _DEBUGGERNOTEBOOK_H
#define _DEBUGGERNOTEBOOK_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include <wx/notebook.h>
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"


namespace Debugger
{

	class DebuggerNotebook : public wxEvtHandler, public DebuggerElement
	{
	public:
		DebuggerNotebook(Emulator *emu, const pugi::xml_node &node);
		virtual ~DebuggerNotebook();

		void UpdateInfo();
		wxNotebook *GetWidget(wxWindow *parent, wxWindowID id);

	private:
		struct Item {
			std::string name;
			std::vector<DebuggerElement *> elements;
		};

		Emulator *_emu;
		wxNotebook *_widget;
		std::vector<Item> _items;
	};
}

#endif