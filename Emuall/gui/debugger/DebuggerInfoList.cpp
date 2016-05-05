
#include "debuggerInfoList.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerInfoList::DebuggerInfoList(Emulator *emu, const pugi::xml_node &node) :
	_emu(emu), _widget(NULL), _width(-1)
{
	// Create an internal representation of the data
	wxASSERT(strcmp(node.name(), "infolist") == 0);
	wxASSERT(emu != NULL);
	_width = node.attribute("width").as_int(-1);
	pugi::xml_object_range<pugi::xml_node_iterator> children = node.children();
	int i = 0;
	for (pugi::xml_node_iterator iChild = children.begin(); iChild != children.end(); ++iChild)
	{
		if (strcmp(iChild->name(), "info") == 0)
		{
			Item item;
			item.name = iChild->child("name").text().as_string("Error!");
			item.id = iChild->attribute("id").as_int(-1);
			item.size = iChild->child("size").text().as_int(0);
			item.readOnly = iChild->child("readonly").text().as_bool(true);

			wxString mode = iChild->child("mode").text().as_string("dec");

			if (mode == "dec")
			{
				item.mode = Item::dec;
				item.format = wxString::Format("%%0%dd", item.size);
			}
			else if (mode == "hex") {
				item.mode = Item::hex;
				item.format = wxString::Format("0x%%0%dX", item.size);
			}
			else if (mode == "string") {
				item.mode = Item::string;
				item.format = _("%s");
			}
			else if (mode == "flag") {
				item.mode = Item::flag;
				item.format = _("%d");
			}
			else if (mode == "oct") {
				item.mode = Item::oct;
				item.format = wxString::Format("0o%%0%do", item.size);
			}
			else {
				Log(Warn, "Unknown info type \"%s\" found", mode.c_str().AsChar());
				continue;
			}
			_items[i++] = item;
		}
		else if (strcmp(iChild->name(), "flag") == 0)
		{
			Item item;
			item.name = iChild->child("name").text().as_string("Error!");
			item.id = iChild->attribute("id").as_int(-1);
			item.readOnly = iChild->child("readonly").text().as_bool(true);
			item.mode = Item::flag;
			item.format = _("%d");
			_items[i++] = item;
		}
		else {
			Log(Warn, "Unknown element \"%s\" in InfoList", iChild->name());
		}
	}
}

DebuggerInfoList::~DebuggerInfoList()
{

}

wxListCtrl *DebuggerInfoList::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widget != NULL)
		return _widget;

	// Create the widget
	std::map<int, Item> newMap;
	wxSize size(_width, -1);
	_widget = new wxListCtrl(parent, id, wxDefaultPosition, size, wxLC_HRULES | wxLC_NO_SORT_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL | wxHSCROLL);
	_widget->AppendColumn("Name");
	_widget->AppendColumn("Value", wxLIST_FORMAT_LEFT);
	_widget->SetMinClientSize(size);

	std::map<int, Item>::iterator iItem;
	for (iItem = _items.begin(); iItem != _items.end(); ++iItem)
	{
		long pos = _widget->InsertItem(_widget->GetItemCount(), iItem->second.name);
		newMap[pos] = iItem->second;

		if (iItem->second.mode == Item::string)
		{
			_widget->SetItem(pos, 1, wxString::Format(iItem->second.format, ""));
		}
		else {
			_widget->SetItem(pos, 1, wxString::Format(iItem->second.format, 0));
		}
	}

	// Replace the map
	_items = newMap;
	// Update the widget with the correct values
	UpdateInfo();
	return _widget;
}

void DebuggerInfoList::UpdateInfo()
{
	if (_widget == NULL)
		return; // Nothing to update

	if (_widget->IsShownOnScreen())
	{
		if (_emu->emu != NULL)
		{
			for (std::map<int, Item>::iterator iItem = _items.begin(); iItem != _items.end(); ++iItem)
			{
				wxASSERT_MSG(!iItem->second.format.IsEmpty(), "Format is empty for item in a infolist");
				if (iItem->second.mode == Item::string)
				{
					_widget->SetItem(iItem->first, 1, wxString::Format(iItem->second.format, _emu->emu->GetString(_emu->handle, iItem->second.id)));
				}
				else {
					_widget->SetItem(iItem->first, 1, wxString::Format(iItem->second.format, _emu->emu->GetValI(_emu->handle, iItem->second.id)));
				}
			}
		}
		else {
			for (std::map<int, Item>::iterator iItem = _items.begin(); iItem != _items.end(); ++iItem)
			{
				wxASSERT_MSG(!iItem->second.format.IsEmpty(), "Format is empty for item in a infolist");
				if (iItem->second.mode == Item::string)
				{
					wxASSERT_MSG(false, "Not implemented");
					_widget->SetItem(iItem->first, 1, wxString::Format(iItem->second.format, ""));
				}
				else {
					_widget->SetItem(iItem->first, 1, wxString::Format(iItem->second.format, 0));
				}
			}
		}
	}
}