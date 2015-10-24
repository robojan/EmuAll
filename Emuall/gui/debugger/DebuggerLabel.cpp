#include "DebuggerLabel.h"

using namespace Debugger;

DebuggerLabel::DebuggerLabel(Emulator *emu, const pugi::xml_node &node) :
mEmu(emu), mWidget(NULL)
{
	wxASSERT(strcmp(node.name(), "label") == 0);
	wxASSERT(emu != NULL);
	mLabel = node.text().as_string("Error!");
}

DebuggerLabel::~DebuggerLabel()
{

}

wxStaticText *DebuggerLabel::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	mWidget = new wxStaticText(parent, id, mLabel);

	UpdateInfo();
	return mWidget;
}

void DebuggerLabel::UpdateInfo()
{
	return; // nothing to do with static text
}