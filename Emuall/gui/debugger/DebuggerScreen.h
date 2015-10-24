#ifndef _DEBUGGERSCREEN_H
#define _DEBUGGERSCREEN_H

#include "DebuggerElement.h"
#include <wx/wx.h>
#include "../GLPane.h"
#include "../../util/xml/pugixml.hpp"
#include "../../Emulator/Emulator.h"

namespace Debugger {
	class DebuggerScreen : public DebuggerElement, public GLPaneI
	{
	public:
		DebuggerScreen(Emulator *emu, const pugi::xml_node &node);
		virtual ~DebuggerScreen();

		void UpdateInfo();
		GLPane *GetWidget(wxWindow *parent, wxWindowID id);

	private:
		void DrawGL(int user);
		bool InitGL(int user);
		void DestroyGL(int user);
		void OnMotion(wxMouseEvent &evt);

		struct Item {
			int id;
			int width;
			int height;
			int mouseX;
			int mouseY;
		};

		Emulator *mEmu;
		GLPane *mWidget;
		Item mScreen;
	};
}

#endif