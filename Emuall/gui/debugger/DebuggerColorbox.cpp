#include "DebuggerColorbox.h"

using namespace Debugger;

DebuggerColorbox::DebuggerColorbox(Emulator *emu, const pugi::xml_node &node) :
_emu(emu), _widgets(NULL)
{
	wxASSERT(strcmp(node.name(), "colorbox") == 0);
	wxASSERT(emu != NULL);
	_id = node.attribute("id").as_int(-1);
}

DebuggerColorbox::~DebuggerColorbox()
{

}

wxPanel *DebuggerColorbox::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widgets != NULL)
	{
		return _widgets; // Widget already created
	}

	_widgets = new wxPanel(parent, id, wxDefaultPosition, wxSize(20, 20), wxBORDER_SUNKEN);
	_widgets->SetMinSize(wxSize(20, 20));
	_widgets->SetBackgroundColour(wxColour((unsigned long)0x000000)); // black
	UpdateInfo();
	return _widgets;
}

void DebuggerColorbox::UpdateInfo()
{
	if (_emu->emu == NULL || _widgets == NULL)
	{
		return; // Nothing to do
	}

	if (_widgets->IsShownOnScreen())
	{
		_widgets->SetBackgroundColour(wxColour((unsigned long)_emu->emu->GetValU(_emu->handle, _id)));
		_widgets->Refresh();
	}
}