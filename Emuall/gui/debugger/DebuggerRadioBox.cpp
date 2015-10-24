#include "DebuggerRadioBox.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerRadioBox::DebuggerRadioBox(Emulator *emu, const pugi::xml_node &node) :
mEmu(emu), mWidget(NULL)
{
	wxASSERT(strcmp(node.name(), "radio") == 0);
	wxASSERT(emu != NULL);
	mItem.id = node.attribute("id").as_int(-1);
	mItem.name = node.child("name").text().as_string("Error!");
	mItem.readOnly = node.child("readonly").text().as_bool(false);
	pugi::xml_object_range<pugi::xml_named_node_iterator> options = node.children("option");
	for (pugi::xml_named_node_iterator iOption = options.begin(); iOption != options.end(); ++iOption)
	{
		int value = iOption->attribute("val").as_int(0);
		mItem.options[iOption->text().as_string("Error!")] = value;
	}
}

DebuggerRadioBox::~DebuggerRadioBox()
{

}

wxRadioBox *DebuggerRadioBox::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	wxArrayString options;
	for (std::map<std::string, int>::iterator iOption = mItem.options.begin(); iOption != mItem.options.end(); ++iOption)
	{
		options.Add(iOption->first);
	}

	mWidget = new wxRadioBox(parent, id, mItem.name, wxDefaultPosition, wxDefaultSize, options, 0, wxRA_SPECIFY_ROWS);
	mWidget->Bind(wxEVT_RADIOBOX, &DebuggerRadioBox::OnClicked, this);

	UpdateInfo();
	return mWidget;
}

void DebuggerRadioBox::UpdateInfo()
{
	if (mEmu->emu == NULL || mWidget == NULL)
	{
		return; // Nothing to do
	}

	if (mWidget->IsShownOnScreen())
	{
		int selection = mEmu->emu->GetValU(mEmu->handle, mItem.id);
		for (std::map<std::string, int>::iterator iOption = mItem.options.begin(); iOption != mItem.options.end(); ++iOption)
		{
			if (iOption->second == selection)
			{
				mWidget->SetSelection(mWidget->FindString(iOption->first));
				break;
			}
		}
	}
}

void DebuggerRadioBox::OnClicked(wxCommandEvent &evt)
{
	if (mEmu->emu == NULL || mWidget == NULL)
		return;

	wxString selection = mWidget->GetString(evt.GetSelection());
	
	std::map<std::string, int>::const_iterator iVal = mItem.options.find(selection.ToStdString());
	if (iVal == mItem.options.end())
	{
		Log(Warn, "Could not find option '%s' in radiobox", selection);
		return;
	}

	mEmu->emu->SetValI(mEmu->handle, mItem.id, iVal->second);
}