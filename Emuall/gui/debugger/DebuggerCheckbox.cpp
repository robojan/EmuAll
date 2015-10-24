#include "DebuggerCheckbox.h"

using namespace Debugger;

DebuggerCheckbox::DebuggerCheckbox(Emulator *emu, const pugi::xml_node &node, bool read, bool write) :
	mEmu(emu), mWidget(NULL)
{
	wxASSERT(strcmp(node.name(), "checkbox") == 0 || strcmp(node.name(), "flag") == 0);
	wxASSERT(emu != NULL);
	mCheckbox.id = node.attribute("id").as_int(-1);
	mCheckbox.name = node.child("name").text().as_string("Error!");
	bool readonly = node.child("readonly").text().as_bool(false);
	mCheckbox.read = readonly ? true : read;
	mCheckbox.write = readonly ? false : write;
}

DebuggerCheckbox::~DebuggerCheckbox()
{

}

wxCheckBox *DebuggerCheckbox::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	mWidget = new wxCheckBox(parent, id, mCheckbox.name);
	mWidget->Bind(wxEVT_CHECKBOX, &DebuggerCheckbox::OnClicked, this);

	UpdateInfo();
	return mWidget;
}

void DebuggerCheckbox::UpdateInfo()
{
	if (mEmu->emu == NULL || mWidget == NULL || !mCheckbox.read)
	{
		return; // Nothing to do
	}

	if (mWidget->IsShownOnScreen())
	{
		mWidget->SetValue(mEmu->emu->GetValU(mEmu->handle, mCheckbox.id) != 0);
	}
}

void DebuggerCheckbox::OnClicked(wxCommandEvent &evt)
{
	if (mEmu->emu == NULL || mWidget == NULL)
		return;
	if (mCheckbox.write)
	{
		mEmu->emu->SetValU(mEmu->handle, mCheckbox.id, evt.IsChecked() ? 1 : 0);
	}
	else {
		mWidget->SetValue(!evt.IsChecked());
	}
}