#include "DebuggerBoxSizer.h"
#include "DebuggerManager.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerBoxSizer::DebuggerBoxSizer(Emulator *emu, const pugi::xml_node &node, int orient) :
mEmu(emu), mWidget(NULL), mOrient(orient)
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
			mItems.push_back(item);
		}
	}
}

DebuggerBoxSizer::~DebuggerBoxSizer()
{
	for (std::vector<Item>::iterator iElement = mItems.begin();
		iElement != mItems.end(); ++iElement)
	{
		delete iElement->element;
	}
	mItems.clear();
}

wxPanel *DebuggerBoxSizer::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	mWidget = new wxPanel(parent, id);
	wxBoxSizer *sizer = new wxBoxSizer(mOrient);
	for (std::vector<Item>::iterator iItem = mItems.begin();
		iItem != mItems.end(); ++iItem)
	{
		sizer->Add(iItem->element->GetWidget(mWidget, wxID_ANY), iItem->proportion, wxEXPAND | wxALL, 5);
	}
	mWidget->SetSizerAndFit(sizer);
		
	UpdateInfo();
	return mWidget;
}

void DebuggerBoxSizer::UpdateInfo()
{
	if (mEmu->emu == NULL || mWidget == NULL)
	{
		return; // Nothing to do
	}


	for (std::vector<Item>::iterator iItem = mItems.begin();
		iItem != mItems.end(); ++iItem)
	{
		iItem->element->UpdateInfo();
	}
}