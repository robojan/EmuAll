#include "DebuggerRoot.h"
#include "DebuggerManager.h"
#include "../../util/log.h"
#include "../../Emulator/Emulator.h"

using namespace Debugger;

DebuggerRoot::DebuggerRoot(Emulator *emu, const pugi::xml_node &node) :
mEmu(emu), mWidget(NULL)
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
			mItems.push_back(item);
		}
	}
}

DebuggerRoot::~DebuggerRoot()
{
	for (std::vector<Item>::iterator iItem = mItems.begin();
		iItem != mItems.end(); ++iItem)
	{
		delete iItem->element;
	}
	mItems.clear();
}

wxPanel *DebuggerRoot::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	mWidget = new wxPanel(parent, id);
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	for (std::vector<Item>::iterator iItem = mItems.begin();
		iItem != mItems.end(); ++iItem)
	{
		sizer->Add(iItem->element->GetWidget(mWidget, wxID_ANY), iItem->proportion, wxEXPAND | wxALL, 5);
	}
	mWidget->SetSizerAndFit(sizer);

	UpdateInfo();
	return mWidget;
}

void DebuggerRoot::UpdateInfo()
{
	if (mEmu->emu == NULL || mWidget == NULL)
	{
		return; // Nothing to do
	}

	if (mWidget->IsShownOnScreen())
	{
		for (std::vector<Item>::iterator iItem = mItems.begin();
			iItem != mItems.end(); ++iItem)
		{
			iItem->element->UpdateInfo();
		}
	}
}