#include "DebuggerGrid.h"
#include "DebuggerManager.h"
#include "../../util/log.h"

using namespace Debugger;

DebuggerGrid::DebuggerGrid(Emulator *emu, const pugi::xml_node &node) :
	mEmu(emu), mWidget(NULL), mRows(-1), mColumns(-1)
{
	wxASSERT(strcmp(node.name(), "grid") == 0);
	wxASSERT(emu != NULL);

	mRows = node.child("rows").text().as_int(-1);
	mColumns = node.child("columns").text().as_int(-1);
	mName = node.attribute("name").as_string("");

	if (mColumns < 0)
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
			mItems.push_back(item);
		}
	}
}

DebuggerGrid::~DebuggerGrid()
{
	for (std::vector<Item>::iterator iElement = mItems.begin();
		iElement != mItems.end(); ++iElement)
	{
		delete iElement->element;
	}
	mItems.clear();
}

wxWindow *DebuggerGrid::GetWidget(wxWindow *parent, wxWindowID id)
{
	if (mWidget != NULL)
	{
		return mWidget; // Widget already created
	}

	wxBoxSizer *bsizer = new wxBoxSizer(wxVERTICAL);
	if (!mName.IsEmpty())
	{

		mWidget = new wxStaticBox(parent, id, mName);
		bsizer->AddSpacer(15);
	}
	else {
		mWidget = new wxPanel(parent, id);
	}

	wxFlexGridSizer *sizer;
	if(mRows > 0) {
		sizer = new wxFlexGridSizer(mRows,mColumns,5,5);
	}
	else {
		sizer = new wxFlexGridSizer(mColumns, 5, 5);
	}
	for (std::vector<Item>::iterator iItem = mItems.begin();
		iItem != mItems.end(); ++iItem)
	{
		sizer->Add(iItem->element->GetWidget(mWidget, wxID_ANY), iItem->proportion, wxEXPAND | wxALL, 5);
	}
	bsizer->Add(sizer);
	mWidget->SetSizerAndFit(bsizer);

	UpdateInfo();
	return mWidget;
}

void DebuggerGrid::UpdateInfo()
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