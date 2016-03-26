#include "DebuggerCheckbox.h"

using namespace Debugger;

DebuggerCheckbox::DebuggerCheckbox(Emulator *emu, const pugi::xml_node &node, bool read, bool write) :
	_emu(emu), _widgets(NULL)
{
	wxASSERT(strcmp(node.name(), "checkbox") == 0 || strcmp(node.name(), "flag") == 0);
	wxASSERT(emu != NULL);
	_checkbox.id = node.attribute("id").as_int(-1);
	_checkbox.name = node.child("name").text().as_string("Error!");
	bool readonly = node.child("readonly").text().as_bool(false);
	_checkbox.read = readonly ? true : read;
	_checkbox.write = readonly ? false : write;
}

DebuggerCheckbox::~DebuggerCheckbox()
{

}

wxCheckBox *DebuggerCheckbox::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (_widgets != NULL)
	{
		return _widgets; // Widget already created
	}

	_widgets = new wxCheckBox(parent, id, _checkbox.name);
	_widgets->Bind(wxEVT_CHECKBOX, &DebuggerCheckbox::OnClicked, this);

	UpdateInfo();
	return _widgets;
}

void DebuggerCheckbox::UpdateInfo()
{
	if (_emu->emu == NULL || _widgets == NULL || !_checkbox.read)
	{
		return; // Nothing to do
	}

	if (_widgets->IsShownOnScreen())
	{
		_widgets->SetValue(_emu->emu->GetValU(_emu->handle, _checkbox.id) != 0);
	}
}

void DebuggerCheckbox::OnClicked(wxCommandEvent &evt)
{
	if (_emu->emu == NULL || _widgets == NULL)
		return;
	if (_checkbox.write)
	{
		_emu->emu->SetValU(_emu->handle, _checkbox.id, evt.IsChecked() ? 1 : 0);
	}
	else {
		_widgets->SetValue(!evt.IsChecked());
	}
}