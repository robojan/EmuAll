#ifndef _DEBUGGERMANAGER_H
#define _DEBUGGERMANAGER_H

#include "../../util/log.h"
#include "../../util/xml/pugixml.hpp"
#include "DebuggerElement.h"
class Emulator;

namespace Debugger {
	DebuggerElement *GetElement(Emulator *emu, pugi::xml_node &node);
}

#endif