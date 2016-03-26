#include "DebuggerRoot.h"
#include "DebuggerManager.h"
#include "../../util/log.h"
#include "../../Emulator/Emulator.h"

using namespace Debugger;

DebuggerRoot::DebuggerRoot(Emulator *emu, const pugi::xml_node &node) :
_emu(emu), _widget(NULL)
{
	wxASSERT(emu != NULL);

	pugi::xml_object_range<pugi::xml_node_iterator> children = node.children();
	int pos = 0;
	for (pugi::xml_node_iterator iChild = children.begin(); iChild != children.end(); ++iChild)
	{
		Item item;
		item.element = GetElement(emu, *iChild);
		item.proportion = iChild->attribute("prop").as_int(0);
		if (item.element != NULL)
		{
			_items.push_back(item);
		}
	}
}

DebuggerRoot::~DebuggerRoot()
{
	for (std::vector<Item>::iterator iItem = _items.begin();
		iItem != _items.end(); ++iItem)
	{
		delete iItem->element;
	}
	_items.clear();
}

wxPanel *DebuggerRoot::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widget != NULL)
	{
		return _widget; // Widget already created
	}

	_widget = new wxPanel(parent, id);
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	for (std::vector<Item>::iterator iItem = _items.begin();
		iItem != _items.end(); ++iItem)
	{
		sizer->Add(iItem->element->GetWidget(_widget, wxID_ANY), iItem->proportion, wxEXPAND | wxALL, 5);
	}
	_widget->SetSizerAndFit(sizer);

	UpdateInfo();
	return _widget;
}

void DebuggerRoot::UpdateInfo()
{
	if (_emu->emu == NULL || _widget == NULL)
	{
		return; // Nothing to do
	}

	if (_widget->IsShownOnScreen())
	{
		for (std::vector<Item>::iterator iItem = _items.begin();
			iItem != _items.end(); ++iItem)
		{
			iItem->element->UpdateInfo();
		}
	}
}