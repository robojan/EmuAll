#include "DebuggerGrid.h"
#include "DebuggerManager.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerGrid::DebuggerGrid(Emulator *emu, const pugi::xml_node &node) :
	_emu(emu), _widget(NULL), _rows(-1), _colums(-1)
{
	wxASSERT(strcmp(node.name(), "grid") == 0);
	wxASSERT(emu != NULL);

	_rows = node.child("rows").text().as_int(-1);
	_colums = node.child("columns").text().as_int(-1);
	_name = node.attribute("name").as_string("");

	if (_colums < 0)
	{
		Log(Warn, "Grid colums size is invalid");
		return;
	}

	pugi::xml_object_range<pugi::xml_node_iterator> children = node.children();
	for (pugi::xml_node_iterator iChild = children.begin(); iChild != children.end(); ++iChild)
	{
		if (strcmp(iChild->name(), "columns") == 0 || strcmp(iChild->name(), "rows") == 0)
		{
			continue;
		}
		Item item;
		item.element = GetElement(emu, *iChild);
		item.proportion = iChild->attribute("prop").as_int(0);
		if (item.element != NULL)
		{
			_items.push_back(item);
		}
	}
}

DebuggerGrid::~DebuggerGrid()
{
	for (std::vector<Item>::iterator iElement = _items.begin();
		iElement != _items.end(); ++iElement)
	{
		delete iElement->element;
	}
	_items.clear();
}

wxWindow *DebuggerGrid::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widget != NULL)
	{
		return _widget; // Widget already created
	}

	wxBoxSizer *bsizer = new wxBoxSizer(wxVERTICAL);
	if (!_name.IsEmpty())
	{

		_widget = new wxStaticBox(parent, id, _name);
		bsizer->AddSpacer(15);
	}
	else {
		_widget = new wxPanel(parent, id);
	}

	wxFlexGridSizer *sizer;
	if(_rows > 0) {
		sizer = new wxFlexGridSizer(_rows,_colums,5,5);
	}
	else {
		sizer = new wxFlexGridSizer(_colums, 5, 5);
	}
	for (std::vector<Item>::iterator iItem = _items.begin();
		iItem != _items.end(); ++iItem)
	{
		sizer->Add(iItem->element->GetWidget(_widget, wxID_ANY), iItem->proportion, wxEXPAND | wxALL, 5);
	}
	bsizer->Add(sizer);
	_widget->SetSizerAndFit(bsizer);

	UpdateInfo();
	return _widget;
}

void DebuggerGrid::UpdateInfo()
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