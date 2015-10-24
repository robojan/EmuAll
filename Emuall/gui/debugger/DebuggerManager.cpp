#include "DebuggerManager.h"

#include "DebuggerInfolist.h"
#include "DebuggerCheckbox.h"
#include "DebuggerRadioBox.h"
#include "DebuggerScreen.h"
#include "DebuggerNotebook.h"
#include "DebuggerBoxSizer.h"
#include "DebuggerGrid.h"
#include "DebuggerLabel.h"
#include "DebuggerColorbox.h"

using namespace Debugger;

DebuggerElement *Debugger::GetElement(Emulator *emu, pugi::xml_node &node)
{
	if (strcmp(node.name(), "infolist") == 0)
	{
		return new DebuggerInfoList(emu, node);
	}
	else if (strcmp(node.name(), "checkbox") == 0) {
		return new DebuggerCheckbox(emu, node, true, true);
	}
	else if (strcmp(node.name(), "flag") == 0)
	{
		return new DebuggerCheckbox(emu, node, true, false);
	}
	else if (strcmp(node.name(), "radio") == 0) {
		return new DebuggerRadioBox(emu, node);
	}
	else if (strcmp(node.name(), "screen") == 0) {
		return new DebuggerScreen(emu, node);
	}
	else if (strcmp(node.name(), "notebook") == 0) {
		return new DebuggerNotebook(emu, node);
	}
	else if (strcmp(node.name(), "hor") == 0) {
		return new DebuggerBoxSizer(emu, node, wxHORIZONTAL);
	}
	else if (strcmp(node.name(), "ver") == 0) {
		return new DebuggerBoxSizer(emu, node, wxVERTICAL);
	}
	else if (strcmp(node.name(), "grid") == 0) {
		return new DebuggerGrid(emu, node);
	} 
	else if (strcmp(node.name(), "label") == 0) {
		return new DebuggerLabel(emu, node);
	}
	else if (strcmp(node.name(), "colorbox") == 0) {
		return new DebuggerColorbox(emu, node);
	}
	else {
		Log(Warn, "Unknown element '%s'", node.name());
		return NULL;
	}
}