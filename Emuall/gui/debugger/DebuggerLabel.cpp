#include "DebuggerLabel.h"

using namespace Debugger;

DebuggerLabel::DebuggerLabel(Emulator *emu, const pugi::xml_node &node) :
_emu(emu), _widget(NULL)
{
	wxASSERT(strcmp(node.name(), "label") == 0);
	wxASSERT(emu != NULL);
	_label = node.text().as_string("Error!");
}

DebuggerLabel::~DebuggerLabel()
{

}

wxStaticText *DebuggerLabel::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widget != NULL)
	{
		return _widget; // Widget already created
	}

	_widget = new wxStaticText(parent, id, _label);

	UpdateInfo();
	return _widget;
}

void DebuggerLabel::UpdateInfo()
{
	return; // nothing to do with static text
}