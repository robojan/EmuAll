#include "DebuggerColorbox.h"

using namespace Debugger;

DebuggerColorbox::DebuggerColorbox(Emulator *emu, const pugi::xml_node &node) :
mEmu(emu), mWidget(NULL)
{
	wxASSERT(strcmp(node.name(), "colorbox") == 0);
	wxASSERT(emu != NULL);
	mID = node.attribute("id").as_int(-1);
}

DebuggerColorbox::~DebuggerColorbox()
{

}

wxPanel *DebuggerColorbox::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	mWidget = new wxPanel(parent, id, wxDefaultPosition, wxSize(20, 20), wxBORDER_SUNKEN);
	mWidget->SetMinSize(wxSize(20, 20));
	mWidget->SetBackgroundColour(wxColour((unsigned long)0x000000)); // black
	UpdateInfo();
	return mWidget;
}

void DebuggerColorbox::UpdateInfo()
{
	if (mEmu->emu == NULL || mWidget == NULL)
	{
		return; // Nothing to do
	}

	if (mWidget->IsShownOnScreen())
	{
		mWidget->SetBackgroundColour(wxColour((unsigned long)mEmu->emu->GetValU(mEmu->handle, mID)));
		mWidget->Refresh();
	}
}