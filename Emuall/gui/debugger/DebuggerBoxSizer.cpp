#include "DebuggerBoxSizer.h"
#include "DebuggerManager.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerBoxSizer::DebuggerBoxSizer(Emulator *emu, const pugi::xml_node &node, int orient) :
_emu(emu), _widget(NULL), _orient(orient)
{
	wxASSERT(strcmp(node.name(), "hor") == 0 || strcmp(node.name(), "ver") == 0);
	wxASSERT(emu != NULL);

	pugi::xml_object_range<pugi::xml_node_iterator> children = node.children();
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

DebuggerBoxSizer::~DebuggerBoxSizer()
{
	for (std::vector<Item>::iterator iElement = _items.begin();
		iElement != _items.end(); ++iElement)
	{
		delete iElement->element;
	}
	_items.clear();
}

wxPanel *DebuggerBoxSizer::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widget != NULL)
	{
		return _widget; // Widget already created
	}

	_widget = new wxPanel(parent, id);
	wxBoxSizer *sizer = new wxBoxSizer(_orient);
	for (std::vector<Item>::iterator iItem = _items.begin();
		iItem != _items.end(); ++iItem)
	{
		sizer->Add(iItem->element->GetWidget(_widget, wxID_ANY), iItem->proportion, wxEXPAND | wxALL, 5);
	}
	_widget->SetSizerAndFit(sizer);
		
	UpdateInfo();
	return _widget;
}

void DebuggerBoxSizer::UpdateInfo()
{
	if (_emu->emu == NULL || _widget == NULL)
	{
		return; // Nothing to do
	}


	for (std::vector<Item>::iterator iItem = _items.begin();
		iItem != _items.end(); ++iItem)
	{
		iItem->element->UpdateInfo();
	}
}