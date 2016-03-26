#include "DebuggerRadioBox.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerRadioBox::DebuggerRadioBox(Emulator *emu, const pugi::xml_node &node) :
_emu(emu), _widget(NULL)
{
	wxASSERT(strcmp(node.name(), "radio") == 0);
	wxASSERT(emu != NULL);
	_item.id = node.attribute("id").as_int(-1);
	_item.name = node.child("name").text().as_string("Error!");
	_item.readOnly = node.child("readonly").text().as_bool(false);
	pugi::xml_object_range<pugi::xml_named_node_iterator> options = node.children("option");
	for (pugi::xml_named_node_iterator iOption = options.begin(); iOption != options.end(); ++iOption)
	{
		int value = iOption->attribute("val").as_int(0);
		_item.options[iOption->text().as_string("Error!")] = value;
	}
}

DebuggerRadioBox::~DebuggerRadioBox()
{

}

wxRadioBox *DebuggerRadioBox::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widget != NULL)
	{
		return _widget; // Widget already created
	}

	wxArrayString options;
	for (std::map<std::string, int>::iterator iOption = _item.options.begin(); iOption != _item.options.end(); ++iOption)
	{
		options.Add(iOption->first);
	}

	_widget = new wxRadioBox(parent, id, _item.name, wxDefaultPosition, wxDefaultSize, options, 0, wxRA_SPECIFY_ROWS);
	_widget->Bind(wxEVT_RADIOBOX, &DebuggerRadioBox::OnClicked, this);

	UpdateInfo();
	return _widget;
}

void DebuggerRadioBox::UpdateInfo()
{
	if (_emu->emu == NULL || _widget == NULL)
	{
		return; // Nothing to do
	}

	if (_widget->IsShownOnScreen())
	{
		int selection = _emu->emu->GetValU(_emu->handle, _item.id);
		for (std::map<std::string, int>::iterator iOption = _item.options.begin(); iOption != _item.options.end(); ++iOption)
		{
			if (iOption->second == selection)
			{
				_widget->SetSelection(_widget->FindString(iOption->first));
				break;
			}
		}
	}
}

void DebuggerRadioBox::OnClicked(wxCommandEvent &evt)
{
	if (_emu->emu == NULL || _widget == NULL)
		return;

	wxString selection = _widget->GetString(evt.GetSelection());
	
	std::map<std::string, int>::const_iterator iVal = _item.options.find(selection.ToStdString());
	if (iVal == _item.options.end())
	{
		Log(Warn, "Could not find option '%s' in radiobox", selection);
		return;
	}

	_emu->emu->SetValI(_emu->handle, _item.id, iVal->second);
}