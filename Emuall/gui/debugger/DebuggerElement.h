#ifndef _DEBUGGERELEMENT_H
#define _DEBUGGERELEMENT_H

#include <wx/wx.h>
#include "../../util/xml/pugixml.hpp"

namespace Debugger
{
	class DebuggerElement
	{
	public:
		virtual ~DebuggerElement() {}
		virtual void UpdateInfo() = 0;
		virtual wxWindow *GetWidget(wxWindow *parent, wxWindowID id) = 0;
	};
}


#endif