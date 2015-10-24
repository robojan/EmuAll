#include "DebuggerNotebook.h"
#include "DebuggerManager.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerNotebook::DebuggerNotebook(Emulator *emu, const pugi::xml_node &node) :
mEmu(emu), mWidget(NULL)
{
	wxASSERT(strcmp(node.name(), "notebook") == 0);
	wxASSERT(emu != NULL);

	pugi::xml_object_range<pugi::xml_node_iterator> children = node.children();
	int pos = 0;
	for (pugi::xml_node_iterator iChild = children.begin(); iChild != children.end(); ++iChild)
	{
		if (strcmp(iChild->name(), "tab") == 0)
		{
			Item item;
			item.name = iChild->attribute("name").as_string("Error!");
			pugi::xml_object_range<pugi::xml_node_iterator> tabChildren = iChild->children();
			for (pugi::xml_node_iterator iElement = tabChildren.begin(); iElement != tabChildren.end(); ++iElement)
			{
				DebuggerElement *element = GetElement(emu, *iElement);
				if (element != NULL)
				{
					item.elements.push_back(element);
				}
			}
			mItems.push_back(item); // Position not final, can change when creating widget
		}
		else {
			Log(Warn, "Unknown notebook element '%s'", iChild->name());
		}
	}
}

DebuggerNotebook::~DebuggerNotebook()
{
	std::vector<Item>::iterator iItem;
	for (iItem = mItems.begin(); iItem != mItems.end(); ++iItem)
	{
		for (std::vector<DebuggerElement *>::iterator iElement = iItem->elements.begin();
			iElement != iItem->elements.end(); ++iElement)
		{
			delete *iElement;
		}
		iItem->elements.clear();
	}
}

wxNotebook *DebuggerNotebook::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	mWidget = new wxNotebook(parent, id);

	std::vector<Item>::iterator iItem;
	for (iItem = mItems.begin(); iItem != mItems.end(); ++iItem)
	{
		wxPanel *page = new wxPanel(mWidget);
		wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
		for (std::vector<DebuggerElement *>::iterator iElement = iItem->elements.begin();
			iElement != iItem->elements.end(); ++iElement)
		{
			sizer->Add((*iElement)->GetWidget(page, wxID_ANY), 1, wxEXPAND|wxALL, 5);
		}
		page->SetSizerAndFit(sizer);
		mWidget->AddPage(page, iItem->name);
	}

	UpdateInfo();
	return mWidget;
}

void DebuggerNotebook::UpdateInfo()
{
	if (mEmu->emu == NULL || mWidget == NULL)
	{
		return; // Nothing to do
	}

	if (mWidget->IsShownOnScreen())
	{
		std::vector<Item>::iterator iItem;
		for (iItem = mItems.begin(); iItem != mItems.end(); ++iItem)
		{
			for (std::vector<DebuggerElement *>::iterator iElement = iItem->elements.begin();
				iElement != iItem->elements.end(); ++iElement)
			{
				(*iElement)->UpdateInfo();
			}
		}
	}
}